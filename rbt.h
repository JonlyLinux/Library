#ifndef __RBT_H__
#define __RBT_H__

#include <stdio.h>
#include <stdlib.h>

enum   rbt_color_s;
struct rbt_node_s;
struct rbt_tree_s;

typedef enum   rbt_color_e rbt_color_t;
typedef struct rbt_node_s  rbt_node_t;
typedef struct rbt_tree_s  rbt_tree_t;

enum rbt_color_e {
    RBT_RED,
    RBT_BLACK
};

struct rbt_node_s {
    rbt_color_t color;
    rbt_node_t *p;
    rbt_node_t *left;
    rbt_node_t *right;

    long key;
    long layer;
    rbt_node_t *prev;
    rbt_node_t *next;
};

/* default rbt node cmp function */
int rbt_cmp_func(rbt_node_t *x, rbt_node_t *y);

typedef int (*rbt_cmp_func_t)(rbt_node_t *, rbt_node_t *);
typedef void (*rbt_free_func_t)(void *);
 /* Note: prev will alway be passed NULL */
typedef int (*rbt_travel_func_t)(rbt_tree_t *t, rbt_node_t *prev, rbt_node_t *curr);
typedef int (*rbt_bfs_func_t)(rbt_tree_t *t, rbt_node_t *prev, rbt_node_t *curr);


/* there is no NULL in rb tree instead of nil */
#define rbt_end(t) ((t)->nil)

struct rbt_tree_s {
    rbt_node_t *root;
    rbt_node_t *nil;
    rbt_node_t sentinel;
    unsigned long size;

    rbt_cmp_func_t cmp;
    rbt_free_func_t free;
    rbt_travel_func_t travel;
    rbt_bfs_func_t bfs;
};

/* init rb tree */
void rbt_init(rbt_tree_t *t, 
        rbt_cmp_func_t cmp,
        rbt_free_func_t free,
        rbt_travel_func_t travel,
        rbt_bfs_func_t bfs);


/* clear rb tree */
void rbt_clear(rbt_tree_t *t, rbt_node_t *r);

/* destroy rb tree */
void rbt_destroy(rbt_tree_t *t);

/* rbt basic rotate functions */
void rbt_left_rotate(rbt_tree_t *t, rbt_node_t *n);
void rbt_right_rotate(rbt_tree_t *t, rbt_node_t *n);

/* rbt find functions */
rbt_node_t *rbt_min(rbt_tree_t *t, rbt_node_t *min);
rbt_node_t *rbt_max(rbt_tree_t *t, rbt_node_t *max);
rbt_node_t *rbt_find(rbt_tree_t *t, rbt_node_t *n);

/* rbt iterator function */
rbt_node_t *rbt_next(rbt_tree_t *t, rbt_node_t *n);

/* rbt transplant function */
void rbt_transplant(rbt_tree_t *t, rbt_node_t *x, rbt_node_t *y);

/* rb tree insert */
rbt_node_t *rbt_insert(rbt_tree_t *t, rbt_node_t *n);

/* rb tree delete */
rbt_node_t *rbt_delete(rbt_tree_t *t, rbt_node_t *z);

/* 
 * tree node Travel use
 * preorder, infix oder and post order
 *
 * t: tree which want to travel
 * r: is the root of the tree
 *
 */
void rbt_preorder(rbt_tree_t *t, rbt_node_t *r);
void rbt_inorder(rbt_tree_t *t, rbt_node_t *r);
void rbt_postorder(rbt_tree_t *t, rbt_node_t *r);

/* 
 * tree node QUEUE 
 */

void rbt_queue_init(rbt_node_t *queue);
void rbt_enqueue(rbt_node_t *queue, rbt_node_t *n);
rbt_node_t* rbt_dequeue(rbt_node_t *queue);
int rbt_queue_empty(rbt_node_t *queue);

/* 
 * tree node BFS
 * do a bread first searching on a rb tree 
 */

/* when func return non zero value, the rbt_bfs will stop */
void rbt_bfs(rbt_tree_t *t);


/* 
 * you can use this function to check if the
 * the tree is a red black tree.
 * 
 */
int rbt_check(rbt_tree_t *t);
#endif /* __RBT_H__ */
