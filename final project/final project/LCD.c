/*
 * LCD.c
 *
 * Created: 3/7/2025 11:16:43 PM
 *  Author: Joe Maloney
 */
#include "LCD.h"

#include "delay.h"
#include "portAPI.h"
#include <stdarg.h>
#include <stdio.h>

// configuration settings used by LCDInit()
struct LCDConfig LCDConfig = {
    .isCursorIncrement = 1,
    .isDisplayShift = 0,
    .isDisplayOn = 1,
    .isCursorOn = 0,
    .isCursorBlinkOn = 0,
    .is8BitData = 1,
    .is2LineMode = 1,
    .is5x11Font = 0,
};

/* The current set of callback functions called by the LCD class before and
 * after writing data to the LCD.*/

// sends pulse on E to latch the data sent to LCD
inline void LCDLatchData() {
  portWritePin(&LCD_LATCH_PORT, LCD_LATCH_PIN, 1);
  delayMicroseconds(1);  // pulse needs to be at least 150ns long, and at
                         // least 1.2us between pulses
  portWritePin(&LCD_LATCH_PORT, LCD_LATCH_PIN, 0);
}

// sends the cursor on the LCD to the home position
void LCDCursorHome() {
  LCDWriteCommand(0x2);               // from datasheet
  delayMicroseconds(LCD_LONG_DELAY);  // wait for LCD to process
}

// Clears all LCD communication pins
inline void LCDZeroOutputs() {
  portWritePin(&LCD_RW_PORT, LCD_RW_PIN, 0);
  portWritePin(&LCD_LATCH_PORT, LCD_LATCH_PIN, 0);
  portWritePin(&LCD_REG_SELECT_PORT, LCD_REG_SELECT_PIN, 0);
  portWrite(&LCD_DATA_PORT, 0x0);
}

inline void LCDWriteData(uint8_t data) {
  // select data mode on the LCD
  LCDConfigPins();
  portWritePin(&LCD_REG_SELECT_PORT, LCD_REG_SELECT_PIN, 1);
  portWrite(&LCD_DATA_PORT, data);     // put the data on the data lines
  LCDLatchData();                      // send a pulse on E to latch the data
  delayMicroseconds(LCD_SHORT_DELAY);  // wait for LCD to process
  LCDZeroOutputs();                    // return all LCD pins to zero
}

inline void LCDWriteCommand(uint8_t data) {
  // select command mode on the LCD
  LCDConfigPins();
  portWritePin(&LCD_REG_SELECT_PORT, LCD_REG_SELECT_PIN, 0);
  portWrite(&LCD_DATA_PORT, data);     // put the data on the data lines
  LCDLatchData();                      // send a pulse on E to latch the data
  delayMicroseconds(LCD_SHORT_DELAY);  // wait for LCD to process
  LCDZeroOutputs();                    // return all LCD pins to zero
}

// clears display, and sets cursor at 0,0
inline void LCDClear() {
  LCDWriteCommand(0x1);               // from datasheet
  delayMicroseconds(LCD_LONG_DELAY);  // wait for LCD to process
}

inline void LCDConfigPins() {
  configOutputPort(&LCD_DATA_PORT);  // set the LCD data port as output
  // set the other LCD pins as outputs
  configOutputPin(&LCD_LATCH_PORT, LCD_LATCH_PIN);
  configOutputPin(&LCD_REG_SELECT_PORT, LCD_REG_SELECT_PIN);
  // RW needs to be low for duration of LCD usage
  configOutputPin(&LCD_RW_PORT, LCD_RW_PIN);
}

void LCDInit() {
  LCDConfigPins();
  LCDZeroOutputs();  // clear outputs on LCD pins
  // "function set" from datasheet
  uint8_t tmp = ((1 << 5) | (LCDConfig.is8BitData << 4) |
                 (LCDConfig.is2LineMode << 3) | (LCDConfig.is5x11Font << 2));
  LCDWriteCommand(tmp);
  tmp = (1 << 3);  // "Display ON/OFF" from datasheet
  LCDWriteCommand(tmp);
  // "Entry Mode set" from datasheet
  tmp = ((1 << 2) | (LCDConfig.isCursorIncrement << 1) |
         LCDConfig.isDisplayShift);
  LCDWriteCommand(tmp);
  // "Display ON/OFF" from datasheet
  tmp = ((1 << 3) | (LCDConfig.isDisplayOn << 2) | (LCDConfig.isCursorOn << 1) |
         LCDConfig.isCursorBlinkOn);
  LCDWriteCommand(tmp);
  LCDClear();  // clear the display, and sets the cursor to 0,0
}

void LCDWriteCustomChar(CustomChar *customChar, uint8_t addr) {
  //"Set CGRAM address" from datasheet
  addr = (1 << 6) | (addr << 3);  // this line from arduino library,
                                  // the CGRAM addressing scheme (for writing)
                                  // is not explained in the LCD datasheet
  // set the address of the incoming custom character on the LCD
  LCDWriteCommand(addr);
  // without this additional delay, sometimes not all bytes of the customChar
  // are read by LCD
  delayMicroseconds(LCD_SHORT_DELAY);
  for (uint8_t i = 0; i < 8; i++) {  // send each of the 8 bytes that make up a
    // custom character to the LCD
    // only bits 0:5 of each byte are used by the LCD
    LCDWriteData(customChar->lines[i]);
  }
}

/* Sets the cursor to a position on the LCD.  Valid positions on the 1602 LCD
 * include are 0x00-0x67 (2-line display mode).  The first char on the second
 * line is at position 0x28.*/
void LCDSetCursorPos(uint8_t pos) {
  pos |= (1 << 7);  // LCD datasheet: 6:0 are the address, bit 7 is always 1
  LCDWriteCommand(pos);  // set the cursor pos
}

//creates the formatted string, given a string format and va_args list
void vLCDPuts(const char* str,va_list *args){
	
char buffer[64];
char * buffer_read_ptr = buffer;
vsprintf(buffer, str, *args);
  while (*buffer_read_ptr) {
    LCDWriteData(*buffer_read_ptr++);
  }
}

// writes a single string to the LCD 
void LCDPuts(const char *str,...) {
va_list args;
va_start(args,str);
vLCDPuts(str,&args);
va_end(args);
}

/*Same as LCDPuts(), except writes the string to the bottom line of the LCD.
*/
void LCDPuts2(const char *str,...) {
LCDSetCursorPos(0x28);
va_list args;
va_start(args,str);
vLCDPuts(str,&args);
va_end(args);
}