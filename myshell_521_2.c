#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_SIZE 255
#define BUFSIZE 100

char* my_getcwd(char* user_name);
void execute(char *cmd);
void mycd(int argc, char **args);
void mypwd(int argc, char **args);
void myecho(int argc, char **args);
void mymkdir(int argc, char **args);
void myrmdir(int argc, char **args);
void mymv(int argc, char **args);
void mycp(int argc, char **args);




int main(int argc, char** argv, char** env){
    char line[MAX_SIZE];
    char *cwd;
    char user_name[MAX_SIZE];
    char host_name[MAX_SIZE];

    getlogin_r(user_name, MAX_SIZE);
    gethostname(host_name, MAX_SIZE);
    while(1){
            cwd = my_getcwd(user_name);
            fprintf(stdout, "%s@%s:%s$ ", user_name, host_name, cwd);
            if(!fgets(line, sizeof(line) -1, stdin)) break;
            if(strcmp(line, "\n") == 0) continue;

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
	char* token;
	int argc = 0;
		
	cmd[strlen(cmd) - 1] = '\0';
	
	token = strtok(cmd, " \n\0");
	while(token!=NULL){
		args[argc++] = token;
		token = strtok(NULL, " \n\0");
	}
	//printf("%s %s %d\n", args[0], args[1], argc);
	
	
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
                wfd = open(args[2], O_CREAT | O_WRONLY);

                if(rfd == -1){
                        printf("args[1]: %s file open error\n", args[1]);
                        return;
                }
                else if(wfd == -1){
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
		wfd = open(args[2], O_CREAT | O_WRONLY);
		
		if(rfd == -1){
			printf("args[1]: %s file open error\n", args[1]);
			return;
		}
		else if(wfd == -1){
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




