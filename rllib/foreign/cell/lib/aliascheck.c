/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

/*  Note:

An alias can only reference to tags either within it's own program or 
Controller Scoped tags.
An alias can not reference to tags within other programs.  This makes alias 
checking
quite a bit easier as we only need to check two places...

This routine will search for alias references.  If one is found, the alias 
tag ID data will be
rewrote to point to the actual data object.  The rest of the data remains 
untouched.  Hopefully
this will work correctly....

*/


/* Three possible return values

   0 = Success, alias found
   -1  Error
   1 = Tag not found
*/

int aliascheck(_tag_detail * tag, _tag_data * proglist, _tag_data * 
mainlist,
		int debug)
{
int y;
	unsigned long tagbase=tag->topbase&0xffff;

if (tag == NULL)
  {
  CELLERROR(1,"Tag structure not allocated");
  return -1;
  }

if (tag->type >> 12 == 1)
  {
  CELLERROR(2,"Tag type incorrect");
  return -1;
  }

//  if (((tag->topbase>>24) & 0xf) != 0x4)
if (((tag->topbase) & 0x04000000) )
    {
    return 1; // not an alias
    }

if ((tag->topbase) & 0x20000)	/* Is the tag program scoped? */
	{
	if (proglist == NULL)
	  {
	  CELLERROR(4,"Proglist structure is null");
	  return -1;
	  }
	do
		{
		for (y = 0; y < proglist->count; y++)
			{
			if (proglist->tag[y]->id == tagbase)
			  break; //found alias
			}
		if (y>proglist->count)
			{
			printf("Error in alias chain\n");
			return -1;
			}
		tagbase=proglist->tag[y]->topbase&0xffff;
		}
	while(!(proglist->tag[y]->topbase & 0x04000000) &&
					proglist->tag[y]->topbase & 0x20000);

	if (proglist->tag[y]->topbase & 0x20000)
		{
		  printf( "%s aliases to %s - %08lX\n", tag->name,
			 proglist->tag[y]->name, proglist->tag[y]->id);
		  tag->alias_size = proglist->tag[y]->size;
		  tag->alias_linkid = proglist->tag[y]->linkid;
		  tag->alias_type = proglist->tag[y]->type;
		  tag->alias_topbase = proglist->tag[y]->topbase;
		  tag->alias_id = proglist->tag[y]->id;
		  return 0;
		}
	do
		{
		for (y = 0; y < mainlist->count; y++)
			{
			if (mainlist->tag[y]->base == tagbase)
			  break; //found alias
			}
		if (y>mainlist->count)
			{
			printf("Error in alias chain\n");
			return -1;
			}
		tagbase=mainlist->tag[y]->topbase&0xffff;
		}
	while(!(mainlist->tag[y]->topbase & 0x04000000));
	  dprint(DEBUG_VALUES, "%s aliases to %s - %08lX\n", tag->name,
		 mainlist->tag[y]->name, mainlist->tag[y]->base);
	  tag->alias_size = mainlist->tag[y]->size;
	  tag->alias_linkid = mainlist->tag[y]->linkid;
	  tag->alias_type = mainlist->tag[y]->type;
	  tag->alias_topbase = mainlist->tag[y]->topbase;
	  tag->alias_base = mainlist->tag[y]->base;
	  return 0;
	}

if ((tag->topbase) & 0x10000)	/* Is the alias to a controller scoped tag? */
  {
	do
		{
		for (y = 0; y < mainlist->count; y++)
			{
			if (mainlist->tag[y]->base == tagbase)
			  break; //found alias
			}
		if (y>mainlist->count)
			{
			printf("Error in alias chain\n");
			return -1;
			}
		tagbase=mainlist->tag[y]->topbase&0xffff;
		}
	while(!(mainlist->tag[y]->topbase & 0x04000000));
	  dprint(DEBUG_VALUES, "%s aliases to %s - %08lX\n", tag->name,
		 mainlist->tag[y]->name, mainlist->tag[y]->base);
	  tag->alias_size = mainlist->tag[y]->size;
	  tag->alias_linkid = mainlist->tag[y]->linkid;
	  tag->alias_type = mainlist->tag[y]->type;
	  tag->alias_topbase = mainlist->tag[y]->topbase;
	  tag->alias_base = mainlist->tag[y]->base;
	  return 0;

/*
    {
      char ac_dmesg[80];

      sprintf(ac_dmesg,"WARN: Could not find alias tag %s - %08lX\n", 
tag->name,
	   tag->topbase);
      CELLERROR(3,ac_dmesg);
    }
*/
//    return -1;
  }

else
	{
	//Tag neither controller or program scoped??
	return -1;
	}
//  CELLERROR(9,"tag->id was not zero structure is null");
return -1;			/* If we get here, we have problems...  */
}

