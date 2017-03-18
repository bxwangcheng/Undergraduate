#include <stdio.h>

int main (void)
{
	FILE *fp;
	char ch;
	if((fp=fopen("foo.txt","r"))==NULL)
	{
		printf("Cannot open foo.txt. Exit..");
		return(-1);
	}
	while((ch=fgetc(fp))!=EOF)
		fputc(ch,stdout);
	fclose(fp);
	return(0);
}
