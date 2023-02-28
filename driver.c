/* Project: PR Quadtrees
* driver.c :
*            = main program of the project
*
* Created by Ayush Tyagi (ayusht@student.unimelb.edu.au)
* 03/09/2022
*
* -------------------------------------------------------------- 
* Read footpath data from data file specified in csv format
* and make a quadtree using the starting and ending latitudes and 
* longtitudes of the footpaths, adding querying functionality
*
* Stage 3: 
* Accept co-ordinate pairs from stdin, search the constructed quadtree
* for the point region containing the co-ordinate pair and print all 
* matching records to the output file
* 
*
* Stage 4:
* accept sets of pairs of co-ordinate long double type values from stdin,
* and efficiently use the quadtree to find all footpaths which are
* within the bounds of the query
*
* ----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "data.h"
#include "quadtree.h"
#include "array.h"
#include "linkedlist.h"

#define NOTFOUND "NOTFOUND"
#define EXACT_QUERY 3
#define RANGE_QUERY 4

// makes a quadtree from input file and quadtree span from command line arguments
qTree_t* getQuadTree(char* fileName, char* botLeftX, char* botLeftY, char* topRightX, char* topRightY);

// function to query qtree for point region matches through `inFile`
// prints to `outFile` and `infoFile`
void qTreeExactQuerying(char *dataFile, char* botLeftX, char* botLeftY, char* topRightX, char* topRightY,
                        FILE *inFile, FILE *outFile, FILE *infoFile);

// function to query qtree for region matches through `inFile`, looks for all points within range given by `inFile`
// prints to `outFile` and `infoFile`
void qTreeRangeQuerying(char* dataFile, char* botLeftX, char* botLeftY, char* topRightX, char* topRightY,
                        FILE *inFile, FILE *outFile, FILE *infoFile);

int main(int argc, char *argv[]) {
    FILE *infoFile = fopen(argv[3], "w");
	assert(infoFile);

     // runs respective query system
    switch (atoi(argv[1])) {
        case EXACT_QUERY:
            qTreeExactQuerying(argv[2], argv[4], argv[5], argv[6], argv[7], stdin, stdout, infoFile);
        case RANGE_QUERY:
            qTreeRangeQuerying(argv[2], argv[4], argv[5], argv[6], argv[7], stdin, stdout, infoFile);
    }

    fclose(infoFile);
    return 0;
}

// makes a quadtree from input file and quadtree span from command line arguments
qTree_t* getQuadTree(char* fileName, char* botLeftX, char* botLeftY, char* topRightX, char* topRightY) {
    FILE *inFile = fopen(fileName, "r");
    assert(inFile);

    // constructing rectangle spanned by the tree
    rectangle_t* rootRectangle = newRectangle(strtold(botLeftX, NULL), strtold(botLeftY, NULL),
                                            strtold(topRightX, NULL), strtold(topRightY, NULL));

                                    
    qTree_t* qTree = qTreeCreate(rootRectangle);

    footpathSkipHeaderLine(inFile);

    // variables needed for getline function
    char *linePtr = NULL;
    size_t len = 0;

    footpath_t *footpath;
    
    // variable to duplicate `footpath` to avoid freeing memory twice
    footpath_t *dupFootpath;  
    

    while (getline(&linePtr, &len, inFile) != -1) {
        footpath = footpathRead(linePtr);
        dupFootpath = malloc(sizeof(*dupFootpath));
        assert(dupFootpath);

        // duplicating `footpath` to `dupFootpath`
        memcpy (dupFootpath, footpath, sizeof(*dupFootpath));

        // ensuring a deep copy of `footpath`
        dupFootpath->address = strdup(footpath->address);
        dupFootpath->clueSa = strdup(footpath->clueSa);
        dupFootpath->assetType = strdup(footpath->assetType);
        dupFootpath->segSide = strdup(footpath->segSide);
        

        point_t* startPoint = newPoint(footpath->startLon, footpath->startLat);
        point_t* endPoint = newPoint(dupFootpath->endLon, dupFootpath->endLat);

        qTreeInsert(qTree, startPoint, footpath);
        qTreeInsert(qTree, endPoint, dupFootpath);
    }

    free(linePtr);
	fclose(inFile);

	return qTree;
}

// function to query qtree for point region matches through `inFile`
// prints to `outFile` and `infoFile`
void qTreeExactQuerying(char *dataFile, char* botLeftX, char* botLeftY, char* topRightX,
                char* topRightY, FILE *inFile, FILE *outFile, FILE *infoFile) {

    qTree_t* qTree = getQuadTree(dataFile, botLeftX, botLeftY, topRightX, topRightY);

    // variables needed for getline function
    char* linePtr = NULL;
    size_t len = 0;

    while (getline(&linePtr, &len, inFile) != -1) {
        // formatting input read from a line
        char* x = strtok(linePtr, " ");
        char* y = strtok(NULL, "\n");

        // variable to store which quadrants of tree visited to reach match
        list_t* quadrants = listCreate();

        point_t* query = newPoint(atof(x), atof(y));

        // searching `qTree` for `query`, updating `quadrants` to keep track of quadrants visited
        qTreeSearch(qTree, query, quadrants, infoFile, x, y);

        if (quadrants->n == 0) {
            fprintf(outFile, "%s %s --> %s\n", x, y, NOTFOUND);
        } else {
            fprintf(outFile, "%s %s --> ", x, y);
            listPrint(quadrants, outFile);
        }
        free(query);
        listFree(quadrants);
    }
    free(linePtr);
    qTreeFree(qTree);
}

// function to query qtree for region matches through `inFile`, looks for all points within range given by `inFile`
// prints to `outFile` and `infoFile`
void qTreeRangeQuerying(char* dataFile, char* botLeftX, char* botLeftY, char* topRightX,
                 char* topRightY, FILE *inFile, FILE *outFile, FILE *infoFile) {
                     
    qTree_t* qTree = getQuadTree(dataFile, botLeftX, botLeftY, topRightX, topRightY);

    // variables needed for getline function
    char* linePtr = NULL;
    size_t len = 0;

    while (getline(&linePtr, &len, inFile) != -1) {
        // formatting input read from a line
        char* botLeftX = strtok(linePtr, " ");
        char* botLeftY = strtok(NULL, " ");
        char* topRightX = strtok(NULL, " ");
        char* topRightY = strtok(NULL, "\n");

        // variable to store which quadrants of tree visited to reach match
        list_t* quadrants = listCreate();

        // query range we use to search points within
        rectangle_t* range = newRectangle(strtold(botLeftX, NULL), strtold(botLeftY, NULL),
                            strtold(topRightX, NULL), strtold(topRightY, NULL));

        // variable to keep track of footpaths visited to avoid printing duplicate footpaths
        array_t* footpathVisited = arrayCreate();

        array_t* results = arrayCreate();

        // searches quad tree for points within range
        queryRange(qTree->root, range, footpathVisited, quadrants, results);  

        fprintf(infoFile, "%s %s %s %s\n", botLeftX, botLeftY, topRightX, topRightY);
        for (int i = 0; i < results->n; i++)
            footpathPrint(results->A[i], infoFile);

        if (quadrants->n == 0) {
            fprintf(outFile, "%s %s %s %s --> %s\n", botLeftX, botLeftY,
                    topRightX, topRightY, NOTFOUND);
        } else {
            fprintf(outFile, "%s %s %s %s -->", botLeftX, botLeftY,
                     topRightX, topRightY);
                     
            listPrint(quadrants, outFile);
        }
        listFree(quadrants);
        free(range);

        // freeing `footpathVisited` array
        for (int i = 0; i < footpathVisited->n; i++) {
            // setting to NULL because it is freed later
            footpathVisited->A[i] = NULL;
        }
        free(footpathVisited->A);
        free(footpathVisited);

        // freeing `results` array
        for (int i = 0; i < results->n; i++) {
            // setting to NULL because it is freed later
            results->A[i] = NULL;
        }
        free(results->A);
        free(results);
    }
    free(linePtr);
    qTreeFree(qTree);
}
