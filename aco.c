#include <stdlib.h>
#include <stdio.h>

#include "readint.h"
#include "gimp.h"

#define RGB 0
#define HSB 1
#define CMYK 2
#define LAB 7
#define GRAYSCALE 8

int main(int argc, char *argv[])
{
	char *p;
	int count;
	int is_v2 = 0;
	FILE *aco, *gpl;
	char filename[260];
	unsigned char buf[10];
	unsigned short type, val1, val2, val3, val4;

	if(argc < 2)
	{
		fprintf(stderr, "usage: aseconv <palette.ase>\n");
		return 1;
	}

	strcpy(filename, argv[1]);

	p = strrchr(filename, '.');

	if(p == NULL)
	{
		fprintf(stderr, "expected ACO file\n");
		return 1;
	}

	memcpy(p, ".gpl", 4);

	aco = fopen(argv[1], "rb");
	gpl = fopen(filename, "wb");

	if(fread(buf, 1, 4, aco) != 4)
	{
		fprintf(stderr, "unexpected end of file\n");
		return 1;
	}

	if(memcmp(buf, "\x00\x01", 2))
	{
		fprintf(stderr, "expected ACO file\n");
		return 1;
	}
	count = read_uint16_be(&buf[2]);

	fseek(aco, 0, SEEK_END);
	is_v2 = (ftell(aco) > (count * 10) + 4) ? 1 : 0;
	fseek(aco, 4, SEEK_SET);

	printf("count %d\nis v2: %d\n", count, is_v2);

	gimp_emit_header(gpl);
	while(count--)
	{
		if(fread(buf, 1, 10, aco) != 10)
		{
			fprintf(stderr, "unexpected end of file\n");
			return 1;
		}

		type = read_uint16_le(&buf[0]);
		if(type == RGB)
		{
			val1 = read_uint16_le(&buf[2]);
			val2 = read_uint16_le(&buf[4]);
			val3 = read_uint16_le(&buf[6]);
			gimp_emit_line(gpl, (unsigned char)val1, (unsigned char)val2, (unsigned char)val3);
		}
		if(type == HSB)
		{
		}
		if(type == CMYK)
		{
		}
		if(type == LAB)
		{
		}
		if(type == GRAYSCALE)
		{
			val1 = read_uint16_le(&buf[2]);
			gimp_emit_line(gpl, (unsigned char)val1, (unsigned char)val1, (unsigned char)val1);
		}
	}

	return 0;
}
