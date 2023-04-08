NAME	:=	ft_ping

CC		:=	gcc
CFLAGS	:=	-Wall -Werror -Wextra

SRCS	:=	srcs/main.c		\
			srcs/pars.c		\
			srcs/utils.c	\

OBJS	:=	$(SRCS:.c=.o)

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)
