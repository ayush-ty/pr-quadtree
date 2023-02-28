/*-------------------------------------------------------------- 
..Project: PR QuadTrees
  array.c :
          = the implementation of module array of the project

		  ****LARGELY TAKEN FROM WORKSHOP 3 ADS AT UNIMELB****
----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "array.h"
#include "data.h"

// creates & returns an empty array
array_t *arrayCreate() {
	array_t *arr = malloc(sizeof(*arr));
	assert(arr);
	int size = INIT_SIZE;      // "size" used for convenience
	arr->size = size;
	arr->A = malloc(size * sizeof(*(arr->A)));
	assert(arr->A);
	arr->n = 0;
	return arr;
}

// free memory used by array "arr"
void arrayFree(array_t *arr) {
	for (int i = 0; i < arr->n; i++) {
		footpathFree(arr->A[i]);
	}
	free(arr->A);
	free(arr);
}


// shrinks the array, to reduce array size to the same 
//    as the number of element used
void arrayShrink(array_t *arr) {
	if (arr->size != arr->n) {
		// the new size should be at least INIT_SIZE
		arr->size = (arr->n > INIT_SIZE) ? arr->n : INIT_SIZE;
		arr->A = realloc(arr->A, arr->size * sizeof(*(arr->A)));
		assert(arr->A);
	}
}


// makes sure that array's size exceeds its number of elements
//    ie. there is space for adding new element
void arrayEnableInsert(array_t *arr) {
	if (arr->n == arr->size) {
		arr->size <<= 1;       // same as arr->size *= 2;
		arr->A= realloc(arr->A, arr->size * sizeof(*(arr->A)));
		assert(arr->A);
	}
}


// inserts data "footpath" into array "arr", ensuring "arr" is sorted 
void sortedArrayInsert(array_t *arr, footpath_t *footpath) {
	int i;
	arrayEnableInsert(arr);
	// starting from the end, shift all elements > s one position to the right
	for (i = arr->n - 1; i >= 0 && footpathCmpID(footpath, arr->A[i]) == -1; i-- ) {
		arr->A[i + 1] = arr->A[i];
	}
	// now "footpath" should be in A[i+1]
	arr->A[i + 1] = footpath;
	arr->n++;
}

// searches for footpath with "id" in sorted array "arr"
// returns the pointer to the found footpath, NULL if not found
footpath_t *arrayBinarySearch(array_t *arr, int id) {
	footpath_t **A = arr->A;    // for convenience 
	int mid, lo = 0, hi = arr->n - 1;
	while (lo <= hi) {
		mid = (lo + hi)/2;
		int cmp = id - footpathGetID(A[mid]);
		if (cmp == 0) return A[mid];
		if (cmp < 0) {
			hi = mid - 1;
		} else {
			lo = mid + 1;
		}
	}
	return NULL;
}
