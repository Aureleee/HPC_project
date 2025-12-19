NAME		= openmp_ex02

# macOS: clang + libomp usually
CC		?= clang
CFLAGS	?= -O3 -std=c11 -Wall -Wextra -Werror

# If you use gcc: make CC=gcc OMPFLAGS=-fopenmp LDFLAGS=-fopenmp
# macOS needs libomp headers (omp.h). try Homebrew default paths.
LIBOMP_PREFIX	?= $(firstword $(wildcard /opt/homebrew/opt/libomp /usr/local/opt/libomp))

ifeq ($(LIBOMP_PREFIX),)
OMPFLAGS	?= -Xpreprocessor -fopenmp
LDFLAGS	?= -lomp
else
OMPFLAGS	?= -Xpreprocessor -fopenmp -I$(LIBOMP_PREFIX)/include
LDFLAGS	?= -L$(LIBOMP_PREFIX)/lib -lomp
endif

SRCS		= main.c vector_utils.c math_ops.c omp_core.c
OBJS		= $(SRCS:.c=.o)

# args: n lower diag upper threads display
SIZE		?= 50
LOWER		?= -2
DIAG		?= 1
UPPER		?= 2
THREADS		?= 4
DISPLAY		?= 0
ARGS		= $(SIZE) $(LOWER) $(DIAG) $(UPPER) $(THREADS) $(DISPLAY)

.SILENT:

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OMPFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

%.o: %.c header.h
	$(CC) $(CFLAGS) $(OMPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME) $(ARGS)
	$(MAKE) fclean

.PHONY: all clean fclean re run
