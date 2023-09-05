/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 18:58:01 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/09/05 17:46:36 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include "../printf/ft_printf.h"
# include "../libft/libft.h"
# include "../get_next_line/get_next_line.h"
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>

# define ERR_ARG	1
# define ERR_MC		2
# define ERR_CNF	3
# define ERR_PERM	4
# define ERR_NFD	5
# define ERR_PERR	10

typedef struct s_pipe
{
	int		infile;
	int		outfile;
	int		permission;
	char	**routes;
	pid_t	proc;
	int		tube[2];
	char	**cmd_args;
	char	*cmd;
	int		j;
	int		here_doc;
	char	*limiter;
}	t_pipe;

void	init_pipex(t_pipe *pipex, char **argv, char **envp);
int		find_route(t_pipe *pipex, char **envp);
char	*find_path(char **envp, int *found);
char	*find_cmd(char **routes, char *cmd);
int		wait_forks(t_pipe *pipex);
void	last_pipe(t_pipe *pipex, int argc);
int		ft_error(int ext, int err, char *cmd);
void	close_pipes(t_pipe *pipex);

#endif