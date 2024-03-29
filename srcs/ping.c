/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 04:47:01 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/16 03:52:13 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

int	send_ping(t_ping *ping)
{
	t_icmp_packet	icmp_pkt;
	struct timeval	tv;
	ssize_t			num_bytes_sent;
	int				packet_len;

	ft_bzero(&icmp_pkt, sizeof(icmp_pkt));
	icmp_pkt.type = ICMP_ECHO_REQUEST;
	icmp_pkt.code = 0;
	icmp_pkt.id = ft_htons(getpid());
	icmp_pkt.seq_num = ft_htons(++(ping->seq_num));
	gettimeofday(&tv, NULL);
	icmp_pkt.timestamp = tv;
	packet_len = ICMP_HEADER_LEN + sizeof(tv);
	icmp_pkt.checksum = 0;
	icmp_pkt.checksum = compute_icmp_checksum(&icmp_pkt, packet_len);
	num_bytes_sent = sendto(ping->sockfd, &icmp_pkt, packet_len, 0,
			(struct sockaddr*)(&(ping->host_addr)), sizeof(ping->host_addr));
	if (num_bytes_sent < 0)
	{
		if (DEBUG_EXEC)
			dprintf(2, "sendto: %s\n", strerror(errno));
		return (-1);
	}
	return (0);
}

static int	proceed_receive(t_ping *ping, t_ping_info *infos, char *buffer)
{
	gettimeofday(&(infos->tv_recv), NULL);
	ft_memcpy(&(infos->ip), buffer, sizeof(struct iphdr));
	ft_memcpy(&(infos->icmp_hdr), (buffer + sizeof(struct iphdr)),
		sizeof(struct icmphdr));
	if (infos->icmp_hdr.type != ICMP_ECHO_REPLY)
		return (-2);
	ft_memcpy(&(infos->icmp_pkt), (buffer + sizeof(struct iphdr)),
		infos->pkt_len - sizeof(struct iphdr));
	if (infos->icmp_pkt.type != ICMP_ECHO_REPLY
		|| infos->icmp_pkt.id != ft_htons(getpid()))
		return (-3);
	infos->rtt = get_diff_tv((infos->tv_recv), infos->icmp_pkt.timestamp);
	if (infos->rtt > ping->rtt_max)
		ping->rtt_max = infos->rtt;
	if (infos->rtt < ping->rtt_min)
		ping->rtt_min = infos->rtt;
	ping->rtt_sum += infos->rtt;
	return (0);
}

static int	receive_ping(t_ping *ping, t_ping_info *infos)
{
	char			buffer[BUFFER_SIZE];
	socklen_t		addrlen;

	++infos->icmp_pkt.seq_num;
	ping->cur_addr.sa_family = ping->host_addr.sin_family;
	addrlen = sizeof(ping->cur_addr);
	infos->pkt_len = recvfrom(ping->sockfd, buffer, BUFFER_SIZE, 0,
			&(ping->cur_addr), &addrlen);
	if (infos->pkt_len < 0)
	{
		if (DEBUG_EXEC)
			dprintf(2, "recvfrom: %s\n", strerror(errno));
		++(ping->sys_error);
		return (-1);
	}
	return (proceed_receive(ping, infos, buffer));
}

static int	ft_init_ping(t_ping *ping)
{
	int	ret;

	ret = init_socket(ping);
	if (ret)
		return (ret);
	ping->rtt_min = 99999999;
	gettimeofday(&(ping->deadline_time), NULL);
	ping->deadline_time.tv_sec += ping->deadline.tv_sec;
	signal(SIGINT, end_prg);
	signal(SIGALRM, handler_alarm);
	printf("PING %s (%s) using protocol ICMP\n", ping->domain, ping->ip_str);
	ping->end = 0;
	handler_alarm(SIGALRM);
	return (0);
}

int	ft_ping(t_ping *ping)
{
	t_ping_info	infos;
	int			ret;

	if (ft_init_ping(ping))
		return (1);
	ft_bzero(&infos, sizeof(t_ping_info));
	while (!is_finish(ping))
	{
		ret = receive_ping(ping, &infos);
		if (ret < -1)
		{
			print_error(ping, &infos);
			++(ping->error);
		}
		else if (ret != -1)
		{
			print_success(ping, &infos);
			++(ping->packets_rcvd);
		}
		if (!is_finish(ping))
			usleep(1000000);
	}
	print_resume(ping);
	close(ping->sockfd);
	return (0);
}
