#include  "stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


#define MAX_ARGS 20
#define buf_size 200

extern char **environ;

char *my_argv[MAX_ARGS];
char vals[100][50];
char keys[100][50];
char copybuf[100];
char buf[buf_size];


char *key;
char *val;
char *lclVar;

char *O_file;
char *I_file;
char *err_file;

int count = 0;
int IO_chk = 0;

int fd_OUT;
int fd_error;
int fd_INPUT;

int savedfd_error = 0;		// copy stderr for ability to restore it when redirection ends
int savedfd_INPUT = 0;		// copy stdin for ability to restore it when redirection ends
int savedfd_OUTPUT = 0;		// copy stdout for ability to restore it when redirection ends


void new_process(char *buffer, char *args);
int cd_util(char *path);

int Parse_Buf(char *buffer, char **argv);
int Io_dir();
int var_hndl();

//Mario Shell Utilites
int Printenv();
int ls();
int pwd();
int echo();
int export();
int which();
int cd();
int Exit();

//Mario main Program
int chk_cmd();


int main(int argc, char **argv)
{

    while (1) {

	printf("Mario Shell Prompt $ ");
	fgets(buf, buf_size, stdin);
	buf[strlen(buf) - 1] = 0;

	if (strlen(buf) == 0)
	    continue;

	strcpy(copybuf, buf);
	Parse_Buf(copybuf, my_argv);
	IO_chk = Io_dir();

	//_________________             Check IO_Redirection    _________________//


	//_________________             INPUT   _________________//

	if (IO_chk == 1) {
	    savedfd_INPUT = dup(0);	// copy stdout for ability to restore it when redirection ends

	    int fd_INPUT =
		open(I_file, O_RDONLY | O_CREAT | O_TRUNC, 0644);

	    if (fd_INPUT < 0)
		return 0;

	    dup2(fd_INPUT, 0);
	    close(fd_INPUT);
	}
	//_______________________________________________//


	//_________________             OUTPUT  _________________//

	if (IO_chk == 2) {
	    savedfd_OUTPUT = dup(1);	// copy stdout for ability to restore it when redirection ends

	    int fd_OUT = open(O_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	    if (fd_OUT < 0)
		return 0;

	    dup2(fd_OUT, 1);
	    close(fd_OUT);
	}
	//_______________________________________________//


	//_________________             ERROR   _________________//

	else if (IO_chk == 3) {
	    savedfd_error = dup(2);	// copy stderr for ability to restore it when redirection ends

	    int fd_error =
		open(err_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	    if (fd_error < 0)
		return 0;

	    dup2(fd_error, 2);
	    close(fd_error);
	}
	//_______________________________________________//


	//_________________             Run The whole Program   _________________//

	if (!chk_cmd())
	    write(2, "Invalid command\n", 16);
	//_________________     //_________________     //_________________     //



	//_________________             Restoring the whole streams             _________//

	if (IO_chk == 1) {
	    dup2(savedfd_INPUT, 0);
	    close(savedfd_INPUT);
	}

	if (IO_chk == 2) {
	    dup2(savedfd_OUTPUT, 1);
	    close(savedfd_OUTPUT);
	}

	if (IO_chk == 3) {
	    dup2(savedfd_error, 2);
	    close(savedfd_error);
	}
	//_________________     //_________________     //_________________     //

    }
    return 0;
}



	/********************************************************************************/
	/*******************************     chk_cmd    *********************************/
	/********************************************************************************/

int chk_cmd()
{
    int err = 0;
	/********************************     var_hndl      ********************************/
    err |= var_hndl();
	/********************************     echo     		********************************/
    err |= echo();
	/********************************     pwd     		********************************/
    err |= pwd();
	/********************************     ls     		********************************/
    err |= ls();
	/********************************    export     	********************************/
    err |= export();
	/********************************     which     	********************************/
    err |= which();
	/********************************     Printenv      ********************************/
    err |= Printenv();
	/********************************     cd     		********************************/
    err |= cd();
	/********************************     exit      	********************************/
    err |= Exit();

    if (!err)
	return 0;
    else
	return 1;
}

	/********************************************************************************/
	/*****************************     Parse_Buf    *********************************/
	/********************************************************************************/

int Parse_Buf(char *buffer, char **argv)
{
    int i = 0;
    char *token = strtok(buffer, " ");

    while (token && MAX_ARGS - 1) {

	my_argv[i++] = token;
	token = strtok(NULL, " ");
    }
    my_argv[i] = NULL;
}

	/********************************************************************************/
	/********************************     Io_dir    *********************************/
	/********************************************************************************/

int Io_dir()
{
    for (int i = 1; my_argv[i]; i++) {

	//------------  INPUT   ------------//
	if (strcmp(my_argv[i], "<") == 0) {
	    my_argv[i] = NULL;	// cut argv here

	    if (my_argv[i + 1]) {
		I_file = my_argv[i + 1];
		return 1;
	    } else {
		printf("syntax error: expected file after '2>' ");
		return 0;
	    }
	}
	//------------  OUTPUT  ------------//
	if (strcmp(my_argv[i], ">") == 0) {
	    my_argv[i] = NULL;	// cut argv here

	    if (my_argv[i + 1]) {
		O_file = my_argv[i + 1];
		return 2;
	    } else {
		printf("syntax error: expected file after '>' ");
		return 0;
	    }
	}
	//------------  ERROR   ------------//
	if (strcmp(my_argv[i], "2>") == 0) {
	    my_argv[i] = NULL;	// cut argv here

	    if (my_argv[i + 1]) {
		err_file = my_argv[i + 1];
		return 3;
	    } else {
		printf("syntax error: expected file after '2>' ");
		return 0;
	    }
	}
    }

    return 0;
}

/****************************************************************************************************************/
/************************************************	New Process Fn		*****************************************/
/****************************************************************************************************************/

void new_process(char *buffer, char *args)
{
    pid_t pid = fork();

    int status;
	/***********************************************************************************/
	/**********************************     PARENT     *********************************/
	/***********************************************************************************/
    if (pid > 0) {

	//printf("PARENT PID : %d , My CHILD PID : %d \n", getpid(), pid);
	wait(&status);
	//printf("PARENT PID : %d , My CHILD STATUS : %d\n ", getpid(), WEXITSTATUS(status));
    }

	/**********************************************************************************/
	/**********************************		CHILD     *********************************/
	/**********************************************************************************/
    else if (pid == 0) {
	char *newargv[] = { buffer, args, NULL };

	execve(buffer, newargv, environ);
	printf("Execution failed");
	exit(-1);
    }

    else
	printf("Parent Failed to fork !!");

}

/************************************************************************************************************************/
/************************************************************************************************************************/
/************************************************          cd Utility Fn          ***************************************/
/************************************************************************************************************************/
/************************************************************************************************************************/
int cd_util(char *path)
{
    if (path == NULL)
	path = getenv("HOME");
    if (path == "-")
	path = getenv("OLDPWD");

    char currentPwd[100];	//for saving current path to OLDPWD
    getcwd(currentPwd, sizeof(currentPwd));


    if (chdir(path) != 0) {
	perror("cd");
	return (-1);
    }

    setenv("OLDPWD", currentPwd, 1);

    char newPwd[100];		//for saving new path to PWD
    getcwd(newPwd, sizeof(newPwd));
    setenv("PWD", newPwd, 1);

    return 0;
}

/************************************************************************************************************************/
/************************************************************************************************************************/
/************************************************          var_hndl_Fn          ***************************************/
/************************************************************************************************************************/
/************************************************************************************************************************/
int var_hndl(void)
{
    if (!strchr(my_argv[0], '='))
	return 0;

    char *eq = strchr(my_argv[0], '=');
    *eq = '\0';

    strcpy(keys[count], my_argv[0]);
    strcpy(vals[count], eq + 1);
    count++;
    return 1;
}


	/********************************************************************************/
	/*****************************     Printenv     *********************************/
	/********************************************************************************/
int Printenv()
{

    if (my_argv[0] == NULL || strcmp(my_argv[0], "printenv") != 0) {
	return 0;
    }
    for (int i = 0; environ[i]; i++) {
	printf("env[%d] = %s\n", i, environ[i]);
    }
    return 1;
}

	/********************************************************************************/
	/**********************************     ls     *********************************/
	/********************************************************************************/

int ls()
{
    char path[100] = "/usr/bin/ls";



    if (my_argv[0] == NULL || strcmp(my_argv[0], "ls") != 0) {
	return 0;
    }

    int i = 1;

    while (my_argv[0]) {

	if (my_argv[i] && strcmp(my_argv[i], "-l") == 0
	    && my_argv[i + 1] == NULL) {
	    new_process(path, "-l");
	    return 1;
	}

	if (my_argv[i] && strcmp(my_argv[i], "/") == 0
	    && my_argv[i + 1] == NULL) {
	    new_process(path, "/");
	    return 1;
	}

	if (my_argv[i] == NULL) {
	    new_process(path, NULL);
	    return 1;

	}

	i++;

    }

    return 0;
}

	/********************************************************************************/
	/*********************************     echo     *********************************/
	/********************************************************************************/

int echo()
{
    if (my_argv[0] == NULL || strcmp(my_argv[0], "echo") != 0) {
	return 0;
    }


    int counter = 1;



    while (my_argv[counter]) {

	if (my_argv[counter][0] == '$') {

	    // For Local Variables
	    for (int i = 0; i < count; i++) {
		if (strcmp(my_argv[counter] + 1, keys[i]) == 0) {
		    printf("%s\n", vals[i]);
		    return 1;
		}
	    }

	    // For environments Variables
	    char *env = getenv(my_argv[counter] + 1);
	    if (env) {
//                write(1, val, strlen(val));
		printf("%s\n", env);
		return 1;
	    }
	}

	else
	    printf("%s ", my_argv[counter]);

	counter++;
    }

    printf("\n");
    return 1;
}


	/********************************************************************************/
	/*******************************     export     *********************************/
	/********************************************************************************/
int export()
{
    if (my_argv[0] == NULL || strcmp(my_argv[0], "export") != 0) {
	return 0;
    }

    if (my_argv[1] != NULL) {
	char *NewEnv = malloc(strlen(my_argv[1]) + 1);
	strcpy(NewEnv, my_argv[1]);
	putenv(NewEnv);
	return 1;
	//var_hndl();
    } else
	return 0;
}

	/********************************************************************************/
	/********************************     which     *********************************/
	/********************************************************************************/
int which()
{
    if (my_argv[0] == NULL || strcmp(my_argv[0], "which") != 0) {
	return 0;
    }

    if (my_argv[1] != NULL) {
	char path[50] = "/usr/bin/which";
	new_process(path, my_argv[1]);
	return 1;
    } else
	return 0;
}

	/********************************************************************************/
	/********************************     cd     ************************************/
	/********************************************************************************/
int cd()
{
    if (my_argv[0] == NULL || strcmp(my_argv[0], "cd") != 0) {
	return 0;
    }

    if (my_argv[1] != NULL) {
	cd_util(my_argv[1]);
	return 1;

    }

    else
	return 0;

}

	/********************************************************************************/
	/**********************************     pwd     *********************************/
	/********************************************************************************/
int pwd()
{
    if (my_argv[0] == NULL || strcmp(my_argv[0], "pwd") != 0) {
	return 0;
    }

    char *PWD = getenv("PWD");
    printf("%s\n", PWD);
    return 1;

}

	/********************************************************************************/
	/**********************************     exit     ********************************/
	/********************************************************************************/
int Exit()
{
    if (my_argv[0] == NULL || strcmp(my_argv[0], "exit") != 0) {
	return 0;
    }
    printf("Bye!\n");
    exit(0);
}
