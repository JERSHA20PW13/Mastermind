#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LETTERS "ABCDEF"

char *mastermind_get_random_code(void);
int mastermind_validate_guess(char *guess);
char *mastermind_check_guess(char *guess, char *secret_code);
