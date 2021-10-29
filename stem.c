// Stems words and prints them one per line

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stemmer.h"

#define MAXWORD 100

int main(void) {
	char word[MAXWORD + 1];
	while (scanf("%s", word) == 1) {
		stem(word, 0, strlen(word) - 1);
		printf("%s\n", word);
	}
}

