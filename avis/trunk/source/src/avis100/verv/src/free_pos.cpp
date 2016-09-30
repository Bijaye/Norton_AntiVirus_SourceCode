#include <stdlib.h>
#include "autoverv.h"

void free_pos(struct posrec *head)
  { 
    struct posrec *curr;

    while(head)  /* while still nodes */
      {
      curr=head->next;   /* get next */
      free(head);       /* kill this one */
      head=curr;
      }
  }  
