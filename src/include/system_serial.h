#ifndef _SYSTEM_SERIAL_H_
#define _SYSTEM_SERIAL_H_

#ifdef __cplusplus
extern "C"
{
#endif
/* SERIAL.C */
void serial_install();
int serial_received();
char read_serial();
void write_serial(char a);
int is_transmit_empty();
#ifdef __cplusplus
}
#endif
#endif
