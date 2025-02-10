#include <stdio.h>
#include <stdint.h>
//#include "SDL2/SDL.h"

#define W 300
#define H 200

void grinit();
void grquit();
void grclear();
void grrect(int x, int y, int l, int b);
void grcircle(int x, int y, int r);
void ppm();
void png();

struct {
	uint32_t color;
	char *title;
	int wx, wy;
} gc;

uint32_t pixels[W*H];

int main(int argc, char **argv)
{
	grinit();
	gc.color = 0x0000ff;
	grclear();
	gc.color = 0xff0000;
	grrect(10, 10, 100, 100);
	grcircle(100, 100, 10);
	ppm();
	png();

	grquit();
}

void grinit()
{
	gc.wx=0;
	gc.wy=0;
	gc.color = 0xffffff;
}

void grquit()
{}

void grclear()
{
	int i;
	for(i=0; i<W*H; i++) {
		pixels[i] = gc.color;
	}
}

void grrect(int x, int y, int l, int b)
{
	int i, j;
	for(j=y; j<y+b; j++) {
		for(i=x; i<x+l; i++) {
			pixels[j*W+i] = gc.color;
		}
	}
}

void grcircle(int x, int y, int r)
{
	/*
	int i, j, x0;
	for(j=y-r; j<y+r; j++) {
		x0 = x+sqrt(r*r - (j-y)*(j-y));
		for(i=x0-r; i<x0+r; i++) {
			pixels[j*W+i] = gc.color;
		}
	}
	*/
}

void ppm()
{
	int i, j;
	FILE *f = fopen("out.ppm", "wb");
	fprintf(f, "P3\n%d %d 255\n", W, H);
	for(i=0; i<W*H; i++) {
		uint32_t p = pixels[i];
		uint8_t v[3] = {
			(p>>(8*2)) & (0xff),
			(p>>(8*1)) & (0xff),
			(p>>(8*0)) & (0xff)
		};
		for(j=0; j<3; j++) {
			fprintf(f, "%d ", v[j]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

void rev(void *buf, size_t n)
{
	uint8_t *b = buf;
	int i;
	for(i=0; i<n/2; i++) {
		uint8_t t = b[i];
		b[i] = b[n-i-1];
		b[n-i-1] = t;
	}
}

void png()
{
	FILE *f = fopen("demo.png", "rb");
	uint8_t sig[8];
	size_t i;
	
	//signature
	fread(sig, 8, 1, f);
	printf("signature: ");
	for(i=0; i<8; i++) printf("%d ", sig[i]);
	printf("\n");

	i=20;
	uint32_t chunklen;
	uint8_t chunktyp[4];
	uint32_t chunkcrc;
	while(*(uint32_t*)chunktyp != 0x444E4549) {
		//chunk length
		fread(&chunklen, 4, 1, f);
		rev(&chunklen, 4);
		printf("chunk len: %u\n", chunklen);

		//chunk type
		fread(chunktyp, 4, 1, f);
		printf("chunk typ: %.*s 0x%08X\n", 4, chunktyp, *(uint32_t*)chunktyp);

		//skip data section
		fseek(f, chunklen, SEEK_CUR);

		//chunk crc
		fread(&chunkcrc, 4, 1, f);
		rev(&chunkcrc, 4);
		printf("chunk crc: %u\n", chunkcrc);
		printf("------------------------------------\n");
	}
	fclose(f);
}

	/*
	len
	chunk type 
	chunk data
	IHDR: (w, h):4, (bitdepth, colortype, compressionmethod, filtermethod, interlacemethod):1
	crc
	*/

void sdl()
{}
