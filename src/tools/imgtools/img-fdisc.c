#include "img-common.h"

int main(int argc, char** argv)
{
	if(argc!=3)
	{
		printf("usage:\nimg-fdisc <imgfile> <numsectors>\n");
		return 2;
	}
	ffp=fopen(argv[1],"rb+");
	if(!ffp)
	{
		printf("failed to open file %s\n",argv[1]);
		return 3;
	}
	unsigned numsectors=atoi(argv[2]);

//	hdc_mount();

	//f_unlink(argv[2]);
	BYTE work[FF_MAX_SS];
	LBA_t plist[]={numsectors};
	f_fdisk(0,plist,work);

//	f_mount(0,"",0);

	fclose(ffp);
	return 0;
}
