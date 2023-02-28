/*-------------------------------------------------------------- 
..Project: PR QuadTrees
  array.c : 
          = the interface of module array of the project

		  ****LARGELY TAKEN FROM WORKSHOP 3 ADS AT UNIMELB****
----------------------------------------------------------------*/

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "data.h"

#define INIT_SIZE 2     // initial size of arrays 

typedef struct array {
	footpath_t **A;      // use as array of footpath_t *, ie. footpath_t *A[]
	int size;           // current array size
	int n;              // current number of elements
} array_t;

// creates & returns an empty array
array_t *arrayCreate();

// free memory used by array "arr"
void arrayFree(array_t *arr);

// inserts data "footpath" into array "arr", ensuring "arr" is sorted 
void sortedArrayInsert(array_t *arr, footpath_t *footpath);

// searches for footpath with "id" in sorted array "arr"
// returns the pointer to the found footpath, NULL if not found
footpath_t *arrayBinarySearch(array_t *arr, int id);

// shrinks the array, to reduce array size to the same 
// as the number of element used
void arrayShrink(array_t *arr);

#endif
