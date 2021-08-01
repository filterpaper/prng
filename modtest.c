#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


uint8_t jsf8(void) {
	#define rot8(x,k) (((x) << (k))|((x) >> (8 - (k))))
	static uint8_t a = 0xf1, b = 0xee, c = 0xee, d = 0xee;

	uint8_t e = a - rot8(b, 1);
	a = b ^ rot8(c, 4);
	b = c + d;
	c = d + e;
	return d = e + a;
}

int main(int argc, char const *argv[]) {
	/* code */
	for (int i = 1; i < 25687; ++i) {
		int a = i ^ ((i << 1)|(i >> 4));
		int b = i ^ ((i << 3)|(i >> 5));
		printf ("%u %u\n", a, b);
	}
	return 0;
}
