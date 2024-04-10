#include "serialCom.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/* UART fifo is 16 bytes. */
#define DATA_SIZE (2 * 16)

static char rxData[DATA_SIZE];
static unsigned int rxDataIndex;
static newSerialDataSlot_t newDataSlot;

void serialCom_registerNewDataSlot(newSerialDataSlot_t slot) {
    newDataSlot = slot;
}

void UARTIntHandler(void) {
    uint32_t ui32Status;
    // Get the interrrupt status.
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);
    // Clear the asserted interrupts.
    ROM_UARTIntClear(UART0_BASE, ui32Status);

    while (ROM_UARTCharsAvail(UART0_BASE) && rxDataIndex < (DATA_SIZE - 1)) {
        rxData[rxDataIndex++] = ROM_UARTCharGetNonBlocking(UART0_BASE);
    }
    if (newDataSlot != NULL) {
        newDataSlot(rxData, rxDataIndex);
    }
    rxDataIndex = 0;
}

void serialCom_init() {
    /* Enable the peripherals. */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    /* Enable processor interrupts. */
    ROM_IntMasterEnable();
    /* Set GPIO A0 and A1 as UART pins. */
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    /* Configure the UART for 115,200, 8-N-1 operation. */
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    /* Enable the UART interrupt. */
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void serialCom_send(const char* data, unsigned int size) {
    while (size-- > 0) {
        ROM_UARTCharPut(UART0_BASE, *data++);
    }
}
