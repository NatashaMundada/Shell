#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h> 
#include<signal.h>
#include<sys/wait.h>
#include<stdlib.h>

int isRedirection = 0;
int isBackground = 0;
pid_t child_pid, child2, child1;
int isPipe = 0;

void signal_handler(int signal) {
	kill(child_pid, SIGINT);	
}

void perform_pipe_operation(char *pipe1[], char *pipe2[]) { 
	int pipe_fd[2];
	pipe(pipe_fd);
	child1 = fork();

	if (child1 > 0) {
		child2 = fork();
	}
	
	if (child1 == 0) {
		close(STDOUT_FILENO);
		dup(pipe_fd[1]);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		execvp(pipe1[0], pipe1);
		perror("error execvp");
	} else if (child2 == 0) {
		close(STDIN_FILENO);
		dup(pipe_fd[0]);
		close(pipe_fd[0]);
		close(pipe_fd[1]);		
		execvp(pipe2[0], pipe2);
		perror("error execvp");
	} else {
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		waitpid(child1, NULL, 0);
		waitpid(child2, NULL, 0);
	}
}

void parse_command(char buffer[], char *command[], char *pipe1[], char *pipe2[]) {
	int i, j = 0, k = 0;

	//Overwriting new line character with end-of-string character.
	buffer[strlen(buffer) - 1] = '\0';

	signal(SIGINT, &signal_handler); 
	
	//Storing pointer to first word.
	command[j] = &buffer[0];
	pipe1[k] = &buffer[0];
        j++;
	k++;
        for (i = 0; buffer[i] != '\0'; i++) {
                if (buffer[i] == ' ') {
                        buffer[i] = '\0';
                        command[j] = &buffer[i+1];
                        if (pipe1[k-1] != NULL) {
				pipe1[k] = &buffer[i+1];
				k++;
			}
			j++;
			
                } if (buffer[i] == '>') {
			isRedirection = 1;
		} if (buffer[i] == '&') {
			isBackground = 1;
			
		} if (buffer[i] == '|') {
			pipe1[k-1] = NULL;
			isPipe = 1;
			pipe2[0] = &buffer[i+2];
			pipe2[1] = NULL;
		}
        }
	
        //Appending last pointer to NULL.
	command[j] = NULL;

	child_pid = fork();
	int fd;
 
	if (isPipe == 1) {
		perform_pipe_operation(pipe1, pipe2);
	} else if(isPipe == 0) {
		if (child_pid == 0) {
			if (isRedirection == 1) {
				fd = open(command[j-1], O_WRONLY | O_CREAT, 0644);
				dup2(fd, 1);
				close(fd);
				command[j-2] = NULL;
				execvp(command[0], command);
			}
			if (isBackground == 1) {
				command[j-1] = NULL;
				execvp(command[0], command);
			} else {
				execvp(command[0], command);		
			}		
		} else {
			if (isBackground != 1) {
				waitpid(child_pid, NULL, 0);
			}
		}	
	}
}
	
int main() {
	while(1) {
		char *command[100];
		char buffer[100];
		char *pipe1[100], *pipe2[100];
		isRedirection = 0;
		isBackground = 0;
		isPipe = 0;
		printf("\n****Welcome to the Unix Shell****\n");
		printf("\n%");
		if (fgets(buffer, 100, stdin) == NULL && feof(stdin)) {
			printf("Trouble reading stdin. End-of-file? Exiting....\n");
			exit(1);
		}
		parse_command(buffer, command, pipe1, pipe2);
	}
	return 0;
}		
