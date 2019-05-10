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
#define SUCCESS 1
#define SEC 1
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
#define RESULT_FILE "results.csv"
#define EXECUTE_COMPILED_FILE "./program.out"
#define EXECUTE_COMPILED_COMPARE_PROGRAM "./comp.out"
#define END_OF_TXT ".txt"
// Grades and reasons
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
#define GRADE_0 "NO_C_FILE"
#define GRADE_20 "COMPILATION_ERROR"
#define GRADE_40 "TIMEOUT"
#define GRADE_60 "BAD_OUTPUT"
#define GRADE_80 "SIMILAR_OUTPUT"
#define GRADE_100 "GREAT_JOB"
// macro to identify c files
#define C_LOC(X) (X-1)
#define DOT_LOC(X) (X-2)

/**
 * The function EndProgram - write the error message (as a result from system call failure),
 * To file descriptor 2 (stderr), and exit the program.
 */
void EndProgram(){
    write(STDERR_FILENO,ERROR_MESSAGE,strlen(ERROR_MESSAGE));
    exit(ERROR);
}
/**
 * The function getPaths - reads the three lines from the configuration file,
 * And initial the given parameters with the correct value.
 * @param configPath - the path to the configuration file,
 * That holds the paths to the others files.
 * @param directory - the path to the directory that contain the students directorys.
 * @param input - path to the input text
 * @param output - path to the correct output text
 */
void getPaths(const char* configPath, char* directory,char* input,char* output){
    // Buffer for reading
    char buffer [MAX_CHAR_IN_LINE*NUMBER_OF_LINES];
    int fd = open(configPath,O_RDONLY);
    // Check the system call open
    if(fd<INITIAL_VALUE){
        EndProgram();
    }
    // Check the system call read after reading from the file
    if(read(fd,buffer,MAX_CHAR_IN_LINE*NUMBER_OF_LINES)==SYSTEM_CALL_FAILURE) {
        close(fd);
        EndProgram();
    }
    /**
     * Split the buffer into its lines,
     * And save them in the relevant parameters.
     */
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

/**
 * The function IsCFile - checks if the file is c file.
 * @param fileName - the name of the file
 * @return true if its c file,and false otherwise
 */
bool IsCFile(char* fileName){
    size_t length = strlen(fileName);
    // Checks if the suffix of the file coordinate c files
    if(fileName[DOT_LOC(length)]==DOT && fileName[C_LOC(length)]==C_FILE){
        return true;
    }
    return false;
}

/**
 * The function - searchFileInDirectory search c files recursively,
 * in the given directory path and its sub directory,
 * And update the parameter filePath.
 * @param directoryPath - the path to the initial directory
 * @param filePath - parameter that will hold the file path
 */
void searchFileInDirectory(char* directoryPath, char* filePath){
    char path [MAX_CHAR_IN_LINE]={};
    // Pointer for directory entry
    struct dirent* pDirent;
    DIR* pDir = opendir(directoryPath);
    // Stop condition
    if(!pDir){
        return;
    }
    while((pDirent=readdir(pDir))!= NULL){
        // Checks that its not the previous or current directory (. / ..)
        if(strcmp(pDirent->d_name,CURRENT_DIR)!= SUBTRACTION &&
        strcmp(pDirent->d_name,PREVIOUS_DIR)!=SUBTRACTION){
            // Update the path to the current directory
            strcpy(path,directoryPath);
            strcat(path,NEXT_PATH);
            strcat(path,pDirent->d_name);
            // Checks if the type is regular file
            if(pDirent->d_type==DT_REG){
                // If its the C file, save it in the parameter filePath
                if(IsCFile(pDirent->d_name)){
                    strcpy(filePath,path);
                }
            }
            // Repeat it recursive
            searchFileInDirectory(path,filePath);
        }
    }
    // close and check
    if(closedir(pDir)==SYSTEM_CALL_FAILURE){
        EndProgram();
    }
}

/**
 *
 */
int RunStudentFile(char* const inputPath, char const* studentName){
    pid_t pid;
    int status;
    int time=INITIAL_VALUE;

    char outputFileName[MAX_CHAR_IN_LINE]={};
    strcpy(outputFileName,studentName);
    strcat(outputFileName,END_OF_TXT);

    char* const args[] ={EXECUTE_COMPILED_FILE,NULL};
    if((pid = fork()) == SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    if(pid == CHILD_PID) {
        int in;
        int out;
        in = open(inputPath,O_RDONLY);
        if (in == SYSTEM_CALL_FAILURE) {
            EndProgram();
        }
        out = open(outputFileName,O_WRONLY |O_TRUNC|O_CREAT,PERMISSION);
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
    while (time<TIMEOUT && !(waitpid(pid,&status,WNOHANG))){
        time++;
        sleep(SEC);
    }
    if(time==TIMEOUT){
        return INITIAL_VALUE;
    }
    return SUCCESS;
}


signed int CompileStudentFile(char* const studentFilePath){
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
    if(WEXITSTATUS(status)!=INITIAL_VALUE){
        return INITIAL_VALUE;
    }
    return FILE_COMPILED;
}


int CompareOutput(char* const outputPath, char* const programOutputPath){
    int status;
    pid_t pid;
    char* const args[] = {EXECUTE_COMPILED_COMPARE_PROGRAM,outputPath,programOutputPath,NULL};
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

void SaveToFile(const char* name, int status, bool firstStudent){
    char line[MAX_CHAR_IN_LINE]={};
    if(!firstStudent){
        strcat(line,ENTER);
    }
    strcpy(line,name);
    strcat(line,COMMA);
    int fd = open(RESULT_FILE,O_WRONLY|O_APPEND|O_CREAT,PERMISSION);
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

            // In case that c file exist
            if(strlen(studentCFilePath)!=EMPTY_PATH){
                int compile = CompileStudentFile(studentCFilePath);
                if(compile){
                    int a = RunStudentFile(input,studentName);
                    // creating the path for the program output file
                    char programOutputPath[MAX_CHAR_IN_LINE] = {};
                    strcpy(programOutputPath,getcwd(programOutputPath,sizeof(programOutputPath)));
                    strcat(programOutputPath,NEXT_PATH);
                    strcat(programOutputPath,studentName);
                    strcat(programOutputPath,END_OF_TXT);
                    // in case that the program running didn't took 5 sec
                    if(a){
                        int compare = CompareOutput(output,programOutputPath);
                        SaveToFile(studentName,compare);
                    } else {
                        SaveToFile(studentName,TIMEOUT);
                    }
                    unlink(programOutputPath);
                }
                else{
                    SaveToFile(studentName,COMPILATION_ERROR);
                }
            } else{
                SaveToFile(studentName,NO_C_FILE);
            }
        }
    }
    unlink(COMPILED_FILE_NAME);
    closedir(pDir);
    return INITIAL_VALUE;
}
