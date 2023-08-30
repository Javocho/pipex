/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 19:36:35 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/30 18:57:39 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*find_path(char **envp, int *found)
{
	int	i;

	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5)) //tendría que haber arreglado error unset path... he modificado hasta el split
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
	close(pipex->tube[0]);
	close(pipex->tube[1]);
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

// void	close_files(int infile, int outfile)
// {
// 	close(infile);
// 	close(outfile);
// }

// void	ft_free(t_pipe *pipex)
// {
// 	char	**tmp_args;
// 	char	*tmp;

// 	tmp_args = pipex->cmd_args;
// 	while (*tmp_args)
// 	{
// 		tmp = *tmp_args;
// 		tmp_args++;
// 		free(tmp);
// 	}
// 	//free(pipex->cmd_args);
// 	close(pipex->tube[0]);
// 	close(pipex->tube[1]);
// }

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
		++routes; //o es ++(*routes)????
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


int	main(int argc, char **argv, char **envp)
{
	t_pipe	pipex;
	int		found;
	int		status;

	if (argc == 5)
	{
		if (pipe(pipex.tube) == 1)
			return (1);
		pipex.routes = ft_split(find_path(envp, &found), ':');
		if (!found)
			exit(1);
		if (!pipex.routes)
			exit(ft_error(1, ERR_MC, NULL));
		pipex.proc1 = fork();
		if (pipex.proc1 == 0)
			first_child(pipex, argv, envp);
		pipex.proc2 = fork();
		if (pipex.proc2 == 0)
			second_child(pipex, argv, argc, envp);
		close(pipex.tube[0]);
		close(pipex.tube[1]);
		waitpid(pipex.proc1, NULL, 0);
		waitpid(pipex.proc2, &status, 0);
		if (WIFEXITED(status))
			exit(WEXITSTATUS(status));
		exit(1);
	}
	return (ft_error(1, ERR_ARG, NULL));
}
