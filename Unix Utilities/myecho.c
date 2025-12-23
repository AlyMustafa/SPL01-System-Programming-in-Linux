#include  "stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char buf[100];

int main(int argc , char** argv)
{

	for(int i = 1 ; i<argc ; i++)
	{
		strcat(buf , argv[i]);
		if( i!=argc-1){
			strcat(buf," ");
		}

	}

	strcat(buf , "\n");
	
	printf("%s",buf);                 
	return 0;

}
