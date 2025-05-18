#include <stdio.h>
#include <stdint.h>
#include <math.h>

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
} gc;

uint32_t pixels[W*H];

int main(int argc, char **argv)
{
	gc.color = 0xffffff;
	grclear();
	gc.color = 0xff0000;
	grrect(10, 10, 10, 20);
	grcircle(100, 100, 90);
	ppm();
}

void grinit()
{}

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
	int i, j, x0;
	for(j=y-r; j<=y+r; j++) {
		x0 = sqrt(r*r - (j-y)*(j-y));
		for(i=x-x0; i<=x+x0; i++)
			pixels[j*W+i] = gc.color;
	}
}

void grtriangle(
	int x1, int x2, int x3, 
	int y1, int y2, int y3
)
{}

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

	uint32_t bot;
	uint8_t byt;
	while(*(uint32_t*)chunktyp != 0x444E4549) {
		//chunk length
		fread(&chunklen, 4, 1, f);
		rev(&chunklen, 4);
		//chunk type
		fread(chunktyp, 4, 1, f);
		//skip data section
		if(*(uint32_t*)chunktyp == 0x52444849) {
			fread(&bot, 4, 1, f);
			rev(&bot, 4);
			printf("Width: \t\t%zu\n", bot);
			fread(&bot, 4, 1, f);
			rev(&bot, 4);
			printf("Height: \t%zu\n", bot);
			
			fread(&byt, 1, 1, f);
			printf("Bit Depth: \t%u\n", byt);
			fread(&byt, 1, 1, f);
			printf("Color Type: \t%u\n", byt);
			fread(&byt, 1, 1, f);
			printf("Comp Method: \t%u\n", byt);
			fread(&byt, 1, 1, f);
			printf("Filter Method: \t%u\n", byt);
			fread(&byt, 1, 1, f);
			printf("Intrlce Method: %u\n", byt);
		}
		//IDAT chunk
		else if(*(uint32_t*)chunktyp == 0x54414449) {
			uint8_t comp;
			fread(&comp, 1, 1, f);
			printf("Compression flags: \t%x\n", comp);
			fread(&comp, 1, 1, f);
			printf("Additional flags: \t%x\n", comp);
			break;
		}
		else fseek(f, chunklen, SEEK_CUR);
		//chunk crc
		fread(&chunkcrc, 4, 1, f);
		rev(&chunkcrc, 4);

		printf("------------------------------------\n");
		printf("chunk len: %u\n", chunklen);
		printf("chunk typ: %.*s 0x%08X\n", 4, chunktyp, *(uint32_t*)chunktyp);
		printf("chunk crc: %u\n", chunkcrc);
		printf("------------------------------------\n");

/*
	DEFLATE

		int in;
		while(fread(&in, 1, 1, f) != EOF) {
    		match := longest repeated occurrence of input that begins in window
			int m = getm(in);
			int d, l, c;

    		if(m != -1) {
        		d = distance to start of match
	    	    l = length of match
    		    c = char following match in input
		    } else {
    		    d = 0
        		l = 0
        		c = first char of input
		    }
			printf("d:%d, l:%d, c:%c", d, l, c);
    		
			discard l + 1 chars from front of window
	    	s := pop l + 1 chars from front of input
    		append s to back of window
		}
*/	
	}
	fclose(f);
}

void sdl()
{}
