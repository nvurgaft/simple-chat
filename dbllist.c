#include <stdio.h>
#include <stdlib.h>
#include "dbllist.h"

void dbllist_init(dbllist_t* l) {
    dbllist_head(l) = NULL;
    dbllist_tail(l) = NULL;
    dbllist_size(l) = 0;
}

void dbllist_destroy(dbllist_t *l, dbllist_destroy_t dest) {
    
    if (dest==DBLLIST_LEAVE_DATA || dbllist_head(l)==NULL) //0
    {
        free(l);
    }
    if (dest==DBLLIST_FREE_DATA) //1
    {
        dbllist_node_t* n = dbllist_head(l);
        dbllist_node_t* tmp;

        while (n!=NULL)
        {
            tmp = n;
            n = dbllist_next(n);
            free(tmp->data);
            free(tmp);
        }
        free(l);
    }
}

// wraps the data in a node and puts it as the last node in the list
int dbllist_append(dbllist_t *l, void *data) {

    if (l==NULL) return -1;

    dbllist_node_t* new_node = (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    new_node->data = data;  

    if (dbllist_tail(l)==NULL)
    {
        dbllist_tail(l) = new_node;
        dbllist_head(l) = new_node;
    }
    else
    {
        new_node->next = NULL;
        dbllist_tail(l)->next = new_node;
    }
    new_node->prev = dbllist_tail(l);
    dbllist_tail(l) = new_node;

    dbllist_size(l)++;
    return 0;
}

// wraps the data in a node and puts it as the first node in the list
int dbllist_prepend(dbllist_t *l, void *data) {

    if (l==NULL) return -1;

    dbllist_node_t* new_node = (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    new_node->data = data;

    if (dbllist_head(l)==NULL)
    {
        dbllist_head(l) = new_node;
        dbllist_tail(l) = new_node;
    }
    else
    {
        new_node->next = dbllist_head(l);
        dbllist_head(l)->prev = new_node;
    }
    new_node->prev = NULL;
    dbllist_head(l) = new_node;

    dbllist_size(l)++;
    return 0;
}

int dbllist_remove(dbllist_t *l, dbllist_node_t* n, dbllist_destroy_t data) {
    
    if (l==NULL) return -1;

    if( dbllist_next(n) == NULL && dbllist_prev(n) != NULL )
    {
        dbllist_prev(n)->next = NULL;
        dbllist_tail(l) = dbllist_prev(n);
        if(data  == DBLLIST_LEAVE_DATA)
        {
            free(n);
        }
        else if(data ==DBLLIST_FREE_DATA)
        {
            free(dbllist_data(n));
            free(n);
        }
    }
    else if(dbllist_next(n) != NULL && dbllist_prev(n) == NULL )
    {
        dbllist_next(n)->prev = NULL;
        dbllist_head(l) = dbllist_next(n);
        if(data  == DBLLIST_LEAVE_DATA)
        {
            free(n);
        }
        else if(data ==DBLLIST_FREE_DATA)
        {
            free(dbllist_data(n));
            free(n);
        }
    }
    else if(dbllist_next(n) == NULL && dbllist_prev(n) == NULL )
    {
        dbllist_tail(l) = NULL;
        dbllist_head(l) = NULL;
        if(data  == DBLLIST_LEAVE_DATA)
        {
            free(n);
        }
        else if(data ==DBLLIST_FREE_DATA)
        {
            free(dbllist_data(n));
            free(n);
        }
    }
    else
    {
        dbllist_prev(n)->next = dbllist_next(n);
        dbllist_next(n)->prev = dbllist_prev(n);
        if(data == DBLLIST_FREE_DATA)
        {
            free(dbllist_data(n));
            free(n);
        }
        else if(data  == DBLLIST_LEAVE_DATA)
        {
            free(n);
        }
    }
    dbllist_size(l)--;
    return 0;
}

/**     
        accessory functions
    
    these functions are not part of the exercise but are nevertheless
    usefull for tinkering and debugging the list
*/

// traverses the list and prints any int value inside the nodes
void dbllist_print(dbllist_t *l) {

    if (l==NULL) {
        printf("list wasn't initialized\n");
        return;
    }
    
    dbllist_node_t* current = dbllist_head(l);

    while(current!=NULL) {
        printf("%d, ", (int)current->data);   
        current = current->next;
    }
    printf("| list size is %d\n", l->size);
}

/** traverses the list and removes instances equal to param value
    if allInstances = 0, the first instance of value will be removed and the function
    will return, if = 1, all the instances of value in list will be purged */
int dbllist_remove_value(dbllist_t *l, void* value, int allInstances, dbllist_destroy_t dest) {

    if (l==NULL) {
        printf("list wasn't initialized\n");
        return -1;
    }

    dbllist_node_t* current = dbllist_head(l);

    while (current!=NULL) {
        if (current->data==value) {
            dbllist_remove(l, current, dest);
            if (allInstances==0) {
                return 0;
            }
        }
        current = current->next;
    }

    return 0;
}