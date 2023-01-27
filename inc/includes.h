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

# define DEBUG 1

typedef struct s_ping
{
	char	error[512];
}	t_ping;

void			display_sockaddr(struct sockaddr *sockaddr);
void			display_addrinfo(struct addrinfo *addrinfo);

int				ft_ping(const char *host, t_ping *ping);
struct addrinfo	*resolve_dns(const char *host, t_ping *ping);

#endif
