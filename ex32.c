// Yuval Mor - 205380173

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define SYSTEM_CALL_FAILURE (-1)
#define ERROR (-1)
#define INITIAL_VALUE 0
#define IDENTICAL 0
#define FILE_PLACE 1
#define NUMBER_OF_ELEMENTS 2
#define NUMBER_OF_LINES 3
#define MAX_CHAR_IN_LINE 150
#define CURRENT_DIR "."
#define PREVIOUS_DIR ".."
#define INPUT_ERROR "invalid input\n"
#define ERROR_MESSAGE "Error in system call\n"
#define ENTER "\n"
#define NEXT_PATH "/"
#define LESS_ONE(X) (X-1)
#define C_LOC(X) (X-1)
#define DOT_LOC(X) (X-2)
#define C_FILE 'c'
#define DOT '.'

/*
typedef struct{
    char* name;
    char* path;
}Student;
*/
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
    if(line!=NULL){
        strcpy(directory,line);
        line=strtok(NULL,ENTER);
        if(line!=NULL){
            strcpy(input,line);
            line=strtok(NULL,ENTER);
            if(line!=NULL){
                strcpy(output,line);
            }
            return;
        }
    }
}


bool IsCFile(char* fileName){
    size_t length = strlen(fileName);
    // Checks if the suffix of the file coordinate c files
    if(fileName[DOT_LOC(length)]==DOT && fileName[C_LOC(length)]==C_FILE){
        return true;
    }
    return false;
}


void searchFileInDirectory(char* directoryPath, char* filePath){
    char path [MAX_CHAR_IN_LINE]={};
    // Pointer for directory entry
    struct dirent* pDirent;
    DIR* pDir = opendir(directoryPath);
    if(!pDir){
        return;
    }
    while((pDirent=readdir(pDir))!= NULL){
        // Checks that its not the previous or current directory
        if(strcmp(pDirent->d_name,CURRENT_DIR)!=IDENTICAL &&
        strcmp(pDirent->d_name,PREVIOUS_DIR)!=IDENTICAL){
            strcpy(path,directoryPath);
            strcat(path,NEXT_PATH);
            strcat(path,pDirent->d_name);
            // Checks if the type is regular file
            if(pDirent->d_type==DT_REG){
                if(IsCFile(pDirent->d_name)){
                    strcpy(filePath,path);
                }
            }
            searchFileInDirectory(path,filePath);
        }
    }
    closedir(pDir);
}




int main(int argc, char* argv[]) {
    if(argc!= NUMBER_OF_ELEMENTS){
        printf(INPUT_ERROR);
        return ERROR;
    }
    char* configPath = argv[FILE_PLACE];
    char directory[MAX_CHAR_IN_LINE] = {};
    char input[MAX_CHAR_IN_LINE] = {};
    char output[MAX_CHAR_IN_LINE] = {};

    getPaths(configPath,directory,input,output);

    struct dirent* pDirent;
    DIR* pDir = opendir(directory);
    if(!pDir){
        EndProgram();
    }

    while ((pDirent=readdir(pDir))!= NULL){
        if(strcmp(pDirent->d_name,CURRENT_DIR)!=IDENTICAL &&
           strcmp(pDirent->d_name,PREVIOUS_DIR)!=IDENTICAL){
            char directoryPath[MAX_CHAR_IN_LINE]={};
            strcpy(directoryPath,directory);
            char studentName[MAX_CHAR_IN_LINE]={};
            strcpy(studentName,pDirent->d_name);

            char studentCFilePath[MAX_CHAR_IN_LINE]={};
            strcat(directoryPath,NEXT_PATH);
            strcat(directoryPath,pDirent->d_name);

            searchFileInDirectory(directoryPath,studentCFilePath);

            printf("name =  %s\n",studentName);
            printf("path = %s\n",studentCFilePath);
            printf("length = %d\n",(int)strlen(studentCFilePath));

        }
    }
    closedir(pDir);
    return 0;
}
