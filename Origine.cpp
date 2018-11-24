#include <emmintrin.h>
#include <xmmintrin.h>

//ss2 intrinsics

#include <vector>
#include <iostream>

#define NEW(a,b) new a; addName(#a); std::cout << "ALLOCATED " << #a <<" line: "<<##__LINE__ << " " << ##__FUNCTION__ << std::endl
#define NVARHEAP 100
#define NAMELENGTH 30


void* vett[NVARHEAP];
char names[NVARHEAP][NAMELENGTH];
int currentPlace = 0;
int previousPlace = 0;
int maxPlace = 0;
int counter = 0;

void* operator new(size_t size) {
	
	//std::cout << "allocated" << std::endl;
	void* p = malloc(size);
	vett[currentPlace] = p;
	previousPlace = currentPlace;
	if (previousPlace > maxPlace) maxPlace = previousPlace;
	for (int i = 0; i < NVARHEAP; i++) {
		if (vett[i] == nullptr) {
			currentPlace = i;
			break;
		}
	}

	counter++;
	return p;
}

void operator delete(void* p) {
	//std::cout << "deallocated" << std::endl;
	for (int i = 0; i < maxPlace+1; i++) {
		if (vett[i] == p) {
			vett[i] = nullptr;
			memset(names[i], 0, NAMELENGTH);
			counter--;
			currentPlace = i;

			if(i==maxPlace)maxPlace--;

			break;
		}
	}

	free(p);
}

void addName(char* a) {
	std::cout << a;
	strcpy_s(names[previousPlace], NAMELENGTH, a);
}

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
		std::cout << "num: " << (int)(((uint8_t*)dstCasted)[i]) << std::endl;
	}

}

int main(int argc, char **argv){
	
	uint8_t* ciao = NEW(uint8_t[20]);
	uint8_t* ciaoa = NEW(uint8_t[20]);

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

	std::cout << "size of __m128i: " << sizeof(__m128i) << std::endl;
	std::cout << "result of the simd operation" << (unsigned int)dest[15] << std::endl;

	_aligned_free(dest);
	_aligned_free(b);
	_aligned_free(a);

	delete ciao;
	delete ciaoa;

	int* aaa = NEW(int);
	delete aaa;

	return 0;
}


