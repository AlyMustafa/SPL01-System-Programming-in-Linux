#include  "stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;


int main(int argc , char** argv)
{

	printf("Hello World!!\n");
/*
	for(int i = 0 ; i<argc ; i++)
        {
                printf("argv[%d] = %s\n",i ,argv[i] );


        }
	*/
	

for(int i = 0 ; environ[i] ; i++)
        {
                printf("env[%d] = %s\n",i ,environ[i] );


        }


	return 0;
}

