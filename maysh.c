#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE     16	// space for storing each space-separated commands
#define SPACE_SEP_WORDS 8	// space for space-separated words

int inputStream(char *, int *);
int parseStream(char *[], char *, int);
int checkExit(char *);
void strncopy(char [], char [], int);
void strcopy(char [], char []);
void executeCMD_extern(char *[], int);
void executeCMD_intern(char *[], int);
void executeCMD_cd(char *[], int);
void executeCMD_history(char *[], int);
void executeCMD_pwd(char *[], int);
void executeHelp(char *[], int);

int main(int argc, char **argv){

	char *line = (char *) malloc(sizeof(char) * (SPACE_SEP_WORDS * BUFFER_SIZE));
	int loopBreakFlag = 0;

	while(loopBreakFlag == 0){
		char file_to_home[128];
		strcat(strcpy(file_to_home, getenv("HOME")), "/.maysh_history");
		FILE *fhandle = fopen(file_to_home, "a");

		char *tmp = (char *)malloc(sizeof(char) * 1024);
		printf("%s@maysh:%s\n$ ", getlogin(), getcwd(tmp, 1024));
		char *lineptr[SPACE_SEP_WORDS];

		int linelen, lineptrlen;

		if((linelen = inputStream(line, &loopBreakFlag)) > 0 ) {
			fprintf(fhandle, "%s\n", line);
			fclose(fhandle);
			if(checkExit(line) != 0 && loopBreakFlag != 1){
				lineptrlen = parseStream(lineptr, line, linelen);
				if(!strcmp("cd", lineptr[0]) || !strcmp("history", lineptr[0]) || 
						!strcmp("pwd", lineptr[0]) || !strcmp("help", lineptr[0])){
					executeCMD_intern(lineptr, lineptrlen);
				}
				else
					executeCMD_extern(lineptr, lineptrlen);
			} else 
				break;
		}
		for(int i=0; i < lineptrlen; ++i)
			free(lineptr[i]);
	}

	return 0;
}

void executeCMD_extern(char *lineptr[], int lineptrlen){
	if(fork() == 0)
		if(execvp(lineptr[0], lineptr) < 0){
			fprintf(stderr, "error: %s not found\n", lineptr[0]);
			exit(1);
		}
		else
			wait(NULL);
	else{
		wait(NULL);
	}
}

void executeCMD_intern(char *lineptr[], int lineptrlen){
	if(!strcmp(lineptr[0], "cd"))
		executeCMD_cd(lineptr, lineptrlen);
	else if(!strcmp(lineptr[0], "history"))
		executeCMD_history(lineptr, lineptrlen);
	else if(!strcmp(lineptr[0], "pwd"))
		executeCMD_pwd(lineptr, lineptrlen);
	else if(!strcmp(lineptr[0], "help"))
		executeHelp(lineptr, lineptrlen);
}

void executeCMD_cd(char *lineptr[], int lineptrlen){
	if(lineptrlen == 1)
		chdir(getenv("HOME"));
	else if(lineptrlen == 2)
		chdir(lineptr[1]);
	else
		fprintf(stderr, "error: unexpected arguments to %s\n", lineptr[0]);
}

void executeCMD_history(char *lineptr[], int lineptrlen){
	char file_to_home[128];
	strcat(strcpy(file_to_home, getenv("HOME")), "/.maysh_history");
	if(lineptrlen == 1){
		char data;		
		FILE *fhandle = fopen(file_to_home, "r");

		while((data = fgetc(fhandle)) != EOF)
			fprintf(stdout, "%c", data);
		fclose(fhandle);
	} else if(lineptrlen == 2){
		if(!strcmp(lineptr[1], "-c")){
			FILE *fhandle = fopen(file_to_home, "w");
			fclose(fhandle);
		} else {
			fprintf(stderr, "history: %s not found\n", lineptr[1]);
		}
	} else {
		fprintf(stderr, "history: unknown options\n");
	}

}

void executeCMD_pwd(char *lineptr[], int lineptrlen){
	char *tmp = (char *)malloc(sizeof(char) * 1024);
	if(lineptrlen == 1){
		fprintf(stdout, "%s\n", getcwd(tmp, 1024));
	} else if(lineptrlen == 2){
       		if(!strcmp(lineptr[1], "-P")){
			fprintf(stdout, "%s\n", getcwd(tmp, 1024));
		} else if(!strcmp(lineptr[1], "-L")){
			fprintf(stdout, "%s\n", getcwd(tmp, 1024));
		} else {
			fprintf(stderr, "%s: undefined argument %s\n", lineptr[0], lineptr[1]);
		}
	} else {
		fprintf(stderr, "pwd: undefined argument(s)\n");
	}
	free(tmp);
}

void executeHelp(char *lineptr[], int lineptrlen){
	printf("maysh version-0.1 - A Linux Shell\n"
			"List of implemented internal commands:\n"
			"\t1. cd [-LP] [dir]\n"
			"\t2. exit\n"
			"\t3. history [-c]\n"
			"\t4. pwd [-LP]\n"
			"\t5. echo [-neE] [arg ...]\n"
			"All external commands in linux are supported\n"
			"in maysh (provided there exists a binary of the command in /usr/bin/).\n"
			"\nA little bit of backstory - The name `maysh` is actually inspired by a\n"
			"character namely `May Kasahara` in Haruki Murakami's The Wind-up Bird Chronicle!\n");
}

int inputStream(char *line, int *flag){
	/* Takes a pointer to line
	 * allocates space to that pointer as buffer
	 * reads input from stdin
	 * writes the input in line
	 *
	 * flag is 1 if EOF is encountered until then 0
	 *
	 * RETURNS : length of line, otherwise -1 if "exit" or EOF
	 */

	int c, index = 0;

	while(((c = getchar()) != EOF) && (c != '\n')){
		if(c == '\t')
			c = ' ';
		line[index++] = c;
	}

	if(c == EOF || c == '\n'){
		if(c == EOF)
			*flag = 1;
		line[index++] = '\0';
	}

	return index;
}

int parseStream(char *lineptr[], char *line, int linelen){
	int index = 0;

	char *sptr, *ptr1, *ptr2;
	sptr = line;
	ptr1 = line;
	ptr2 = line;

	while((ptr2 - sptr) < linelen){
		char tmp;
		while((tmp = *ptr2) == ' ' || tmp == '\0')
			++ptr2;
		ptr1 = ptr2;
		
		while((tmp = *ptr2++) != ' ' && tmp != '\0');

		int bytes = (ptr2 - ptr1) * sizeof(char);
		lineptr[index] = (char *) malloc(sizeof(char) * (bytes - 1));
		strncopy(lineptr[index++], ptr1, bytes - 1);

		while((tmp = *ptr2) == ' ' || tmp == '\0')
			++ptr2;
		ptr1 = ptr2;
	}

	lineptr[index] = NULL;
	
	return index;
}

void strcopy(char to[], char from[]){
	int i = 0;

	while((to[i] = from[i]) != '\0')
		++i;
}

void strncopy(char to[], char from[], int len){
	int i = 0;

	while((i < len) && ((to[i] = from[i]) != '\0'))
		++i;
	if(i == len){
		to[i] = '\0';
	}
}

int checkExit(char *line){
	char exit_cmd[4] = "exit";

	int status = strcmp(line, exit_cmd);
	return status;
}
