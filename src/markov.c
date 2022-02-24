#include <string.h>

// 7 standard diatonic chords, I ii iii IV V vi vii(dim)
// plus V7
#define NUMBER_OF_VALID_CHORDS (7 + 1)  

int weights[NUMBER_OF_VALID_CHORDS][NUMBER_OF_VALID_CHORDS] = {
// from                                   to
// I  ii iii IV V  vi vii(dim) V7
	{0, 0, 0,  0, 0, 0, 0,       0},  // I
	{0, 0, 0,  0, 0, 0, 0,       0},  // ii
	{0, 0, 0,  0, 0, 0, 0,       0},  // iii
	{0, 0, 0,  0, 0, 0, 0,       0},  // IV
	{0, 0, 0,  0, 0, 0, 0,       0},  // V
	{0, 0, 0,  0, 0, 0, 0,       0},  // vi
	{0, 0, 0,  0, 0, 0, 0,       0},  // vii(dim)
	{0, 0, 0,  0, 0, 0, 0,       0},  // V7
};

int encode_chord(char * chord) {
	// base everything in C major
	// -1 means we couldn't encode
	
	return -1;
}

char * decode_chord(int n, char * buffer) {

	return buffer;
}


