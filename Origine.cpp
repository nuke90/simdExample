#include <emmintrin.h>
#include <xmmintrin.h>
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


