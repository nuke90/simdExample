#include <emmintrin.h>
#include <xmmintrin.h>

#include <iostream>


void simdBitwiseAnd(unsigned char *dst, const unsigned char *src, unsigned block_size){
	const __m128i *srcCasted = (__m128i *) src;
	__m128i *dstCasted = (__m128i *) dst;


	__m128i xmm1;
	__m128i xmm2;

	xmm1 = _mm_load_si128(srcCasted);
	xmm2 = _mm_load_si128(dstCasted);
	xmm1 = _mm_and_si128(xmm1, xmm2);

	_mm_store_si128(dstCasted, xmm1);

}

int main(int argc, char **argv){
	
	unsigned char *dest;
	unsigned char *a;
	unsigned char *b;

	//_aligned_malloc(sizeof(__m128i), sizeof(__m128i));

	a = (unsigned char*)_aligned_malloc(sizeof(__m128i), sizeof(__m128i));
	b = (unsigned char*)_aligned_malloc(sizeof(__m128i), sizeof(__m128i));
	dest = (unsigned char*)_aligned_malloc(sizeof(__m128i), sizeof(__m128i));

	memset(a, 0, sizeof(__m128i));
	memset(b, 0, sizeof(__m128i));
	memset(dest, 0, sizeof(__m128i));


	a[15] = 0x02;
	b[15] = 0x03;

	// 128-bit SIMD Bitwise AND
	memcpy(dest, a, sizeof(__m128i));

	simdBitwiseAnd(dest, b, sizeof(__m128i));

	std::cout << "size of __m128i: " << sizeof(__m128i) << std::endl;
	std::cout << "result of the simd operation" << (unsigned int)dest[15] << std::endl;

	free(dest);
	free(b);
	free(a);

	return 0;
}