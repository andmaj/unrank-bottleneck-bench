/**
 * Unrank binomial size
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

int main()
{
	size_t sum = 0;

	typedef sdsl::bit_vector bit_vector;
	typedef sdsl::rrr_vector<9> rrr_vector9;
	typedef sdsl::rrr_vector<17> rrr_vector17;
	typedef sdsl::rrr_vector<33> rrr_vector33;
	typedef sdsl::rrr_vector<65> rrr_vector65;
	typedef sdsl::rrr_vector<129> rrr_vector129;

	bit_vector bv;
	rrr_vector9 rv9;
	rrr_vector17 rv17;
	rrr_vector33 rv33;
	rrr_vector65 rv65;
	rrr_vector129 rv129;
	
	bv = bit_vector(256);
	for(int i=0; i<256; i++)
		bv[i]=i%2;
		
	rv9 = rrr_vector9(bv);
	rv17 = rrr_vector17(bv);
	rv33 = rrr_vector33(bv);
	rv65 = rrr_vector65(bv);
	rv129 = rrr_vector129(bv);
	
	std::cout << "rrr_vector<9>" << std::endl;
	sum += rv9[10];
	std::cout << "rrr_vector<17>" << std::endl;	
	sum += rv17[10];
	std::cout << "rrr_vector<33>" << std::endl;	
	sum += rv33[10];
	std::cout << "rrr_vector<65>" << std::endl;	
	sum += rv65[10];
	std::cout << "rrr_vector<129>" << std::endl;	
	sum += rv129[10];
	
	std::cout << "chk val: " << sum << std::endl;
	
	return 0;
}
