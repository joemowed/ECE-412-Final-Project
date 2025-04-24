/*
 * portapi.h
 *
 * Created: 3/7/2025 9:21:09 AM
 *  Author: Joe Maloney
 */

#ifndef PORTAPI_H_
#define PORTAPI_H_

#include "regAPI.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdint-gcc.h>

// port registers on the atmega328pb follow this addressing scheme
typedef struct PortStruct {
    volatile uint8_t PINx; // write only
    volatile uint8_t DDRx;
    volatile uint8_t PORTx;
} PortStruct;
typedef PortStruct *const Port;

/* gets a valid base address for a PortStruct from the microchip provided
 * definitions of the port registers, (e.g PORTC,PORTA,etc.).  This allows for
 * the nice syntax of "portAPIFunction(&PORTC,...)" */
inline Port getAddrFromPort(volatile uint8_t *portRegister) {
    return (Port)(portRegister - 2);
}

// configures a whole port as output
inline void configOutputPort(volatile uint8_t *portaddress) {
    Port port = getAddrFromPort(portaddress);
    port->PORTx = 0U;
    port->DDRx = 0xFF;
    return;
}
// configures a whole port as input
inline void configInputPort(volatile uint8_t *portaddress) {
    Port port = getAddrFromPort(portaddress);
    port->PORTx = 0U;
    port->DDRx = 0U;
    return;
}

// configures a pin as input
inline void configInputPin(volatile uint8_t *portaddress, uint8_t pin) {
    Port port = getAddrFromPort(portaddress);
    SET_BIT(port->PORTx, pin);  // set output high, enables PUR
    CLEAR_BIT(port->DDRx, pin); // set pin as input
    return;
}

// configures a pin as output, and sets output low
inline void configOutputPin(volatile uint8_t *portaddress, uint8_t pin) {
    Port port = getAddrFromPort(portaddress);
    CLEAR_BIT(port->PORTx, pin); // set output low, if set
    SET_BIT(port->DDRx, pin);    // set pin as output
    return;
}

// toggles a pin using the PINx register
// takes  250ns with inline (4 cycles)
inline void portTogglePin(volatile uint8_t *portaddress, uint8_t pin) {
    Port port = getAddrFromPort(portaddress);
    port->PINx = (1 << pin); // toggle the pin
    return;
}

// write a pin to a given value using the PORTx register
//  takes  438ns with inline (7 cycles)
inline void portWritePin(volatile uint8_t *portaddress, uint8_t pin,
                         bool value) {
    Port port = getAddrFromPort(portaddress);
    // set the pin output to the given value
    SET_BIT_VALUE(port->PORTx, pin, value);
    return;
}

// read an entire port into a byte
inline volatile uint8_t portRead(volatile uint8_t *portaddress) {
    Port port = getAddrFromPort(portaddress);
    return port->PINx; // read the port
}

// read a single pin
inline volatile bool portReadPin(volatile uint8_t *portaddress, uint8_t pin) {
    Port port = getAddrFromPort(portaddress);
    return READ_BIT(port->PINx, pin); // read the pin
}

// write an entire port's outputs to a given value
inline void portWrite(volatile uint8_t *portaddress, uint8_t value) {
    Port port = getAddrFromPort(portaddress);
    port->PORTx = value; // write the outputs
    return;
}

#endif /* PORTAPI_H_ */
