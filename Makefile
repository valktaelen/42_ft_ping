NAME	:=	ft_ping

CC		:=	gcc
CFLAGS	:=	-Wall -Werror -Wextra # -fsanitize=address -g3

INC		:=	inc/includes.h

SRCS	:=	srcs/main.c		\
			srcs/pars.c		\
			srcs/utils.c	\
			srcs/socket.c	\
			srcs/ping.c	\
			srcs/print.c	\
			srcs/signal.c	\
			srcs/memory.c	\
			srcs/network.c	\

OBJS	:=	$(SRCS:.c=.o)

all:	$(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o:	%.c $(INC)
	${CC} ${CFLAGS} -c $< -o $@ 

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
