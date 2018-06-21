/*
 * Number.h
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#ifndef _UTIL_NUMBER_H_
#define _UTIL_NUMBER_H_
#include <stdint.h>
#include <stdlib.h>

namespace Base {

class Number {
public:
	class Unique {
	public:
		Unique(size_t range);
		~Unique(); 
		size_t Get(); 
		void Release(size_t num); 
		
	private:
		size_t _range;
		uint8_t * flag;
	};
	
	static uint16_t MakeU16(uint8_t h, uint8_t l);
	static uint32_t MakeU32(uint16_t h, uint16_t l);
	static uint32_t MakeU32(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
	static uint64_t MakeU64(uint32_t h, uint32_t l); 
	static uint8_t LoByte(uint16_t v); 
	static uint8_t HiByte(uint16_t v); 
	static uint16_t LoWord(uint32_t v); 
	static uint16_t HiWord(uint32_t v); 
	static uint32_t LoDword(uint64_t v); 
	static uint32_t HiDword(uint64_t v); 
};

}

#endif /* 3RD_BASE_UTIL_NUMBER_H_ */
