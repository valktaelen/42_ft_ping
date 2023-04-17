/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   includes.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 05:55:55 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/15 22:32:38 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_H
# define INCLUDES_H

# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <time.h>
# include <sys/time.h>
# include <netdb.h>
# include <signal.h>
# include <limits.h>

# define ICMP_ECHO_REQUEST 8
# define ICMP_ECHO_REPLY 0
# define ICMP_HEADER_LEN 8
# define BUFFER_SIZE 1024
# define DEBUG_PARSING 0
# define DEBUG_EXEC 0

typedef struct s_icmp_packet {
	uint8_t			type;
	uint8_t			code;
	uint16_t		checksum;
	uint16_t		id;
	uint16_t		seq_num;
	struct timeval	timestamp;
}	t_icmp_packet;

typedef struct s_ping
{
	t_icmp_packet		icmp_template;
	struct sockaddr_in	host_addr;
	struct sockaddr		cur_addr;
	struct timeval		timeout;
	struct timeval		deadline;
	struct timeval		deadline_time;
	double				rtt_min;
	double				rtt_max;
	double				rtt_sum;
	int					sockfd;
	int					ttl;
	int					seq_num;
	int					end;
	int					count;
	int					packets_sent;
	int					packets_rcvd;
	int					error;
	int					sys_error;
	uint8_t				verbose;
	uint8_t				quiet;
	char				ip_str[INET_ADDRSTRLEN];
	char				*domain;
}	t_ping;

typedef struct s_ping_info
{
	struct iphdr	ip;
	ssize_t			pkt_len;
	double			rtt;
	t_icmp_packet	icmp_pkt;
	struct icmphdr	icmp_hdr;
	struct timeval	tv_recv;
}	t_ping_info;

t_ping		*get_ping(void);
void		end_prg(int sig);
void		handler_alarm(int sig);

int			parsing(const int argc, const char **argv, t_ping *ping);

int			send_ping(t_ping *ping);
int			ft_ping(t_ping *ping);

int			init_socket(t_ping *ping);

uint16_t	compute_icmp_checksum(const void *buff, int length);
double		get_diff_tv(struct timeval tv_recv, struct timeval tv_send);
int			is_finish(t_ping *ping);
int			ft_atoi(const char *str);

void		display_help(int fd);
void		print_success(t_ping *ping, t_ping_info *info);
void		print_error(t_ping *ping, t_ping_info *info);
void		print_resume(t_ping *ping);

void		*ft_memcpy(void *dst, const void *src, size_t n);
void		ft_bzero(void *s, size_t n);

uint16_t	ft_htons(uint16_t n);
uint16_t	ft_ntohs(uint16_t n);

#endif
