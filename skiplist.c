#include "skiplist.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

/* 
 * initialize a skip list 
 * with max level and cmp func 
 * 
 * skip list own a header and tailer
 * a empty skip list's memory map:
 *
 *         *|  -------> |-------|
 *         *|  -------> |       |
 *  	   *|  -------> |       |
 * header  *|  -------> |tailer |
 *         *|  -------> |       |
 *         *|  -------> |       |
 *         *|  -------> |-------|
 * */
void skiplist_init(skiplist_t *sl, int max_level)
{
	int i;

	sl->cmp = skiplist_default_cmp;
	sl->clr = skiplist_default_clr;
	sl->alloc = skiplist_default_alloc;
	sl->free = skiplist_default_free;
	sl->random = skiplist_default_random;

	sl->max_level = max_level;
	sl->level = 0;
	sl->size = 0;
	sl->head = sl->alloc(SKIPNODE_SIZE(sl->max_level));
	sl->null = sl->alloc(SKIPNODE_SIZE(0));
	sl->null->key = 0;

	for (i = 0; i < sl->max_level; i++) {
		sl->head->forward[i] = sl->null;
	}
}

/* remove all skip node */
void skiplist_clear(skiplist_t *sl)
{
	skipnode_t *head = sl->head;
	skipnode_t *next;
	int i;

	/* only need free once */
	while (head->forward[0] != sl->null) {
		next = head->forward[0];
		head->forward[0] = next->forward[0];
		sl->size--;
		sl->clr(next);
	}
	
	for (i = 0; i < sl->max_level; i++) {
		head->forward[i] = sl->null;
	}
}

void skiplist_destroy(skiplist_t *sl)
{
	skiplist_clear(sl);
	skiplist_default_free(sl->head);
	skiplist_default_free(sl->null);
	
	sl->max_level = 0;
	sl->level = 0;
	sl->size = 0;
	sl->head = NULL;
	sl->null = NULL;
	sl->cmp = NULL;
}

/* Recording search path with a given skip node */
static void _skiplist_position(skiplist_t *sl, 
		skipnode_t **pos, /* length of pos is max_level */
		skipnode_t *n)
{
	int i = 0;
	skipnode_t *cur;
	
	for (i = sl->max_level - 1; i >= 0; i--) {
		pos[i] = sl->head;
	}

	cur = sl->head;
	for (i = sl->level - 1; i >= 0; i--) {
		while (cur->forward[i] != sl->null &&
				sl->cmp(n, cur->forward[i]) > 0) {
			cur = cur->forward[i];
		}

		pos[i] = cur;
	}
}

/* 
 * Find skip node with the given skipnode.
 * return:
 *        Non-NULL: if found
 *        NULL: if not found
 * */
skipnode_t *skiplist_find(skiplist_t *sl, skipnode_t *n)
{
#if 0
	skipnode_t *pos[sl->max_level];
	skipnode_t *ret;

	_skiplist_position(sl, pos, n);

	ret = pos[0]->forward[0];

	if (ret != sl->null &&
			sl->cmp(n, ret) == 0) {
		return ret;
	}
#else
	int i = 0;
	skipnode_t *cur;
	int v;

	cur = sl->head;
	for (i = sl->level - 1; i >= 0; i--) {
		while (cur->forward[i] != sl->null) {
			v = sl->cmp(n, cur->forward[i]);
			if (v > 0) {
				cur = cur->forward[i];
			} else if (v == 0) {
				return cur->forward[i];
			} else {
				break;
			}
		}
	}
#endif

	return NULL;
}

/*
 * Insert the skipnode into skiplist.
 * if skipnode has been exist then return it.
 * or insert the give skipnode and return it.
 * */
skipnode_t *skiplist_insert(skiplist_t *sl, skipnode_t *n)
{
	int i;
	skipnode_t *pos[sl->max_level];

	_skiplist_position(sl, pos, n);
	
	/* exist then return it */
	if (pos[0]->forward[0] != sl->null &&
			sl->cmp(n, pos[0]->forward[0]) == 0) {
		return pos[0]->forward[0];
	}

	/* Note: 1 <= n->level <= sl->max_level */
	/* make search path completely */
	for (i = sl->level; i < n->level; i++) {
		pos[sl->level] = sl->head;
		sl->level++;
	}

	/* update forward pointer */
	for (i = 0; i < n->level; i++) {
		n->forward[i] = pos[i]->forward[i];
		pos[i]->forward[i] = n;
	}

	sl->size++;

	return n;
}

skipnode_t *skiplist_remove(skiplist_t *sl, skipnode_t *n)
{
	int i;
	int level;
	skipnode_t *pos[sl->max_level];
	skipnode_t *ret;

	_skiplist_position(sl, pos, n);
	
	ret = pos[0]->forward[0];

	/* not exist then return NULL */
	if (ret == sl->null ||
			sl->cmp(n, ret) != 0) {
		return NULL;
	}
	
	/* update forward pointer 
	 * Note: updating only when next forward equal ret
	 * */
	for (i = 0; i < sl->level; i++) {
		if (pos[i]->forward[i] != ret)
			break;
		pos[i]->forward[i] = ret->forward[i];
	}

	/* update skiplist's level */
	level = sl->level;
	for (i = level - 1; i >= 0; i--) {
		if (sl->head->forward[i] == sl->null) {
			sl->level--;
		}
	}

	sl->size--;

	return ret;
}

int skiplist_default_cmp(skipnode_t *x, skipnode_t *y)
{
	return x->key - y->key;
}

void skiplist_default_clr(skipnode_t *x)
{
	free(x);
}

/*
 * when insert a element, the level at least 1
 * and the probability of increasement is 0.25
 * refer to the redis and dpdk open source
 *
 * (you can choose 0.5) SKIPLIST_P = 50;
 *
 * probability distribute:
 * level 1: 1-p
 * level 2: p * (1-p)
 * level 3: p**2 * (1-p)
 * level 4: p**3 * (1-p)
 * level k: p**(k-1) * (1-p)
 * */
int skiplist_default_random(skiplist_t *sl)
{
	int max = sl->max_level;
	int level = 1;
	
	while ((random() & 0xFFFF) < (0xFFFF * SKIPLIST_P / 100))
		level++;

	if (level > sl->max_level) {
		level = sl->max_level;
	}

	return level;
}

skipnode_t *skiplist_default_alloc(int size)
{
	return calloc(1, size);
}

void skiplist_default_free(skipnode_t *ptr)
{
	free(ptr);
}

void skiplist_mmap(skiplist_t *sl)
{
	int i;

	printf("-------------------------------------------\n");
	printf("------------skip list memory map-----------\n");
	printf("-------------------------------------------\n");
	printf("Head: %p\n", sl->head);
	printf("Null: %p\n", sl->null);
	printf("Size: %d\n", sl->size);
	printf("Max level: %d\n", sl->max_level);
	printf("Level: %d\n", sl->level);
	printf("Map:\n");
	
	skipnode_t *head, *cur;

	head = sl->head;
	for (i = sl->max_level - 1; i >= 0; i--) {
		cur = head;
		printf("\tlevel %d: ", i);
		fflush(stdout);
		while (cur != sl->null) {
			printf("%p(%d) ---> ", cur, cur->key);
			fflush(stdout);
			cur = cur->forward[i];
		}
		printf("%p\n", cur);
		fflush(stdout);
	}
}
