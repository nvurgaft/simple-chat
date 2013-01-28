/** C source file stub */

#include <stdio.h>
#include <string.h>
#include "dbllist.c"

int main(int argc, char* argv[])
{
	dbllist_t* names = (dbllist_t*) malloc(sizeof(dbllist_t));
	dbllist_t* names_to_delete = (dbllist_t*) malloc(sizeof(dbllist_t));

	dbllist_init(names);
	dbllist_init(names_to_delete);

	char buffer[1024] = "ben; gabe; rich; mike; cthulhu; derek; kenneth;";

	char* usr = strtok(buffer, "; ");
    while (usr!=NULL)
    {
        puts(usr);
        dbllist_append(names, usr);
        usr = strtok(NULL, "; ");
    }
	
	puts("---");
	/*dbllist_append(names, (void*) "ben");
	dbllist_append(names, (void*) "gabe");
	dbllist_append(names, (void*) "rich");
	dbllist_append(names, (void*) "mike");
	dbllist_append(names, (void*) "cthulhu");
	dbllist_append(names, (void*) "derek");
	dbllist_append(names, (void*) "kenneth");*/

	dbllist_append(names_to_delete, (void*) "rich");
	dbllist_append(names_to_delete, (void*) "cthulhu");

	dbllist_node_t* n_curr;
	dbllist_node_t* ntd_curr;

	for (ntd_curr = names_to_delete->head; ntd_curr!=NULL; ntd_curr = ntd_curr->next) 
	{
		for (n_curr = names->head; n_curr!=NULL; n_curr = n_curr->next) 
		{
			if (strncmp((char*) n_curr->data, (char*) ntd_curr->data, sizeof(n_curr->data))==0) {
				printf("'%s' was deleted! \n", (char*) n_curr->data);
				dbllist_remove(names, n_curr, 0);
			}		
		}
	}

	
	n_curr = names->head;
	while(n_curr!=NULL) {
		puts((char*) n_curr->data);
		n_curr = n_curr->next;
	}

	dbllist_destroy(names, 0);
	dbllist_destroy(names_to_delete, 0);

	return 0;
}
