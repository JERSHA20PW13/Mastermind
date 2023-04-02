#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <sys/resource.h>


#define MAX_CONNECTIONS 1
#define MAX_GUESSES 10


typedef struct thread_info {

	pthread_t tid;
    	int sockfd;
    	struct sockaddr_in cli_addr;
    	socklen_t clilen;
    	int guess_count;
    	int running;
  	int completed;
    	int won;
    	char *secret_code;

} thread_info;


thread_info *create_thread_info(int connfd, struct sockaddr_in cli_addr, socklen_t clilen, char *secret_code);
void interrupt_handler(int signal);
void *game_handler(void *param);
void print_welcome_to(char *buffer);

