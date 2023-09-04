/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 18:24:03 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/09/04 20:37:16 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	find_route(t_pipe *pipex, char **envp)
{
	int	found;

	pipex->routes = ft_split(find_path(envp, &found), ':');
	if (!found)
		return (1);
	if (!pipex->routes)
		return (ft_error(1, ERR_MC, NULL));
	return (0);
}

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

char	*find_cmd(char **routes, char *cmd)
{
	char	*tmp;
	char	*cmdroute;

	dprintf(2, "hola\n");
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

void	close_pipes(t_pipe *pipex)
{
	close(pipex->tube[0]);
	close(pipex->tube[1]);
}
