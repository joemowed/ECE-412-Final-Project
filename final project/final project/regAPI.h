/*
 * regAPI.h
 *
 * Created: 3/7/2025 11:47:43 AM
 *  Author: Joe Maloney
 */

#ifndef REGAPI_H_
#define REGAPI_H_

// sets the bit at given position in the provided register
#define SET_BIT(reg, bitPos) ((reg) |= (1 << (bitPos)))

// sets the bit at the given position to the given boolean value
#define SET_BIT_VALUE(reg, bitPos, value)                                      \
    ((reg) ^= (((-value) ^ (reg)) & (1 << (bitPos))))

// reads the bit at given position in the provided register
#define READ_BIT(reg, bitPos) ((reg) & (1 << (bitPos)))

// clears the bit at given position in the provided register
#define CLEAR_BIT(reg, bitPos) ((reg) &= (~(1 << (bitPos))))

// saves all general purpose registers, defined in assembly file
extern void saveRegs();
// restores all general purpose registers, defined in assembly file
extern void restoreRegs();

//wrapper for void functions, saves and restores registers
inline void wrapASM(void(*asmFunction)(void)){
	saveRegs();
	asmFunction();
	restoreRegs();
}
#endif /* REGAPI_H_ */
