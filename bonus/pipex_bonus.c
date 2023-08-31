/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 18:51:16 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/31 19:19:57 by fcosta-f         ###   ########.fr       */
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
	pipex.cmd_args = ft_split(argv[2], ' ');
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	if (!pipex.cmd)
		exit(127);
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
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
	dup2(pipex.tube[0], STDIN_FILENO);
	dup2(pipex.outfile, STDOUT_FILENO);
	close(pipex.outfile);
	close(pipex.tube[1]);
	close(pipex.tube[0]);
	pipex.cmd_args = ft_split(argv[3], ' '); //protege feo
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	if (!pipex.cmd)
		exit(127);
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
}

void	child(t_pipe pipex, char **argv, int argc, char **envp)
{
	if (pipex.j == 2)
		first_child(pipex, argv, envp);
	if (pipex.j == argc - 2)
		second_child(pipex, argv, argc, envp);
	else
	{
		dup2(pipex.tube[0], STDIN_FILENO);
		close(pipex.tube[1]);
		close(pipex.tube[0]);
		pipex.cmd_args = ft_split(argv[2], ' ');
		pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
		if (!pipex.cmd)
			exit(127);
		else
			execve(pipex.cmd, pipex.cmd_args, envp);
	}
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
		pipex.j = 2; //+ flag de here_dock más adelante xd aaaa
		while (pipex.j < argc)
		{
			if (pipex.j < argc - 1 && pipe(pipex.tube))
				return (1);
			pipex.proc = fork();
			if (pipex.proc == 0)
				child(pipex, argv, argc, envp);
			dup2(pipex.tube[0], STDIN_FILENO);
			close(pipex.tube[0]);
			close(pipex.tube[1]);
		}
		while (pipex.j > 2) //+flags
		{
			if (waitpid(-1, &status, 0) == pipex.proc)
				exit_code = status;
			pipex.j--;
		}
		if (WIFEXITED(exit_code))
			exit(WEXITSTATUS(exit_code));
		exit(1);
	}
	return (ft_error(1, ERR_ARG, NULL));
}
