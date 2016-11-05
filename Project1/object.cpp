#include "stdafx.h"
char *  NAME = "name";
void object_new(object *self, char *name)
{


	self->first.key = NAME;
	char *newString = (char *)malloc(strlen(name)+1);
	self->first.value.str = newString;
	strcpy(newString, name);
	self->first.type = self->first.STR;
	self->first.next = NULL;
	
}
void object_delete(object *self)
{
	property *ptr = &self->first;
	free(self->first.value.str);
	if (ptr->next == NULL) return;
	ptr = ptr->next;
	do {

		if (ptr->type == ptr->STR)
			free(ptr->value.str);
		free(ptr->key);
		property *nextptr = ptr->next;
		free(ptr);
		ptr = nextptr;
	} while (ptr != NULL);
	free(ptr);
}
void object_set_int(object *self, char *key, int value)
{
	char *newString = (char *)malloc(strlen(key)+1);
	strcpy(newString, key);
	property *ptr = &self->first;
	while (ptr->next != NULL) ptr = ptr->next;
	ptr->next = (property *)malloc(sizeof(property));
	ptr->next->key = newString;
	ptr->next->value.num = value;
	ptr->next->type = ptr->next->NUM;
	ptr->next->next = NULL;
}