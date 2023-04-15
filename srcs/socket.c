/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 03:01:14 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/15 03:32:20 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

static int	iter_addr(t_ping *ping, struct addrinfo *result,
	struct addrinfo *addr, struct sockaddr_in *sa_in)
{
	addr = result;
	while (addr != NULL)
	{
		sa_in = (struct sockaddr_in *) addr->ai_addr;
		ping->host_addr = *sa_in;
		inet_ntop(AF_INET, &(ping->host_addr.sin_addr), ping->ip_str, \
		INET_ADDRSTRLEN);
		ping->sockfd = socket(addr->ai_family, SOCK_RAW, IPPROTO_ICMP);
		if (ping->sockfd < 0)
		{
			if (DEBUG_EXEC)
				dprintf(2, "socket: %s\n", strerror(errno));
			addr = addr->ai_next;
			continue ;
		}
		break ;
	}
	freeaddrinfo(result);
	if (!addr)
	{
		dprintf(2, "Could not create socket\n");
		return (-1);
	}
	return (0);
}

static int	set_socket(t_ping *ping)
{
	if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL,
			&(ping->ttl), sizeof(ping->ttl)) < 0)
	{
		dprintf(2, "setsockopt: %s\n", strerror(errno));
		close(ping->sockfd);
		return (1);
	}
	if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO,
			&(ping->timeout), sizeof(ping->timeout)) < 0)
	{
		dprintf(2, "setsockopt: %s\n", strerror(errno));
		close(ping->sockfd);
		return (1);
	}
	return (0);
}

int	init_socket(t_ping *ping)
{
	struct addrinfo		hints;
	struct addrinfo		*result;
	int					ret;

	ft_bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	ret = getaddrinfo(ping->domain, NULL, &hints, &result);
	if (ret)
	{
		dprintf(2, "getaddrinfo: %s\n", gai_strerror(ret));
		return (-1);
	}
	ret = iter_addr(ping, result, NULL, NULL);
	if (ret)
		return (ret);
	return (set_socket(ping));
}
