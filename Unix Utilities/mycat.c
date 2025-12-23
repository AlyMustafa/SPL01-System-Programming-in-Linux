#include  "stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define COUNT 100
int main(int argc , char** argv)
{
	char buf[COUNT];
	printf("argc = %d\n" , argc);
	if (argc <  2)
	{
		printf("Process %s Falid\n", argv[0]);
		exit(-1);
	}

	for(int i = 0 ; i<argc ; i++)
	{
		printf("argv[%d] = %s\n",i ,argv[i] );


	}


	int fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		printf("could not open the file!!\n");
		exit(-1);
	}
	int read_num;

	while ((read_num = read(fd, buf , COUNT)) > 0 )
	{
		//write
		if(write(1,buf,read_num) < 0 )  
			{
				printf("Write Failed!!\n");
				exit(-1);
			}
	}
	close(fd);
	
	return 0;

}
