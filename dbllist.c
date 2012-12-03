#include <stdio.h>
#include <stdlib.h>
#include "dbllist.h"

void dbllist_init(dbllist_t* l)
{
    dbllist_head(l) = NULL;
    dbllist_tail(l) = NULL;
    dbllist_size(l) = 0;
}

void dbllist_destroy(dbllist_t *l, dbllist_destroy_t dest)
{
    if (dest==DBLLIST_LEAVE_DATA || dbllist_head(l)==NULL)   //0
    {
        free(l);
    }
    if (dest==DBLLIST_FREE_DATA)                   //1
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

int dbllist_append(dbllist_t *l, void *data)
{
    dbllist_node_t* new_node = (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    new_node->data = data;

    if (l==NULL) return -1;

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

int dbllist_prepend(dbllist_t *l, void *data)
{
    dbllist_node_t* new_node = (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    new_node->data = data;

    if (l==NULL) return -1;

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

int dbllist_remove(dbllist_t *l, dbllist_node_t* n, dbllist_destroy_t data)
{
	if(	dbllist_next(n) == NULL && dbllist_prev(n) != NULL )
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
