#include <stdio.h>

void gimp_emit_header(FILE *file)
{
	fprintf(file, "GIMP Palette\nName: RGB palette\nColumns: 10\n");
}

void gimp_emit_line(FILE *file, unsigned int r, unsigned int g, unsigned int b)
{
	fprintf(file, "%u%c%u%c%u%c%c%02x%02x%02x%c", r, 0x09, g, 0x09, b, 0x09, '#', r, g, b, 0x0a);
	fprintf(stdout, "%u%c%u%c%u%c%c%02x%02x%02x%c", r, 0x09, g, 0x09, b, 0x09, '#', r, g, b, 0x0a);
}
