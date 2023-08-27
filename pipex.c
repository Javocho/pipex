/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 19:36:35 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/27 20:35:10 by fcosta-f         ###   ########.fr       */
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
		//dprintf(2, "route %s\n", tmp);
		cmdroute = ft_strjoin(tmp, cmd);
		//dprintf(2, "cmdroute %s\n", cmdroute);
		if (!cmdroute)
			return (NULL);
		free(tmp);
		if (access(cmdroute, F_OK | X_OK) == 0)
			return (cmdroute);
		free(cmdroute);
		++routes;
	}
	return (NULL);
}

void	first_child(t_pipe pipex, char **argv, char **envp)
{
	//mejor abrir en hijos uwu
	dup2(pipex.infile, STDIN_FILENO);
	dup2(pipex.tube[1], STDOUT_FILENO);
	// ft_printf(2, "1ST::::he entrau%i,%i\n", pipex.infile, pipex.tube[1]);
	close(pipex.tube[0]);
	close(pipex.tube[1]);
	close(pipex.outfile);
	close(pipex.infile);
	pipex.cmd_args = ft_split(argv[2], ' ');
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	if (!pipex.cmd)
	{
		ft_free(&pipex);
		close_files(pipex.infile, pipex.outfile);
		exit(0);
	}
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
}

void	second_child(t_pipe pipex, char **argv, char **envp)
{
	dup2(pipex.tube[0], STDIN_FILENO);
	dup2(pipex.outfile, STDOUT_FILENO);
	// ft_printf(2, "he entrau%i,%i\n", pipex.outfile, pipex.tube[0]);
	close(pipex.tube[1]);
	close(pipex.tube[0]);
	close(pipex.infile);
	close(pipex.outfile);
	// printf("hola");
	pipex.cmd_args = ft_split(argv[3], ' ');
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	//dprintf(2, "\n%s\n", pipex.cmd);
	if (!pipex.cmd)
	{
		ft_free(&pipex);
		close_files(pipex.infile, pipex.outfile);
		exit(0);
	}
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
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
		pipex.outfile = open(argv[argc - 1], O_WRONLY | O_TRUNC | O_CREAT, 0666);
		pipex.permission = access(argv[1], W_OK);
		if (pipex.outfile == -1 || pipex.permission == -1)
			return (-1); //close?
		if (pipe(pipex.tube) == -1)
			return (-1); //close??
		pipex.routes = ft_split(find_path(envp), ':');
		pipex.proc1 = fork();
		//printf("%d\n", pipex.proc1);
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
