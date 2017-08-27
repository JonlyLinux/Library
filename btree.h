#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BTREE_MAX_DEGREE 4

#define BTREE_MIN_KEY   ((BTREE_MAX_DEGREE) - 1)
#define BTREE_MAX_KEY   ((BTREE_MAX_DEGREE) * 2 - 1)
#define BTREE_MIN_CHILD (BTREE_MAX_DEGREE)
#define BTREE_MAX_CHILD ((BTREE_MAX_DEGREE) * 2)

struct bnode_s;
typedef struct bnode_s bnode_t;

typedef void (*bnode_travle_t)(bnode_t *node);
typedef int (*bnode_key_cmp_t)(long ka, long kb);


struct bnode_s {
    int n;
    int leaf;
    long key[BTREE_MAX_KEY];
    bnode_t *child[BTREE_MAX_CHILD];

    bnode_t *prev;
    bnode_t *next;
    int layer;
};

typedef struct btree_s {
    bnode_t *root;
    bnode_key_cmp_t key_cmp;
    bnode_travle_t  travel_func;
} btree_t;

static inline int default_bnode_key_cmp(long ka, long kb)
{
    return ka - kb;
}
static inline void default_bnode_travle(bnode_t *node)
{
    int i;

    if (node) {
        printf("node: %p, n: %d, leaf: %d, key[", node, node->n, node->leaf);
        for (i = 0; i < node->n; i++) {
            printf(" %ld ", node->key[i]);
        }
        printf("], layer: %d\n", node->layer);
    }
}

btree_t *btree_init(btree_t *t, bnode_key_cmp_t key_cmp, bnode_travle_t  travel_func);
bnode_t *btree_splite_child(bnode_t *x, long idx);

int btree_insert(btree_t *t, long k);

long btree_find_max(bnode_t *x);
long btree_find_min(bnode_t *x);
int btree_merge_child(bnode_t *x, long idx);
int btree_stole_right(bnode_t *x, long idx);
int btree_stole_left(bnode_t *x, long idx);

int btree_delete(btree_t *t, long k);

int btree_search(btree_t *t,  long k,
                 bnode_t **retn,  long *idx);

/*queue*/
int btinitqueue(bnode_t *queue);
int btenqueue(bnode_t *queue, bnode_t *node);
bnode_t *btdequeue(bnode_t *queue);
void btree_bfs(btree_t *tree);

#endif // BTREE_H
