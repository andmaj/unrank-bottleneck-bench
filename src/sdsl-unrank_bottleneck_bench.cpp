/**
 * Unrank bottleneck benchmark
 * 
 * Written by Andras Majdan
 * License: GNU General Public License Version 3
 * 
 * Report bugs to <majdan.andras@gmail.com>
 */

#include <string>

#include "sdsl/rrr_vector.hpp"
#include <vector>
#include <cstdlib>

#include <valgrind/callgrind.h>

#define LOOP_COUNT 500000

int main()
{
	int loop_i;
	int randtable[LOOP_COUNT];
	size_t maxbits;
	size_t sum = 0;

	typedef sdsl::bit_vector bit_vector;
	typedef sdsl::rrr_vector<63> rrr_vector;

	bit_vector bv;
	rrr_vector rv;

	FILE *fileh;
	unsigned char ch; 
	size_t filesize;
	int ci;
	
	// Unranking operations
	loop_i = LOOP_COUNT;

	fileh = fopen("data/testfile.dat", "r");
	fseek(fileh, 0, SEEK_END);
	filesize = ftell(fileh);
	bv = bit_vector(filesize*8);
	rewind(fileh);

	ci = 0;
	while(fread(&ch, sizeof(unsigned char), 1, fileh) == 1)
	{
		for(int i=0; i<8; i++)
			bv[ci*8 + i] = ch & (unsigned char)(1<<i);
	
		++ci;
	}
	
	if(!feof(fileh))
	{
		fprintf(stderr, "I/O error\n");
		exit(1);
	}
	fclose(fileh);

	maxbits = bv.bit_size();
	std::cout << maxbits << std::endl;
	
	rv = rrr_vector(bv);
	srand (9876);

	for(int i=0;i<LOOP_COUNT;i++)
	{
		randtable[i] = rand() % maxbits;
	}
	
	loop_i=LOOP_COUNT;
	
	CALLGRIND_START_INSTRUMENTATION;
	while(--loop_i > 0)
	{
		sum += rv[randtable[loop_i]];
	}
	CALLGRIND_STOP_INSTRUMENTATION;
	
	std::cout << sum << std::endl;
	
	return 0;
}
