/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 20:18:55 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/15 22:36:10 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

t_ping	*get_ping(void)
{
	static t_ping	ping;

	return (&ping);
}

void	end_prg(int sig)
{
	t_ping	*ping;

	if (sig == SIGINT)
	{
		ping = get_ping();
		ping->end = 1;
		ping->quiet = 1;
		print_resume(ping);
		close(ping->sockfd);
		exit(1);
	}
}

void	handler_alarm(int sig)
{
	t_ping	*ping;

	if (sig != SIGALRM)
		return ;
	ping = get_ping();
	if (is_finish_handler(ping))
		return ;
	if (send_ping(ping) < 0)
	{
		if (DEBUG_EXEC)
			dprintf(2, "Error sending ICMP packet\n");
	}
	++(ping->packets_sent);
	alarm(1);
}
