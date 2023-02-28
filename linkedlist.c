/* Project: PR QuadTrees
* linkedlist.c :
*            = implementation of module alinkedlist of the project
*
* Created by Ayush Tyagi (ayusht@student.unimelb.edu.au)
* 06/09/2022
*
* ----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "linkedlist.h"

// creates and returns empty linked list
list_t* listCreate() {
    list_t *list = malloc(sizeof(*list));
    assert(list);
    list->head = NULL;
    list->n = 0;
    return list;
}

// function to append `label` to `list`
void listAppend(list_t* list, char* label) {
    node_t *node= createListNode(label);
    // check if list empty
    if (list->n == 0) {
        list->head = node;
        (list->n)++;
        return;
    }
    
    node_t *current = list->head;
    while (current->next) {
        current = current->next;
    }

    // current is now tail node
    current->next = node;
    (list->n)++;
}

// creates and returns a node for `label`
node_t* createListNode(char* label) {
    node_t* node = malloc(sizeof(*node));
    assert(node);

    node->label = label;
    node->next = NULL;
    return node;
}

// function to print `list`
void listPrint(list_t *list, FILE *outFile) {
    node_t *current = list->head;
    int index = 0;

    while (current && index < (list->n)-1) {
        fprintf(outFile, "%s ", current->label);
        current = current->next;
        index++;
    }
    fprintf(outFile, "%s\n", current->label);

}

// function to free allocated memory used by `list`
void listFree(list_t *list) {
    node_t *current;

    while (list->head !=  NULL) {
        current = list->head;
        list->head = list->head->next;
        free(current);
    }
    free(list);
}








