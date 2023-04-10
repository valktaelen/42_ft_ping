/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 06:03:57 by aartiges          #+#    #+#             */
/*   Updated: 2023/01/27 06:04:48 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

static int send_ping(t_ping* ping, int seq_num) {
	struct icmp_packet	icmp_pkt;
	struct timeval		tv;
	ssize_t				num_bytes_sent;
	int					packet_len;

	memset(&icmp_pkt, 0, sizeof(icmp_pkt));
	icmp_pkt.type = ICMP_ECHO_REQUEST;
	icmp_pkt.code = 0;
	icmp_pkt.id = htons(getpid());
	icmp_pkt.seq_num = htons(seq_num);

	gettimeofday(&tv, NULL);
	icmp_pkt.timestamp = tv;

	packet_len = ICMP_HEADER_LEN + sizeof(tv);

	icmp_pkt.checksum = 0;
	icmp_pkt.checksum = compute_icmp_checksum(&icmp_pkt, packet_len);

	num_bytes_sent = sendto(ping->sockfd, &icmp_pkt, packet_len, 0,
							(struct sockaddr*)(&(ping->host_addr)), sizeof(ping->host_addr));
	if (num_bytes_sent < 0) {
		//perror("sendto");
		return -1;
	}
	return 0;
}

static int receive_ping(t_ping *ping, int seq_num, long *rtt, ssize_t* icmp_pkt_len, struct iphdr* ip) {
	struct icmp_packet	icmp_pkt;
	struct icmphdr		icmp_hdr;
	struct timeval		tv_recv, tv_send, tv_diff;
	socklen_t			addrlen;
	ssize_t				num_bytes_rcvd;
	char				buffer[BUFFER_SIZE];
	double				time;

	ping->cur_addr.sa_family = ping->host_addr.sin_family;
	addrlen = sizeof(ping->cur_addr);
	num_bytes_rcvd = recvfrom(ping->sockfd, buffer, BUFFER_SIZE, 0,
							  &(ping->cur_addr), &addrlen);
	if (num_bytes_rcvd < 0) {
		/*if (EAGAIN != errno)
			perror("recvfrom");*/
		return -1;
	}
	gettimeofday(&tv_recv, NULL);
	memcpy(&icmp_hdr, (buffer + sizeof(struct iphdr)), sizeof(struct icmphdr));
	if (icmp_hdr.type != ICMP_ECHO_REPLY)
		return -2;
	memcpy(&icmp_pkt, (buffer + sizeof(struct iphdr)), num_bytes_rcvd - sizeof(struct iphdr));
	if (icmp_pkt.type != ICMP_ECHO_REPLY || icmp_pkt.id != htons(getpid()) || icmp_pkt.seq_num != htons(seq_num))
		return -3;
	*icmp_pkt_len = num_bytes_rcvd;
	memcpy(ip, buffer, sizeof(struct iphdr));

	tv_send = icmp_pkt.timestamp;
	tv_diff.tv_sec = tv_recv.tv_sec - tv_send.tv_sec;
	tv_diff.tv_usec = tv_recv.tv_usec - tv_send.tv_usec;
	time = (double)tv_diff.tv_sec * 1000. + (double)tv_diff.tv_usec / 1000.;
	if (time > ping->rtt_max)
		ping->rtt_max = time;
	if (time < ping->rtt_min)
		ping->rtt_min = time;
	ping->rtt_sum += time;
	*rtt = time;
	return 0;
}

static int	ft_init_ping(t_ping* ping) {
	struct addrinfo		hints = {0};
	struct addrinfo		*result, *rp;
	struct sockaddr_in	*sa_in;
	int					ret;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	if ((ret = getaddrinfo(ping->domain, NULL, &hints, &result))) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sa_in = (struct sockaddr_in*)rp->ai_addr;
		ping->host_addr = *sa_in;
		inet_ntop(AF_INET, &(ping->host_addr.sin_addr), ping->ip_str, INET_ADDRSTRLEN);
		if ((ping->sockfd = socket(rp->ai_family, SOCK_RAW, IPPROTO_ICMP)) < 0) {
			//perror("socket");
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (!rp) {
		fprintf(stderr, "Could not create socket\n");
		return -1;
	}
	if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL, &(ping->ttl), sizeof(ping->ttl)) < 0) {
		//perror("setsockopt");
		close(ping->sockfd);
		return 1;
	}
	if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &(ping->timeout), sizeof(ping->timeout)) < 0) {
		//perror("setsockopt");
		close(ping->sockfd);
		return 1;
	}
	ping->rtt_min = 99999999;
	gettimeofday(&(ping->deadline_time), NULL);
	ping->deadline_time.tv_sec += ping->deadline.tv_sec;
	ping->deadline_time.tv_usec = 1000;
	return 0;
}

static int ft_ping(t_ping* ping) {
	int					seq_num = 0;
	int					count = 0;
	int					error = 0;
	char				domain[NI_MAXHOST];
	char				ip_str[INET_ADDRSTRLEN];
	int 				ret = 0;
	long				rtt;
	ssize_t				size = 0;
	struct sockaddr_in	*addr;
	struct timeval		now;
	struct iphdr		ip = {0};

	if (ft_init_ping(ping))
		return 1;
	printf("PING %s (%s) using protocol ICMP\n", ping->domain, ping->ip_str);
	signal(SIGINT, end_prg);
	end = 0;
	while (!end) {
		if (ping->count && ping->count == count)
			break;
		gettimeofday(&now, NULL);
		if (ping->deadline.tv_sec != 0 && (now.tv_sec > ping->deadline_time.tv_sec || (now.tv_sec == ping->deadline_time.tv_sec && now.tv_usec >= ping->deadline_time.tv_usec)))
			break;
		if (send_ping(ping, seq_num) < 0) {
			if (DEBUG_EXEC)
				printf("Error sending ICMP packet\n");
			continue;
		}
		++(ping->packets_sent);

		size = 0;
		if ((ret = receive_ping(ping, seq_num, &rtt, &size, &ip)) < 0) {
			// error
			if (ret == -2) {
				if (!ping->quiet) {
					addr = (struct sockaddr_in *)(&ping->cur_addr);;
					bzero(ip_str, INET_ADDRSTRLEN);
					inet_ntop(AF_INET, &(addr->sin_addr), ip_str, INET_ADDRSTRLEN);
					bzero(domain, NI_MAXHOST);
					if (getnameinfo(&(ping->cur_addr), sizeof(struct sockaddr), domain, NI_MAXHOST, NULL, 0, 0))
						printf("From %s (%s) icmp_seq=x Time to live exceeded\n", ip_str, ip_str);
					else
						printf("From %s (%s) icmp_seq=x Time to live exceeded\n", domain, ip_str);
				}
				++error;
			} else if (ret == -1) {
				if (DEBUG_EXEC)
					printf("Request timed out\n");
			}
			else {
				if (DEBUG_EXEC)
					printf("Not send by me\n");
			}
		} else {
			// success
			if (!ping->quiet) {
				bzero(domain, NI_MAXHOST);
				printf("%ld bytes from ", size);
				if (getnameinfo(&(ping->cur_addr), sizeof(struct sockaddr), domain, NI_MAXHOST, NULL, 0, 0))
					printf("%s", ping->ip_str);
				else
					printf("%s", domain);
				printf(" (%s): time=%.2fms TTL=%d\n", ping->ip_str, (double)rtt, ip.ttl);
			}
			++(ping->packets_rcvd);
		}
		++count;
		usleep(1024000);
	}
	printf("\n--- %s ping statistics ---\n", ping->domain);
	printf("%d packets transmitted, %d received, %.0f%% packet loss, ",
			ping->packets_sent, ping->packets_rcvd, 100.0 * (ping->packets_sent - ping->packets_rcvd) / ping->packets_sent);
	if (error)
		printf("%d errors, ", error);
	printf("\navg=%.0fms, min=%.0fms, max=%.0fms\n", ping->rtt_sum / (double)ping->packets_rcvd, ping->rtt_min, ping->rtt_max);
	close(ping->sockfd);
	return 0;
}

int main(int argc, const char *argv[]) {
	t_ping	ping = {0};
	int		ret;

	ping.ttl = 64;
	ping.timeout.tv_sec = 1;
	ret = parsing(argc, argv, &ping);
	if (ret == 1)
		display_help(2);
	if (ret)
		return 1;
	//printf("%s\n", ping.domain);
	ft_ping(&ping);
	return 0;
}
