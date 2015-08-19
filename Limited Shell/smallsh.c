/*********************************************************************************
 * Author: barlanj
 * Description: a small shell written for Linux. Has built in cmds for exit, status, and cd
 *
 * Reference: http://stephen-brennan.com/2015/01/16/write-a-shell-in-c/
 *********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_CHAR_LENGTH 2048
#define TOK_DELIM " \t\r\n\a"

// forward declarations of functions
char *read_line();
char **split_line(char *line, int *argCount);
int smallsh_execute(char **args, bool bgFlag);
bool isBackground(const char *c);
bool isComment(const char *c);
bool isRedirection(const char *c);


/******************************************************************************/

int main()
{
    int argCount;           // the number of arguments passed
    bool bgFlag = false;    // flag if this process is to run in background
    bool redirect = false;  // redirection flag
    int exitStatus = -1;    // execution exit status for foreground [0 = success | 1 = failure]
    int exitSignal = -1;    // signal status for the exit of foreground process
    int bg_exitStatus = -1; // execution exit status for BG
    int bg_exitSignal = -1; // signal status for BG
    int status;             // reported status for FG
    int bg_status;          // reported status for BG
    bool loop = true;       // control for the do-loop
    int child_pid;          // child process ID
    int newFD;

    //IGNORE CTRL-C
    signal(SIGINT, SIG_IGN);

    do {

        //reset variables
        bgFlag = false;
        argCount = 0;
        redirect = false;

        //prompt user
        char *line = read_line();
        //tokenize the arguments passed
        char **args = split_line(line, &argCount);

        //check if line is a blank
        if(argCount > 0 ) {
            //check if last char is & - run in background
            if(isBackground(args[argCount - 1])) {
                bgFlag = true;
                args[argCount - 1] = NULL;
            }
            //set redirection flag
            if(argCount == 3)
                redirect = isRedirection(args[1]);

           //check if user wants to exit
            if(strcmp(args[0], "exit") == 0) {
                exitStatus = 0;
                loop = false;
                break;
            }
            else if (strcmp(args[0], "cd") == 0) {
                if(argCount > 1) {
                    if(chdir(args[1]) != 0) {
                        printf("No such file or directory found. \n");
                        exitStatus = 1;
                    }
                }
                else
                {
                    if(chdir(getenv("HOME")) == 0)
                        exitStatus = 0;
                }
            }
            else if (strcmp(args[0], "status") == 0) {
                if(exitSignal >= 0)
                    printf("Process was termindated by signal %d \n", exitSignal);
                else if (exitStatus >= 0)
                    printf("Process exited with status %d \n", exitStatus);
            }
            else if(!isComment(args[0]) )
            {

                newFD;
                child_pid = fork();
                switch(child_pid)
                {
                    case -1:
                        exit(1);
                        break;

                    case 0:
                        if(redirect == true)
                        {
                            // is this an output redirection?
                            if (strcmp(args[1], ">") == 0) {
                                newFD = open(args[2], O_WRONLY|O_CREAT|O_TRUNC, 0644);
                                if(newFD == -1)
                                {
                                    printf("smallsh: cannot open %s for output. \n",args[2]);
                                    exit(1);
                                }
                                dup2(newFD, 1);

                            }
                            // is this an input redirection?
                            else if (strcmp(args[1], "<") == 0) {
                                newFD = open(args[2], O_RDONLY);
                                if(newFD == -1)
                                {
                                    printf("smallsh: cannot open %s for input. \n",args[2]);
                                    exit(1);
                                }
                                dup2(newFD, 0);
                            }
                            //bg? or no file
                            else if (bgFlag)
                            {

                                newFD = open("/dev/null", O_WRONLY|O_CREAT|O_TRUNC, 0644);
                                if(newFD == -1)
                                {
                                    printf("smallsh: cannot open %s for output. \n",args[2]);
                                    exit(1);
                                }
                                dup2(newFD, 1);
                            }

                            args[1] = NULL;
                            //close(newFD);

                            //execute command
                            if(execvp(args[0], args) < 0) {
                                printf("Command not recognized. Exec failed, \n");
                                exitStatus = 1;
                                exit(1);
                            }
                        }
                        else
                        {
                            if(execvp(args[0], args) < 0) {
                                printf("Command not recognized. Exec failed. \n");
                                exitStatus = 1;
                                exit(1);
                            }
                        }
                        break;

                    //THE PARENT
                    default :
                        //check if the child started was a background process
                        if(bgFlag)
                        {
                            printf("BG process %d started. \n", child_pid);
                        }
                        else
                        {
                            //wait for child
                            waitpid(child_pid, &exitStatus, 0);

                            if(WIFSIGNALED(exitStatus))
                                exitSignal = WTERMSIG(exitStatus);
                            else
                                exitSignal = -1;

                            if(WIFEXITED(exitStatus))
                                status = WEXITSTATUS(exitStatus);
                            else
                                status = -1;
                        }
                        break;
                }

            }

            // Background loop
            while ( (child_pid = waitpid(-1, &bg_exitStatus, WNOHANG) ) > 0)
            {
                if(WIFSIGNALED(bg_exitStatus))
                {
                    bg_exitSignal = WTERMSIG(bg_exitStatus);
                    printf("BG process %d is done: terminated by signal %d \n", child_pid, bg_exitSignal);
                }

                if(WIFEXITED(bg_exitStatus))
                {
                    bg_status = WEXITSTATUS(bg_exitStatus);
                    printf("BG process %d is done: exit value %d \n", child_pid, bg_exitStatus);
                }
            }


        }

        //deallocation
        free(line);
        free(args);

    } while (loop);

    return EXIT_SUCCESS;
}

/******************************************************************************/
bool isBackground(const char *c)
{
    if(strncmp(c, "&", 1) == 0) {
        return true;
    } else {
        return false;
    }
}

bool isComment(const char *c)
{
    if(strncmp(c, "#", 1) == 0) {
        return 1;
    } else {
        return 0;
    }
}

bool isRedirection(const char *c)
{
    if(strncmp(c, ">", 1) == 0 || strncmp(c, "<", 1) == 0) {
        return true;
    } else {
        return false;
    }
}

char *read_line()
{
    char *input = NULL;
    ssize_t bufsize = 0;

    fflush(stdout);
    printf(": ");
    getline(&input, &bufsize, stdin);
    return input;
}

char **split_line(char *line, int *argCount)
{
    int buffer_size = MAX_CHAR_LENGTH;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;
    *argCount = 0;

    if(!tokens) {
        fprintf(stderr, "smallsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        if(position >= buffer_size) {
            buffer_size += MAX_CHAR_LENGTH;
            tokens = realloc(tokens, buffer_size  * sizeof(char));
            if(!tokens){
                fprintf(stderr, "smallsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
        (*argCount)++;
    }
    tokens[position] = NULL;
    return tokens;
}
