# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/24 13:05:07 by fcosta-f          #+#    #+#              #
#    Updated: 2023/09/01 20:28:56 by fcosta-f         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS	= src/pipex.c

SRCS_BONUS	= bonus/pipex_bonus.c

OBJS	= $(SRCS:.c=.o)

OBJS_BONUS	= $(SRCS_BONUS:.c=.o)

DEPS	= $(OBJS:.o=.d)

DEPS_BONUS	= $(OBJS_BONUS:.o=.d)

CC			= cc
RM			= rm -f
CFLAGS		= -Wall -Wextra -Werror -g -MMD #-fsanitize=address

NAME		= pipex

NAME_BONUS	= pipex_bonus

all:
	@$(MAKE) -C libft
	@$(MAKE) -C printf  
	@$(MAKE) $(NAME)
	
bonus:
	@$(MAKE) -C libft
	@$(MAKE) -C printf  
	@$(MAKE) $(NAME_BONUS)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) printf/libftprintf.a libft/libft.a $(OBJS) -o ${NAME}

$(NAME_BONUS): $(OBJS_BONUS)
	$(CC) $(CFLAGS) printf/libftprintf.a libft/libft.a $(OBJS_BONUS) -o ${NAME_BONUS}

-include $(DEPS)
-include $(DEPS_BONUS)

clean:
	$(RM) $(OBJS) $(DEPS)
	$(RM) $(OBJS_BONUS) $(DEPS_BONUS)

fclean: clean
		$(RM) $(NAME)
		@$(MAKE) -C libft fclean
		@$(MAKE) -C printf fclean

re: fclean all

rebonus: fclean bonus

.PHONY: clean all bonus fclean re

#MIRAR 10.5.2 Pattern Rule Examples