#include "stdafx.h"
#pragma once
struct property {
	char *key;
	enum {NUM, STR} type;
	union {
		int num;
		char *str;
	} value;

	struct property *next;
};

struct object {
	struct property first;
};

void object_new(object *self, char *name);
void object_set_int(object *self, char *key, int value);
void object_delete(object *self);