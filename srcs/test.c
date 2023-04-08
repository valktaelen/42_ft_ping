#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY 0
#define ICMP_HEADER_LEN 8
#define IP_HEADER_LEN 20
#define BUFFER_SIZE 1024


typedef	struct icmp_packet {
	uint8_t			type;
	uint8_t			code;
	uint16_t		checksum;
	uint16_t		id;
	uint16_t		seq_num;
	struct timeval	timestamp;
}	t_icmp_packet;

typedef	struct s_ping
{
	t_icmp_packet	icmp_template;

}	t_ping;

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

int send_ping(int sockfd, const struct sockaddr_in *dest_addr, int ttl, int seq_num) {
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

	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		perror("setsockopt");
		return -1;
	}

	num_bytes_sent = sendto(sockfd, &icmp_pkt, packet_len, 0,
							(struct sockaddr*)dest_addr, sizeof(*dest_addr));
	if (num_bytes_sent < 0) {
		perror("sendto");
		return -1;
	}
	return 0;
}

int receive_ping(int sockfd, struct sockaddr_in *src_addr, int seq_num, long *rtt) {
	struct icmp_packet	icmp_pkt;
	struct timeval		tv_recv, tv_send, tv_diff;
	socklen_t			addrlen;
	ssize_t				num_bytes_rcvd;
	char				buffer[BUFFER_SIZE];
	int					icmp_pkt_len;

	addrlen = sizeof(*src_addr);
	num_bytes_rcvd = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
							  (struct sockaddr*)src_addr, &addrlen);
	if (num_bytes_rcvd < 0) {
		perror("recvfrom");
		return -1;
	}
	gettimeofday(&tv_recv, NULL);
	memcpy(&icmp_pkt, buffer + IP_HEADER_LEN, num_bytes_rcvd - IP_HEADER_LEN);
	icmp_pkt_len = num_bytes_rcvd - IP_HEADER_LEN;

	if (icmp_pkt.type != ICMP_ECHO_REPLY || icmp_pkt.id != htons(getpid()) || icmp_pkt.seq_num != htons(seq_num)) {
		return -1;
	}

	tv_send = icmp_pkt.timestamp;
	tv_diff.tv_sec = tv_recv.tv_sec - tv_send.tv_sec;
	tv_diff.tv_usec = tv_recv.tv_usec - tv_send.tv_usec;

	*rtt = tv_diff.tv_sec * 1000 + tv_diff.tv_usec / 1000;
	return 0;
}

int ping(const char *hostname, int ttl, int count) {
	struct addrinfo		hints;
	struct addrinfo		*result, *rp;
	struct sockaddr_in	*sa_in;
	int					sockfd, ret, seq_num;
	long				rtt_sum = 0;
	int					packets_sent = 0, packets_rcvd = 0;
	char				ip_str[INET_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	ret = getaddrinfo(hostname, NULL, &hints, &result);

	if (ret != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sa_in = (struct sockaddr_in*)rp->ai_addr;
		inet_ntop(AF_INET, &(sa_in->sin_addr), ip_str, INET_ADDRSTRLEN);

		sockfd = socket(rp->ai_family, SOCK_RAW, IPPROTO_ICMP);

		if (sockfd < 0) {
			perror("socket");
			continue;
		}

		break;
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not create socket\n");
		return -1;
	}

	printf("PING %s (%s) using protocol ICMP\n", hostname, ip_str);

	for (seq_num = 1; seq_num <= count; ++seq_num) {
		if (send_ping(sockfd, sa_in, ttl, seq_num) < 0) {
			printf("Error sending ICMP packet\n");
			continue;
		}

		++packets_sent;

		long rtt;

		if (receive_ping(sockfd, sa_in, seq_num, &rtt) < 0) {
			printf("Request timed out\n");
		} else {
			printf("Reply from %s: time=%.2fms TTL=%d\n", ip_str, (double)rtt, ttl);
			rtt_sum += rtt;
			++packets_rcvd;
		}

		sleep(1);
	}

	if (packets_rcvd > 0) {
		printf("\n--- %s ping statistics ---\n", hostname);
		printf("%d packets transmitted, %d received, %.0f%% packet loss, time=%.0fms\n",
			   packets_sent, packets_rcvd, 100.0 * (packets_sent - packets_rcvd) / packets_sent,
			   (double)rtt_sum / packets_rcvd);
	} else {
		printf("No packets received\n");
	}

	close(sockfd);
	freeaddrinfo(result);

	return 0;
}

int main(int argc, char *argv[]) {
	t_ping	ping = {0};

	
	if (argc != 4) {
		fprintf(stderr, "Usage: %s hostname ttl count\n", argv[0]);
		return -1;
	}

	return ping(argv[1], atoi(argv[2]), atoi(argv[3]));
}
