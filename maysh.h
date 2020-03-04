/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>

#define BUFFER_SIZE     16	// space for storing each space-separated commands
#define SPACE_SEP_WORDS 8	// space for space-separated words

/* Function declarations */
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

