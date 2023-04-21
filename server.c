#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#include "socket.h"

#define BUFFER_LEN 256
#define MAX_CLIENTS 100

int numClients = 0;



struct thread_args{
  FILE* from;
  FILE* to;
  int socket_fd;
};

void* threadFunc(void* args){
    int client_socket_fd = *(int*) args;

    FILE* to_client = fdopen(dup(client_socket_fd), "wb");
    if(to_client == NULL) {
      perror("Failed to open stream to client");
      exit(2);
    }
    
    FILE* from_client = fdopen(dup(client_socket_fd), "rb");
    if(from_client == NULL) {
      perror("Failed to open stream from client");
      exit(2);
    }

  struct thread_args* myArg = (struct thread_args*) args;
  int going = 1;
  while(going){
    // Receive a message from the client
    char buffer[BUFFER_LEN];
    printf("TID: %d From: %p, To: %p, SocketFD: %d\n", (int) pthread_self(), from_client, to_client, client_socket_fd);

    if(fgets(buffer, BUFFER_LEN, from_client) == NULL) {
      perror("Reading from client failed");
      exit(2);
    }

    if(strcmp(buffer, "quit\n") == 0){
      going = 0;
      break;
    } 

    printf("Client sent: %s\n", buffer);
    for(int i = 0; i < BUFFER_LEN; ++i){
      buffer[i] = toupper(buffer[i]);
    }
    printf("Trying to send back to %p: %s\n", to_client, buffer);

    fprintf(to_client, "%s", buffer);
    fflush(to_client);
  }
  fclose(to_client);
  fclose(from_client);
  //  close(myArg->socket_fd);
  printf("Exiting thread\n");

  return NULL;
}

int main() {
  // Open a server socket
  unsigned short port = 0;
  int server_socket_fd = server_socket_open(&port);
  if(server_socket_fd == -1) {
    perror("Server socket was not opened");
    exit(2);
  }
  
  pthread_t threads[MAX_CLIENTS];
  // Start listening for connections, with a maximum of one queued connection
  if(listen(server_socket_fd, MAX_CLIENTS)) {
      perror("listen failed");
      exit(2);
    }
  while(1){
    
    
    printf("Server listening on port %u\n", port);
    
    // Wait for a client to connect
    int client_socket_fd = server_socket_accept(server_socket_fd);
    if(client_socket_fd == -1) {
      perror("accept failed");
      exit(2);
    }
    
    printf("Client connected!\n");
    
    // Set up file streams to access the socket

    if(numClients == MAX_CLIENTS){
      printf("too many clinets, please try again later...");
      continue;
    }
    pthread_create(&threads[numClients++], 0, threadFunc, (void*) &(client_socket_fd));
    printf("Num clients: %d\n", numClients);
  }  
  
  close(server_socket_fd);
  
  return 0;
}





















