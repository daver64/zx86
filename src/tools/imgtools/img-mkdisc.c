#include "img-common.h"

int main(int argc, char** argv)
{
	int32_t nbr=0;
	FILE* fp;
	if(argc!=3 )
	{
		fprintf(stdout,"usage:\nimg-mkdisc <outputfilename> <sectorcount>\n");
		return 1;
	}
	unsigned numsectors=atoi(argv[2]);
	fp=fopen(argv[1],"wb");
	if(fp)
	{
		_fseeki64(fp,(numsectors*512)-1,SEEK_SET);
		char b=0;
		fwrite(&b,1,1,fp);
		fflush(fp);
		fclose(fp);
	}
	char cmd[256];
	sprintf(cmd,"img-fdisc %s %d\n",argv[1],numsectors);
	system(cmd);
	sprintf(cmd,"img-mkfs %s\n",argv[1]);
	system(cmd);

	return 0;
}
