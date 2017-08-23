#ifndef AVLTREE_H
#define AVLTREE_H

#include <stdint.h>
#include "stdmacro.h"

typedef struct avlnode_s {
    struct avlnode_s *left, *right;
    uint32_t height;

    struct avlnode_s *prev, *next;
    uint32_t layer;
} avlnode_t;

static inline void avlnode_copy(avlnode_t *dst, avlnode_t *src)
{
    dst->left = src->left;
    dst->right = src->right;
    dst->prev = src->prev;
    dst->next = src->next;
    dst->height = src->height;
    dst->layer = src->layer;
}

#define HEIGHT(node) ((node) ? ((node)->height) : (-1))
#define CALHEIGHT(node) ((node) ? (MAX(HEIGHT((node)->left), HEIGHT((node)->right)) + 1) : (-1))

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
