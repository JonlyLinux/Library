#ifndef AVLTREE_H
#define AVLTREE_H

#include <stdint.h>
#include "stdmacro.h"

#define HEIGHT(node) ((node) ? ((node)->height) : (-1))

typedef struct avlnode_s {
    struct avlnode_s *left, *right, *parent;
    struct avlnode_s *prev, *next;
    uint32_t height;
    uint32_t layer;
} avlnode_t;

static int CALHEIGHT(avlnode_t *node)
{
    if (node == NULL) return -1;
    uint32_t hl = HEIGHT(node->left);
    uint32_t hr = HEIGHT(node->right);

    return MAX(hl, hr) + 1;
}

typedef int (*avlnode_cmp_func_t)(avlnode_t *, avlnode_t *);
typedef int (*avlnode_del_func_t)(avlnode_t *);
typedef int (*avlnode_travel_func_t)(avlnode_t *);

typedef struct avltree_s {
    avlnode_t *root;
    avlnode_cmp_func_t cmp_func;
    avlnode_del_func_t del_func;
    avlnode_travel_func_t travel_func;
    uint32_t spinlock;
} avltree_t;

#endif // AVLTREE_H
