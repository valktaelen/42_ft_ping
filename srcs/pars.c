/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pars.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 01:46:16 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/15 01:46:19 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

static int	parsing_time(struct timeval *tv, const char **argv, int *i,
	const char *flag)
{
	if (argv[*i + 1] == NULL)
		return (1);
	tv->tv_sec = ft_atoi(argv[*i + 1]);
	if (DEBUG_PARSING)
		printf("%s: %ld %ld\n", flag, tv->tv_sec, tv->tv_usec);
	if (!tv->tv_sec && !tv->tv_usec)
		return (1);
	++(*i);
	return (0);
}

static int	parsing_int(int *val, const char **argv, int *i,
	const char *flag)
{
	if (argv[*i + 1] == NULL)
		return (1);
	*val = atoi(argv[*i + 1]);
	if (DEBUG_PARSING)
		printf("%s: %d\n", flag, *val);
	if (!*val)
		return (1);
	++(*i);
	return (0);
}

static int	parse_param(const char **argv, t_ping *ping, int *i)
{
	if (argv[*i][1] == 'c')
	{
		if (parsing_int(&(ping->count), argv, i, "ttl"))
			return (1);
	}
	else if (argv[*i][1] == 't')
	{
		if (parsing_int(&(ping->ttl), argv, i, "ttl"))
			return (1);
	}
	else if (argv[*i][1] == 'W')
	{
		if (parsing_time(&(ping->timeout), argv, i, "timeout"))
			return (1);
	}
	else if (argv[*i][1] == 'w')
	{
		if (parsing_time(&(ping->deadline), argv, i, "deadline"))
			return (1);
	}
	return (0);
}

static int	iter_flags(const char **argv, t_ping *ping, int *i)
{
	if (!argv[*i][1] || argv[*i][2])
	{
		dprintf(2, "Error: Unknown flag %s\n", argv[*i]);
		return (1);
	}
	if ((argv[*i][1] == 'c') || (argv[*i][1] == 't')
	|| (argv[*i][1] == 'W') || (argv[*i][1] == 'w'))
	{
		if (parse_param(argv, ping, i))
			return (1);
	}
	else if (argv[*i][1] == 'v')
		ping->verbose = 1;
	else if (argv[*i][1] == 'q')
		ping->quiet = 1;
	else if (argv[*i][1] != '4')
		return (1);
	return (0);
}

int	parsing(const int argc, const char **argv, t_ping *ping)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (argv[i][0] != '-')
		{
			if (ping->domain == NULL)
				ping->domain = (char *) argv[i];
			else
				return (1);
		}
		else
		{
			if (iter_flags(argv, ping, &i))
				return (1);
		}
		++i;
	}
	if (ping->domain == NULL)
		return ((dprintf(2, "Usage error: destination require\n") & 0) + 2);
	if (ping->count && ping->deadline.tv_sec)
		ping->deadline.tv_sec = 0;
	return (0);
}
