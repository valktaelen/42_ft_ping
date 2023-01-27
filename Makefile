NAME	:=	ft_ping

CC		:=	gcc
CFLAGS	:=	-Wall -Werror -Wextra

SRCS	:=	srcs/main.c		\
			srcs/display.c	\
			srcs/ping.c		\
			srcs/dns.c		
OBJS	:=	$(SRCS:.c=.o)

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)
