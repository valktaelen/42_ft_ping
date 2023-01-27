/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 06:03:57 by aartiges          #+#    #+#             */
/*   Updated: 2023/01/27 06:04:48 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

int	main(int argc, char const *argv[])
{
	t_ping	ping;

	bzero(&ping, sizeof(t_ping));
	if (argc < 2)
	{
		dprintf(2, "Usage: ft_ping host");
		return (1);
	}
	// TODO parse command
	if (ft_ping(argv[1], &ping))
	{
		dprintf(2, "ft_ping: %s\n", ping.error);
		return (1);
	}
	return (0);
}
