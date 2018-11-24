//ss2 intrinsics
#include <emmintrin.h>
#include <xmmintrin.h>



//seems that my mac doesn't have support for avx512
//AVX512
#include <immintrin.h>

#include <vector>
#include <iostream>
#include <string.h>

void simdBitwiseAnd(unsigned char *dst, const unsigned char *src, unsigned block_size) {
	const __m128i *srcCasted = (__m128i *) src;
	__m128i *dstCasted = (__m128i *) dst;


	__m128i xmm1;
	__m128i xmm2;

	xmm1 = _mm_load_si128(srcCasted);
	xmm2 = _mm_load_si128(dstCasted);
	xmm1 = _mm_and_si128(xmm1, xmm2);

	_mm_store_si128(dstCasted, xmm1);

}

void simdSubtraction(unsigned char *dst, const unsigned char *src, unsigned block_size) {

	const __m128i *srcCasted = (__m128i *) src;
	__m128i *dstCasted = (__m128i *) dst;


	__m128i xmm1;
	__m128i xmm2;

	xmm1 = _mm_load_si128(srcCasted);
	xmm2 = _mm_load_si128(dstCasted);

	//this does the subtraction without the sign(with the u)
	//xmm1 = _mm_subs_epu8(xmm2, xmm1);

	//xmm1 = _mm_sl
	//this shifts by 1!
	xmm1 = _mm_and_si128(_mm_set1_epi8(0xFF >> 1), _mm_srli_epi32(xmm1, 1));
	xmm2 = _mm_and_si128(_mm_set1_epi8(0xFF >> 1), _mm_srli_epi32(xmm2, 1));

	//subtracts as if they where signed(with the i)
	//xmm1 = _mm_subs_epi8(xmm2, xmm1);

	//http://0x80.pl/notesen/2018-03-11-sse-abs-unsigned.html

	//this part gives out the result of the absolute difference, the subs epu gives a value from 0 to 255,
	//this positive value is present only in the right side, the or combines all the sides

	const __m128i ab = _mm_subs_epu8(xmm2, xmm1);
	const __m128i ba = _mm_subs_epu8(xmm1, xmm2);

	xmm1 = _mm_or_si128(ab, ba);

	//reshift.(to double it back)
	xmm1 = _mm_and_si128(_mm_set1_epi8(0xFF << 1), _mm_slli_epi32(xmm1, 1));

	_mm_store_si128(dstCasted, xmm1);

	for (unsigned int i = 0; i < 16; i++) {
		std::cout<<(int)*(( uint8_t* )(dstCasted)+i)<<std::endl;
		
	}

}

void simdSubtractionAVX(unsigned char *dst, const unsigned char *src, unsigned block_size) {

	const __m256i *srcCasted = (__m256i *) src;
	__m256i *dstCasted = (__m256i *) dst;


	__m256i xmm1;
	__m256i xmm2;
 	
	xmm1 = _mm256_stream_load_si256(srcCasted);
	xmm2 =  _mm256_stream_load_si256(dstCasted);

	xmm2 = _mm256_subs_epu8(xmm2,xmm1);
	//xmm1 = _mm_load_si128(srcCasted);
	//xmm2 = _mm_load_si128(dstCasted);

	////this does the subtraction without the sign(with the u)
	////xmm1 = _mm_subs_epu8(xmm2, xmm1);

	////xmm1 = _mm_sl
	////this shifts by 1!
	//xmm1 = _mm_and_si128(_mm_set1_epi8(0xFF >> 1), _mm_srli_epi32(xmm1, 1));
	//xmm2 = _mm_and_si128(_mm_set1_epi8(0xFF >> 1), _mm_srli_epi32(xmm2, 1));

	////subtracts as if they where signed(with the i)
	////xmm1 = _mm_subs_epi8(xmm2, xmm1);

	////http://0x80.pl/notesen/2018-03-11-sse-abs-unsigned.html

	////this part gives out the result of the absolute difference, the subs epu gives a value from 0 to 255,
	////this positive value is present only in the right side, the or combines all the sides

	//const __m128i ab = _mm_subs_epu8(xmm2, xmm1);
	//const __m128i ba = _mm_subs_epu8(xmm1, xmm2);

	//xmm1 = _mm_or_si128(ab, ba);

	////reshift.(to double it back)
	//xmm1 = _mm_and_si128(_mm_set1_epi8(0xFF << 1), _mm_slli_epi32(xmm1, 1));

	//_mm_store_si128(dstCasted, xmm1);

	//for (unsigned int i = 0; i < 16; i++) {
		//std::cout<<(int)*(( uint8_t* )(dstCasted)+i)<<std::endl;
		
	//}

}
int main(int argc, char **argv){
	
	//uint8_t* ciao = NEW(uint8_t[20]);
	//uint8_t* ciaoa = NEW(uint8_t[20]);
	uint8_t* ciao =( uint8_t* ) malloc( sizeof(uint8_t)*(20));
	uint8_t* ciaoa =( uint8_t* ) malloc( sizeof(uint8_t)*(20));

	unsigned char *dest;
	unsigned char *a;
	unsigned char *b;

	//_aligned_malloc(sizeof(__m128i), sizeof(__m128i));

	a = (unsigned char*)_mm_malloc(sizeof(__m512i), sizeof(__m512i));
	b = (unsigned char*)_mm_malloc(sizeof(__m512i), sizeof(__m512i));
	dest = (unsigned char*)_mm_malloc(sizeof(__m512i), sizeof(__m512i));

	memset(a, 0, sizeof(__m512i));
	memset(b, 0, sizeof(__m512i));
	memset(dest, 0, sizeof(__m512i));


	a[15] = 0x02;
	b[15] = 0x03;

	// 128-bit SIMD Bitwise AND
	memcpy(dest, a, sizeof(__m128i));

	simdBitwiseAnd(dest, b, sizeof(__m128i));

	memset(a, 0, sizeof(__m128i));
	memset(b, 0, sizeof(__m128i));
	memset(dest, 0, sizeof(__m128i));

	uint8_t vett[16] = { 250,254,250,0,255,254,250,0 ,255,254,250,0 ,255,254,250,0 };
	uint8_t vettS[16] = { 255,249,245,0,250,249,245,0, 250,249,245,0, 250,249,245,0};
	
	//puts 16 * 1byte inside the mmx vectors and then does the subtraction.
	
	memcpy(dest, vett, sizeof(uint8_t) * sizeof(__m128i));
	memcpy(a, vettS, sizeof(uint8_t) * sizeof(__m128i));

	//*a = 0x10;
	//*dest = 0x11;
	simdSubtraction(dest, a, sizeof(__m128i));

	//uint8_t vett[16] = { 250,254,250,0,255,254,250,0 ,255,254,250,0 ,255,254,250,0 };
	//uint8_t vettS[16] = { 255,249,245,0,250,249,245,0, 250,249,245,0, 250,249,245,0};
	
	//puts 16 * 1byte inside the mmx vectors and then does the subtraction.
	
	memcpy(dest, vett, sizeof(uint8_t) * sizeof(__m256i));
	memcpy(a, vettS, sizeof(uint8_t) * sizeof(__m256i));

	simdSubtractionAVX(dest, a, sizeof(__m256i));


	_mm_free(dest);
	_mm_free(b);
	_mm_free(a);


	return 0;
}


