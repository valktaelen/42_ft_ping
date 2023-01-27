/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dns.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 06:03:48 by aartiges          #+#    #+#             */
/*   Updated: 2023/01/27 06:04:21 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

struct addrinfo	*resolve_dns(const char *host, t_ping *ping)
{
	struct addrinfo	*res;
	struct addrinfo	hint;

	res = NULL;
	bzero(&hint, sizeof (struct addrinfo));
	hint.ai_family = PF_INET;
	getaddrinfo(host, NULL, &hint, &res);
	if (DEBUG)
		display_addrinfo(res);
	if (!res)
		sprintf(ping->error, "cannot resolve %s: Unknown host", host);
	return (res);
}
