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
				printf("%d\n", i + 1 == argc);
				if (i + 1 == argc)
					return 1;
				ping->count = atoi(argv[i + 1]);
				if (!ping->count)
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
