/**
 * Unrank bottleneck benchmark
 * 
 * Written by Andras Majdan
 * License: GNU General Public License Version 3
 * 
 * Report bugs to <majdan.andras@gmail.com>
 */

#include <string>

#include <libcdsBasics.h>
#include <libcdsBitString.h>
#include <BitSequence.h>
#include <BitSequenceRRR.h>

#include <valgrind/callgrind.h>

#define LOOP_COUNT 500000

int main()
{
	// RRR compression
	cds_utils::BitString bitstring("data/testfile.dat");
	cds_static::BitSequenceRRR bsrrr = cds_static::BitSequenceRRR(bitstring,33);

	int loop_i;
	int randtable[LOOP_COUNT];
	size_t maxbits;
	size_t sum = 0;
	
	// Unranking operations
	loop_i = LOOP_COUNT;
	maxbits = bitstring.getLength();
	
	std::cout << maxbits << std::endl;
	
	srand (9876);

	for(int i=0;i<LOOP_COUNT;i++)
	{
		randtable[i] = rand() % maxbits;
	}
	
	loop_i=LOOP_COUNT;
	
	CALLGRIND_START_INSTRUMENTATION;
	while(--loop_i > 0)
	{
		sum += bsrrr.access(randtable[loop_i]);
	}
	CALLGRIND_STOP_INSTRUMENTATION;
	
	std::cout << sum << std::endl;
	
	return 0;
}
