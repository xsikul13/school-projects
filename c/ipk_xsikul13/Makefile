SERVER          = server
CLIENT          = client
SERVER_SOURCES  = server.c
CLIENT_SOURCES  = client.c

#CFLAGS         = -g
CC              = gcc

all: $(SERVER) $(CLIENT)

SERVER_OBJECTS  = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJECTS  = $(CLIENT_SOURCES:.c=.o)

$(SERVER):	$(SERVER_OBJECTS)
		$(CC) $(SERVER_OBJECTS) -o $@

$(CLIENT):	$(CLIENT_OBJECTS)
		$(CC) $(CLIENT_OBJECTS) -o $@
    
    
clean:
	rm -fr core* *~ $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(SERVER) $(CLIENT) .make.state .sb