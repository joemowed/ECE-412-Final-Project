/*
 * random.h
 *
 * Created: 4/24/2025 11:36:14 AM
 *  Author: Joe Maloney
 */ 


#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdlib.h>

//returns a random int in the range of low-high
inline int randomInRange(int low, int high){
	return (rand() % (high - low + 1)) + low ;
}


#endif /* RANDOM_H_ */