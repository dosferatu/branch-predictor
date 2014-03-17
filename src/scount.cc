#include "scount.h"

// default constructor makes a 2-bit counter
SATCOUNTER::SATCOUNTER() {
	init(2);
}

// constructor
SATCOUNTER::SATCOUNTER(uint bits) {
	init(bits);
}

// service function for constructors 
void SATCOUNTER::init(uint bits) {
	// initialize num_bits (width of counter)
	num_bits = bits;
	// allocate array for counter
	scount = new uint[num_bits];
	// initialize the counter before use to weakly taken
	for (uint i=0; i < num_bits - 1; i++)
		scount[i] = 1;
	scount[num_bits - 1] = 0; 
	// initialize maximum representable number
	max_number = pow(2, num_bits) - 1;
}

// destructor
SATCOUNTER::~SATCOUNTER() {
	delete [] scount;
	num_bits = 0;
}

// get_bits returns number of bits allocate for the counter (width)
uint SATCOUNTER::get_bits() {
	return num_bits;
}

// increment (count up)
void SATCOUNTER::increment() {
	uint carry = 0;

	if (max_number != get_count()) {
		carry = scount[0] & 1;
		scount[0] ^= 1;
		int old_carry;
		for (uint i = 1; i < num_bits; i++) {
			old_carry = carry;
			carry = scount[i] & old_carry;
			scount[i] ^= old_carry; 
		}
	}
}

// decrement (count down)
void SATCOUNTER::decrement() {
	if (0 != get_count()) {
		uint i = 0;
		while(i < num_bits && !scount[i]) {
			scount[i] = 1; 
			++i;
		}
		scount[i] = 0;
	}
}

// calculate and return the decimal value 
// currently in the counter - for debugging
uint SATCOUNTER::get_count() {
	uint value = 0;
	for(uint i=0; i < num_bits; i++) 
		value += scount[i] * pow(2, i);
	return value;
}

// reset clears all bits of the counter
void SATCOUNTER::reset() {
	for (uint i=0; i < num_bits; i++) 
		scount[i] = 0;
}

// get_sign returns MSB - the sign bit, which is used for deciding take / not take
uint SATCOUNTER::get_sign() {
	return scount[num_bits - 1];
}

// set_weak0 clears MSB and sets all other bits: 011 (3-bit) as in weakly not taken  
void SATCOUNTER::set_weak0() {
	for (uint i=0; i < num_bits - 1; i++)
		scount[i] = 1;
	scount[num_bits - 1] = 0; 
}

// set_weak1 sets MSB and clears all other bits: 100 (3-bit) as in weakly taken  
void SATCOUNTER::set_weak1() {
	for (uint i=0; i < num_bits - 1; i++)
		scount[i] = 0;
	scount[num_bits - 1] = 1; 
}
