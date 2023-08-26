/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 19:36:35 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/26 13:39:47 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*find_path(char **envp)
{
	//tiene que existir
	while (ft_strncmp(*envp, "PATH=", 5))
		envp++;
	return (*envp + 5);
}

void	close_files(int infile, int outfile)
{
	close(infile);
	close(outfile);
}

void	ft_free(t_pipe *pipex)
{
	char	**tmp_args;
	char	*tmp;

	tmp_args = pipex->cmd_args;
	while (*tmp_args)
	{
		tmp = *tmp_args;
		tmp_args++;
		free(tmp);
	}
	free(pipex->cmd_args);
}

char	*find_cmd(char **routes, char *cmd)
{
	char	*tmp;
	char	*cmdroute;

	while (*routes)
	{
		tmp = ft_strjoin(*routes, "/");
		cmdroute = ft_strjoin(tmp, cmd);
		if (!cmdroute)
			return (NULL);
		free(tmp);
		if (access(tmp, F_OK | X_OK) == 0)
			return (cmdroute);
		free(cmdroute);
		++routes;
	}
	return (NULL);
}

void	first_child(t_pipe pipex, char **argv, char **envp)
{
	//mejor abrir en hijos uwu
	dup2(pipex.infile, 0);
	dup2(pipex.tube[1], 1);
	close(pipex.tube[0]);
	close(pipex.tube[1]);
	close(pipex.outfile);
	close(pipex.infile);
	pipex.cmd_args = ft_split(argv[2], ' ');
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	dprintf(2, "cmd: %s\n", pipex.cmd);
	if (!pipex.cmd)
	{
		ft_free(&pipex);
		close_files(pipex.infile, pipex.outfile);
		exit(0);
	}
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
	exit(1);
}

void	second_child(t_pipe pipex, char **argv, char **envp)
{
	dup2(pipex.tube[0], 0);
	dup2(pipex.outfile, 1);
	close(pipex.tube[1]);
	close(pipex.tube[0]);
	close(pipex.infile);
	close(pipex.outfile);
	pipex.cmd_args = ft_split(argv[3], ' ');
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	if (!pipex.cmd)
	{
		ft_free(&pipex);
		close_files(pipex.infile, pipex.outfile);
		exit(0);
	}
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
	exit(1);
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
			return (-1); //close?
		if (pipe(pipex.tube) == -1)
			return (-1); //close??
		pipex.routes = ft_split(find_path(envp), ':');
		pipex.proc1 = fork();
		printf("%d\n", pipex.proc1);
		if (pipex.proc1 == 0)
			first_child(pipex, argv, envp);
		pipex.proc2 = fork();
		if (pipex.proc2 == 0)
			second_child(pipex, argv, envp);
		close_files(pipex.infile, pipex.outfile);
		close(pipex.tube[0]);
		close(pipex.tube[1]);
		waitpid(pipex.proc1, NULL, 0);
		waitpid(pipex.proc2, NULL, 0);
		//ft_free(&pipex);
	}
}
