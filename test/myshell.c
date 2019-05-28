#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/times.h>
#include <time.h>

#define MAX_SIZE 255
#define BUFSIZE 100

typedef struct history {
	char cmd[20];
} H;

pid_t wait(int *stat_loc);

char* my_getcwd(char* user_name);
void execute(char *cmd);
void mycd(int argc, char **args);
void mypwd(int argc, char **args);
void myecho(int argc, char **args);
void mymkdir(int argc, char **args);
void myrmdir(int argc, char **args);
void myhistory(int argc, H *hptr);
void myhelp(int argc);

static int  hisCnt = 0;
H *hptr;
char *rootCwd;

int main(int argc, char** argv, char** env){
	char line[MAX_SIZE];
	char line2[MAX_SIZE];
	char *cwd;
	int i;
	char user_name[MAX_SIZE];
	char host_name[MAX_SIZE];
	int status;
	int hcnt=0;
	pid_t pid;
	pid_t waitPid;

	hptr =(H *)malloc(sizeof(H) * MAX_SIZE);
	
	getlogin_r(user_name, MAX_SIZE);
	gethostname(host_name, MAX_SIZE);
	rootCwd = getcwd(NULL, BUFSIZE);

	myhelp(1);
	while(1){
			cwd=my_getcwd(user_name);
			fprintf(stdout, "%s@%s:%s$ ", user_name, host_name, cwd);
			if(!fgets(line, sizeof(line)-1, stdin)) break;
			if(strcmp(line, "\n")==0) continue;

			line[strlen(line) - 1] = '\0';
			strcpy(hptr[hisCnt++].cmd, line);
			
			execute(line);			
	}
	
	return EXIT_SUCCESS;
}

char* my_getcwd(char* user_name)
{
	char* cwd;
	char* home_path;
	int str_len;
	home_path = (char *)malloc(strlen(user_name) + strlen("/home/")) + 1;
	home_path[strlen(user_name) + strlen("/home/")] = '\0';
	sprintf(home_path, "/home/%s", user_name);
	cwd = getcwd(NULL, BUFSIZE);
	if(strstr(cwd, home_path) == NULL)
		return cwd;
	else{
		str_len = strlen(home_path);
		cwd[str_len-1] = '~';
		return &cwd[str_len-1];
	}
}

void execute(char *cmd){
	char *args[MAX_SIZE];
	char *history[MAX_SIZE];
	char* token;
	int argc = 0;
	int status;
	pid_t pid;
	pid_t waitPid;
	char *mycpCwd;
	char *mymvCwd;
	char *mydateCwd;

	token = strtok(cmd, " \n\0");
	while(token!=NULL){
		args[argc++] = token;
		token = strtok(NULL, " \n\0");
	}
	args[argc] = (char *)0;

	if(strcmp(args[0], "exit") == 0){
		puts("시스템 종료");
		exit(0);
	}
	else if(strcmp(args[0], "cd") == 0){
		mycd(argc, args);
		return;
	}
	else if(strcmp(args[0], "pwd") == 0){
		mypwd(argc, args);
		return;
	}
	else if(strcmp(args[0], "echo") == 0){
		myecho(argc, args);
		return;
	}
	else if(strcmp(args[0], "mkdir") == 0){
		mymkdir(argc, args);
		return;
	}
	else if(strcmp(args[0], "rmdir") == 0){
		myrmdir(argc, args);
		return;
	}
		else if(strcmp(args[0], "history") == 0){
		myhistory(argc, hptr); 
		return;
	}
	else if(strcmp(args[0], "help") == 0){
		myhelp(argc);
		return;
	}
	else{
		pid = fork();
		if(pid>0){
			while((((waitPid = wait(&status)) == -1) && errno ==
						EINTR));

			if(waitPid == -1){
				printf("에러 넘버: %d \n", errno);
				perror("wait 함수 오류 반환");
			}
			else{
				if(WIFSIGNALED(status)){
					printf("wait  : 자식 프로세스 비정상 종료 %d\n",
							WTERMSIG(status));
				}
			}
		}
		else if(pid == 0){
			if(strcmp(args[0],"cp") == 0){
				mycpCwd = (char *)malloc(strlen(rootCwd) + strlen("mycp")) + 1;
				mycpCwd[strlen(rootCwd) + strlen("mycp")] = '\0';
				sprintf(mycpCwd, "%s/%s", rootCwd, "mycp");
				if(execl(mycpCwd,"mycp",args[1], args[2], (char *)0) == -1)
					printf("execl오류 at mycp\n");
				free(mycpCwd);
			}
			else if(strcmp(args[0], "mv") == 0){
				if(argc != 3){
					perror("argument count error at mymv");
				}
				else{
					mymvCwd = (char *)malloc(strlen(rootCwd) + strlen("mymv")) + 1;
					mymvCwd[strlen(rootCwd) + strlen("mymv")] = '\0';
					sprintf(mymvCwd, "%s/%s", rootCwd, "mymv");
					if(execl(mymvCwd,"mymv",args[1], args[2], (char *)0) == -1)
						printf("execl오류 at mymv\n");
					free(mymvCwd);
				}
			}
			else if(strcmp(args[0], "date")==0){
				if(argc != 1){
					perror("argument count error at mymv");
				}
				else{
					mydateCwd = (char *)malloc(strlen(rootCwd) + strlen("mydate")) + 1;
					mydateCwd[strlen(rootCwd) + strlen("mydate")] = '\0';
					sprintf(mydateCwd, "%s/%s", rootCwd, "mydate");
					if(execl(mydateCwd,"mydate", (char *)0) == -1)
						printf("execl오류 at mydate\n");
					free(mydateCwd);
				}
			}
			else{
				if((execvp(args[0], args)) == -1)
					printf("execvp오류\n");
			}
		}
		else{
			perror("fork Fail!\n");
			return;
		}

		return;
	}

	printf("wrong cmd\n");
	return;
}
void mycd(int argc, char **args){
	char cwd[BUFSIZE];
	char user_name[MAX_SIZE];
	char *BUF;

	getlogin_r(user_name, MAX_SIZE);
	
	BUF = (char *)malloc(strlen("/home") + strlen(user_name)) + 1;
	BUF[strlen("/home") + strlen(user_name)] = '\0';
	sprintf(BUF, "%s/%s", "/home", user_name);

	if(argc==1){
		chdir(BUF);
		puts(getcwd(cwd, sizeof(cwd)));

	}
	else if(argc==2){
		if(!chdir(args[1])){
			if(getcwd(cwd, sizeof(cwd)) != NULL){
				puts(cwd);
			}
			else{
				perror("getcwd error at mycd");
			}
		}
		else{
			perror("not exist directory");
			return;		
		}
	}
	else{
		perror("argument count error at mycd");
		return;
	}
	return;
}


void mypwd(int argc, char **args){
	char pwd[BUFSIZE];

	if(argc != 1){
		perror("argc fault at mypwd");
		return;
	}
	else{
		puts(getcwd(pwd, sizeof(pwd))); 
		return;
	}
}

void myecho(int argc, char **args){
	int i;
	for(i = 1; i<argc; i++){
		printf("%s\n", args[i]);
	}
}

void mymkdir(int argc, char **args){
	int i=0;
	if(argc == 1)
	{
		perror("argument count error at mymkdir");
		return;
	}
	else{
		for(i=1; i<argc; i++){
			if(!mkdir(args[i], 0755))
				printf("made %s\n", args[i]);
			else
				perror("mkdir error at mymkdir");
		}

	}
}

void myrmdir(int argc, char **args){
	int i=0;
	if(argc == 1){
		perror("argument count error at myrmdir");
		return;
	}
	else{
		for(i=1; i<argc;i++){
			if(!rmdir(args[i]))
				printf("removed %s\n", args[i]);
			else
				perror("rmdir error at myrmdir");
		}
	}
	return;

}
void myhistory(int argc, H *hptr){
	int i=0;
	if(argc != 1){
		perror("argc count error at myhistory");
		return;
	}
	else{
		for(i=0;i<hisCnt;i++)
			printf("%d : %s\n", i+1, hptr[i].cmd);
	}
}

void myhelp(int argc){
	if(argc != 1){
		perror("argc count error at myhelp");
		return;
	}
	printf("-----------------Welcome to myShell------------------\n");
	puts("exit");
	puts("cd directory");
	puts("echo String");
	puts("mv sourceFile destinationFile");
	puts("cp sourceFild destinationFile");
	puts("mkdir directory");
	puts("rmdir directory");
	puts("history");
	puts("date");
	puts("pwd");
	puts("나머지 커맨드라인은 execvp함수를 통해서 사용가능");
	printf("-----------------------------------------------------\n");
	return;
}
