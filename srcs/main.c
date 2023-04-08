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
/*
static int	ft_ping_init(t_ping *ping) {
	struct timeval timeout = {0, 750};

	if ((ping->sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		perror("socket");
		return 1;
	}
	if (setsockopt(ping->sock_fd, IPPROTO_IP, IP_TTL, &(ping->ttl), sizeof(ping->ttl)) < 0) {
		perror("setsockopt");
		close(ping->sock_fd);
		return 1;
	}
	if (setsockopt(ping->sock_fd, SOL_SOCKET, SO_RCVTIMEO, &(timeout), sizeof(timeout)) < 0) {
		perror("setsockopt");
		return 1;
	}
	ping->icmp_header.icmp_type = ICMP_ECHO;
	ping->icmp_header.icmp_code = 0;
	ping->icmp_header.icmp_id = getpid();
	return 0;
}

int	ft_ping(t_ping* ping) {
	char buf[1024];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

	ping->icmp_header.icmp_seq = htons(ping->seq);
	ping->icmp_header.icmp_cksum = 0;
	ping->icmp_header.icmp_cksum = checksum((unsigned short *)&(ping->icmp_header), sizeof(struct icmp));
	++(ping->seq);
	// send
	if (sendto(ping->sock_fd, &(ping->icmp_header), sizeof(ping->icmp_header), 0,
				(struct sockaddr *)&ping->target, sizeof(ping->target)) < 0) {
		perror("sendto\n");
		return 1;
	}
	if (recvfrom(ping->sock_fd, buf, 1024, 0, (struct sockaddr *)&from, fromlen) < 0) {
		perror("recvfrom");
		return 1;
	}
	// if send
	struct iphdr	*ip_header = (struct iphdr *)buf;
	struct icmp		*recv_icmp_header = (struct icmp *)(buf + (ip_header->ihl << 2));
	if (recv_icmp_header->icmp_type == ICMP_ECHOREPLY) {
		printf("Reply from %s: bytes=%d time=%.2fms\n", inet_ntoa(from.sin_addr), packet_size, (double)tv.tv_sec * 1000 + (double)tv.tv_usec / 1000);
		return 0;
	} else {
		printf("Received ICMP packet with type %d\n", recv_icmp_header->icmp_type);
		return 1;
	}
	return 0;
}

int	main(int argc, char const *argv[])
{
	t_ping	ping = {0};
	int		ret;

	ping.ttl = 64;
	ret = parsing(argc, argv, &ping);
	if (ret == 1)
		display_help(2);
	if (ret)
		return 1;
	ft_ping_init(&ping);
}
*/

static void	display_help(int fd) {
	dprintf(fd, "\nUsage\nft_ping [options] <destination>\n\nOptions:\n\
\t<destination>\tdns name or ip address\n\t-v\t\tverbose output\n\t-h\t\thelp\n\t-c\t\tcount <c>\
\n");
}

uint16_t compute_icmp_checksum(const void *buff, int length) {
	const uint16_t	*ptr = buff;
	uint32_t		sum;

	for (sum = 0; length > 1; length -= 2) {
		sum += *ptr++;
	}
	if (length == 1) {
		sum += *(uint8_t*)ptr;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}

int send_ping(t_ping* ping, int seq_num) {
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
		perror("sendto");
		return -1;
	}
	return 0;
}

int receive_ping(t_ping *ping, int seq_num, long *rtt) {
	struct icmp_packet	icmp_pkt;
	struct timeval		tv_recv, tv_send, tv_diff;
	socklen_t			addrlen;
	ssize_t				num_bytes_rcvd;
	char				buffer[BUFFER_SIZE];
	//int					icmp_pkt_len;

	addrlen = sizeof(ping->host_addr);
	num_bytes_rcvd = recvfrom(ping->sockfd, buffer, BUFFER_SIZE, 0,
							  (struct sockaddr*)(&(ping->host_addr)), &addrlen);
	if (num_bytes_rcvd < 0) {
		perror("recvfrom");
		return -1;
	}
	gettimeofday(&tv_recv, NULL);
	memcpy(&icmp_pkt, buffer + IP_HEADER_LEN, num_bytes_rcvd - IP_HEADER_LEN);
	//icmp_pkt_len = num_bytes_rcvd - IP_HEADER_LEN;

	if (icmp_pkt.type != ICMP_ECHO_REPLY || icmp_pkt.id != htons(getpid()) || icmp_pkt.seq_num != htons(seq_num)) {
		return -1;
	}

	tv_send = icmp_pkt.timestamp;
	tv_diff.tv_sec = tv_recv.tv_sec - tv_send.tv_sec;
	tv_diff.tv_usec = tv_recv.tv_usec - tv_send.tv_usec;

	*rtt = tv_diff.tv_sec * 1000 + tv_diff.tv_usec / 1000;
	return 0;
}

int	ft_init_ping(t_ping* ping) {
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
			perror("socket");
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
		perror("setsockopt");
		close(ping->sockfd);
		return 1;
	}
	if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &(ping->timeout), sizeof(ping->timeout)) < 0) {
		perror("setsockopt");
		close(ping->sockfd);
		return 1;
	}
	return 0;
}

int ft_ping(t_ping* ping) {
	int					seq_num = 0;
	long				rtt_sum = 0;
	int					count = 0;

	if (ft_init_ping(ping))
		return 1;
	printf("PING %s (%s) using protocol ICMP\n", ping->domain, ping->ip_str);
	while (1) {
		if (ping->count && ping->count == count)
			break;
		if (send_ping(ping, seq_num) < 0) {
			printf("Error sending ICMP packet\n");
			continue;
		}
		++(ping->packets_sent);

		long rtt;
		if (receive_ping(ping, seq_num, &rtt) < 0) {
			printf("Request timed out\n");
		} else {
			printf("Reply from %s: time=%.2fms TTL=%d\n", ping->ip_str, (double)rtt, ping->ttl);
			rtt_sum += rtt;
			++(ping->packets_rcvd);
		}
		++count;
		usleep(1024000);
	}
	printf("\n--- %s ping statistics ---\n", ping->domain);
	printf("%d packets transmitted, %d received, %.0f%% packet loss, avg=%.0fms\n",
			ping->packets_sent, ping->packets_rcvd, 100.0 * (ping->packets_sent - ping->packets_rcvd) / ping->packets_sent,
			(double)rtt_sum / ping->packets_rcvd);
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
	printf("%s\n", ping.domain);
	ft_ping(&ping);
	return 0;
	//return ping(argv[1], atoi(argv[2]), atoi(argv[3]));
}
