#include "img-common.h"

int copy_image_to_host(const char* sourcefile, const char* destfile)
{
	FIL* sfp=malloc(sizeof(FIL));
	FRESULT r=f_open(sfp,sourcefile,FA_READ);
	if(r)
	{
		free(sfp);
		return false;
	}
	FILE *dfp=fopen(destfile,"wb");
	unsigned srclen=f_size(sfp);
	unsigned char* buffer=malloc(srclen);
	unsigned br=0;
	f_read(sfp,buffer,srclen,&br);
	f_close(sfp);
	free(sfp);
	fwrite(buffer,1,srclen,dfp);
	fclose(dfp);
	return true;
}

int main(int argc, char** argv)
{
	if(argc!=3)
	{
		printf("usage:\nimg-get <imgfile> <file>\n");
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

	copy_image_to_host(argv[2],argv[2]);


	f_mount(0,"",0);

	fclose(ffp);
	return 0;
}
