#include "img-common.h"


int main(int argc, char** argv)
{
	if(argc!=3)
	{
		printf("usage:\nimg-mkdir <imgfile> <file>\n");
		return 2;
	}
	ffp=fopen(argv[1],"rb+");
	if(!ffp)
	{
		printf("failed to open file %s\n",argv[1]);
		return 3;
	}
	
	
	if(hdc_mount())
	{
		fclose(ffp);
		printf("mount failed\n");
		return 4;
	}

	f_mkdir(argv[2]);

	char buff[256];
	strcpy(buff,"/");	
	scan_files(buff);

	f_mount(0,"",0);

	fclose(ffp);
	return 0;
}
