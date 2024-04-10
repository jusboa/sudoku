#ifndef SERIAL_COM_H
#define SERIAL_COM_H

/* A callback(slot) to notify client that a new data has been received. */
typedef void (*newSerialDataSlot_t)(const char* data, unsigned int size);

/* Register a new data callback. */
void serialCom_registerNewDataSlot(newSerialDataSlot_t slot);
/* Initialize the module. */
void serialCom_init();
/* Send a given data. Blocking. */
void serialCom_send(const char* data, unsigned int size);

#endif /* SERIAL_COM_H */
