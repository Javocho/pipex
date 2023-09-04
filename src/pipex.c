/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 19:36:35 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/09/04 20:37:11 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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
//NO CAL PERÒ ACONSELLABLE??

void	first_child(t_pipe pipex, char **argv, char **envp)
{
	pipex.infile = open(argv[1], O_RDONLY);
	if (pipex.infile == -1)
	{
		close_pipes(&pipex);
		exit(ft_error(1, ERR_NFD, argv[1]));
	}
	pipex.permission = access(argv[1], F_OK | R_OK);
	if (pipex.permission == -1)
	{
		close_pipes(&pipex);
		exit(ft_error(1, ERR_PERM, argv[1]));
	}
	dup2(pipex.infile, STDIN_FILENO);
	dup2(pipex.tube[1], STDOUT_FILENO);
	close_pipes(&pipex);
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
		close_pipes(&pipex);
		exit(ft_error(1, ERR_NFD, argv[argc - 1]));
	}
	pipex.permission = access(argv[argc - 1], W_OK);
	if (pipex.permission == -1)
	{
		close_pipes(&pipex);
		exit(ft_error(1, ERR_PERM, argv[argc - 1]));
	}
	dup2(pipex.tube[0], STDIN_FILENO);
	dup2(pipex.outfile, STDOUT_FILENO);
	close(pipex.outfile);
	close_pipes(&pipex);
	pipex.cmd_args = ft_split(argv[3], ' ');
	if (!pipex.cmd_args)
		return ;
	pipex.cmd = find_cmd(pipex.routes, pipex.cmd_args[0]);
	if (!pipex.cmd)
		exit(127);
	else
		execve(pipex.cmd, pipex.cmd_args, envp);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipe	pipex;
	int		status;

	if (argc == 5)
	{
		if (pipe(pipex.tube) == 1)
			exit (1);
		if (find_route(&pipex, envp) == 1)
			exit(1);
		pipex.proc1 = fork();
		if (pipex.proc1 == 0)
			first_child(pipex, argv, envp);
		pipex.proc2 = fork();
		if (pipex.proc2 == 0)
			second_child(pipex, argv, argc, envp);
		close_pipes(&pipex);
		waitpid(pipex.proc1, NULL, 0);
		waitpid(pipex.proc2, &status, 0);
		if (WIFEXITED(status))
			exit(WEXITSTATUS(status));
		exit(1);
	}
	return (ft_error(1, ERR_ARG, NULL));
}
