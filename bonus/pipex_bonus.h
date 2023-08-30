/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 18:58:01 by fcosta-f          #+#    #+#             */
/*   Updated: 2023/08/30 19:06:45 by fcosta-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../printf/ft_printf.h"
#include "../libft/libft.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

# define ERR_ARG	(int)1
# define ERR_MC		(int)2
# define ERR_CNF	(int)3
# define ERR_PERM	(int)4
# define ERR_NFD	(int)5
# define ERR_PERR	(int)10

typedef struct s_pipe
{
	int		infile;
	int		outfile;
	int		permission;
	char	**routes;
	pid_t	proc1;
	pid_t	proc2;
	int		tube[2];
	char	**cmd_args;
	char	*cmd;
}	t_pipe;
