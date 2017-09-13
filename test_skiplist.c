#include "skiplist.h"
#include <stdio.h>
#include <sys/time.h>

skiplist_t sl;


static unsigned int dtime(struct timeval *x, struct timeval *y)
{
	unsigned int z;

	z = 
		(x->tv_sec * 1000 + x->tv_usec / 1000)
	  - (y->tv_sec * 1000 + y->tv_usec / 1000);

	return z+1;
}


int main(int argc, char *argv[])
{
	struct timeval stv, etv;
	skiplist_init(&sl, 20);
	skiplist_clear(&sl);

	int i = 0;
	int max = 10000000;
	int level = 0;
	skipnode_t *new = NULL;

#if 1
	gettimeofday(&stv, NULL);
	//insert
	for (i = 0; i < max; i++) {
		level = sl.random(&sl);
		//printf("Insert key: %d, level: %d\n", i, level);

		new = sl.alloc(SKIPNODE_SIZE(level));
		new->key = i;
		new->level = level;

		skiplist_insert(&sl, new);
	}
	gettimeofday(&etv, NULL);
	printf("Insert speed: %d\n", max / dtime(&etv, &stv) * 1000);
#endif

#if 1
	gettimeofday(&stv, NULL);
	//find
	skipnode_t tmp;
	for (i = 0; i < max; i++) {
		tmp.key = i;

		new = skiplist_find(&sl, &tmp);
		if (new == NULL) {
			printf("Find %d error\n", i);
			break;
		}

		//printf("Find %d, %d\n", i, new->key);
	}
	gettimeofday(&etv, NULL);
	printf("Find speed: %d\n", max / dtime(&etv, &stv) * 1000);
#endif

#if 1
	gettimeofday(&stv, NULL);
	//remove
	for (i = 0; i < max; i++) {
		tmp.key = i;

		new = skiplist_remove(&sl, &tmp);
		if (new == NULL) {
			printf("Remove %d error\n", i);
			break;
		}
	}
	gettimeofday(&etv, NULL);
	printf("Remove speed: %d\n", max / dtime(&etv, &stv) * 1000);
#endif
	
#if 0
	skiplist_mmap(&sl);
#endif

#if 0
	for (i = sl.level - 1; i >= 0; i--) {
		skipnode_t *head = sl.head;
		printf("Level %d:", i);
		while (head->forward[i] != sl.null) {
			head = head->forward[i];
			printf("%d ", head->key);
		}
		printf("\n");
	}
#endif

	skiplist_clear(&sl);
	skiplist_destroy(&sl);

	return 0;
}
