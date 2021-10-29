// Dict.c ... implementation of the Dictionary ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "WFreq.h"



// you may define your own structs here
struct DictRep {
    Dict left;
    Dict right;
    char *word;
    int freq;
    int height;
};



// Preorder search of binary tree
Dict preorderSearch(Dict root, char *word) {
    if (root == NULL || strcmp(word, root->word) == 0) {
        return root;
        
    } else if (strcmp(word, root->word) < 0) {
        return preorderSearch(root->left, word);
        
    } else if (strcmp(word, root->word) > 0) {
        return preorderSearch(root->right, word);
        
    } else {
        return NULL;
        
    }

}

// Creates a Node for a specific word
Dict DictNodeCreate(char *word) {
    Dict node = malloc(sizeof(struct DictRep));
    node->left = NULL;
    node->right = NULL;
    node->word = malloc(101);
    strcpy(node->word, word);
    node->freq = 1;   
    node->height = 1;
    
    return node;
}


// Creates a new Dictionary
Dict DictNew(void) {
    Dict root = malloc(sizeof(struct DictRep));
    root->left = NULL;
    root->right = NULL;
    root->word = malloc(101);
    root->freq = 0;  
    root->height = 0; 
    
    return root;
}

// Frees the given Dictionary
void DictFree(Dict d) {
	if (d == NULL) {
		return;
	}
	
	DictFree(d->left);
  	DictFree(d->right);
    free(d->word);
    d->word = NULL;
    free(d);
   	
}

int max(int a, int b) {
	return (a > b)? a : b;
}

int height(Dict node) {
	if (node == NULL) {
		return 0;
	} 
	return node->height;
}

Dict rightRotate(Dict node) {
	Dict new_root = node->left;
	
	if (new_root == NULL) { //case where node is a leaf-node
		return node;
	}
	
	Dict temp = new_root->right;
	
	//perform rotation
	new_root->right = node;
	node->left = temp;
	
	//update heights
	node->height = max(height(node->left), height(node->right))+1;
    new_root->height = max(height(new_root->left), height(new_root->right))+1;
 
    // Return new root
    return new_root;
	
}

Dict leftRotate(Dict node) {
	Dict new_root = node->right;
	
	if (new_root == NULL) { //case where node is a leaf-node
		return node;
	}
	
	Dict temp = new_root->left;
	
	//perform rotation
	new_root->left = node;
	node->right = temp;
	
	//update heights
	node->height = max(height(node->left), height(node->right))+1;
    new_root->height = max(height(new_root->left), height(new_root->right))+1;
 
    // Return new root
    return new_root;

}

int getBalance(Dict node) {
	if (node == NULL) {
		return 0;
	}
	return height(node->left) - height(node->right);
}

Dict DictAVLinsert(Dict node, char *word) {    
    //regular BST insertion
    if (node == NULL) {
        return DictNodeCreate(word);   
    }
    
    if (strcmp(word, node->word) < 0) { //word is lexographically less
        node->left = DictAVLinsert(node->left, word);
        
    } else if (strcmp(word, node->word) > 0) { //word is lexographically more
        node->right = DictAVLinsert(node->right, word);      
        
    } else { //word is same
            node->freq += 1;
            return node;     
    }
    
    //update height of ancestor
    node->height = 1 + max(height(node->left), height(node->right));
    
    int balance = getBalance(node);
    
	//Left Left Case
    if (balance > 1 && strcmp(word, node->word) < 0) {  
        return rightRotate(node);
        
    }
    
    // Right Right Case
    if (balance < -1 && strcmp(word, node->word) > 0) { 
        return leftRotate(node);
        
    } 
    // Left Right Case
    if (balance > 1 && strcmp(word, node->word) > 0) {
        node->left =  leftRotate(node->left);
        return rightRotate(node);
    }
 
    // Right Left Case
    if (balance < -1 && strcmp(word, node->word) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
 
    /* return the (unchanged) node pointer */
    return node;
}


// Inserts an occurrence of the given word into the Dictionary
void DictInsert(Dict d, char *word) {    
	
    if (d->freq == 0) { //only applies to root node
    	strcpy(d->word, word);
    	d->freq += 1;
    	return;
    }
    
    if (strcmp(word, d->word) < 0) { //word is lexographically less 
    	d->left = DictAVLinsert(d->left, word);
    	
    } else if (strcmp(word, d->word) > 0) {//word is lexographically more 
    	d->right = DictAVLinsert(d->right, word);
    	
    } else { //word is same;
    	d->freq += 1;
    }
    
    return;
}

// Returns the occurrence count of the given word. Returns 0 if the word
// is not in the Dictionary.
int DictFind(Dict d, char *word) {
    Dict temp = preorderSearch(d, word); //Preorder search for the word
    
    if (temp == NULL) {
        return 0;
    } else {
    	return temp->freq;
    }
}

//MaxFinder: Searches the tree for the highest value less than or equal to the 
// previously found value.
void MaxFinder(WFreq *temp, Dict root, int max, int val, int n, Dict foundtw) {
    if (root == NULL) {
        return;
    }
    
    MaxFinder(temp, root->right, max, val, n, foundtw);
    
    if (root->freq >= max && root->freq <= val && DictFind(foundtw, root->word) == 0) {
            temp->freq = root->freq; 
            strcpy(temp->word, root->word);
            max = root->freq;      
    }
    
    MaxFinder(temp, root->left, max, val, n, foundtw); 
     
}

// Finds  the top `n` frequently occurring words in the given Dictionary
// and stores them in the given  `wfs`  array  in  decreasing  order  of
// frequency,  and then in increasing lexicographic order for words with
// the same frequency. Returns the number of WFreq's stored in the given
// array (this will be min(`n`, #words in the Dictionary)) in  case  the
// Dictionary  does  not  contain enough words to fill the entire array.
// Assumes that the `wfs` array has size `n`.
int DictFindTopN(Dict d, WFreq *wfs, int n) {
    
    int counter = 0;
    int max = 2800;
    int val = 2800;
    
    int nWords = n;
    
    Dict foundtw = DictNew();
    strcpy(foundtw->word, "");
    
    while (counter < nWords && max != 0) {
    
    	// Temp WFREQ struct
		WFreq *temp = malloc(sizeof(WFreq));
		temp->word = malloc(101);
		temp->freq = 0;
		
		MaxFinder(temp, d, max, val, nWords, foundtw); 
		   
        if (temp->freq != 0) { //if MaxFinder returned a value
            DictInsert(foundtw, temp->word);
            max = temp->freq;
            val = temp->freq;
 			
 			WFreq new_topword;
 			new_topword.word = strdup(temp->word);
 			new_topword.freq = temp->freq;
 			
            wfs[counter] = new_topword; // Store word found
   
            ++counter;
            
        } else {
            --max;
        }
		free(temp->word);
		free(temp);	   	
    }
    DictFree(foundtw);
    return counter;
}

// Displays the given Dictionary. This is purely for debugging purposes,
// so  you  may  display the Dictionary in any format you want.  You may
// choose not to implement this.
void DictShow(Dict d) {

}

