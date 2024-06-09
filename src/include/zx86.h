#ifndef _ZX86_H_
#define _ZX86_H_
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ATEXIT_MAX_FUNCS 128

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned uarch_t;
    struct atexit_func_entry_t
    {
        /*
         * Each member is at least 4 bytes large. Such that each entry is 12bytes.
         * 128 * 12 = 1.5KB exact.
         **/
        void (*destructor_func)(void *);
        void *obj_ptr;
        void *dso_handle;
    };
    int __cxa_atexit(void (*f)(void *), void *objptr, void *dso);
    void __cxa_finalize(void *f);

#ifdef __cplusplus
};
#endif

__extension__ typedef int __guard __attribute__((mode(__DI__)));
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release(__guard *);
extern "C" void __cxa_guard_abort(__guard *);
extern void *operator new(size_t size);
extern void *operator new[](size_t size);
extern void operator delete(void *p);
extern void operator delete[](void *p);

#endif
