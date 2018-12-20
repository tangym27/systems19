#include "pipe_networking.h"

static void sighandler(int signo){
  // Catches the SIGUSR1 signal

	if (signo == SIGINT){
		//Before exiting, append a message to a file noting that the program exited due to SIGINT
		printf("Exited process %d due to SIGINT \n", getpid());
		remove("wkp");
	//	remove(client_pipe);
		// char message[] =  "The program ended due to SIGINT\n";
		// int w = write(fd, message, sizeof(message));
		// close(fd);
		exit(0);
	}
}
/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
		char message[1024];
   signal(SIGINT, sighandler);

  char buffer[HANDSHAKE_BUFFER_SIZE];
  if ( mkfifo("wkp", 0644) < 0 ) {
    printf("Error %d: %s\n", errno, strerror(errno));
    exit(0);
  }
  printf("Congrats! The server pipe was just created.\n");
	printf("Still waiting for a client/friend...\n" );

	int from_client = open("wkp", O_RDONLY);
  printf("Connecting to a client!\n");

  memset(buffer, 0, sizeof(buffer));
  read(from_client, buffer, sizeof(buffer));
  printf("Connecting to pipe %s (from client!)\n", buffer);
	remove("wkp");


  *to_client = open(buffer, O_WRONLY);
  strcpy(buffer, ACK);
  write(*to_client, buffer, sizeof(buffer));
  read(from_client, buffer, sizeof(buffer));
  printf("Houston. We have a connection.\n");

	while(1){
		read(from_client, message, sizeof(message));
		printf("Received message from the client %s", message);
		message[0] -= 1;
		printf("Sending back message to client %s", message);
		write(*to_client, message, sizeof(message));
	}
  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
	signal(SIGINT, sighandler);
  char buffer[HANDSHAKE_BUFFER_SIZE];
  char client_pipe[256];
  sprintf(client_pipe,"%d",getpid());
  strcpy(buffer,client_pipe);
  if ( mkfifo(client_pipe, 0644) < 0 ) {
    printf("Error %d: %s\n", errno, strerror(errno));
    exit(0);
  }
  printf("Congrats! A pipe was created! Name: %s\n", client_pipe);

  *to_server = open("wkp", O_WRONLY);

  write(*to_server, buffer, sizeof(buffer));
  printf("A connection was just established with the server!\n");

  int from_server = open(client_pipe, O_RDONLY);

  read(from_server, buffer, sizeof(buffer));
  printf("Server sent a message! (second handshake): %s\n", buffer);

  remove(client_pipe);
  strcpy(buffer, ACK);
  write(*to_server, buffer, sizeof(buffer));

	char message[1024];
	while(1){
		  printf("Enter data: ");
		  fgets(message, 1024, stdin);
		  printf("CLIENT: Sending message %s", message);
		  write(*to_server, message, sizeof(message));
		  read(from_server, message, 1024);
		  printf("CLIENT: received message %s", message);
	}

  return from_server;
}
