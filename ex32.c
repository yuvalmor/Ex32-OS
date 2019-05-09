// Yuval Mor - 205380173

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <wait.h>
#include <errno.h>

#define SYSTEM_CALL_FAILURE (-1)
#define ERROR (-1)
#define INITIAL_VALUE 0
#define SUBTRACTION 0
#define EMPTY_PATH 0
#define CHILD_PID 0
#define STANDARD_INPUT 0
#define STANDARD_OUTPUT 1
#define FILE_COMPILED 1
#define FILE_PLACE 1
#define NUMBER_OF_ELEMENTS 2
#define NUMBER_OF_LINES 3
#define MAX_CHAR_IN_LINE 150
#define PERMISSION 0666
#define C_FILE 'c'
#define DOT '.'
#define CURRENT_DIR "."
#define PREVIOUS_DIR ".."
#define INPUT_ERROR "invalid input\n"
#define ERROR_MESSAGE "Error in system call\n"
#define ENTER "\n"
#define NEXT_PATH "/"
#define COMMA ","
#define GCC_COMMAND "gcc"
#define GCC_FLAG "-o"
#define COMPILED_FILE_NAME "program.out"
#define EXECUTE_COMPILED_FILE "./program.out"
#define EXECUTE_COMPILED_COMPARE_PROGRAM "./comp.out"
#define OUTPUT_FILE "programOutput.txt"
#define RESULT_FILE "results.csv"

#define NO_C_FILE 0
#define IDENTICAL 1
#define DIFFERENT 2
#define SAME 3
#define COMPILATION_ERROR 4
#define TIMEOUT 5

#define NO_C_GRADE "0"
#define COMPILATION_GRADE "20"
#define TIMEOUT_GRADE "40"
#define DIFFERENT_GRADE "60"
#define SAME_GRADE "80"
#define IDENTICAL_GRADE "100"


#define GRADE_0 "NO_C_FILE\n"
#define GRADE_20 "COMPILATION_ERROR\n"
#define GRADE_40 "TIMEOUT\n"
#define GRADE_60 "BAD_OUTPUT\n"
#define GRADE_80 "SIMILAR_OUTPUT\n"
#define GRADE_100 "GREAT_JOB\n"
#define C_LOC(X) (X-1)
#define DOT_LOC(X) (X-2)

/**
 *
 */
void EndProgram(){
    write(STDERR_FILENO,ERROR_MESSAGE,strlen(ERROR_MESSAGE));
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
        if(strcmp(pDirent->d_name,CURRENT_DIR)!= SUBTRACTION &&
        strcmp(pDirent->d_name,PREVIOUS_DIR)!=SUBTRACTION){
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


void RunStudentFile(char* const inputPath){
    pid_t pid;
    char* const args[] ={EXECUTE_COMPILED_FILE,NULL};
    if((pid = fork()) == SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    if(pid == CHILD_PID) {
        int in;
        int out;
        in = open(inputPath, O_RDONLY);
        if (in == SYSTEM_CALL_FAILURE) {
            EndProgram();
        }
        out = open(OUTPUT_FILE, O_RDWR | O_CREAT | O_TRUNC, PERMISSION);
        if (out == SYSTEM_CALL_FAILURE) {
            close(in);
            EndProgram();
        }
        // Replace standard input with input file
        if (dup2(in, STANDARD_INPUT) == SYSTEM_CALL_FAILURE) {
            close(in);
            close(out);
            EndProgram();
        }
        if (dup2(out, STANDARD_OUTPUT) == SYSTEM_CALL_FAILURE) {
            close(in);
            close(out);
            EndProgram();
        }
        close(in);
        close(out);
        execvp(args[INITIAL_VALUE], args);
        EndProgram();
    }
}


int CompileStudentFile(char* const studentFilePath){
    int status;
    pid_t pid;
    char* const args[] = {GCC_COMMAND,GCC_FLAG,COMPILED_FILE_NAME,studentFilePath,NULL};
    if((pid = fork()) == SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    if(pid == CHILD_PID){
       execvp(args[INITIAL_VALUE],args);
       EndProgram();
    }
    // Wait for the child process to finish
    if(waitpid(pid,&status,INITIAL_VALUE)==SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    if(WEXITSTATUS(status)==INITIAL_VALUE){
        return FILE_COMPILED;
    }
    return ERROR;
}


int CompareOutput(char* const outputPath){
    int status;
    pid_t pid;
    char* const args[] ={EXECUTE_COMPILED_COMPARE_PROGRAM,outputPath,
                         OUTPUT_FILE,NULL};
    if((pid = fork()) == SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    if(pid == CHILD_PID){
        execvp(args[INITIAL_VALUE],args);
        EndProgram();
    }
    // Wait for the child process to finish
    if(waitpid(pid,&status,INITIAL_VALUE)==SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    return WEXITSTATUS(status);
}

void SaveToFile(const char* name, int status){
    char line[MAX_CHAR_IN_LINE]={};
    strcpy(line,name);
    strcat(line,COMMA);
    int fd = open(RESULT_FILE, O_RDWR | O_CREAT | O_TRUNC, PERMISSION);
    if (fd == SYSTEM_CALL_FAILURE) {
        EndProgram();
    }
    switch (status){
        case (NO_C_FILE):
            strcat(line,NO_C_GRADE);
            strcat(line,COMMA);
            strcat(line,GRADE_0);
            break;
        case (COMPILATION_ERROR):
            strcat(line,COMPILATION_GRADE);
            strcat(line,COMMA);
            strcat(line,GRADE_20);
            break;
        case (TIMEOUT):
            strcat(line,TIMEOUT_GRADE);
            strcat(line,COMMA);
            strcat(line,GRADE_40);
            break;
        case (DIFFERENT):
            strcat(line,DIFFERENT_GRADE);
            strcat(line,COMMA);
            strcat(line,GRADE_60);
            break;
        case (SAME):
            strcat(line,SAME_GRADE);
            strcat(line,COMMA);
            strcat(line,GRADE_80);
            break;
        case (IDENTICAL):
            strcat(line,IDENTICAL_GRADE);
            strcat(line,COMMA);
            strcat(line,GRADE_100);
            break;
        default:
            break;
    }
    write(fd,line,strlen(line));
    close(fd);
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
        if(strcmp(pDirent->d_name,CURRENT_DIR)!=SUBTRACTION &&
           strcmp(pDirent->d_name,PREVIOUS_DIR)!=SUBTRACTION){
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

            // In case that c file exist
            if(strlen(studentCFilePath)!=EMPTY_PATH){
                int compile = CompileStudentFile(studentCFilePath);
                if(compile){
                    RunStudentFile(input);
                    int compare = CompareOutput(output);
                    printf("compare = %d\n",compare);
                    SaveToFile(studentName,compare);
                    //dont forget unlink!!!!!!!!!!!
                }
            }
        }
    }
    closedir(pDir);
    return 0;
}
