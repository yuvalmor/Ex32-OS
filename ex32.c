// Yuval Mor - 205380173

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define SYSTEM_CALL_FAILURE (-1)
#define ERROR (-1)
#define INITIAL_VALUE 0
#define FILE_PLACE 1
#define NUMBER_OF_ELEMENTS 2
#define NUMBER_OF_LINES 3
#define MAX_CHAR_IN_LINE 150
#define INPUT_ERROR "invalid input\n"
#define ERROR_MESSAGE "Error in system call\n"
#define ENTER "\n"
#define LESS_ONE(X) (X-1)
#define PLUS_ONE(X) (X+1)

/**
 *
 */
void EndProgram(){
    write(STDERR_FILENO,ERROR_MESSAGE,LESS_ONE(sizeof(ERROR_MESSAGE)));
    exit(ERROR);
}

/**
 *
 */
void getPaths(const char* path, char* directory,char* input,char* output){
    char buffer [MAX_CHAR_IN_LINE*NUMBER_OF_LINES];
    int fd = open(path,O_RDONLY);
    if(fd<INITIAL_VALUE){
        EndProgram();
    }
    if(read(fd,buffer,MAX_CHAR_IN_LINE*NUMBER_OF_LINES)==SYSTEM_CALL_FAILURE) {
        close(fd);
        EndProgram();
    }
    char* line = strtok(buffer,ENTER);
    strcpy(directory,line);
    line=strtok(NULL,ENTER);
    strcpy(input,line);
    line=strtok(NULL,ENTER);
    strcpy(output,line);
}


int main(int argc, char* argv[]) {
    if(argc!= NUMBER_OF_ELEMENTS){
        printf(INPUT_ERROR);
        return ERROR;
    }
    char* path = argv[FILE_PLACE];
    char directory[MAX_CHAR_IN_LINE] = {};
    char input[MAX_CHAR_IN_LINE] = {};
    char output[MAX_CHAR_IN_LINE] = {};
    getPaths(path,directory,input,output);
    return 0;
}
