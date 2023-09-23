#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>

#include "color.h"
#include "readbytes.h"
#include "gimp.h"

float bytes_to_float(const unsigned char *bytes);
unsigned int bytes_to_rgb(const unsigned char *bytes);
struct rgb *from_cmyk(const unsigned char *bytes);

int main(int argc, char *argv[])
{
	char *p;
	int count;
	FILE *out, *in;
	float f1, f2, f3;
	char filename[512];
	unsigned int r, g, b;
	struct rgb *rgb_colors;
	unsigned char buf[1024];
	unsigned short chunk_size;
	const char *ase = "ASEF", *rgb = "RGB ", *cmyk = "CMYK", *lab = "LAB ", *gray = "Gray";

	if(argc < 2)
	{
		fprintf(stderr, "usage: aseconv <palette.ase>\n");
		return 1;
	}

	strcpy(filename, argv[1]);
	p = strrchr(filename, '.');

	if(p == NULL)
	{
		fprintf(stderr, "usage: aseconv <palette.ase>\n");
		return 1;
	}

	if(p[1] == '\0' || p[2] == '\0' || p[3] == '\0')
	{
		fprintf(stderr, "usage: aseconv <palette.ase>\n");
		return 1;
	}

	p[1] = 'g';
	p[2] = 'p';
	p[3] = 'l';
	p[4] = '\0';

	if(!(in = fopen(argv[1], "rb")))
	{
		fprintf(stderr, "aseconv: %s\n", strerror(errno));
		return 1;
	}

	if(!(out = fopen(filename, "wb")))
	{
		fprintf(stderr, "aseconv: %s\n", strerror(errno));
		return 1;
	}

	/* read header, version and chunk count */
	fread(buf, 1, 12, in);

	if(memcmp(ase, buf, 4))
	{
		fprintf(stderr, "not an ASE file\n");
		return 1;
	}

	/* TODO: read ASE version, currently at buf 4-7 */
#if 0
	printf("%x\n", buf[8]); printf("%x\n", buf[9]); printf("%x\n", buf[10]); printf("%x\n", buf[11]);
#endif

	count = read_uint32_be(&buf[8]);
	fprintf(stdout, "chunk count: %d\n", count);

	/* CHUNK types: */
	/* palette name	= 0xC0010000 */
	/* mystery		= 0xC0020000 */
	/* swatch		= 0x00010000 */

	gimp_emit_header(out);

	while(count)
	{
		if(fread(buf, 1, 6, in) != 6)
		{
			fprintf(stderr, "ERROR: truncated chunk\n");
			break;
		}

		/* palette name chunk, no valuable data to be found, skip */
		if(!memcmp(&buf[0], "\xC0\x01", 2))
		{
			chunk_size = read_uint16_be(&buf[4]);
			fseek(in, chunk_size, SEEK_CUR);
		}
		/* swatch chunk */
		else if(!memcmp(&buf[0], "\x00\x01", 2))
		{
			chunk_size = read_uint16_be(&buf[4]);
			fread(buf, 1, chunk_size, in);

			if(!memcmp(rgb, &buf[chunk_size - 18], 4))
			{
				r = bytes_to_float(&buf[chunk_size - 14]) * 255;
				g = bytes_to_float(&buf[chunk_size - 10]) * 255;
				b = bytes_to_float(&buf[chunk_size - 6]) * 255;
			}
			else if(!memcmp(cmyk, &buf[chunk_size - 22], 4))
			{
				rgb_colors = from_cmyk(&buf[chunk_size - 18]);
				r = rgb_colors->r;
				g = rgb_colors->g;
				b = rgb_colors->b;
				free(rgb_colors);
			}
			else if(!memcmp(gray, &buf[chunk_size - 10], 4))
			{
				r = bytes_to_float(&buf[chunk_size - 6]) * 255;
				g = r;
				b = r;
			}
			else if(!memcmp(lab, &buf[chunk_size - 18], 4))
			{
				/* approximate conversion */
				f1 = bytes_to_float(&buf[chunk_size - 14]);
				f2 = bytes_to_float(&buf[chunk_size - 10]);
				f3 = bytes_to_float(&buf[chunk_size - 6]);
				rgb_colors = from_lab(f1 * 100, f2, f3);
				r = rgb_colors->r;
				g = rgb_colors->g;
				b = rgb_colors->b;
				free(rgb_colors);
			}
			else
			{
				fprintf(stderr, "FIXME: unsupported color sequences\n");
				count--;
				continue;
			}

			gimp_emit_line(out, r, g, b);
		}
		/* end of folder? */
		else if(!memcmp(&buf[0], "\xC0\x02", 2))
		{
		}
		else
		{
			/* end of the line */
			break;
		}
		count--;
	}

	if(count != 0)
		fprintf(stderr, "BUG: %d chunks remain\n", count);

	fclose(in);
	fclose(out);
	return 0;
}

float bytes_to_float(const unsigned char *bytes)
{
	unsigned int u;
	float f;
	u = read_uint32_be(bytes);
	memcpy(&f, &u, 4);
	return f;
}

struct rgb *from_cmyk(const unsigned char *bytes)
{
	struct rgb *color;
	float f1, f2, f3, f4;
	uint32_t u1, u2, u3, u4;

	color = malloc(sizeof(struct rgb));

	if(!color)
	{
		fprintf(stderr, "Out of memory");
		exit(1);
	}

	u1 = read_uint32_be(&bytes[0]);
	u2 = read_uint32_be(&bytes[4]);
	u3 = read_uint32_be(&bytes[8]);
	u4 = read_uint32_be(&bytes[12]);

	memcpy(&f1, &u1, 4);
	memcpy(&f2, &u2, 4);
	memcpy(&f3, &u3, 4);
	memcpy(&f4, &u4, 4);

#if 0
	color->r = (uint32_t)round(255 * (1 - f1) * (1 - f4));
	color->g = (uint32_t)round(255 * (1 - f2) * (1 - f4));
	color->b = (uint32_t)round(255 * (1 - f3) * (1 - f4));
#else
	color->r = (uint32_t)floor((255 * (1 - f1) * (1 - f4)) + 0.5);
	color->g = (uint32_t)floor((255 * (1 - f2) * (1 - f4)) + 0.5);
	color->b = (uint32_t)floor((255 * (1 - f3) * (1 - f4)) + 0.5);
#endif

	return color;
}
