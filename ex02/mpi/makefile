# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aurele <aurele@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/19 00:27:39 by aurele            #+#    #+#              #
#    Updated: 2025/12/19 22:28:52 by aurele           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= mpi_ex02
CC			= mpicc
CFLAGS		?= -O3 -std=c11 -Wall -Wextra -Werror

SRCS		= main.c vector_utils.c mpi_core.c math_ops.c
OBJS		= $(SRCS:.c=.o)

N 		    ?= 4

# args: n lower diag upper display
SIZE		?= 50
LOWER		?= -2
DIAG		?= 1
UPPER		?= 2
DISPLAY		?= 0
ARGS		= $(SIZE) $(LOWER) $(DIAG) $(UPPER) $(DISPLAY)
.SILENT:
all: $(NAME)
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean all

#run that makefile with: make run N=4 and then clean the files with: make fclean
run: $(NAME)
	mpirun -np $(N) ./$(NAME) $(ARGS)
	$(MAKE) fclean

.PHONY: all clean fclean re run