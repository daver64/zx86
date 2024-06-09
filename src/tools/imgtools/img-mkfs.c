#include "img-common.h"

int main(int argc, char** argv)
{
	if(argc!=2)
	{
		printf("usage:\nimg-mkfs <imgfile>\n");
		return 2;
	}
	ffp=fopen(argv[1],"rb+");
	if(!ffp)
	{
		printf("failed to open file %s\n",argv[1]);
		return 3;
	}

//	hdc_mount();

	//f_unlink(argv[2]);
	BYTE work[FF_MAX_SS];
//	LBA_t plist[]={pls};
	f_mkfs("0:",0,work,sizeof work);

//	f_mount(0,"",0);

	fclose(ffp);
	return 0;
}
