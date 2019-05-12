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
 * The function RunStudentFile - runs the C file found in the student directory.
 * It checks if the program running time take less then five sec.
 * It returns true (1) if the program finished until five sec,
 * And false otherwise.
 * @param inputPath - the input to the program
 * @param studentName - the student that the file belong to
 * @return - true (1) if the program running time took under five sec,
 * And false (0) otherwise.
 */
int RunStudentFile(char* const inputPath, char const* studentName){
    pid_t pid;
    int status;
    int time=INITIAL_VALUE;
    char outputFileName[MAX_CHAR_IN_LINE]={};
    //Set the name of the output text according to the student name
    strcpy(outputFileName,studentName);
    strcat(outputFileName,END_OF_TXT);
    char* const args[] ={EXECUTE_COMPILED_FILE,NULL};
    // Create child process
    if((pid = fork()) == SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    // If its the child process
    if(pid == CHILD_PID) {
        int in;
        int out;
        // Open the file that contain the input to the program
        in = open(inputPath,O_RDONLY);
        // Check system call open
        if (in == SYSTEM_CALL_FAILURE) {
            EndProgram();
        }
        // open (and create if not exist) the output program file
        out = open(outputFileName,O_WRONLY |O_TRUNC|O_CREAT,PERMISSION);
        //Check system call open
        if (out == SYSTEM_CALL_FAILURE) {
            close(in);
            EndProgram();
        }
        // Replace standard input with input file and check validation
        if (dup2(in, STANDARD_INPUT) == SYSTEM_CALL_FAILURE) {
            close(in);
            close(out);
            EndProgram();
        }
        // Replace standard output with output file and check validation
        if (dup2(out, STANDARD_OUTPUT) == SYSTEM_CALL_FAILURE) {
            close(in);
            close(out);
            EndProgram();
        }
        // Close both of the files and check
        if(close(in)==SYSTEM_CALL_FAILURE || close(out)==SYSTEM_CALL_FAILURE){
            EndProgram();
        }
        // Run the program
        execvp(args[INITIAL_VALUE], args);
        EndProgram();
    }
    // Father process
    // Checks if the kid proccess ended until 5 seconds
    while (time<TIMEOUT && !(waitpid(pid,&status,WNOHANG))){
        time++;
        sleep(SEC);
    }
    // The kid process didn't finish in time
    if(time==TIMEOUT){
        return INITIAL_VALUE;
    }
    // The kid process finish in time
    return SUCCESS;
}

/**
 * The function CompileStudentFile - compile the student file,
 * And return true (1) if it succeed, and false (0) otherwise.
 * @param studentFilePath - the path to the C file
 * @return- true (1) if the file compiled, and false (0) otherwise.
 */
int CompileStudentFile(char* const studentFilePath){
    int status;
    pid_t pid;
    char* const args[] = {GCC_COMMAND,GCC_FLAG,COMPILED_FILE_NAME,studentFilePath,NULL};
    if((pid = fork()) == SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    // Checks if its the child procces, and compile the file
    if(pid == CHILD_PID){
       execvp(args[INITIAL_VALUE],args);
       EndProgram();
    }
    // Wait for the child process to finish
    if(waitpid(pid,&status,INITIAL_VALUE)==SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    /*
     * Checks the exit status of the child process,
     * If he succeed to compile the file or not
     * */
    if(WEXITSTATUS(status)!=INITIAL_VALUE){
        return INITIAL_VALUE;
    }
    return FILE_COMPILED;
}


/**
 * The function CompareOutput - gets two paths to files,
 * Compare between the file using the program we write in ex31.
 * @param outputPath - the correct output file path
 * @param programOutputPath - the program output file path
 * @return 1 for identical, 2 for different and 3 for similar
 */
int CompareOutput(char* const outputPath, char* const programOutputPath){
    int status;
    pid_t pid;
    char* const args[] = {EXECUTE_COMPILED_COMPARE_PROGRAM,outputPath,programOutputPath,NULL};
    // Create child process and check
    if((pid = fork()) == SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    // Checks if ita the child process and run the compare program
    if(pid == CHILD_PID){
        execvp(args[INITIAL_VALUE],args);
        EndProgram();
    }
    // Wait for the child process to finish
    if(waitpid(pid,&status,INITIAL_VALUE)==SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    // Return the value that returned from the program
    return WEXITSTATUS(status);
}

/**
 * The function SaveToFile - get student name and its status.
 * According to the student status its save his grade with his name and comment.
 * @param name - the student name
 * @param status - his work status
 * @param firstStudent - boolean variable that holds if its the first student
 */
void SaveToFile(const char* name, int status, bool firstStudent){
    // Initial the line that will be written to the file
    char line[MAX_CHAR_IN_LINE]={};
    // If its not the first student, add enter to separate between the students.
    if(!firstStudent){
       strcat(line,ENTER);
    }
    strcat(line,name);
    strcat(line,COMMA);
    // Open and check the result file
    int fd = open(RESULT_FILE,O_WRONLY|O_APPEND|O_CREAT,PERMISSION);
    if (fd == SYSTEM_CALL_FAILURE) {
        EndProgram();
    }
    /*
     * According to the student status,
     * It set the appropriate grade and comment.
    */
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
    // write to the result file the line and check
    if(write(fd,line,strlen(line))!=strlen(line)){
        EndProgram();
    }
}


/**
 * main program - get configuration file,
 * Take from it path to students directory, input file text,
 * And correct output file txt.
 * The program search the C file in every student directory,
 * And give every student grade for its work.
 * The grades save in result.csv file.
 */
int main(int argc, char* argv[]) {
    // Counter number of students
    int counter = INITIAL_VALUE;
    // flag for the first student
    bool firstStudent = true;
    // Checks the valid of the input
    if(argc!= NUMBER_OF_ELEMENTS){
        printf(INPUT_ERROR);
        return ERROR;
    }
    // Get the path from the command to the configuration file
    char* configPath = argv[FILE_PLACE];
    // Initial the variables that will hold the three paths
    char directory[MAX_CHAR_IN_LINE] = {};
    char input[MAX_CHAR_IN_LINE] = {};
    char output[MAX_CHAR_IN_LINE] = {};
    // Initial the variable with the paths
    getPaths(configPath,directory,input,output);
    struct dirent* pDirent;
    DIR* pDir = opendir(directory);
    // Checks the system call opendir
    if(!pDir){
        EndProgram();
    }
    // Scan all the sub directorys inside the given directory
    while ((pDirent=readdir(pDir))!= NULL){
        // Checks that its not the current or the previous directory (. / ..)
        if(strcmp(pDirent->d_name,CURRENT_DIR)!=SUBTRACTION &&
           strcmp(pDirent->d_name,PREVIOUS_DIR)!=SUBTRACTION){
            counter++;
            // Checks if its the first student
            if(counter>SUCCESS){
                firstStudent = false;
            }
            // Set the current path
            char directoryPath[MAX_CHAR_IN_LINE]={};
            strcpy(directoryPath,directory);
            char studentName[MAX_CHAR_IN_LINE]={};
            strcpy(studentName,pDirent->d_name);
            char studentCFilePath[MAX_CHAR_IN_LINE]={};
            strcat(directoryPath,NEXT_PATH);
            strcat(directoryPath,pDirent->d_name);
            // Search C file inside the directory path (student directory)
            searchFileInDirectory(directoryPath,studentCFilePath);
            // In case that c file exist
            if(strlen(studentCFilePath)!=EMPTY_PATH){
                // Compile the file
                int compile = CompileStudentFile(studentCFilePath);
                // Checks if the compilation succeeded
                if(compile){
                    // Run the compiled program
                    int programEnd = RunStudentFile(input,studentName);
                    // creating the path for the program output file
                    char programOutputPath[MAX_CHAR_IN_LINE] = {};
                    // Get the current directory and check
                    if(getcwd(programOutputPath,sizeof(programOutputPath))==NULL){
                        EndProgram();
                    }
                    strcat(programOutputPath,NEXT_PATH);
                    strcat(programOutputPath,studentName);
                    strcat(programOutputPath,END_OF_TXT);
                    // in case that the program running time took maximum 5 sec
                    if(programEnd){
                        // Compare the correct output file with the program output file
                        int compare = CompareOutput(output,programOutputPath);
                        // Save the student grade in the result file
                        SaveToFile(studentName,compare,firstStudent);
                    } else {
                        // Save the student grade in the result file
                        SaveToFile(studentName,TIMEOUT,firstStudent);
                    }
                    // Remove the program output file
                    unlink(programOutputPath);
                }
                else{
                    // Save the student grade in the result file
                    SaveToFile(studentName,COMPILATION_ERROR,firstStudent);
                }
            } else{
                // Save the student grade in the result file
                SaveToFile(studentName,NO_C_FILE,firstStudent);
            }
        }
    }
    // Remove the compiled program and check
    if(unlink(COMPILED_FILE_NAME)==SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    // Close the directory and check
    if(closedir(pDir)==SYSTEM_CALL_FAILURE){
        EndProgram();
    }
    // The program finished :)
    return INITIAL_VALUE;
}
