#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int handle_response(char *response);
void print_success(void);
void print_failure(char *code);
void print_invalid(void);
void print_hint(char correct, char wrong_pos);
void print_conn_lost(void);
