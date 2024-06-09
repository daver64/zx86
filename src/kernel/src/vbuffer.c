#include <system.h>

static int screenw;
static int screenh;
static int vbmaxlen;

void vbuffer_init()
{
	get_screen_size(&screenw,&screenh);
	vbmaxlen=screenw*screenh;
}


vbuffer_t* vbuffer_create()
{
	vbuffer_t* vb=malloc(sizeof(vbuffer_t));
	vb->ptr=malloc(screenw*screenh*sizeof(unsigned short));
	vb->write_ptr=vb->ptr;
	vb->len=0;
	return vb;
}
void vbuffer_reset(vbuffer_t* vb)
{
	vb->write_ptr=vb->ptr;
	vb->len=0;
}

void vbuffer_append(vbuffer_t* vb, char c, int fgcolour, int bgcolour)
{
	if(vb->len>vbmaxlen)
		return;
	int attrib=((bgcolour<<4) | (fgcolour & 0x0f)) << 8;
	*vb->write_ptr = c | attrib;
	vb->len++;
	vb->write_ptr++;
}

void vbuffer_fill(vbuffer_t* vb, char c, int fgcolour,int bgcolour)
{
	vbuffer_reset(vb);
	int vblen=screenw*screenh*sizeof(unsigned short);
	for(int i=0;i<vblen;i++)
	{
		vbuffer_append(vb,c,fgcolour,bgcolour);
	}
}
void vbuffer_draw(vbuffer_t* vb)
{
	unsigned short* vmem=video_mem_ptr();
	memcpy(vmem,vb->ptr,vb->len*sizeof(unsigned short));
}

void vbuffer_print_at(vbuffer_t* vb, int x, int y, char c, int fgcolour, int bgcolour)
{
	unsigned short* vp=vb->ptr + (y*80 + x);
	int attrib=((bgcolour<<4) | (fgcolour & 0x0f)) << 8;
	*vp=attrib | c;
}

void vbuffer_print_string(vbuffer_t* vb, int x, int y, const char *str, int fgcolour, int bgcolour)
{
	size_t len=strlen(str);
	for(size_t i=0;i<len;i++)
	{
		if(x+i >= 0 && x+i <screenw)
			vbuffer_print_at(vb,x+i,y,*(str++),fgcolour,bgcolour);
	}
}


