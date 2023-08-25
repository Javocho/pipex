/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 19:36:35 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/25 21:15:51 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*find_path(char **envp)
{
	while (ft_strncmp(*envp, "PATH=", 5))
		envp++;
	return (*envp + 5);
}

char *	find_cmd(char **routes, char *cmd)
{
	char	*tmp;
	char	*cmdroute;

	while(*routes)
	{
		tmp = ft_strjoin(*routes, '/');
		cmdroute = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(tmp, F_OK | X_OK) == 0)
			return (cmdroute);
		free(cmdroute);
		return (NULL);
	}
}

void	first_child(t_pipe pipex, char **argv, char **envp)
{
	char	*cmd;

	dup2(pipex.infile, 0);
	dup2(pipex.tube[1], 1);
	close(pipex.tube[0]);
	pipex.cmd_args = ft_split(argv[2], ' ');
	pipex.cmd = find_cmd(pipex.routes, argv[1]);
	if (!pipex.cmd)
		return ; //tendría que liberar cositas
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
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
		pipex.proc1 = fork();
		if (pipex.proc1 == 0)
			first_child(pipex, argv, envp);
		pipex.proc2 = fork();
		//if (pipex.proc2 == 0)
			//second_child();
		//hacer hijo con fork y llamar a función hijo en los dos casos con sus respectivos dup2
		//mirar mejor fork y pipe
		close_files(pipex.infile, pipex.outfile);
	}
}
