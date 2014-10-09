/**
 * Generates a repeatable random bitmap.
 * 
 * Written by Andras Majdan
 * License: GNU General Public License Version 3
 * 
 * Report bugs to <majdan.andras@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "posix1c_rand_r.h"

#define PROGRAM_NAME "randbitmap"

int make_randbitmap(
	unsigned int seed,
	double density,
	unsigned int bitsnumber, 
	char *filename)
{
	int fd;
	char *addr;
	int result;
	unsigned int randval, randint, randmod;
	unsigned int filesize;
	unsigned int bitstofill;
	
	if (density < 0.0f || density > 1.0f)
	{
		fprintf(stderr, "density >= 0 && density <= 1\n");
		return EXIT_FAILURE;
	}
	
	filesize = bitsnumber / 8 + !!(bitsnumber % 8);
	bitstofill = bitsnumber * density;
	
	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1)
	{
		perror("open");
		return EXIT_FAILURE;
	}
	
	result = lseek(fd, filesize-1, SEEK_SET);
    if (result == -1)
    {
		close(fd);
		perror("lseek");
		return EXIT_FAILURE;
    }
    
    result = write(fd, "", 1);
    if (result != 1)
    {
		close(fd);
		perror("writing last byte");
		return (EXIT_FAILURE);
    }

	addr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		close(fd);
		perror("mmap");
		return EXIT_FAILURE;
	}
	
	/* For performance */
	if (density > 0.5f)
	{
		memset(addr, '\255', filesize);
		bitstofill = bitsnumber - bitstofill;
	}
	
	while (bitstofill)
	{
		randval = posix1c_rand_r(&seed) % bitsnumber;
		randint = randval/8;
		randmod = randval%8;

		if (density > 0.5f)
		{
			/* Fill with 0 */
			if (!(~addr[randint] & 1<<randmod))
			{
				addr[randint] = ~(~addr[randint] | 1<<randmod);
				--bitstofill;
			}
		}
		else
		{
			/* Fill with 1 */
			if (!(addr[randint] & 1<<randmod))
			{
				addr[randint] = addr[randint] | 1<<randmod;
				--bitstofill;
			}
		}
	}
	
    munmap(addr, filesize);
    close(fd);
    return EXIT_SUCCESS;
}

void exit_with_usage(FILE *stream)
{	
	fprintf(stream, "Usage: %s [seed] [density] [bitsnumber] [filename]\n\n", PROGRAM_NAME);
	fprintf(stream,	"Generates a repeatable random bitmap.\n"
					"This is accomplished by using an internal pseudorandom number generator.\n\n"
					"Written by Andras Majdan\n"
					"License: GNU General Public License Version 3\n"
					"Report bugs to <majdan.andras@gmail.com>\n");
	
	if (stream == stderr)
		exit(1);
		
	exit(0);
}

int main(int argc, char *argv[])
{
	unsigned int seed;
	double density;
	unsigned int bitsnumber;
	char *filename;

	/* Check for help */
	if (argc == 2)
	{
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
			exit_with_usage(stdout);
		else 
			exit_with_usage(stderr);
	}
	
	if (argc != 5 )
		exit_with_usage(stderr);

	char *check;
	seed = strtoul (argv[1], &check, 10);
	if (*check != '\0')
	{
		fprintf(stderr, "Seed error\n\n");
		exit_with_usage(stderr);
	}
		
	density = strtod (argv[2], &check);
	if (*check != '\0')
	{
		fprintf(stderr, "Density error\n\n");
		exit_with_usage(stderr);
	}
		
	bitsnumber = strtoul (argv[3], &check, 10);
	if (*check != '\0')
	{
		fprintf(stderr, "Bitsnumber error\n\n");
		exit_with_usage(stderr);
	}
	
	filename = argv[4];
	
	return make_randbitmap(seed, density, bitsnumber, filename);
}
