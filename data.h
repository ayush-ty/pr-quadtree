/* Project: PR QuadTrees
* data.h :
*            = interface of the module data of the project
*
* Created by Ayush Tyagi (ayusht@student.unimelb.edu.au)
* 03/09/2022
*
* ----------------------------------------------------------------*/

#ifndef _DATA_H_
#define _DATA_H_

#define MAX_CHARS 128

typedef struct footpath {
    int footpathID;
    char *address;
    char *clueSa;
    char *assetType;
    double deltaZ;
    double distance;
    double grade1in;
    int mccID;
    int mccIDInt;
    double rlMax;
    double rlMin;
    char *segSide;
    int statusID;
    int streetID;
    int streetGroup;
    double startLat;
    double startLon;
    double endLat;
    double endLon;
} footpath_t;

// skip the header line of .csv file `f`
void footpathSkipHeaderLine(FILE *f);

// reads a footpath from file "f" to build a footpath_t data.
// returns the pointer, or NULL if reading is unsuccessful.
footpath_t* footpathRead(char* record);

// prints a footpath record `*footpath` to file `outFile`
void footpathPrint(footpath_t *footpath, FILE *outFile);

// free allocated memory used to construct `footpath`
void footpathFree(footpath_t *footpath);

// replace commas inside quotations because they should be retained
void replaceIncludedCommas(char* record);

// compares 2 footpath "a" and "b" by id, returns -1, 0, +1 for < =, >  
int footpathCmpID(footpath_t* a, footpath_t* b);

// getter that returns footpathID
int footpathGetID(footpath_t* footpath);

// delete any quotation marks and put back included commas
void ensureStringFormat(char* string);

#endif
