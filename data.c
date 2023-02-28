/* Project: PR QuadTrees
* data.c :
*            = implementation of module data of the project
*
* Created by Ayush Tyagi (ayusht@student.unimelb.edu.au)
* 03/09/2022
*
* ----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "data.h"

// skip the header line of .csv file `f`
void footpathSkipHeaderLine(FILE *f) {
	while (fgetc(f) != '\n');
}

// reads a footpath from file "f" to build a footpath_t data.
// returns the pointer, or NULL if reading is unsuccessful.
footpath_t *footpathRead(char* record) {
    // replace commas inside quotations because they should be retained
    replaceIncludedCommas(record);
    
    // buffers for data fields to deal with empty strings and special strings
    char footpathID[MAX_CHARS + 1];
    char address[MAX_CHARS + 1];
    char clueSa[MAX_CHARS + 1];
    char assetType[MAX_CHARS + 1];
    char deltaZ[MAX_CHARS + 1];
    char distance[MAX_CHARS + 1];
    char grade1in[MAX_CHARS + 1];
    char mccID[MAX_CHARS + 1];
    char mccIDInt[MAX_CHARS + 1];
    char rlMax[MAX_CHARS + 1];
    char rlMin[MAX_CHARS + 1];
    char segSide[MAX_CHARS + 1];
    char statusID[MAX_CHARS + 1];
    char streetID[MAX_CHARS + 1];
    char streetGroup[MAX_CHARS + 1];
    char startLat[MAX_CHARS + 1];
    char endLat[MAX_CHARS + 1];
    char startLon[MAX_CHARS + 1];
    char endLon[MAX_CHARS + 1];

    footpath_t *footpath = malloc(sizeof(*footpath));
    assert(footpath);

    char* field;
    int index = 0;
    // field will contain everything until the comma from row of csv
    while ((field = strsep(&record, ",")) != NULL) {
        switch(index) {
            case 0:
                strcpy(footpathID, field);
                index++;
                break;
            case 1:
                strcpy(address, field);
                index++;
                break;
            case 2:
                strcpy(clueSa, field);
                index++;
                break;
            case 3:
                strcpy(assetType, field);
                index++;
                break;
            case 4:
                strcpy(deltaZ, field);
                index++;
                break;
            case 5:
                strcpy(distance, field);
                index++;
                break;
            case 6:
                strcpy(grade1in, field);
                index++;
                break;
            case 7:
                strcpy(mccID, field);
                index++;
                break;
            case 8:
                strcpy(mccIDInt, field);
                index++;
                break;
            case 9:
                strcpy(rlMax, field);
                index++;
                break;
            case 10:
                strcpy(rlMin, field);
                index++;
                break;
            case 11:
                strcpy(segSide, field);
                index++;
                break;
            case 12:
                strcpy(statusID, field);
                index++;
                break;
            case 13:
                strcpy(streetID, field);
                index++;
                break;
            case 14:
                strcpy(streetGroup, field);
                index++;
                break;
            case 15:
                strcpy(startLat, field);
                index++;
                break;
            case 16:
                strcpy(startLon, field);
                index++;
                break;
            case 17:
                strcpy(endLat, field);
                index++;
                break;
            case 18:
                strcpy(endLon, field);
                index++;
                break;
        }
    }
    // delete any quotation marks and put back included commas
    // from the string fields
    ensureStringFormat(address);
    ensureStringFormat(clueSa);
    ensureStringFormat(assetType);
    ensureStringFormat(segSide);

    // assign fields into struct with correct formatting
    footpath->address = strdup(address);
    footpath->clueSa = strdup(clueSa);
    footpath->assetType = strdup(assetType);
    footpath->segSide = strdup(segSide);
    footpath->footpathID = atoi(footpathID);
    footpath->deltaZ = atof(deltaZ);
    footpath->distance = atof(distance);
    footpath->grade1in = atof(grade1in);
    footpath->mccID = atoi(mccID);
    footpath->mccIDInt = atoi(mccIDInt);
    footpath->rlMax = atof(rlMax);
    footpath->rlMin = atof(rlMin);
    footpath->statusID = atoi(statusID);
    footpath->streetID = atoi(streetID);
    footpath->streetGroup = atoi(streetGroup);
    footpath->startLat = atof(startLat);
    footpath->endLat = atof(endLat);
    footpath->startLon = atof(startLon);
    footpath->endLon = atof(endLon);

    return footpath;
}

// replace commas inside quotations because they should be retained
void replaceIncludedCommas(char* record) {
    int quotation = 0;

    int i=0;
    // because of new line only go upto second last character
    while (i < strlen(record) - 1) {
        if (record[i] == '\"' && !quotation)
            quotation = 1;
        else if (record[i] == '\"')
            quotation = 0;
        
        if (record[i] == ',' && quotation)
            record[i] = '*';
            
        i++;
    }
}

// delete any quotation marks and put back included commas
void ensureStringFormat(char* string) {
    if (string[0] == '\"') {
        // removing quotation marks
        string[strlen(string) - 1] = '\0';
        memmove(string, string+1, strlen(string));
    }

    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == '*') 
            string[i] = ',';
    }
}

// compares 2 footpath "a" and "b" by id, returns -1, 0, +1 for < =, >  
int footpathCmpID(footpath_t* a, footpath_t* b) {
    if (a->footpathID < b->footpathID) return -1;
	if (a->footpathID > b->footpathID) return +1;
	return 0;
}

// getter to return footpathID from `footpath`
int footpathGetID(footpath_t* footpath) {
    return footpath->footpathID;
}

// prints a footpath record `*footpath` to file `outFile`
void footpathPrint(footpath_t *footpath, FILE *outFile) {
    fprintf(outFile, "--> footpath_id: %d || address: %s || clue_sa: %s ||"
    " asset_type: %s || deltaz: %.2lf || distance: %.2lf || grade1in: %.1lf ||"
    " mcc_id: %d || mccid_int: %d || rlmax: %.2lf || rlmin: %.2lf || segside: %s"
    " || statusid: %d || streetid: %d || street_group: %d || start_lat: %lf ||"
    " start_lon: %lf || end_lat: %lf || end_lon: %lf || \n",
    footpath->footpathID, footpath->address, footpath->clueSa,
    footpath->assetType, footpath->deltaZ,footpath->distance,
    footpath->grade1in, footpath->mccID, footpath->mccIDInt,
    footpath->rlMax, footpath->rlMin, footpath->segSide, footpath->statusID,
    footpath->streetID, footpath->streetGroup,footpath->startLat,
    footpath->startLon, footpath->endLat, footpath->endLon);
}

// free allocated memory used to construct `footpath`
void footpathFree(footpath_t *footpath) {
    free(footpath->address);
    free(footpath->clueSa);
    free(footpath->assetType);
    free(footpath->segSide);
	free(footpath);
}
