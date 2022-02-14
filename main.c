#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
void writeInLogFile(int signum)
{
    int flag=waitpid(-1,NULL,WNOHANG);
    //wait for any child process to finish execution (same as wait(NULL)) , WNOHANG is a non-blocking flag that returns immediately instead of blocking if no child changed state
    // it returns pid of terminated child in flag
    if(flag!=-1 && flag!=0) //if waitpid successful return
    {
        FILE *fp=fopen("logfile.txt","a");
        fprintf(fp,"Child process was terminated.\n");
        fclose(fp);
    }
}

int main()
{
    FILE *fp=fopen("logfile.txt","w");//clear file before starting shell
    fclose(fp);
    while(1)
    {
        char command[30];
        printf("\nEnter command: \n >> ");
        fgets(command,30,stdin);

        if(strcmp(command,"exit\n")==0)
        {
            break;
        }

        int num_of_tokens=10;
        char * tokens[num_of_tokens];//array of strings to store command and arguments
        int n = 0;
        for (char * token = strtok(command, " \n"); token!=NULL; token = strtok(NULL, " \n"))
        {
            tokens[n++] = token;
        }

        int bgProcess=0;
        if(strcmp(tokens[n-1],"&")==0)  //if it is bg process
        {
            bgProcess=1;
            tokens[n-1]=NULL;  //null-terminate the array of strings
        }
        else
        {
            tokens[n]=NULL;
        } //null-terminate the array of strings

        if(strcmp(tokens[0],"cd")==0) //if it is cd command
        {
            char curr_dir[50];
            chdir(tokens[1]);
            printf("New directory: %s\n", getcwd(curr_dir, 50));
            continue;
        }

        pid_t pid=fork(); //spawn a new child process
        if(pid<0)  /*child creation failed*/
        {
            printf("Fork failed, unable to create child process.\n");
            return 1;
        }

        else if (pid == 0)   /* child process */
        {
            execvp(tokens[0],tokens);
            printf("\nError, invalid shell command.\n");
            exit(0);
        }

        else   /* parent process */
        {
            if(bgProcess==0)   //if command does not include '&' (foreground process not background)
            {
                waitpid(pid,NULL,0);
                // wait for child process with specific pid to finish
                // 0 means blocking (if the child does not change state)
            }
            else if(bgProcess==1) // if it is a background process (no wait)
            {
                signal(SIGCHLD, writeInLogFile);
                //SIGCHLD signal sent from child to parent process upon child termination
                //writeInLogFile function is signal handler called when signal is received
            }
        }
    }
    return 0;
}
