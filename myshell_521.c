#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE 255
#define BUFSIZE 100

char* my_getcwd(char* user_name);
void execute(char *cmd);

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
	
	while(1) {
		cmd[strlen(cmd) - 1] = '\0';
		//cd
		token = strtok(cmd, " \n");
		while(token!=NULL){
			args[argc++] = token;
			token = strtok(NULL, " \n");
		}
		printf("%s, %s, %d\n", args[0], args[1], argc);
		break;
	}
	
	
	
//	
//    if(strcmp(args[0], "exit") == 0){
//        exit(0);
//    }
//    else if{
//    }
}

