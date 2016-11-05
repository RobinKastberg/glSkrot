#include "stdafx.h"
#pragma once
struct hash_entry {
	int key;
	short value;
};

struct hash {
	struct hash_entry *mem;
	int size;
};
int hash_get(struct hash *h, int key);
void hash_set(struct hash *h, int key, short value);
void hash_init(struct hash *h);
unsigned short inline hashfunction(register int key);