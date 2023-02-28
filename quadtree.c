/* Project: PR QuadTrees
* quadtree.c :
*            = implementation of the module quadtree of the project
*
* Created by Ayush Tyagi (ayusht@student.unimelb.edu.au)
* 03/09/2022
*
* ----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "quadtree.h"
#include "array.h"
#include "linkedlist.h"

// creates and returns a new point
point_t* newPoint(double x, double y) {
    point_t* point = malloc(sizeof(*point));
    assert(point);

    point->x = x;
    point->y = y;

    return point;
}

// function to print `point` to `outFile`
void printPoint(FILE* outFile, point_t* point) {
    fprintf(outFile, "%lf %lf\n", point->x, point->y);
}

// function to create and return a new rectangle
rectangle_t* newRectangle(long double botLeftX, long double botLeftY,
                        long double topRightX, long double topRightY) {

    rectangle_t* rectangle = malloc(sizeof(*rectangle));
    assert(rectangle);

    rectangle->botLeftX = botLeftX;
    rectangle->botLeftY = botLeftY;
    rectangle->topRightX = topRightX;
    rectangle->topRightY = topRightY;

    return rectangle;
}

// function to check if `point` should be inside `rectangle` for stage 3
// doesn't include left and top border of rectangle
// returns 1(true) or 0(false)
int inRectangle(rectangle_t* rectangle, point_t* point) {
    if (point->x > rectangle->botLeftX && point->x <= rectangle->topRightX) {
        if (point->y >= rectangle->botLeftY && point->y < rectangle->topRightY) {
            return 1;
        }
    }
    return 0;
}

// function to check if `point` should be inside `rectangle` for stage 4
// includes left and top border of rectangle
// returns 1(true) or 0(false)
int inRectangleStage4(rectangle_t* rectangle, point_t* point) {
    if (point->x >= rectangle->botLeftX && point->x <= rectangle->topRightX) {
        if (point->y >= rectangle->botLeftY && point->y <= rectangle->topRightY) {
            return 1;
        }
    }
    return 0;
}

// creates and returns empty quadTree
qTree_t* qTreeCreate(rectangle_t* rectangle) {
    qTree_t* qTree = malloc(sizeof(*qTree));  
    assert(qTree);

    // creating initial root without a point and label
    qTree->root = createNode(NULL, rectangle, "\0");

    return qTree;
}

// creates and returns a node for `point` with `rectangle` (area of node)
// and `label` (the quadrant the node is in)
qTreeNode_t* createNode(point_t* point, rectangle_t* rectangle, char* label) {
    qTreeNode_t* node = malloc(sizeof(*node));  
    assert(node);

    node->rectangle = rectangle;
    node->point = point;
    node->footpaths = arrayCreate();
    node->label = label;
    node->NE = NULL;
    node->NW = NULL;
    node->SE = NULL;
    node->SW = NULL;

    return node;
}

// handle function to insert `point` to `qTree`
qTree_t* qTreeInsert(qTree_t* qTree, point_t* point, footpath_t* footpath) { 
    // recursively inserts `point` into `qTree`
    qTreeInsertPoint(qTree->root, point, footpath);

    return qTree;

}

// inserts `footpath` into `arr` making sure `arr` stays sorted by footpathID
void insertFootpathInArray(array_t *arr, footpath_t *footpath) {
    sortedArrayInsert(arr, footpath);
    arrayShrink(arr);
}

// recursively inserts point into qTree
void qTreeInsertPoint(qTreeNode_t* root, point_t* point, footpath_t* footpath) {
    // empty leaf node so insert `point`
    if (root->point == NULL && root->NW == NULL) {
        root->point = point;
        insertFootpathInArray(root->footpaths, footpath);
        return;
    }

     // handling equality if point has already been inserted so just add footpaths
     if (root->point != NULL)
        // using EPSILLON to deal with precision error because of equality testing of doubles
        if ((fabs((root->point->x - point->x)) < EPSILON) && (fabs((root->point->y - point->y)) < EPSILON)) {
            insertFootpathInArray(root->footpaths, footpath);

            // freeing point now because point won't be inserted into node as it already exists
            free(point);
            return;
        }
    

    // leaf node not empty
    if (root->NW == NULL) {
        splitNode(root);
    }

    // insert `point` into a quadrant of `root`
    insertIntoQuadrant(root, point, footpath);
}

// function to split `node` into four quadrants, making it an inner node
// function also reinserts current values of `node` 
// into the appropriate new quadrant
void splitNode(qTreeNode_t* node) {
    long double middleX = (node->rectangle->botLeftX + node->rectangle->topRightX) / 2;
    long double middleY = (node->rectangle->botLeftY + node->rectangle->topRightY) / 2;

    // creating quadrant children of node
    rectangle_t* NW = newRectangle(node->rectangle->botLeftX, middleY, middleX, node->rectangle->topRightY);
    node->NW = createNode(NULL, NW, "NW");

    rectangle_t* NE = newRectangle(middleX, middleY, node->rectangle->topRightX, node->rectangle->topRightY);
    node->NE = createNode(NULL, NE, "NE");

    rectangle_t* SW = newRectangle(node->rectangle->botLeftX, node->rectangle->botLeftY, middleX, middleY);
    node->SW = createNode(NULL, SW, "SW");

    rectangle_t* SE = newRectangle(middleX, node->rectangle->botLeftY, node->rectangle->topRightX, middleY);
    node->SE = createNode(NULL, SE, "SE");

    // insert point already in node into a children of node as node is now an interval node
    int quadrant = insertIntoQuadrant(node, node->point, node->footpaths->A[0]);
    node->footpaths->A[0] = NULL;

    // inserts rest of possible footpaths that node had back into new leaf node
    for (int i = 1; i < node->footpaths->n; i++) {
        if (quadrant == 0)
            insertFootpathInArray(node->NW->footpaths, node->footpaths->A[i]);
        else if (quadrant == 1)
            insertFootpathInArray(node->NE->footpaths, node->footpaths->A[i]);
        else if (quadrant == 2)
            insertFootpathInArray(node->SW->footpaths, node->footpaths->A[i]);
        else if (quadrant == 3)
            insertFootpathInArray(node->SE->footpaths, node->footpaths->A[i]);

        // to avoid freeing errors as this is freed later
        node->footpaths->A[i] = NULL;  
    }

    node->footpaths->n = 0;

    // remove point from node as it is now an inner node
    node->point = NULL;
}

// inserts `point` into appropriate quadrant of `node`
int insertIntoQuadrant(qTreeNode_t* node, point_t* point, footpath_t* footpath) {
    int quadrant = findQuadrant(node, point);

    // insert into that quadrant
    if (quadrant == 0) {
        qTreeInsertPoint(node->NW, point, footpath);
    } else if (quadrant == 1) {
        qTreeInsertPoint(node->NE, point, footpath);
    } else if (quadrant == 2) {
        qTreeInsertPoint(node->SW, point, footpath);
    } else if (quadrant == 3) {
        qTreeInsertPoint(node->SE, point, footpath);
    }

    return quadrant;
}

// returns 0,1,2 or 3 to specify which quadrant of `node` `point` belongs in
// returns -1 if point doesn't belong in either quadrant
int findQuadrant(qTreeNode_t* node, point_t* point) {
    long double middleX = (node->rectangle->botLeftX + node->rectangle->topRightX) / 2;
    long double middleY = (node->rectangle->botLeftY + node->rectangle->topRightY) / 2;

    // NW
    if ((point->x <= middleX && point->y >= middleY) && 
    (point->x > node->rectangle->botLeftX && point->y < node->rectangle->topRightY))
        return 0;
    
    // NE
    if ((point->x > middleX && point->y >= middleY) && 
    (point->x <= node->rectangle->topRightX && point->y < node->rectangle->topRightY))
        return 1;

    // SW
    if ((point->x <= middleX && point->y < middleY) && 
    (point->x > node->rectangle->botLeftX && point->y >= node->rectangle->botLeftY))
        return 2;

    // SE
    if ((point->x > middleX && point->y < middleY) && 
    (point->x <= node->rectangle->topRightX && point->y >= node->rectangle->botLeftY))
        return 3;

    // doesn't fit in region of node (not reached for this project)
    return -1;
}

// handle to search `qTree` for `point`
// returns list of quadrants accessed in order to reach `point`
void qTreeSearch(qTree_t *qTree, point_t* point, list_t* quadrants, 
                FILE* infoFile, char* xBuffer, char* yBuffer) {

    // handles recursion
    qTreeSearchNode(qTree->root, point, quadrants, infoFile, xBuffer, yBuffer);
}

// recursively searches qTree for `point` by checking `node`
void qTreeSearchNode(qTreeNode_t* node, point_t* point, list_t* quadrants,
                     FILE* infoFile,  char* xBuffer, char* yBuffer) {

    // leaf node
    if (node->NW == NULL) {
        if ((fabs(node->point->x - point->x) < EPSILON) && (fabs(node->point->y - point->y) < EPSILON)) {
            // found point in node

            // appending current quadrant to list
            listAppend(quadrants, node->label); 

            // printing all footpaths in found point
            fprintf(infoFile, "%s %s\n", xBuffer, yBuffer);
            for (int i=0; i<node->footpaths->n; i++) {
                footpathPrint(node->footpaths->A[i], infoFile); 
            }
            return;
        } else {
            return;  // leaf node does not contain the point so return 
        }
    }

    // if point is within current rectangle append quadrant to list of quadrants
    if (inRectangle(node->rectangle, point)) {
        // skipping root node since it does not have a quadrant
        if (node->label[0] != '\0')  
            listAppend(quadrants, node->label);

        // recursively searching appropriate quadrant
        int quadrant = findQuadrant(node, point);
        if (quadrant == 0) {
            qTreeSearchNode(node->NW, point, quadrants,
                             infoFile, xBuffer, yBuffer);
        } else if (quadrant == 1) {
            qTreeSearchNode(node->NE, point, quadrants,
                             infoFile, xBuffer, yBuffer);
        } else if (quadrant == 2) {
            qTreeSearchNode(node->SW, point, quadrants,
                             infoFile, xBuffer, yBuffer);
        } else if (quadrant == 3) {
            qTreeSearchNode(node->SE, point, quadrants,
                             infoFile, xBuffer, yBuffer);
        }
    }
}

// recursively searches qTree for points within `range`
// stores unique footpaths of those points and direction 
array_t* queryRange(qTreeNode_t* node, rectangle_t* range,
             array_t* footpathVisited, list_t* quadrants, array_t* results) {

    // node span and range of query don't overlap so return
    if (!rectangleOverlap(node->rectangle, range)) {
        return results;
    }

    // not an empty leaf node so append current quadrant to list
    if (!(node->NW == NULL && node->point == NULL))  
        listAppend(quadrants, node->label);

    // got to a leaf node
    if (node->point != NULL) {
        if (inRectangleStage4(range, node->point)) {
            // point in node is in `range` of query 
            // append all unique footpaths in node to `results`
            for (int i = 0; i < node->footpaths->n; i++) {
                // checking if footpath not already visited
                if (arrayBinarySearch(footpathVisited, footpathGetID(node->footpaths->A[i])) == NULL) {
                    insertFootpathInArray(results, node->footpaths->A[i]);
                    insertFootpathInArray(footpathVisited, node->footpaths->A[i]);
                }
            }
        }
    }

    // empty leaf node so return
    if (node->NW == NULL)
        return results;

    // recursively checking all quadrants of current node in order specified to check for any points in query range
    array_t* SW = queryRange(node->SW, range, footpathVisited, quadrants, results);
    for (int i = 0; i < SW->n; i++) {
        if (arrayBinarySearch(footpathVisited, footpathGetID(SW->A[i])) == NULL) {
            insertFootpathInArray(results, SW->A[i]);
            insertFootpathInArray(footpathVisited, SW->A[i]);
        }
    }

    array_t* NW = queryRange(node->NW, range, footpathVisited, quadrants, results);
    for (int i = 0; i < NW->n; i++) {
        if (arrayBinarySearch(footpathVisited, footpathGetID(NW->A[i])) == NULL) {
            insertFootpathInArray(results, NW->A[i]);
            insertFootpathInArray(footpathVisited, NW->A[i]);
        }
    }

    array_t* NE = queryRange(node->NE, range, footpathVisited, quadrants, results);
    for (int i = 0; i < NE->n; i++) {
        if (arrayBinarySearch(footpathVisited, footpathGetID(NE->A[i])) == NULL) {
            insertFootpathInArray(results, NE->A[i]);
            insertFootpathInArray(footpathVisited, NE->A[i]);
        }
    }

    array_t* SE = queryRange(node->SE, range, footpathVisited, quadrants, results);
    for (int i = 0; i < SE->n; i++) {
        if (arrayBinarySearch(footpathVisited, footpathGetID(SE->A[i])) == NULL) {
            insertFootpathInArray(results, SE->A[i]);
            insertFootpathInArray(footpathVisited, SE->A[i]);
        }
    }

    return results;
}

// checks if rectangles `a` and `b` have any overlap
int rectangleOverlap(rectangle_t* a, rectangle_t* b) {
    // checks if corner points of rectangle `a` are in rectangle `b`
    // if they are then the rectangles overlap
    if (rectangleCornerInRectangle(a->botLeftX, a->botLeftY, b))
        return 1;

    if (rectangleCornerInRectangle(a->topRightX, a->topRightY, b))
        return 1;

    if (rectangleCornerInRectangle(a->botLeftX, a->topRightY, b))
        return 1;

    if (rectangleCornerInRectangle(a->topRightX, a->botLeftY, b))
        return 1;

    // checks if corner points of rectangle `b` are in rectangle `a`
    // if they are then the rectangles overlap
    if (rectangleCornerInRectangle(b->botLeftX, b->botLeftY, a))
        return 1;

    if (rectangleCornerInRectangle(b->topRightX, b->topRightY, a))
        return 1;

    if (rectangleCornerInRectangle(b->botLeftX, b->topRightY, a))
        return 1;

    if (rectangleCornerInRectangle(b->topRightX, b->botLeftY, a))
        return 1;

    // rectangles don't overlap
    return 0;

}

// checks if rectangle corner with points `x` and `y` are in `rectangle`
int rectangleCornerInRectangle(long double x, long double y,
                                 rectangle_t* rectangle) {
                                     
    if (x > rectangle->botLeftX && x <= rectangle->topRightX) {
        if (y >= rectangle->botLeftY && y < rectangle->topRightY) {
            return 1;
        }
    }
    return 0;
}

// handle function to free allocated memory used by `qTree`
void qTreeFree(qTree_t *qTree) {
    qTreeFreeNode(qTree->root);
    free(qTree);
}

// function to recursively free qTree by traversing every `node`
void qTreeFreeNode(qTreeNode_t* node) {
    free(node->point);
    
    arrayFree(node->footpaths);
    free(node->rectangle);
    
    if (node->NW) {
        qTreeFreeNode(node->NW);
        qTreeFreeNode(node->NE);
        qTreeFreeNode(node->SW);
        qTreeFreeNode(node->SE);
    }

    
    free(node);
}







