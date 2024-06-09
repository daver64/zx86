#ifndef _SYSTEM_VGA_H_
#define _SYSTEM_VGA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define TXT_TOPLEFT (0xc9)
#define TXT_HORIZONTAL (0xcd)
#define TXT_TOPRIGHT (0xbb)
#define TXT_VERTICAL (0xba)
#define TXT_BOTTOMRIGHT (0xbc)
#define TXT_BOTTOMLEFT (0xc8)
#define TXT_BREAKLEFT (0xc7)
#define TXT_BREAKRIGHT (0xB6)
#define TXT_HORIZONTALSINGLE (0xC4)
  int get_num_columns();
  int get_num_rows();
  typedef struct vbuffer
  {
    unsigned short *ptr;
    unsigned short *write_ptr;
    int len;
  } vbuffer_t;

  void vbuffer_reset(vbuffer_t *vb);
  vbuffer_t *vbuffer_create();
  void vbuffer_append(vbuffer_t *vb, char c, int fgcolour, int bgcolour);
  void vbuffer_fill(vbuffer_t *vb, char c, int fgcolour, int bgcolour);
  void vbuffer_draw(vbuffer_t *vb);
  void vbuffer_print_at(vbuffer_t *vb, int x, int y, char c, int fgcolour, int bgcolour);
  void vbuffer_print_string(vbuffer_t *vb, int x, int y, const char *str, int fgcolour, int bgcolour);
  void vbuffer_init();

#ifdef __cplusplus
}
#endif
#endif
