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

void object_new(object *self, const char *name);
void object_set_int(object *self, const char *key, int value);
int object_get_int(object *self, const char *key);
void object_delete(object *self);