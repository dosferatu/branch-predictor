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
		uint get_count();
		void reset();
		uint get_bits();
		uint get_sign();
};

#endif	// SAT_COUNTER_SEEN
