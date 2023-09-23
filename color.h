#ifndef COLOR_H
#define COLOR_H

struct rgb
{
	unsigned int r, g, b;
};

struct rgb *from_hsv(float hue, float saturation, float value);
struct rgb *from_lab(float l, float a, float b);

#endif
