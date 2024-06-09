#ifndef _SYSTEM_CPU_H_
#define _SYSTEM_CPU_H_
#ifdef __cplusplus
extern "C"
{
#endif

struct regs
{
  unsigned int gs, fs, es, ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};
#ifdef __cplusplus
}
#endif
#endif
