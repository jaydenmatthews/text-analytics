// COMP2521 21T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File
// Written by Jayden Matthews (z5360350)
// 16/07/2021 


#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "stemmer.h"
#include "WFreq.h"

#define MAXLINE 1000
#define MAXWORD 100
#define STOPWORDS 654

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')
#define isProjectGutenberg(c) (strstr((c), "*** START OF") != NULL || strstr((c), "*** END OF") != NULL)

// add function prototypes for your own functions here

void tokenise(char line[MAXLINE + 1]);
void normalise(char line[MAXLINE + 1]);
void linetowords(char line[MAXLINE + 1], char words[MAXLINE][MAXWORD]);
void remstopwords(char words[MAXLINE][MAXWORD], char stopWords[STOPWORDS][MAXWORD]);
void wordstemmer(char words[MAXLINE][MAXWORD]);
void filechecker(char *fileName);
void stopwordgenerator(char stopWords[STOPWORDS][MAXWORD]);
int swBinarySearch(char stopWords[STOPWORDS][MAXWORD], int size, char *word);


int main(int argc, char *argv[]) {
	int   nWords;    // number of top frequency words to show
	char *fileName;  // name of file containing book text

	// process command-line args
	switch (argc) {
		case 2:
			nWords = 10;
			fileName = argv[1];
			break;
		case 3:
			nWords = atoi(argv[1]);
			if (nWords < 10) nWords = 10;
			fileName = argv[2];
			break;
		default:
			fprintf(stderr,"Usage: %s [Nwords] File\n", argv[0]);
			exit(EXIT_FAILURE);
	}
	
    //Checks whether the files exists
	filechecker(fileName);
	
	//Open stopwords txt file and puts them into a array of words (strings)
	char stopWords[STOPWORDS][MAXWORD];
	stopwordgenerator(stopWords);
	
	
	//OPEN THE FILE
	FILE *fp;
    char line[MAXLINE + 1];
    fp = fopen(fileName, "r");
     
     
     
    //Ignore all lines up to and including the "*** START OF" line
    while (fgets(line, MAXLINE + 1, fp) != NULL) {
    	if (strstr(line, "*** START OF") != NULL) {
    		break;
    	}
    }
	
	
	Dict root = DictNew(); 	
	
	// PREPROCESSING & INSERTION OF WORDS
	while (fgets(line, MAXLINE + 1, fp) != NULL && strstr(line, "*** END OF") == NULL) {

	        tokenise(line); 	//tokenises the current line
	        normalise(line); 	//normalises the current line
	        
	        char templine[MAXLINE + 1];  
	        strcpy(templine, line);
	        
	        char words[MAXLINE][MAXWORD]; 
	
	        linetowords(templine, words);  //convert line into array of words
	        remstopwords(words, stopWords); //remove stopwords from the current line
	        wordstemmer(words); //stems the words in the current line
	        
	        int k = 0;
	        while (strcmp(words[k], "\0") != 0) {
	        	
	        	//Inserts words into BST except "X" stopwords case and single chars
	            if (strcmp(words[k], "X") != 0 && strlen(words[k]) >= 2) {
                    	DictInsert(root, words[k]); 
	            } 
	            
	            ++k;
	        }

    }
    
    WFreq wfs[nWords];
    
    int numtw = DictFindTopN(root, wfs, nWords); 

	// PRINTING TOPWORDS   
    int i = 0;
    while (i < numtw) {
    	printf("%d ", wfs[i].freq);
		fputs(wfs[i].word, stdout);
    	printf("\n");
    		++i;
    }
    
    //FREE WFS ARRAY
   	i = 0;
   	while (i < numtw) {
   		free(wfs[i].word);
   		++i;
   	}
    
    //FREE BST
    DictFree(root);
	  
	//CLOSE THE FILE
   	fclose(fp);
	
    return 0;	
}

// MY FUNCTIONS

//Tokenise - Removes non-word characters
void tokenise(char line[MAXLINE + 1]) {
    int i = 0;
	while (line[i] != '\0') {
	    
	    if (!isWordChar(line[i])) { 
	        line[i] = ' ';
	    }
	    
	    i++;
	}
	
}

//Normalise - Changes all uppercase characters to lowercase
void normalise(char line[MAXLINE + 1]) {
    int i = 0;
	while (line[i] != '\0') {
	    line[i] = tolower(line[i]);
	    i++;
	}
}


//LineToWords - Converts a Line of Text to individual words by removing spaces.
void linetowords(char line[MAXLINE + 1], char words[MAXLINE][MAXWORD]) {
    const char s[2] = " ";
	char *token;
	int i = 0;
	
	token = strtok(line, s);

	
	while (token != NULL) {
	    strcpy(words[i], token);
	    words[i][strlen(words[i])] = '\0'; //sets last char of word to '\0'
	    token = strtok(NULL, s);
	    ++i;
	}
	
	words[i][0] = '\0';	//sets last word to null terminate
	
}

//RemStopwords - Removes stopwords from the current line using a Binary Search
void remstopwords(char words[MAXLINE][MAXWORD], char stopWords[STOPWORDS][MAXWORD]) {
	int i = 0;
	
	while (strcmp(words[i], "\0") != 0) {
		if (swBinarySearch(stopWords, STOPWORDS, words[i]) == 1) {
			strcpy(words[i], "X"); 	//replace stopwords with "X"
		}
		i++;
	}
}

//swBinarySearch - A binary search of the stopwords text file.
int swBinarySearch(char stopWords[STOPWORDS][MAXWORD], int size, char *word) {
    int low = 0, high = size - 1;
    int mid;
    while (low <= high) {
    
    	mid = (low + high) / 2;
    	
    	if (strcmp(stopWords[mid], word) == 0) {
    		return 1;
    	} else if (strcmp(word, stopWords[mid]) < 0) {
    		high = mid - 1;
    	} else {
    		low = mid + 1;
    	}
    	
    }
    
    return 0;
}

//WordStemmer - Stems the words on the current line
void wordstemmer(char words[MAXLINE][MAXWORD]) {
    int i = 0;
    
    while (strcmp(words[i], "\0") != 0) { //Passes each word into the stem function
        int r = strlen(words[i]) - 1;
        stem(words[i], 0, r);
        ++i;
    }
}

//FileChecker - Executes error handling for file opening.
void filechecker(char *fileName) {
    FILE *fp;
    fp = fopen(fileName, "r");
    char line[MAXLINE];
    
    if (fp == NULL) { //ERROR: for if fileName cannot be found
        fprintf(stderr, "Can't open %s\n", fileName);
        exit(EXIT_FAILURE);
    }
    
    int hasStart = 0;
    int hasEnd = 0;

    
	while (fgets(line, MAXLINE + 1, fp) != NULL && hasEnd == 0) { 
		if(strstr(line, "*** START OF") != NULL) { //if fp contains START OF	    
            hasStart = 1; 
            
		} else if (strstr(line, "*** END OF") != NULL) { //if fp contains END OF
			hasEnd = 1; 
			
		} 
	}
	
	if (hasStart == 0 || hasEnd == 0) { //ERROR: for no start/end tags
		fprintf(stderr, "Not a Project Gutenberg book\n");
		exit(EXIT_FAILURE);
	}
	
	fclose(fp);
	
}

//StopWordGenerator - Puts the stopwords into an array format
void stopwordgenerator(char stopWords[STOPWORDS][MAXWORD]) {
    int j = 0;
	FILE *sw;
	sw = fopen("stopwords", "r");
	
    if(sw == NULL) { //ERRROR: for no stopwords file
        fprintf(stderr, "Can't open stopwords\n");
        exit(EXIT_FAILURE);
    }
	
	while(j < STOPWORDS) { 
	    fgets(stopWords[j], MAXWORD, sw);
	    stopWords[j][strlen(stopWords[j])-1] = '\0'; //removes newline character
	    ++j;
	}
	
	fclose(sw);
	
}
