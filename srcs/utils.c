#include "../inc/includes.h"

void	display_help(int fd) {
	dprintf(fd, "\nUsage\nft_ping [options] <destination>\n\
\nOptions:\n\
\t<destination>\tdns name or ip address\n\
\t-v\t\tverbose output\n\
\t-h\t\thelp\n\
\t-c\t\tcount <integer>\n\
\t-t\t\ttime to live <integer>\n\
\t-W\t\ttimeout <integer in second>\n\
\t-w\t\tdeadline <integer in second>\n\
\t-q\t\tquiet\n\
\t-4\t\tipv4 only\n\
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

void	end_prg(int sig) {
	if (sig == SIGINT)
		end = 1;
}
