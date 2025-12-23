#include  "stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define COUNT 100
int main(int argc , char** argv)
{
	char buf[COUNT];
	printf("argc = %d\n" , argc);
	if (argc <  3)
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
		exit(-2);
	}

	int fd_dis = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC ,0644);
	//O_TRUNC to override the distenation file...

	if (fd_dis < 0)
        {
                printf("could not open the distenation file!!\n");
                exit(-3);
        }

	int read_num;

	while ((read_num = read(fd, buf , COUNT)) > 0 )
	{
		//write
		if(write(fd_dis,buf,read_num) < 0 )  
			{
				printf("Write Failed!!\n");
				exit(-4);
			}
	}
	close(fd);
        close(fd_dis);

	
	return 0;

}
