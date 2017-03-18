#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFFERSIZE 1024

int main(int argc, char **argv)
{
	int from_fd,to_fd;
	int bytes_read,bytes_write;
	char buffer[BUFFERSIZE];
	char *ptr;

	if (argc!=3)
	{
		fprintf(stderr,"Usage: %s fromfile tofile\n\a",argv[0]);
		exit(1);
	}

	if ((from_fd=open(argv[1],O_RDONLY))==-1)
	{
		fprintf(stderr,"Open %s Error: %s",argv[1],strerror(errno));
		exit(1);
	}

	if ((to_fd=open(argv[2],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1)
        {
                fprintf(stderr,"Open %s Error: %s",argv[2],strerror(errno));
                exit(1);
        }

	while (bytes_read=read(from_fd,buffer,BUFFERSIZE))
	{
		if ((bytes_read==-1)&&(errno!=EINTR)) break;
		else if (bytes_read>0)
		{
			ptr=buffer;
			while (bytes_write=write(to_fd,ptr,bytes_read))
			{
				if ((bytes_write==-1)&&(errno!=EINTR)) break;
				else if (bytes_write==bytes_read) break;
				else if (bytes_write>0)
				{
					ptr+=bytes_write;
					bytes_read-=bytes_write;
				}
			}
			if (bytes_write==-1) break;
		}
	}
	close(from_fd);
	close(to_fd);
	exit(0);
}
