/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 18:51:16 by eralonso          #+#    #+#             */
/*   Updated: 2023/09/03 13:21:00 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

char	*find_path(char **envp, int *found)
{
	int	i;

	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5))
		i++;
	if (!envp[i])
	{
		*found = 0;
		return (NULL);
	}
	*found = 1;
	return (envp[i] + 5);
}

void	close_pipes(t_pipe *pipex)
{
	close(pipex->tube[1]);
	close(pipex->tube[0]);
}

int	ft_error(int ext, int err, char *cmd)
{
	if (err == ERR_ARG)
		ft_printf(2, "bash: Invalid number of arguments\n");
	else if (err == ERR_MC)
		ft_printf(2, "bash: error trying to allocate memory\n");
	else if (err == ERR_CNF)
		ft_printf(2, "pipex: %s: command not found\n", cmd);
	else if (err == ERR_NFD)
		ft_printf(2, "pipex: %s: No such file or directory\n");
	else if (err == ERR_PERM)
		ft_printf(2, "pipex: %s: permission denied\n");
	else if (err == ERR_PERR)
		perror("bash: ");
	return (ext);
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
		{
			ft_error(1, ERR_MC, NULL);
			return (NULL);
		}
		free(tmp);
		if (access(cmdroute, F_OK | X_OK) == 0)
			return (cmdroute);
		free(cmdroute);
		++routes;
	}
	if (access(cmd, F_OK | X_OK) == 0 && ft_strchr(cmd, '/'))
		return (cmd);
	else
		ft_error(127, ERR_CNF, cmd);
	return (NULL);
}

void	first_child(t_pipe pipex, char **argv, char **envp)
{
	pipex.infile = open(argv[1], O_RDONLY);
	if (pipex.infile == -1)
	{
		close(pipex.tube[0]);
		close(pipex.tube[1]);
		exit(ft_error(1, ERR_NFD, argv[1]));
	}
	pipex.permission = access(argv[1], F_OK | R_OK);
	if (pipex.permission == -1)
	{
		close(pipex.tube[0]);
		close(pipex.tube[1]);
		exit(ft_error(1, ERR_PERM, argv[1]));
	}
	dup2(pipex.infile, STDIN_FILENO);
	dup2(pipex.tube[1], STDOUT_FILENO);
	// ft_printf(2, "1ST::::he entrau%i,%i\n", pipex.infile, pipex.tube[1]);
	close(pipex.tube[0]);
	close(pipex.tube[1]);
	close(pipex.infile);
	pipex.cmd_args = ft_split(argv[pipex.j], ' ');
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	if (!pipex.cmd)
		exit(127);
	else
	{
		// ft_printf(2, "\npipex.cmd %s pipex.cmd_args %s %s\n", pipex.cmd, pipex.cmd_args[0], pipex.cmd_args[1]);
		execve(pipex.cmd, pipex.cmd_args, envp);
	}
	exit(1);
}

void	second_child(t_pipe pipex, char **argv, int argc, char **envp)
{
	pipex.outfile = open(argv[argc - 1], O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (pipex.outfile == -1)
	{
		close(pipex.tube[1]);
		close(pipex.tube[0]);
		exit(ft_error(1, ERR_NFD, argv[argc - 1]));
	}
	pipex.permission = access(argv[argc - 1], W_OK);
	if (pipex.permission == -1)
	{
		close(pipex.tube[1]);
		close(pipex.tube[0]);
		exit(ft_error(1, ERR_PERM, argv[argc - 1]));
	}
	dup2(pipex.outfile, STDOUT_FILENO);
	close(pipex.outfile);
	close(pipex.tube[1]);
	close(pipex.tube[0]);
	pipex.cmd_args = ft_split(argv[pipex.j], ' '); //protege feo
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	if (!pipex.cmd)
		exit(127);
	else
	{
		// ft_printf(2, "\npipex.j: %i && pipex.cmd %s && pipex.cmd_args %s %s\n", pipex.j, md, pipex.cmd_args[0], pipex.cmd_args[1]);
		execve(pipex.cmd, pipex.cmd_args, envp);
	}
	exit(1);
}

void	child(t_pipe pipex, char **argv, int argc, char **envp)
{
	//ft_printf(2, "some child %d\n", pipex.proc);
	if (pipex.j == 2)
	{
		//dprintf(2, "hola?\n");
		first_child(pipex, argv, envp);
		//dprintf(2, "adios?\n");
	}
	else if (pipex.j == argc - 2)
		second_child(pipex, argv, argc, envp);
	else
	{
		dup2(pipex.tube[1], STDOUT_FILENO);
		close(pipex.tube[1]);
		close(pipex.tube[0]);
		pipex.cmd_args = ft_split(argv[pipex.j], ' '); //protege feo??
		pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
		if (!pipex.cmd)
			exit(127);
		else
		{
			//ft_printf(2, "\npipex.cmd %s pipex.cmd_args %s %s\n", pipex.cmd, pipex.cmd_args[0], pipex.cmd_args[1]);
			execve(pipex.cmd, pipex.cmd_args, envp); //no hace exit??
		}
	}
	exit(1);
}


int	main(int argc, char **argv, char **envp)
{
	t_pipe	pipex;
	int		found;
	int		status;
	int		exit_code;

	if (argc >= 5)
	{

		pipex.routes = ft_split(find_path(envp, &found), ':');
		if (!found)
			exit(1);
		if (!pipex.routes)
			exit(ft_error(1, ERR_MC, NULL));
		pipex.here_doc = 0;
		if (fta_strncmp(argv[2], "here_doc", 9))
			pipex.here_doc = 1;
		pipex.j = 2 + pipex.here_doc; //falta añadir heredoc next_line, pipe, fork
		while (pipex.j < argc - 1)
		{
			if (pipex.j < argc - 2 && pipe(pipex.tube))
				return (1);
			pipex.proc = fork();
			if (pipex.proc == 0)
			{
				// ft_printf(2, "some child %d\n", pipex.proc);
				child(pipex, argv, argc, envp);
			}
			if (pipex.j < argc - 2)
			{
				dup2(pipex.tube[0], STDIN_FILENO);
				close(pipex.tube[0]);
				close(pipex.tube[1]);
			}
			//ft_printf(2, "esta es la j1 %d \n", pipex.j);
			pipex.j++;
		}
		while (pipex.j > 2) //+flags
		{
			if (wait(&status) == pipex.proc)
				exit_code = status;
			//ft_printf(2, "esta es la j2 %d \n", pipex.j);
			pipex.j--;
		}
		if (WIFEXITED(exit_code))
			exit(WEXITSTATUS(exit_code));
		exit(1);
	}
	return (ft_error(1, ERR_ARG, NULL));
}
