/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 06:03:57 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/15 23:18:59 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

int	main(int argc, const char *argv[])
{
	t_ping	*ping;
	int		ret;

	ping = get_ping();
	ft_bzero(ping, sizeof(t_ping));
	ping->ttl = 64;
	ping->timeout.tv_sec = 1;
	ret = parsing(argc, argv, ping);
	if (ret == 1)
		display_help(2);
	if (ret)
		return (1);
	ft_ping(ping);
	return (0);
}
