#include <stdio.h>
#include "stdafx.h"
static int SEED = 0;

static int p[] = { 208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219 };

int noise2(int x, int y)
{
	int tmp = p[(y + SEED) % 256];
	return p[(tmp + x) % 256];
}
int noise3(int x, int y, int z)
{
	int tmp = p[(z + SEED) % 256];
	int tmp2 = p[(tmp + y) % 256];
	return p[(tmp2 + x) % 256];
}
float lin_inter(float x, float y, float s)
{
	return x + s * (y - x);
}

float smooth_inter(float x, float y, float s)
{
	return lin_inter(x, y, s * s * (3 - 2 * s));
}
static double fade(double t) {
	// Fade function as defined by Ken Perlin.  This eases coordinate values
	// so that they will ease towards integral values.  This ends up smoothing
	// the final output.
	return t * t * t * (t * (t * 6 - 15) + 10);         // 6t^5 - 15t^4 + 10t^3
}

float noise2d(float x, float y)
{
	int x_int = x;
	int y_int = y;
	float x_frac = x - x_int;
	float y_frac = y - y_int;
	int s = noise2(x_int, y_int);
	int t = noise2(x_int + 1, y_int);
	int u = noise2(x_int, y_int + 1);
	int v = noise2(x_int + 1, y_int + 1);
	float low = smooth_inter(s, t, x_frac);
	float high = smooth_inter(u, v, x_frac);
	return smooth_inter(low, high, y_frac);
}
#define inc(x) ((x+1) % 256)
static double grad(int hash, double x, double y, double z)
{
	switch (hash & 0xF)
	{
	case 0x0: return  x + y;
	case 0x1: return -x + y;
	case 0x2: return  x - y;
	case 0x3: return -x - y;
	case 0x4: return  x + z;
	case 0x5: return -x + z;
	case 0x6: return  x - z;
	case 0x7: return -x - z;
	case 0x8: return  y + z;
	case 0x9: return -y + z;
	case 0xA: return  y - z;
	case 0xB: return -y - z;
	case 0xC: return  y + x;
	case 0xD: return -y + z;
	case 0xE: return  y - x;
	case 0xF: return -y - z;
	default: return 0; // never happens
	}
}
float noise3d(float x, float y, float z)
{
	int xi = floor(x);
	int yi = floor(y);
	int zi = floor(z);
	float xf = x - xi;
	float yf = y - yi;
	float zf = z - zi;
	double u = fade(xf);
	double v = fade(yf);
	double w = fade(zf);

	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aab = noise3(xi, yi, zi + 1);
	aba = noise3(xi, yi + 1, zi);
	abb = noise3(xi, yi + 1, zi + 1);
	baa = noise3(xi + 1, yi, zi);
	bab = noise3(xi + 1, yi, zi + 1);
	bba = noise3(xi + 1, yi + 1, zi);
	aaa = noise3(xi, yi, zi);
	bbb = noise3(xi + 1, yi + 1, zi + 1);


	double x1, x2, y1, y2;
	x1 = smooth_inter(grad(aaa, xf, yf, zf),           // The gradient function calculates the dot product between a pseudorandom
		grad(baa, xf - 1, yf, zf),             // gradient vector and the vector from the input coordinate to the 8
		u);                                     // surrounding points in its unit cube.
	x2 = smooth_inter(grad(aba, xf, yf - 1, zf),           // This is all then smooth_intered together as a sort of weighted average based on the faded (u,v,w)
		grad(bba, xf - 1, yf - 1, zf),             // values we made earlier.
		u);
	y1 = smooth_inter(x1, x2, v);

	x1 = smooth_inter(grad(aab, xf, yf, zf - 1),
		grad(bab, xf - 1, yf, zf - 1),
		u);
	x2 = smooth_inter(grad(abb, xf, yf - 1, zf - 1),
		grad(bbb, xf - 1, yf - 1, zf - 1),
		u);
	y2 = smooth_inter(x1, x2, v);

	return (smooth_inter(y1, y2, w) + 1) / 2;
}

float perlin2d(float x, float y, float freq, int depth)
{
	float xa = x*freq;
	float ya = y*freq;
	float amp = 1.0;
	float fin = 0;
	float div = 0.0;

	int i;
	for (i = 0; i<depth; i++)
	{
		div += 256 * amp;
		fin += noise2d(xa, ya) * amp;
		amp /= 2;
		xa *= 2;
		ya *= 2;
	}

	return fin / div;
}
float perlin3d(float x, float y, float z, float freq, int depth)
{
	float xa = x*freq;
	float ya = y*freq;
	float za = z*freq;
	float amp = 1.0;
	float fin = 0;
	float div = 0.0;

	int i;
	for (i = 0; i<depth; i++)
	{
		div += amp;
		fin += noise3d(xa, ya, za) * amp;
		amp /= 2;
		xa *= 2;
		ya *= 2;
		za *= 2;
	}

	return fin / div;
}

