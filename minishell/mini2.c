#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "getch.h"

#define EOL 1
#define ARG 2
#define AMPERSAND 3
#define SEMICOLON 4
#define INPUT_REDIRECTION 5
#define OUTPUT_REDIRECTION 6
#define MAXARG 512
#define MAXBUF 512
#define FOREGROUND 0
#define BACKGROUND 1


static char inpbuf[MAXBUF], tokbuf[2*MAXBUF], *ptr = inpbuf, *tok = tokbuf;
char *prompt = "Command > ";
static char special[] = {' ','\t','&',';','\n','\0', '>', '<'};
struct sigaction act, act2;

int inarg(char c);
int runcommand(char **cline, int where, int narg, bool is_pipe, char **my_cline, int* p_pipe_fd);
void usr_ls();
int gettok(char **outptr);

int userin(char *p){
	int c, count = 0, i = 0;
    ptr = inpbuf;
    tok = tokbuf;
    printf("%s", p);
    bool double_tab = false;

	while(1){
        if((c = getch()) == EOF) {
			//getch or getchar
            printf("eof = %c\n", c);
            return (EOF);
        }
		
		if(c == 27){
			//if typed arrow key
			if((c = getch()) == 91){
				c = getch();
				if(c == 65 || c == 66 || c == 67 || c == 68) continue;
			}
		}
		
        if (count < MAXBUF){
			//insert c at inpbuf one by one
			if(c == 127){
				//if c is back space
				if(count > 0){
					count--;
					inpbuf[count] = '\0';
					fprintf(stderr, "\b \b");
				}
			}
			else inpbuf[count++] = c;
			//if c isn't back space, insert inpbuf
			
			if(c == '\t'){
				//if c is tap
				char *tmp[10];
				int tmp_narg = 0, same_count = 0, same_size;
				int tmp_type;
				//ptr = inpbuf, tok = tokbuf;
				char same_list[10][256];
				bool flag = false;
				int len = 0;

				count--;
				inpbuf[count] = '\n';
				ptr = inpbuf, tok = tokbuf;
				
					//if first tab, get token
					do{
						tmp_type = gettok(&tmp[tmp_narg]);
						if(tmp_narg < MAXARG) tmp_narg++;
					}while(tmp_type == ARG);
					tmp_narg--;
					len = strlen(tmp[tmp_narg-1]);

				struct dirent* dentry;
				DIR* dirp;
				char cwd[50];

				getcwd(cwd, sizeof(cwd));
				fprintf(stderr, "\n");
				
				if(!double_tab){
					//if first tab, get current working directory 
					if((dirp = opendir(cwd)) == NULL) perror("open directory");
				}
				else{
					//if double tab, get token (it's user input directory)
					if((dirp = opendir(tmp[tmp_narg-1])) == NULL) perror("open directory");
				}
				while((dentry = readdir(dirp)) != NULL){
					if(!double_tab){
						//if first tab, compare token with d_name
						if(strncmp(tmp[tmp_narg-1], dentry->d_name, len) ==0) flag = true;
						else flag = false;

						if(len < strlen(dentry->d_name) && flag){
							strncpy(same_list[same_count], dentry->d_name, sizeof(same_list[same_count]));
							same_count++;
						}
					}else{
						//if double tab, print d_name at terminal
						if(strcmp(dentry->d_name, ".") != 0 && strcmp(dentry->d_name, "..") != 0) 
							fprintf(stderr, "%s ", dentry->d_name);
					}
				}
				if(!double_tab){
					//if first tab, insert same character at inpbuf
					if(same_count > 0){
						int i = 0;
						int j = 0;
						same_size = strlen(same_list[0]);
						if(same_count == 1){ fprintf(stderr, "%s", same_list[0]); }
						else{
							flag = true;
							do{
								for(i = 0; i < same_count; i++){
									for(j = i + 1; j < same_count; j++){
										if(strncmp(same_list[i], same_list[j], same_size) != 0){
											same_size--;
											flag = true;
										}else flag = false;
									}
								}
							}while(flag);
						
						}
						for(i = len; i < same_size; i++) {
							inpbuf[count++] = same_list[0][i];
							fprintf(stderr, "%c", same_list[0][i]);
						}
					}
					double_tab = true;
				}else{
					//if double tab, set double_tab to be false
					double_tab = false;
				}
			}
			
			if(c != '\t') fprintf(stderr, "%c", c);
			//if user type key, print at terminal except tab
		}
        
		if (c == '\n' && count < MAXBUF){
			//if c in enter
        	inpbuf[count] = '\0';
			for(int i=0; i<=count; i++){
			}
			if(inpbuf[count-2] == 'q') exit(1);
			//if user type '~~~q\n', exit 
			return count;
        }
        
		if (c == '\n') {
            printf("smallsh: input line too long\n");
            count = 0;
            printf("%s", p);
        }
    }
	fflush(stdin);
}

int gettok(char **outptr){
    int type;
    *outptr = tok;
	//devide command and first arguments
	while(*ptr == ' '|| *ptr == '\t') {
		ptr++;
	}
    *tok++ = *ptr;
    switch(*ptr++) {
    case '\n':
		type = EOL;
		break;
	case '&':
		type = AMPERSAND;
		break;
	case ';':
		type = SEMICOLON;
		break;
	case '<':
		type = INPUT_REDIRECTION;
		break;
	case '>':
		type = OUTPUT_REDIRECTION;
		break;
	default:
		type = ARG;
		while(inarg(*ptr)){
			*tok++ = *ptr++;
		}
    }
	*tok++ = '\0';
	return type;
}

int inarg(char c) {
	char *wrk;
	for (wrk = special; *wrk; wrk++){
		if (c == *wrk) return (0);
	}
	return (1);
}

int procline(void){
	char *arg[MAXARG + 1]; //2dim array
	char *my_cline[5];
	int toktype;
	int narg = 0;
	int type;
	bool is_pipe = false;
	ptr = inpbuf, tok = tokbuf;
	for (;;){
		toktype = gettok(&arg[narg]);
		//set token in arg[narg] and return token's type
		switch(toktype) {
		case ARG: 
		case INPUT_REDIRECTION:
		case OUTPUT_REDIRECTION:
			if (narg < MAXARG) narg++;
			break;
		case EOL:
		case SEMICOLON:
		case AMPERSAND:
			if (toktype == AMPERSAND) type = BACKGROUND;
			else type = FOREGROUND;
			
			if (narg != 0){
				//if user type any command
				arg[narg] = NULL;
				//set final element is null. for exec function
				runcommand(arg , type, narg, is_pipe, my_cline, NULL);
			}
			
			if (toktype == EOL){
				return 0;
			}
			narg = 0;
        	break;
        }
	}
}

int runcommand(char **cline, int where, int narg, bool is_pipe, char** my_cline, int* p_pipe_fd){
	int pid;
	int status;
	int out_redir_index = 0;
	int in_redir_index = 0;
	int out_fd, in_fd;
	bool set_in_fd = false;
	int pipe_fd[2];
	switch(pid = fork()) {
	case -1:
		perror("smallsh");
		return(-1);
	case 0:
		//child process
		act.sa_handler = SIG_DFL;
		act2.sa_handler = SIG_DFL;
		sigaction(SIGINT, &act, NULL);
		sigaction(SIGQUIT, &act2, NULL);
		
		if(is_pipe){
			if(close(p_pipe_fd[1]) < 0) fprintf(stderr, "[%d]close(p_pipe_fd[1]) failed\n", getpid());
			else fprintf(stderr, "[%d]close(p_pipe_fd[1]) success\n", getpid());
			dup2(p_pipe_fd[0], STDIN_FILENO);
		}
		
		for(int i = 0; i < narg; i++){
			if(strcmp(cline[i], "|") == 0){
				is_pipe = true;
				pipe(pipe_fd);
				for(int j = 0; j < i; j++){
					my_cline[j] = cline[j];
					cline = &cline[i+1];
					my_cline[j+1] = '\0';
				}
				runcommand(cline, where, narg-(i+1), is_pipe, my_cline, pipe_fd);
				break;
			}
			is_pipe = false;
		} 
			
		if((strcmp(cline[0], "ls")) == 0 && cline[1] == NULL){
			usr_ls();
		}

		for(int i = 0; i < narg; i++){	
			if(cline[i] != '\0' && strcmp(cline[i], ">") == 0){
				out_redir_index = i;
				//fprintf(stderr, "cline[%d][0] = %c, out_redir_index = %d\n", i, cline[i][0], out_redir_index);
				if((out_fd = open(cline[out_redir_index+1], O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0644)) < 0) perror("failed to open file");
				dup2(out_fd, 1);
				cline[out_redir_index+1] = '\0';
				cline[out_redir_index] = '\0';
				i++;
			}
			else if(strcmp(cline[narg - i - 1], "<") == 0){
				in_redir_index = narg - i - 1;
				//fprintf(stderr, "cline[%d][0] = %c, in_redir_index = %d\n", narg - i - 1, cline[narg - i - 1][0], in_redir_index);
				if((in_fd = open(cline[in_redir_index+1], O_RDONLY)) < 0) perror("failed to open file");
				if(!set_in_fd) dup2(in_fd, 0); //dup2(0, in_fd);
				cline[in_redir_index+1] = '\0';
				cline[in_redir_index] = '\0';
				set_in_fd = true;
			}
		}
		
		execvp(*cline, cline);
		perror(*cline);

		exit(1);
	}
	//parent process
	
	if(is_pipe){
		if(close(p_pipe_fd[0]) < 0 ) fprintf(stderr, "[%d]close(p_pipe_fd[0]) failed\n", getpid());
		else fprintf(stderr, "[%d]close(p_pipe_fd[0]) success\n", getpid());
		
		int pid2;
		int status2;
		dup2(p_pipe_fd[1], STDOUT_FILENO);
		if((pid2 = fork()) < 0) perror("failed to fork");
		else if(pid2 != 0){
			if(close(p_pipe_fd[1]) < 0 ) fprintf(stderr, "[%d]close(p_pipe_fd[1]) failed!!!!!!!!!!!!!!\n", getpid());
			else fprintf(stderr, "[%d]close(p_pipe_fd[1]) success!!!!!!!!!!!!!!!!\n", getpid());
			waitpid(pid2, &status2, 0);
		}
		else{
			execvp(*my_cline, my_cline);
		}
	}
	
	if(where == BACKGROUND) {
		printf("child's pid = %d\n", pid);
		return(0);
	}else if(waitpid(pid, &status, 0) == -1) return(-1);
	else return (status);
	
}

void usr_ls(){
	struct dirent* dentry;
	DIR* dirp;
	char cwd[50];

	getcwd(cwd, sizeof(cwd));
	if((dirp = opendir(cwd)) == NULL) perror("open directory");
	while((dentry = readdir(dirp)) != NULL){
		if(strcmp(dentry->d_name, ".") != 0 && strcmp(dentry->d_name, "..") != 0) 
			fprintf(stdout, "%s ", dentry->d_name);
	}
	fflush(stdout);
	fprintf(stderr, "\n");
	exit(1);
}

void handler(int signo){
	fprintf(stderr, "SIGINT and SIGQUIT is bloked\n");
}

int main(){

	act.sa_handler = handler;
	act.sa_flags = 0;
	act2.sa_handler = handler;
	act2.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigemptyset(&act2.sa_mask);
	if(sigaction(SIGINT, &act, NULL) < 0) perror("failed to set SIGINT handler");
	if(sigaction(SIGQUIT, &act2, NULL) < 0) perror("failed to set SIGQUIT handler");
	while (1) while(userin(prompt) != EOF) procline();
}
