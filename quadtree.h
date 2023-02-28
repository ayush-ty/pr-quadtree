/* Project: PR QuadTrees
* quadtree.h :
*            = interface of the module quadtree of the project
*
* Created by Ayush Tyagi (ayusht@student.unimelb.edu.au)
* 03/09/2022
*
* ----------------------------------------------------------------*/
#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include "data.h"
#include "array.h"
#include "linkedlist.h"

// epsilon value used for comparing equality of variables of type double
#define EPSILON 1e-12  

// struct to keep track of region spanned by node
typedef struct rectangle2D {
    long double botLeftX;
    long double botLeftY;
    long double topRightX;
    long double topRightY;
} rectangle_t;

typedef struct dataPoint {
    double x;
    double y;
} point_t;


typedef struct qTreeNode {
    point_t* point;
    struct qTreeNode *NW;
    struct qTreeNode *NE;
    struct qTreeNode *SW;
    struct qTreeNode *SE;
    rectangle_t* rectangle;  // span of node
    array_t* footpaths;  // dynamic sorted array to contains `footpaths` for the node
    char* label;  // to specify which quadrant node is in 
} qTreeNode_t;

typedef struct quadTree {
    qTreeNode_t* root;
} qTree_t;

// creates and returns empty quadTree
qTree_t* qTreeCreate(rectangle_t* rectangle);

// creates and returns a new point
point_t* newPoint(double x, double y);

// function to print `point` to `outFile`
void printPoint(FILE* outFile, point_t* point);

// function to create and return a new rectangle
rectangle_t* newRectangle(long double botLeftX, long double botLeftY,
                        long double topRightX, long double topRightY);

// function to check if `point` should be inside `rectangle`
// retruns 1(true) or 0(false)
int inRectangle(rectangle_t* rectangle, point_t* point);

// function to check if `point` should be inside `rectangle` for stage 4
// includes left and top border of rectangle
// returns 1(true) or 0(false)
int inRectangleStage4(rectangle_t* rectangle, point_t* point);

// handle function to insert `point` to `qTree`
qTree_t* qTreeInsert(qTree_t* qTree, point_t* point, footpath_t* footpath);

// recursively inserts point into qTree
void qTreeInsertPoint(qTreeNode_t* root, point_t* point, footpath_t* footpath);

// creates and returns a node for `point` with `rectangle`and `label` 
qTreeNode_t* createNode(point_t* point, rectangle_t* rectangle, char* label);

// returns 0,1,2 or 3 to specify which quadrant of `node` `point` belongs in
// returns -1 if point doesn't belong in either quadrant
int findQuadrant(qTreeNode_t* node, point_t* point);

// inserts `point` into appropriate quadrant of `node`
int insertIntoQuadrant(qTreeNode_t* node, point_t* point, footpath_t* footpath);

// function to split `node` into four quadrants, making it an inner node
// function also reinserts current values of `node` into the appropriate new quadrant
void splitNode(qTreeNode_t* node);

// handle to search `qTree` for `point` and returns list of quadrants accessed in order to reach `point`
void qTreeSearch(qTree_t *qTree, point_t* point, list_t* quadrants, 
                FILE* infoFile, char* xBuffer, char* yBuffer);

// recursively searches qTree for `point` by checking `node`
void qTreeSearchNode(qTreeNode_t* node, point_t* point, list_t* quadrants, 
                        FILE* infoFile,  char* xBuffer, char* yBuffer);

// recursively searches qTree for points within `range`
// stores unique footpaths of those points and direction 
array_t* queryRange(qTreeNode_t* node, rectangle_t* range, 
                array_t* footpathVisited, list_t* quadrants, array_t* results);

// checks if rectangles `a` and `b` have any overlap
int rectangleOverlap(rectangle_t* a, rectangle_t* b);

// checks if rectangle corner with points `x` and `y` are in `rectangle`
int rectangleCornerInRectangle(long double x, long double y,
                                 rectangle_t* rectangle);

// inserts `footpath` into `arr` making sure `arr` stays sorted by footpathID
void insertFootpathInArray(array_t *arr, footpath_t *footpath);

// handle function to free allocated memory used by `qTree`
void qTreeFree(qTree_t *qTree);

// function to recursively free qTree by traversing every `node`
void qTreeFreeNode(qTreeNode_t* node);

#endif
