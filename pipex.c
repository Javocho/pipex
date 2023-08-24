/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 19:36:35 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/24 14:03:48 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*find_path(char **envp)
{
	while (ft_strncmp(*envp, "PATH=", 5))
		envp++;
	return (*envp + 5);
}

int	main(int argc, char **argv, char **envp)
{
	int		infile;
	int		outfile;
	int		permission;
	char	**routes;

	if (argc == 5)
	{
		infile = open(argv[1], O_RDONLY);
		permission = access(argv[1], F_OK | R_OK);
		if (infile == -1 || permission == -1)
			return (-1);
		outfile = open(argv[argc - 1], O_TRUNC | O_CREAT, 0666);
		permission = access(argv[1], W_OK);
		if (outfile == -1 || permission == -1)
			return (-1);
		routes = ft_split(find_path(envp), ':');
		while (*routes++)
			printf("%s\n", *routes);
		close(infile);
		close(outfile);
	}
}
