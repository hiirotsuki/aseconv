#ifndef COLOR_H
#define COLOR_H

typedef struct _rgb
{
	unsigned int r, g, b;
} RGB;

RGB *from_hsv(float hue, float saturation, float value);
RGB *from_lab(float l, float a, float b);

#endif
