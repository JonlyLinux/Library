#ifndef _SKIPLIST_H
#define _SKIPLIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <alloca.h>

#define SKIPLIST_P 25
#define SKIPNODE_SIZE(level) (sizeof(skipnode_t)+(level)*sizeof(skipnode_t *))


struct skiplist_s;
struct skipnode_s;

typedef struct skiplist_s skiplist_t;
typedef struct skipnode_s skipnode_t;

typedef int        (*skiplist_cmp_func_t)(skipnode_t *x, skipnode_t *y);
typedef void       (*skiplist_clr_func_t)(skipnode_t *x);
typedef skipnode_t (*(*skiplist_alloc_func_t)(int size));
typedef void               (*skiplist_free_func_t)(skipnode_t *ptr);
typedef int                (*skiplist_random_func_t)(skiplist_t *sl);

struct skiplist_s {
	int max_level;
	int level;
	int size;
	skipnode_t *head;
	skipnode_t *null;

	skiplist_cmp_func_t cmp;
	skiplist_clr_func_t clr;
	skiplist_alloc_func_t alloc;
	skiplist_free_func_t free;
	skiplist_random_func_t random;
};

struct skipnode_s {
	union {
		int   key;
		void* data;
	};
	int level;
	skipnode_t *forward[0];
};

void skiplist_init(skiplist_t *sl, int max_level);
void skiplist_clear(skiplist_t *sl); 
void skiplist_destroy(skiplist_t *sl);

skipnode_t *skiplist_find(skiplist_t *sl, skipnode_t *n);
skipnode_t *skiplist_insert(skiplist_t *sl, skipnode_t *n);
skipnode_t *skiplist_remove(skiplist_t *sl, skipnode_t *n);
void        skiplist_mmap(skiplist_t *sl);

/* overwrite these functions */
int         skiplist_default_cmp(skipnode_t *x, skipnode_t *y);
void        skiplist_default_clr(skipnode_t *x);
skipnode_t *skiplist_default_alloc(int size);
void        skiplist_default_free(skipnode_t *ptr);
int         skiplist_default_random(skiplist_t *sl);

#endif
