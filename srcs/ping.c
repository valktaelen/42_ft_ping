/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 06:03:52 by aartiges          #+#    #+#             */
/*   Updated: 2023/01/27 06:05:18 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

int	ft_ping(const char *host, t_ping *ping)
{
	struct addrinfo	*res;

	res = resolve_dns(host, ping);
	if (!res)
		return (1);
	// TODO ping address
	freeaddrinfo(res);
	return (0);
}
