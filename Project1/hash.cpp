#include "stdafx.h"

unsigned short inline hashfunction(register int key)
{
	return ((((key*key * 5 * key * 0x12345678) + key) % 0xFFFF) & 0xFFFF);
}
void hash_init(struct hash *h)
{
	h->mem = (struct hash_entry *)calloc(4, 2 * 2 * 2 * 65536);
	h->size = 2 * 2 * 2 * 65536;
}
#define _LINEAR_PROBE_MAX 250
void hash_set(struct hash *h, int key, short value)
{
	unsigned int hash = hashfunction(key);
	int i = hash;
	while (h->mem[i].key != 0)
	{
		i++;
		assert(i - hash < _LINEAR_PROBE_MAX);
	}
	h->mem[i].key = key;
	h->mem[i].value = value;
}

int hash_get(struct hash *h, int key)
{
	unsigned int hash = hashfunction(key);
	int i = hash;
	while (h->mem[i].key != key)
	{
		i++;
		if (i - hash > _LINEAR_PROBE_MAX)
			return -1;
	}
	return h->mem[i].value;
}

