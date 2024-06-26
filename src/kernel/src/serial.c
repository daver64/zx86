#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PORT 0x3f8 /* COM1 */

void serial_install();
int serial_received();
char read_serial();
void write_serial(char a);
int is_transmit_empty();

void serial_install()
{
   outportb(PORT + 1, 0x00); // Disable all interrupts
   outportb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
   outportb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
   outportb(PORT + 1, 0x00); //                  (hi byte)
   outportb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
   outportb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
   outportb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
                             // printf("Init Com1.\n");
}

int serial_received()
{
   return inportb(PORT + 5) & 1;
}

char read_serial()
{
   while (serial_received() == 0)
      ;
   return inportb(PORT);
}
int is_transmit_empty()
{
   return inportb(PORT + 5) & 0x20;
}

void write_serial(char a)
{
   while (is_transmit_empty() == 0)
      ;
   outportb(PORT, a);
}

void serial_puts(const char *msg)
{

   while (*msg != '\0')
   {
      write_serial(*msg);
      msg++;
   }
}
