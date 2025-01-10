NAME1 = server
NAME2 = client
CC = cc

CLIENT_SRCS = client.c
SERVER_SRCS = server.c
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

# NAME1: $(SERVER_OBJS)
# 	$(CC) $(SERVER_SRCS) -o $(NAME1)

# NAME2: $(CLIENT_OBJS)
# 	$(CC) $(CLIENT_SRCS) -o $(NAME2)

all:
	$(CC) $(CLIENT_SRCS) -o $(NAME2)
	$(CC) $(SERVER_SRCS) -o $(NAME1)

re: all
	rm -f $(CLIENT_OBJS)
	rm -f $(SERVER_OBJS)
	

clean:
	rm -f $(CLIENT_OBJS)
	rm -f $(SERVER_OBJS)

fclean: clean
	rm -f $(NAME1)
	rm -f $(NAME2)

.PHONY: re clean fclean all