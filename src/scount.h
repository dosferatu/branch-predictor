#ifndef SAT_COUNTER_SEEN
#define SAT_COUNTER_SEEN

#include <cstddef>
#include <cstring>
#include <cmath>
#define uint unsigned int

class SATCOUNTER
{
	private:
		uint num_bits;
		uint *scount; 
		uint max_number;
		void init(uint numbits);

	public:
		SATCOUNTER();
		SATCOUNTER(uint nbits); 
		~SATCOUNTER();
		void increment();
		void decrement();	
		uint get_count();	// returns the decimal value of the current count
		void reset();		// clears all bits of the counter
		uint get_bits();	// returns number of bits reserved for the counter
		uint get_sign();	// returns MSB, or a sign bit, used for decision-making
		void set_weak0();	// clears MSB and sets all other bits - weakly not taken
		void set_weak1();	// sets MSB and clears all other bits - weeakly taken
};

#endif	// SAT_COUNTER_SEEN
