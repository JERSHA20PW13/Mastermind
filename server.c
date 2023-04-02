#include "server.h"
#include "queue.c"
#include "mastermind.c"


static node *threads;
static int connection_count = 0;


thread_info *create_thread_info(int connfd, struct sockaddr_in cli_addr, socklen_t clilen, char *secret_code) {

        thread_info *t;
        t = (thread_info *)malloc(sizeof(thread_info));

        t->sockfd = connfd;
        t->cli_addr = cli_addr;
        t->clilen = clilen;
        t->guess_count = 0;
        t->running = 0;
        t->completed = 0;
        t->won = 0;

        if (secret_code == NULL) {

                char *temp = mastermind_get_random_code();
                t->secret_code = strdup(temp);
                free(temp);

        } else {

                t->secret_code = strdup(secret_code);
        }

        return t;

}


void * game_handler(void *param) {

        thread_info *t = (thread_info *)param;

        int n;
        char buffer[1024], response[1024];

        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));

        print_welcome_to(buffer);

        n = write(t->sockfd, buffer, sizeof(buffer) - 1);

        if (n < 0) {

                close(t->sockfd);
                pthread_exit(NULL);
        }

         while (!t->completed) {

                n = read(t->sockfd, buffer, sizeof(buffer) - 1);

                if (n < 0)      {

                        close(t->sockfd);
                        pthread_exit(NULL);
                }

                if (n > 0) {

                        t->guess_count++;

                        char *pos;
                        if ((pos = strchr(buffer, '\n')) != NULL) {
                                *pos = '\0';
                        }

                        if (mastermind_validate_guess(buffer)) {

                                char *temp = mastermind_check_guess(buffer, t->secret_code);
                                strncpy(response, temp, strlen(temp));
                                free(temp);

                                memset(buffer, 0, sizeof(buffer)); // clear buffer

                                if (strcmp(response, "SUCCESS") == 0) {

                                        snprintf(buffer, sizeof(buffer), "SUCCESS");
                                        t->completed = 1;
                                        t->won = 1;

                                } else {

                                        snprintf(buffer, sizeof(buffer), "%s", response);

                                }

                        } else {

                                snprintf(buffer, sizeof(buffer), "INVALID");

                        }

                        if (!t->won && t->guess_count == MAX_GUESSES) {

                                snprintf(buffer, sizeof(buffer), "FAILURE:%s", t->secret_code);
                                t->completed = 1;
                        }

                        n = write(t->sockfd, buffer, sizeof(buffer) - 1);

                        if (n < 0) {

                                close(t->sockfd);
                                pthread_exit(NULL);
                        }

                        memset(buffer, 0, sizeof(buffer));
                        memset(response, 0, sizeof(response));
                }
        }

        close(t->sockfd);
        t->running = 0;

        return 0;
}


void print_welcome_to(char *buffer) {

        strcat(buffer, "----------------------------------------------------------------\n");
        strcat(buffer, "  Welcome to our masterfully implemented Mastermind server!  \n");
        strcat(buffer, "\n");
        strcat(buffer, " RULES:\n");
        strcat(buffer, " The aim of the game is to guess a SECRET CODE!\n");
        strcat(buffer, " The code is four of the following characters: A B C D E F\n");
        strcat(buffer, " Duplicates are possible!\n");
        strcat(buffer, " You will have 10 guesses at the code before it is game over.\n");
        strcat(buffer, " Send your guesses in the form ABCD.\n");
        strcat(buffer, "\n");
        strcat(buffer, " Good luck!\n");
        strcat(buffer, "----------------------------------------------------------------\n");

}


int main(int argc, char *argv[]) {

  	int portno = -1;
  	char *secret_code;

	if (argc != 2 && argc != 3) {
    		fprintf(stderr,  "Usage: %s [port_number] [default_secret_code]\n", argv[0]);
    		exit(EXIT_FAILURE);
  	}

  	portno = atoi(argv[1]);

	if (portno == -1) {
		fprintf(stderr, "ERRPR: [port_number] invalid.\n");
    		exit(EXIT_FAILURE);
  	}

  	if (argc == 3) {

    		if (mastermind_validate_guess(argv[2])) {
      			secret_code = strdup(argv[2]);
    		} else {
      			fprintf(stderr, "ERROR: [default_secret_code] must be four chars from { A B C D E F }.\n");
      			exit(EXIT_FAILURE);
    		}

  	} else {

    		srand(time(NULL));
   		secret_code = NULL;
  	}

  	fprintf(stdout, "Server running...\n");

  	int listenfd;
  	struct sockaddr_in serv_addr;

  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  	serv_addr.sin_port = htons(portno);

 	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	if (listenfd < 0) {
		fprintf(stderr, "ERROR: could not create TCP socket. Shutting down.\n");
    		exit(EXIT_FAILURE);
  	}

  	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    		fprintf(stderr, "ERROR: could not bind socket to port %d. Shutting down.\n", portno);
    		exit(EXIT_FAILURE);
  	}

  	listen(listenfd, MAX_CONNECTIONS);


  	int connfd;
  	struct sockaddr_in cli_addr;
  	socklen_t clilen;
  	clilen = sizeof(cli_addr);

  	while ((connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen))) {

		if (connfd < 0) {
      			exit(EXIT_FAILURE);
    		}


    		thread_info *t;
    		t = create_thread_info(connfd, cli_addr, clilen, secret_code);
		threads = queue_add(threads, t);

    		if (pthread_create(&t->tid, NULL, game_handler, (void *)t) < 0) {

      			exit(EXIT_FAILURE);

    		} else {

			t->running = 1;
      			connection_count++;
    		}
  	}

  	return 0;

}
