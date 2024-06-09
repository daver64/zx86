#include "img-common.h"

int copy_host_to_image(const char* sourcefile, const char* destfile)
{
	FILE* sfp=fopen(sourcefile,"rb");
	if(!sfp)
		return false;
	FIL* dfp=malloc(sizeof(FIL));
	FRESULT r=f_open(dfp,destfile,FA_WRITE|FA_CREATE_ALWAYS);
	if(r)
	{
		free(dfp);
		fclose(sfp);
		return false;
	}

	
	fseek(sfp,0,SEEK_END);
	unsigned srclen=ftell(sfp);
	fseek(sfp,0,SEEK_SET);
	//printf("source length=%u bytes\n",srclen);
	unsigned char* srcbuffer=malloc(srclen);
	fread(srcbuffer,1,srclen,sfp);
	fclose(sfp);
	unsigned bw=0;
	f_write(dfp,srcbuffer,srclen,&bw);
	f_close(dfp);
	free(dfp);
	free(srcbuffer);
	char buff[256];
	strcpy(buff,"/");	
	scan_files(buff);
	return true;
}

int main(int argc, char** argv)
{
	if(argc!=3)
	{
		printf("usage:\nimg-put <imgfile> <file>\n");
		return 2;
	}
	ffp=fopen(argv[1],"rb+");
	if(!ffp)
	{
		printf("failed to open file %s\n",argv[1]);
		return 3;
	}
	
	hdc_mount();

	copy_host_to_image(argv[2],argv[2]);


	f_mount(0,"",0);

	fclose(ffp);
	return 0;
}
