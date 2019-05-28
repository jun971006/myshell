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

#define MAX_SIZE 255
#define BUFSIZE 100

typedef struct history {
	int line;
	char *cmd;
} H;



pid_t wait(int *stat_loc);

char* my_getcwd(char* user_name);
void execute(char *cmd);
void mycd(int argc, char **args);
void mypwd(int argc, char **args);
void myecho(int argc, char **args);
void mymkdir(int argc, char **args);
void myrmdir(int argc, char **args);
void mymv(int argc, char **args);
void mycp(int argc, char **args);
void myhistory(int argc, char **args);

static int  hisCnt = 0;

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

	getlogin_r(user_name, MAX_SIZE);
	gethostname(host_name, MAX_SIZE);
	while(1){
		cwd = my_getcwd(user_name);
		fprintf(stdout, "%s@%s:%s$ ", user_name, host_name, cwd);
		fgets(line, sizeof(line)-1, stdin);
		if(strcmp(line, "\n")==0) continue;
		
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
	//strcpy(history[hisCnt++], cmd);
	//printf("%s %s\n", history[hisCnt], cmd);

	cmd[strlen(cmd) - 1] = '\0';

	token = strtok(cmd, " \n\0");
	while(token!=NULL){
		args[argc++] = token;
		token = strtok(NULL, " \n\0");
	}

	pid=fork();
	if(pid>0){
		printf("부모 PID : %ld, pid : %d %d \n", (long)getpid(),
				pid, errno);
		while((((waitPid = wait(&status)) == -1) && errno == EINTR));

		if(waitPid == -1){
			printf("에러 넘버 : %d \n", errno);
			perror("wait 함수 오류 반환");
		}
		else{
			if(WIFEXITED(status)){
				printf("wait : 자식 프로세스 정상 종료 %d\n",
						WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status)){
				printf("wait : 자식 프로세스 비정상 종료 %d\n",
						WTERMSIG(status));
			}
		}

		printf("부모 종료 %d %d\n", waitPid, WTERMSIG(status));
	}
	else if(pid == 0){
		printf("자식 PID : %ld \n", (long)getpid());
		if(strcmp(args[0], "exit") == 0){
			puts("시스템 종료");
			exit(0);
		}
		else if(strcmp(args[0], "cd") == 0){
			mycd(argc, args);
		}
		else if(strcmp(args[0], "ls") == 0){
			system(args[0]);
		}
		else if(strcmp(args[0], "pwd") == 0){
			mypwd(argc, args);
		}
		else if(strcmp(args[0], "echo") == 0){
			myecho(argc, args);
		}
		else if(strcmp(args[0], "mkdir") == 0){
			mymkdir(argc, args);
		}
		else if(strcmp(args[0], "rmdir") == 0){
			myrmdir(argc, args);
		}
		else if(strcmp(args[0], "cp") == 0){
			mycp(argc, args);
		}
		else if(strcmp(args[0], "mv") == 0){
			mymv(argc, args);
		}
		else if(strcmp(args[0], "history") == 0){
			//	myhistory(argc, 
		}

			exit(2);
		}
		else{
			perror("fork Fail! \n");
			//return -1;
		}

	

	return;
}
void mycd(int argc, char **args){
	char cwd[BUFSIZE];

	if(argc==1){
		chdir("/");
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
	if(argc != 2)
	{
		perror("argument count error at mymkdir");
		return;
	}
	else{
		if(!mkdir(args[1], 0755))
			printf("made %s\n", args[1]);
		else
			perror("mkdir error at mymkdir");

	}
}

void myrmdir(int argc, char **args){
	if(argc != 2){
		perror("argument count error at myrmdir");
		return;
	}
	else{
		if(!rmdir(args[1]))
			printf("removed %s\n", args[1]);
		else
			perror("rmdir error at myrmdir");
	}
	return;

}

void mymv(int argc, char **args){
	int rfd, wfd, n;
	char buf[2];
	struct stat statBuf;

	if(argc!=3){
		perror("argument count error at mymv");
		return;
	}
	else{
		rfd = open(args[1], O_RDONLY);

		if(rfd == -1){
			printf("args[1]: %s file open error\n", args[1]);
			return;
		}

		wfd = open(args[2], O_CREAT | O_WRONLY);

		if(wfd == -1){
			printf("args[2]: %s file open error\n", args[2]);
			return;
		}
		while ((n=read(rfd, buf, 1))>0)
			if(write(wfd, buf, n) != n) perror("Write error at mymv");

		if(n == -1){
			perror("Read error");
			return;
		}
		fstat(rfd, &statBuf);
		fchmod(wfd, statBuf.st_mode);

		remove(args[1]);

		close(rfd);
		close(wfd);

		return;
	}
}

void mycp(int argc, char **args){
	int rfd, wfd, n;
	char buf[2];
	struct stat statBuf;

	if(argc!=3){
		perror("argument count error at mycp");
		return;
	}
	else{
		rfd = open(args[1], O_RDONLY);

		if(rfd == -1){
			printf("args[1]: %s file open error\n", args[1]);
			return;
		}
		
		wfd = open(args[2], O_CREAT | O_WRONLY);
		if(wfd == -1){
			printf("args[2]: %s file open error\n", args[2]);
			return;
		}
		while ((n=read(rfd, buf, 1))>0)
			if(write(wfd, buf, n) != n) perror("Write error at mycp");

		if(n == -1){ 
			perror("Read error");
			return;
		}
		fstat(rfd, &statBuf);
		fchmod(wfd, statBuf.st_mode);

		close(rfd);
		close(wfd);

		return;
	}
}

void myhistory(int argc, char **args){
	
}



