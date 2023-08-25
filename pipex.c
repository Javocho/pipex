/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 19:36:35 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/25 04:09:04 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*find_path(char **envp)
{
	while (ft_strncmp(*envp, "PATH=", 5))
		envp++;
	return (*envp + 5);
}

void	find_cmd()
{
	//añadir a cada env una / y el comando para buscarlo con access, importante liberar y si no lo encuentra liberar y salir
}

void	first_child(t_pipe pipex, char **argv)
{
	dup2(pipex.infile, 0);
	dup2(pipex.tube[1], 1);
	close(pipex.tube[0]);
	pipex.cmd_args = ft_split(argv[2], ' ');
	//si lo encuentra llamar a execve
}

void	close_files(int infile, int outfile)
{
	close(infile);
	close(outfile);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipe	pipex;

	if (argc == 5)
	{
		pipex.infile = open(argv[1], O_RDONLY);
		pipex.permission = access(argv[1], F_OK | R_OK);
		if (pipex.infile == -1 || pipex.permission == -1)
			return (-1);
		pipex.outfile = open(argv[argc - 1], O_TRUNC | O_CREAT, 0666);
		pipex.permission = access(argv[1], W_OK);
		if (pipex.outfile == -1 || pipex.permission == -1)
			return (-1);
		if (pipe(pipex.tube) == -1)
			return (-1);
		pipex.routes = ft_split(find_path(envp), ':');
		//hacer hijo con fork y llamar a función hijo en los dos casos con sus respectivos dup2
		close_files(pipex.infile, pipex.outfile);
	}
}
