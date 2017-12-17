#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include "getch.h"

#define EOL 1
#define ARG 2
#define AMPERSAND 3
#define SEMICOLON 4
#define MAXARG 512
#define MAXBUF 512
#define FOREGROUND 0
#define BACKGROUND 1


static char inpbuf[MAXBUF], tokbuf[2*MAXBUF], *ptr = inpbuf, *tok = tokbuf;
char *prompt = "Command > ";
static char special[] = {' ','\t','&',';','\n','\0'};
int inarg(char c);
int runcommand(char **cline, int where);
void usr_ls();
int gettok(char **outptr);

int userin(char *p){
	int c, count = 0, i = 0;
    ptr = inpbuf;
    tok = tokbuf;
    printf("%s", p);
    
	while(1){
        if((c = getch()) == EOF) {
			//getch or getchar
            printf("eof = %c\n", c);
            return (EOF);
        }
		
		if(c == 27){
			if((c = getch()) == 91){
				c = getch();
				if(c == 65 || c == 66 || c == 67 || c == 68){
					//if typed arrow key
					continue;
				}
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
			
			if(c == '\t'){
				//if c is tap
				char *tmp[10];
				//fprintf(stderr, "row = %ld, column = %ld\n", sizeof(tmp)/sizeof(tmp[0]), sizeof(tmp[0]));
				int tmp_narg = 0, same_count = 0, same_size;
				int tmp_type;
				//ptr = inpbuf, tok = tokbuf;
				char same_list[10][256];
				bool flag = false;
				
				count--;
				inpbuf[count] = '\n';
				ptr = inpbuf, tok = tokbuf;
				do{
					//fprintf(stderr, "tmp_narg = %d.\n", tmp_narg);
					tmp_type = gettok(&tmp[tmp_narg]);
					if(tmp_narg < MAXARG) tmp_narg++;
				}while(tmp_type == ARG);
				tmp_narg--;
				int len = strlen(tmp[tmp_narg-1]);
				//fprintf(stderr,"last-1 token = %s.\n", tmp[tmp_narg-1]);
				
				struct dirent* dentry;
				DIR* dirp;
				char cwd[50];

				getcwd(cwd, sizeof(cwd));
				//fprintf(stderr, "cwd = %s\n", cwd);

				if((dirp = opendir(cwd)) == NULL) perror("open directory");
				while((dentry = readdir(dirp)) != NULL){
					//fprintf(stderr, "strlen(tmp[tmp_narg-1]) = %ld\n", strlen(tmp[tmp_narg-1]));
					if(strncmp(tmp[tmp_narg-1], dentry->d_name, len) ==0) flag = true;
					else flag = false;

					//fprintf(stderr, "tmp[tmp_narg-1][%d] = %c, dentry->d_name[%d] = %c\n", i, tmp[tmp_narg-1][i], i, dentry->d_name[i]); 
					if(len < strlen(dentry->d_name) && flag){
						strncpy(same_list[same_count], dentry->d_name, sizeof(same_list[same_count]));
						//fprintf(stderr, "\nsame_list[same_count] = %s, same_count = %d.\n\n", same_list[same_count], same_count);
						same_count++;
					}
				}


				if(same_count > 0){
					int i = 0;
					int j = 0;
					same_size = strlen(same_list[0]);
					if(same_count == 1){
						fprintf(stderr, "%s", same_list[0]);
					}
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

			
			}
			
			
			if(c != '\t') fprintf(stderr, "%c", c);
			//for(i = 0; inpbuf[i] != '\0'; i++){
			//	fprintf(stderr, "%c", inpbuf[i]);
			//}
		}
        
		if (c == '\n' && count < MAXBUF){
			//if c in enter
        	inpbuf[count] = '\0';
			for(int i=0; i<=count; i++){
				fprintf(stderr, "inpbuf[i] = %c.\n", inpbuf[i]);
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
		fprintf(stderr, "arg[narg] = %s\n", arg[narg]);
		//set token in arg[narg] and return token's type
		switch(toktype) {
		case ARG: 
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
				runcommand(arg , type);
			}
			
			if (toktype == EOL){
				return 0;
			}
			narg = 0;
        	break;
        }
	}
}

int runcommand(char **cline, int where){
	int pid;
	int status;
	switch(pid = fork()) {
	case -1:
		perror("smallsh");
		return(-1);
	case 0:
		//child process
		//for(int i=0; cline[i] != NULL; i++){
		//	fprintf(stderr, "cline[%d] = %s\n", i, cline[i]);
		//}
		if((strcmp(cline[0], "ls")) == 0 && cline[1] == NULL){
			//cline[0] = "usr_ls";
			usr_ls();
		}
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
	fprintf(stderr, "heheheheheheheheh\n");
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
