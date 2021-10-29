// WFreq.h ... definition of (word, freq) type

#ifndef WFREQ_H
#define WFREQ_H

typedef struct WFreq {
	char  *word;  // word buffer (dynamically allocated)
	int    freq;  // count of number of occurences
} WFreq;

#endif

