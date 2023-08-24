# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fcosta-f <fcosta-f@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/24 13:05:07 by fcosta-f          #+#    #+#              #
#    Updated: 2023/08/24 13:56:24 by fcosta-f         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS	= pipex.c

OBJS	= $(SRCS:.c=.o)

DEPS	= $(OBJS:.o=.d)

CC			= cc
RM			= rm -f
CFLAGS		= -Wall -Wextra -Werror -g -MMD

NAME		= pipex
all:
	@$(MAKE) -C libft 
	@$(MAKE) $(NAME)
	

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) libft/libft.a $(OBJS) -o ${NAME}

-include $(DEPS)

clean:
	$(RM) $(OBJS) $(DEPS)

fclean: clean
		$(RM) $(NAME)
		@$(MAKE) -C libft fclean

re: fclean all

.PHONY: clean all fclean re
