#include <stdlib.h>
#include <math.h>

#include "color.h"

RGB *from_lab(float l, float a, float b)
{
	RGB *rgb_color;
	float z, y, x, fr, fg, fb;

	rgb_color = malloc(sizeof(RGB));

	if(!rgb_color)
	{
		fprintf(stderr, "Out of memory");
		exit(1);
	}

	/* L*a*b* -> XYZ */

	y = (l + 16) / 116;
	x = a / 500 + y;
	z = y - b / 200;

	if(x * x * x > 0.008856)
		x = x * x * x;
	else
		x = (x - 16 / 116) / 7.787;

	if(y * y * y > 0.008856)
		y = y * y * y;
	else
		y = (y - 16 / 116) / 7.787;

	if(z * z * z > 0.008856)
		z = z * z * z;
	else
		z = (z - 16 / 116) / 7.787;

	x = x * 95.047;
	y = y * 100.000;
	z = z * 108.883;

	/* XYZ -> RGB */

	x = x / 100;
	y = y / 100;
	z = z / 100;

	fr = x *  3.2406 + y * -1.5372 + z * -0.4986;
	fg = x * -0.9689 + y *  1.8758 + z *  0.0415;
	fb = x *  0.0557 + y * -0.2040 + z *  1.0570;

	if(fr > 0.0031308)
		fr = 1.055 * pow(fr, 1 / 2.4) - 0.055;
	else
		fr = 12.92 * fr;
	if(fg > 0.0031308)
		fg = 1.055 * pow(fg, 1 / 2.4) - 0.055;
	else
		fg = 12.92 * fg;
	if(fb > 0.0031308)
		fb = 1.055 * pow(fb, 1 / 2.4) - 0.055;
	else
		fb = 12.92 * fb;

	if(fr > 0)
		if(fr < 1)
			rgb_color->r = fr * 255;
		else
			rgb_color->r = 255;
	else
		rgb_color->r = 0;

	if(fg > 0)
		if(fg < 1)
			rgb_color->g = fg * 255;
		else
			rgb_color->g = 255;
	else
		rgb_color->g = 0;

	if(fb > 0)
		if(fb < 1)
			rgb_color->b = fb * 255;
		else
			rgb_color->b = 255;
	else
		rgb_color->b = 0;

	return rgb_color;
}
