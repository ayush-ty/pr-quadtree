/* Project: PR QuadTrees
* linkedlist.h :
*            = interface of the module linkedlist of the project
*
* Created by Ayush Tyagi (ayusht@student.unimelb.edu.au)
* 06/09/2022
*
* ----------------------------------------------------------------*/

#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct node {
    char* label;
    struct node *next;
} node_t;

typedef struct linkedlist {
    struct node *head;
    int n;
} list_t;

// creates and returns empty linked list
list_t* listCreate();

// function to append `label` to `list`
void listAppend(list_t* list, char* label);

// creates and returns a node for `label`
node_t* createListNode(char* label);

// function to print `list`
void listPrint(list_t *list, FILE *outFile);

// function to free allocated memory used by `list`
void listFree(list_t *list);

#endif


