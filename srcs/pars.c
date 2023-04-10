#include "../inc/includes.h"

int	parsing(const int argc, const char **argv, t_ping*	ping) {
	int	i;

	i = 1;
	while (i < argc)
	{
		if (argv[i][0] != '-') {
			if (ping->domain == NULL) {
				ping->domain = (char*)argv[i];
			}
			else
				return (1);
		} else {
			if (argv[i][1] == 'v')
				ping->verbose = 1;
			else if (argv[i][1] == 'h')
				return (1);
			else if (argv[i][1] == 'c') {
				if (i + 1 == argc)
					return 1;
				ping->count = atoi(argv[i + 1]);
				if (DEBUG_PARSING)
					printf("count: %d\n", ping->count);
				if (!ping->count)
					return 1;
				++i;
			}
			else if (argv[i][1] == 't') {
				if (i + 1 == argc)
					return 1;
				ping->ttl = atoi(argv[i + 1]);
				if (DEBUG_PARSING)
					printf("ttl: %d\n", ping->ttl);
				if (!ping->ttl)
					return 1;
				++i;
			}
			else if (argv[i][1] == 'W') {
				if (i + 1 == argc)
					return 1;
				ping->timeout.tv_sec = atol(argv[i + 1]);
				if (DEBUG_PARSING)
					printf("timeout: %ld %ld\n", ping->timeout.tv_sec, ping->timeout.tv_usec);
				if (!ping->timeout.tv_sec && !ping->timeout.tv_usec)
					return 1;
				++i;
			}
			else if (argv[i][1] == 'w') {
				if (i + 1 == argc)
					return 1;
				ping->deadline.tv_sec = atol(argv[i + 1]);
				if (DEBUG_PARSING)
					printf("deadline: %ld %ld\n", ping->deadline.tv_sec, ping->deadline.tv_usec);
				if (!ping->deadline.tv_sec && !ping->deadline.tv_usec)
					return 1;
				++i;
			}
			else
				return (1);
		}
		++i;
	}
	if (ping->domain == NULL) {
		dprintf(2, "Usage error: destination require\n");
		return (2);
	}
	return (0);
}
