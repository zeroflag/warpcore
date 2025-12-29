#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>
 
extern int16_t scroll_x;

int16_t io_port_read(int16_t port);
void io_port_write(int16_t port, int16_t data);

#endif // PORTS_H
