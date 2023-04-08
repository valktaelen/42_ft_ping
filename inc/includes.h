/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   includes.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 05:55:55 by aartiges          #+#    #+#             */
/*   Updated: 2023/01/27 05:58:02 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_H
# define INCLUDES_H
/*

# include <errno.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <signal.h>
# include <netdb.h>
# include <string.h>

# include <sys/types.h>
# include <sys/time.h>
# include <sys/socket.h>

# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# define DEBUG 1
typedef struct s_ping
{
	char				error[512];
	char				buf[1024];
	char				*domain;
	struct sockaddr_in	target;
	struct icmp			icmp_header;
	int			seq;
	short				verbose;
	int					ttl;
	int					timeout;
	int					sock_fd;
}	t_ping;
*/

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
	t_icmp_packet		icmp_template;
	struct sockaddr_in	host_addr;
	struct timeval		timeout;
	int					sockfd;
	int					ttl;
	int					count;
	int					packets_sent;
	int					packets_rcvd;
	char				verbose;
	char				ip_str[INET_ADDRSTRLEN];
	char*				domain;
}	t_ping;

int				parsing(const int argc, const char **argv, t_ping*	ping);
unsigned short	checksum(unsigned short *buf, int len);

#endif
