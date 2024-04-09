#ifndef SERIAL_COM_H
#define SERIAL_COM_H

typedef void (*newDataSlot_t)(const char* data, unsigned int size);

void serialCom_registerNewDataSlot(newDataSlot_t newDataSlot);
void serialCom_init();
void serialCom_send(const char* data, unsigned int size);

#endif /* SERIAL_COM_H */
