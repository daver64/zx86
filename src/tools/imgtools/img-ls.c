#include "img-common.h"

int main(int argc, char** argv)
{
	if(argc<2)
	{
		printf("usage:\nimg-ls <imgfile> <file>\n");
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

	//copy_image_to_host(argv[2],argv[2]);
	char buff[256];
	if(argc<3)
		strcpy(buff,"/");	
	else
		strcpy(buff,argv[2]);
	scan_files(buff);

	f_mount(0,"",0);

	fclose(ffp);
	return 0;
}
