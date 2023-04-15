NAME	:=	ft_ping

CC		:=	gcc
CFLAGS	:=	-Wall -Werror -Wextra -fsanitize=address -g3

SRCS	:=	srcs/main.c		\
			srcs/pars.c		\
			srcs/utils.c	\
			srcs/socket.c	\
			srcs/ping.c	\
			srcs/print.c	\
			srcs/signal.c	\

OBJS	:=	$(SRCS:.c=.o)

all:	$(NAME)

$(NAME):	inc/includes.h $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o:	%.c inc/includes.h
	${CC} ${CFLAGS} -c $< -o $@ 

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
