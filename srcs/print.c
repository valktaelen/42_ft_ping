/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 04:07:39 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/15 22:34:56 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

void	display_help(int fd)
{
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

void	print_success(t_ping *ping, t_ping_info *info)
{
	char	domain[NI_MAXHOST];

	if (ping->quiet)
		return ;
	bzero(domain, NI_MAXHOST);
	printf("%ld bytes from ", info->pkt_len);
	if (getnameinfo(&(ping->cur_addr), sizeof(struct sockaddr),
			domain, NI_MAXHOST, NULL, 0, 0))
		printf("%s", ping->ip_str);
	else
		printf("%s", domain);
	printf(" (%s): time=%.2lfms TTL=%d, icmp_sec=%d\n", ping->ip_str,
		info->rtt, info->ip.ttl, ft_ntohs(info->icmp_pkt.seq_num));
}

static const char	*error_response(uint8_t type, uint8_t code)
{
	static const char	*rep[] = {
		"Destination Unreachable", "Source Quench", "Redirect Message",
		"TTL expired in transit", "Fragment reassembly time exceeded"
		"Parameter Problem: Bad IP header",
		"Error type not define",
	};

	if (type >= ICMP_DEST_UNREACH && type <= ICMP_REDIRECT)
		return (rep[type - ICMP_DEST_UNREACH]);
	if (type == ICMP_TIME_EXCEEDED)
	{
		if (code == ICMP_EXC_TTL)
			return (rep[3]);
		else
			return (rep[4]);
	}
	if (type == ICMP_PARAMETERPROB)
		return (rep[5]);
	return (rep[6]);
}

void	print_error(t_ping *ping, t_ping_info *infos)
{
	struct sockaddr_in	*addr;
	char				domain[NI_MAXHOST];
	char				ip_str[INET_ADDRSTRLEN];

	if (ping->quiet)
		return ;
	addr = (struct sockaddr_in *)(&ping->cur_addr);
	bzero(ip_str, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(addr->sin_addr), ip_str, INET_ADDRSTRLEN);
	bzero(domain, NI_MAXHOST);
	if (getnameinfo(&(ping->cur_addr), sizeof(struct sockaddr),
			domain, NI_MAXHOST, NULL, 0, 0))
		printf("From %s (%s) icmp_seq=%d %s\n",
			ip_str, ip_str, infos->icmp_pkt.seq_num,
			error_response(infos->icmp_hdr.type, infos->icmp_hdr.code));
	else
		printf("From %s (%s) icmp_seq=%d %s\n",
			domain, ip_str, infos->icmp_pkt.seq_num,
			error_response(infos->icmp_hdr.type, infos->icmp_hdr.code));
}

void	print_resume(t_ping *ping)
{
	printf("\n--- %s ping statistics ---\n", ping->domain);
	printf("%d packets transmitted, %d received, %.0f%% packet loss",
		ping->packets_sent, ping->packets_rcvd,
		100.0 * (ping->packets_sent - ping->packets_rcvd) / ping->packets_sent);
	if (ping->error)
		printf(", %d errors", ping->error);
	if (ping->packets_rcvd == 0)
		ping->rtt_min = 0;
	if (ping->rtt_sum == 0)
		ping->packets_rcvd = 1;
	printf("\navg=%.2lfms, min=%.2lfms, max=%.2lfms\n",
		ping->rtt_sum / (double)ping->packets_rcvd,
		ping->rtt_min, ping->rtt_max);
}
