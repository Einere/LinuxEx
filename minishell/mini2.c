#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
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
		
		
        if (count < MAXBUF){
			//insert c at inpbuf one by one
			if(c == '\t'){
				//if c is tap
				//need to implement auto-complete
			}
			if(c == 127){
				//if c is back space
				if(count > 0){
					count--;
					inpbuf[count] = '\0';
					fprintf(stderr, "\b \b");
				}
			}
			else inpbuf[count++] = c;

			fprintf(stderr, "%c", c);
			//for(i = 0; inpbuf[i] != '\0'; i++){
			//	fprintf(stderr, "%c", inpbuf[i]);
			//}
		}
        if (c == '\n' && count < MAXBUF){
        	inpbuf[count] = '\0';
			for(int i=0; i<=count; i++){
				fprintf(stderr, "inpbuf[i] = %c\n", inpbuf[i]);
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
	while(*ptr == ' '|| *ptr == '\t') ptr++;
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
	default:
		type = ARG;
		while(inarg(*ptr)) *tok++ = *ptr++;
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
	int toktype;
	int narg;
	int type;
	narg = 0;
	for (;;){
		toktype = gettok(&arg[narg]);
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
		for(int i=0; cline[i] != NULL; i++){
			fprintf(stderr, "cline[%d] = %s\n", i, cline[i]);
		}
		if((strcmp(cline[0], "ls")) == 0 && cline[1] == NULL){
			//cline[0] = "usr_ls";
			usr_ls();
		}
		execvp(*cline, cline);
		perror(*cline);
		exit(1);
	}
	if(where == BACKGROUND) {
		printf("[Process id %d]\n", pid);
		return(0);
	}
	if(waitpid(pid, &status, 0) == -1)	return(-1);
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
	while(userin(prompt) != EOF)
	procline();
}
