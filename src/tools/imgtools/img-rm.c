#include "img-common.h"


int main(int argc, char** argv)
{
	if(argc!=3)
	{
		printf("usage:\nimg-rm <imgfile> <file>\n");
		return 2;
	}
	ffp=fopen(argv[1],"rb+");
	if(!ffp)
	{
		printf("failed to open file %s\n",argv[1]);
		return 3;
	}
	
	hdc_mount();

	f_unlink(argv[2]);


	f_mount(0,"",0);

	fclose(ffp);
	return 0;
}
