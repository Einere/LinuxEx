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

int inarg(char c);
int runcommand(char **cline, int where, int narg);
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
				//fprintf(stderr, "row = %ld, column = %ld\n", sizeof(tmp)/sizeof(tmp[0]), sizeof(tmp[0]));
				int tmp_narg = 0, same_count = 0, same_size;
				int tmp_type;
				//ptr = inpbuf, tok = tokbuf;
				char same_list[10][256];
				bool flag = false;
				int len = 0;

				count--;
				inpbuf[count] = '\n';
				ptr = inpbuf, tok = tokbuf;
				
				//if(!double_tab){
					//if first tab, get token
					do{
						//fprintf(stderr, "tmp_narg = %d.\n", tmp_narg);
						tmp_type = gettok(&tmp[tmp_narg]);
						if(tmp_narg < MAXARG) tmp_narg++;
					}while(tmp_type == ARG);
					tmp_narg--;
					len = strlen(tmp[tmp_narg-1]);
					//fprintf(stderr,"last-1 token = %s.\n", tmp[tmp_narg-1]);
				//}

				struct dirent* dentry;
				DIR* dirp;
				char cwd[50];

				getcwd(cwd, sizeof(cwd));
				//fprintf(stderr, "cwd = %s\n", cwd);
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
						//fprintf(stderr, "strlen(tmp[tmp_narg-1]) = %ld\n", strlen(tmp[tmp_narg-1]));
						if(strncmp(tmp[tmp_narg-1], dentry->d_name, len) ==0) flag = true;
						else flag = false;

						//fprintf(stderr, "tmp[tmp_narg-1][%d] = %c, dentry->d_name[%d] = %c\n", i, tmp[tmp_narg-1][i], i, dentry->d_name[i]); 
						if(len < strlen(dentry->d_name) && flag){
							strncpy(same_list[same_count], dentry->d_name, sizeof(same_list[same_count]));
							//fprintf(stderr, "\nsame_list[same_count] = %s, same_count = %d.\n\n", same_list[same_count], same_count);
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
								//fprintf(stderr, "same_size = %d\n", same_size);
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
				//fprintf(stderr, "inpbuf[i] = %c.\n", inpbuf[i]);
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
	printf("[gettok] *ptr = %c.\n", *ptr);
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
			printf("[gettok] *ptr is %c.\n", *ptr);
			*tok++ = *ptr++;
		}
    }
	*tok++ = '\0';
	return type;
}

int inarg(char c) {
	char *wrk;
	for (wrk = special; *wrk; wrk++){
		//printf("c = %c, wrk = %c.\n", c, *wrk);
		if (c == *wrk) return (0);
	}
	return (1);
}

int procline(void){
	char *arg[MAXARG + 1]; //2dim array
	int toktype;
	int narg = 0;
	int type;
	ptr = inpbuf, tok = tokbuf;
	for (;;){
		toktype = gettok(&arg[narg]);
		fprintf(stderr, "arg[%d] = %s\n", narg, arg[narg]);
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
				runcommand(arg , type, narg);
			}
			
			if (toktype == EOL){
				return 0;
			}
			narg = 0;
        	break;
        }
	}
}

int runcommand(char **cline, int where, int narg){
	int pid;
	int status;
	int out_redir_index = 0;
	int in_redir_index = 0;
	int out_fd, in_fd;
	bool set_in_fd = false;
	switch(pid = fork()) {
	case -1:
		perror("smallsh");
		return(-1);
	case 0:
		//child process
		for(int i=0; cline[i] != '\0'; i++){
			fprintf(stderr, "cline[%d] = %s\n", i, cline[i]);
		}
		fprintf(stderr, "narg = %d\n", narg);
		if((strcmp(cline[0], "ls")) == 0 && cline[1] == NULL){
			usr_ls();
		}
		
		//for(int i = 0; cline[i] != '\0' ; i++){
		for(int i = 0; i < narg; i++){	
			//fprintf(stderr, "> , cline[%d] = %s, narg-i = %d\n", i, cline[i], narg-i);
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
		//for(int i=0; cline[i] != '\0'; i++) fprintf(stderr, "final cline[%d] = %s\n", i, cline[i]);
		
		execvp(*cline, cline);
		perror(*cline);
		exit(1);
	}
	//parent process	
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
	fprintf(stderr, "cwd = %s\n", cwd);

	if((dirp = opendir(cwd)) == NULL) perror("open directory");
	while((dentry = readdir(dirp)) != NULL){
		if(strcmp(dentry->d_name, ".") != 0 && strcmp(dentry->d_name, "..") != 0) 
			fprintf(stderr, "%s ", dentry->d_name);
	}
	fprintf(stderr, "\n");
	exit(1);
}

int main(){
	while(userin(prompt) != EOF) procline();
}
