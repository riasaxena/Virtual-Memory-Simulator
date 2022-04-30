#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
int fifo = 0, lru = 0;
void parseline (char prompt[100], char* argv[3]) {

    char * str = strtok(prompt, " ");
    int i=0;
    while(str != NULL){
        argv[i] = str;
        str = strtok(NULL, " ");
        i++;
    }
}
void loop() {
    char input[100];
    char* args[3];
    do {
        printf("> ");
        fgets(input, 80, stdin);
        input[strlen(input)-1] = '\0';
        parseline(input, args);
        if (strcmp(args[0], "quit") == 0){
            exit(0);
        }
        //execute(args);
    } while (1);
}
int main(int argc, char** argv) {
    if (argv[1] == NULL || strcmp (argv[1], "FIFO") == 0)
        fifo = 1;
    else if (strcmp (argv[1], "LRU") == 0)
        lru = 1;
    loop();
    return 0;
}