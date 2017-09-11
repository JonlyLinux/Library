#include "rbt.h"

int rbt_cmp_func(rbt_node_t *x, rbt_node_t *y)
{
    return x->key - y->key;
}


void rbt_init(rbt_tree_t *t, 
        rbt_cmp_func_t cmp,
        rbt_free_func_t free,
        rbt_travel_func_t travel,
        rbt_bfs_func_t bfs)
        
{
    t->root = &t->sentinel;
    t->nil = &t->sentinel;

    t->nil->color = RBT_BLACK;
    t->nil->p = t->nil;
    t->nil->left = t->nil;
    t->nil->right = t->nil;

    t->size = 0;

    t->cmp = cmp;
    t->free = free;
    t->travel = travel;
    t->bfs = bfs;
}

void rbt_clear(rbt_tree_t *t, rbt_node_t *r)
{
   if (r == t->nil)
       return ;

   rbt_clear(t, r->left);
   rbt_clear(t, r->right);

   if (r->p == t->nil)
       t->root = t->nil;
   else if (r->p->left == r)
       r->p->left = t->nil;
   else
       r->p->right = t->nil;

   if (t->free)
       t->free(r);

   t->size--;
}

void rbt_destroy(rbt_tree_t *t)
{
    rbt_clear(t, t->root);
    rbt_init(t, t->cmp, t->free, t->travel, t->bfs);
}

void rbt_left_rotate(rbt_tree_t *t, rbt_node_t *n)
{
    rbt_node_t *x = n->right;
    rbt_node_t *y = x->left;

    x->p = n->p;
    if (n->p == t->nil)
        t->root = x;
    else if (n->p->left == n)
        n->p->left = x;
    else 
        n->p->right = x;

    /* Note: derectly changing nil's parent will occur error 
     *       on rbt delete 
     *
     * n->right = y;
     * y->p = n;
     *  
     * you should make some check when deleting a node
     * */
    n->right = y;
    if (y != t->nil) 
        y->p = n;

    x->left = n;
    n->p = x;
}

void rbt_right_rotate(rbt_tree_t *t, rbt_node_t *n)
{
    rbt_node_t *x = n->left;
    rbt_node_t *y = x->right;

    x->p = n->p;
    if (n->p == t->nil)
        t->root = x;
    else if (n->p->left == n)
        n->p->left = x;
    else 
        n->p->right = x;

    n->left = y;
    if (y != t->nil)
        y->p = n;

    x->right = n;
    n->p = x;
}

rbt_node_t *rbt_min(rbt_tree_t *t, rbt_node_t *min)
{
    while (min != t->nil && min->left != t->nil)
        min = min->left;

    return min;
}

rbt_node_t *rbt_max(rbt_tree_t *t, rbt_node_t *max)
{
    while (max != t->nil && max->right != t->nil)
        max = max->right;

    return max;
}

rbt_node_t *rbt_find(rbt_tree_t *t, rbt_node_t *n)
{
    rbt_node_t *x = t->root;
    int v;

    while (x != t->nil) {
        v =  t->cmp(n, x);
        if (v < 0)
            x = x->left;
        else if (v > 0)
            x = x->right;
        else
            break;
    }

    return x;
}

rbt_node_t *rbt_next(rbt_tree_t *t, rbt_node_t *n)
{
    rbt_node_t *next;

    if (n->right != t->nil) {
        next = rbt_min(t, n->right);
    } else {
        next = n->p;
        while (next != t->nil && next->right == n) {
            n = next;
            next = next->p;
        }
    }

    return next;
}

void rbt_transplant(rbt_tree_t *t, rbt_node_t *x, rbt_node_t *y)
{
    y->p = x->p;
    if (x->p == t->nil)
        t->root = y;
    else if (x->p->left == x)
        x->p->left = y;
    else
        x->p->right = y;
}

rbt_node_t *rbt_insert_fixup(rbt_tree_t *t, rbt_node_t *n)
{
    rbt_node_t *p, *g, *u;

    while (n->p->color == RBT_RED) {
        p = n->p;
        g = p->p;
        if (p == g->left) {
            u = g->right;
            if (u->color == RBT_RED) {
                p->color = RBT_BLACK;
                u->color = RBT_BLACK;
                g->color = RBT_RED;
                n = g;
            } else {
                if (n == p->left) {
                    rbt_right_rotate(t, g);
                    p->color = RBT_BLACK;
                    g->color = RBT_RED;
                } else {
                    rbt_left_rotate(t, p);
                    n = p;
                }
            }
        } else {
            u = g->left;
            if (u->color == RBT_RED) {
                p->color = RBT_BLACK;
                u->color = RBT_BLACK;
                g->color = RBT_RED;
                n = g;
            } else {
                if (n == p->right) {
                    rbt_left_rotate(t, g);
                    p->color = RBT_BLACK;
                    g->color = RBT_RED;
                } else {
                    rbt_right_rotate(t, p);
                    n = p;
                }
            }
        }
    }

    t->root->color = RBT_BLACK;

    return NULL;
}

rbt_node_t *rbt_insert(rbt_tree_t *t, rbt_node_t *n)
{
    rbt_node_t *p = t->nil;
    rbt_node_t *x = t->root;
    char left = 0;

    while (x != t->nil) {
        p = x;
        left = 0;
        if (t->cmp(n, x) <= 0) {
            x = x->left;
            left = 1;
        } else {
            x = x->right;
        }
    }

    n->p = p;
    if (p == t->nil)
        t->root = n;
    else if (left == 1)
        p->left = n;
    else
        p->right = n;

    n->left = t->nil;
    n->right = t->nil;

    n->color = RBT_RED;

    rbt_insert_fixup(t, n);

    t->size++;

    return n;
}

rbt_node_t *rbt_delete_fixup(rbt_tree_t *t, rbt_node_t *n)
{
    rbt_node_t *p;
    rbt_node_t *b;

    while (n != t->root && n->color == RBT_BLACK) {
        p = n->p;
        if (n == p->left) {
            b = p->right;
            if (b->color == RBT_RED) {
                b->color = RBT_BLACK;
                p->color = RBT_RED;
                rbt_left_rotate(t, p);
            } else if (b->left->color == RBT_BLACK && b->right->color == RBT_BLACK) {
                b->color = RBT_RED;
                n = p;
            } else if (b->right->color == RBT_BLACK) {
                b->color = RBT_RED;
                b->left->color = RBT_BLACK;
                rbt_right_rotate(t, b);
            } else {
                b->color = p->color;
                b->right->color = RBT_BLACK;
                p->color = RBT_BLACK;
                rbt_left_rotate(t, p); 
                n = t->root;
            }
        } else {
            b = p->left;
            if (b->color == RBT_RED) {
                b->color = RBT_BLACK;
                p->color = RBT_RED;
                rbt_right_rotate(t, p);
            } else if (b->left->color == RBT_BLACK && b->right->color == RBT_BLACK) {
                b->color = RBT_RED;
                n = p;
            } else if (b->left->color == RBT_BLACK) {
                b->color = RBT_RED;
                b->right->color = RBT_BLACK;
                rbt_left_rotate(t, b);
            } else {
                b->color = p->color;
                b->left->color = RBT_BLACK;
                p->color = RBT_BLACK;
                rbt_right_rotate(t, p); 
                n = t->root;
            }

        }
    }

    n->color = RBT_BLACK;
}


rbt_node_t *rbt_delete(rbt_tree_t *t, rbt_node_t *z)
{
    rbt_node_t *y;
    rbt_color_t yc;
    rbt_node_t *x;

    y = z;
    yc = y->color;

    if (y->left == t->nil) {
        x = y->right;
        rbt_transplant(t, y, x);
    } else if (y->right == t->nil) {
        x = y->left;
        rbt_transplant(t, y, x);
    } else {
        y = rbt_min(t, y->right);

        x = y->right;
        yc = y->color;

        rbt_transplant(t, y, x);
        rbt_transplant(t, z, y);
        
        y->left = z->left;
        y->left->p = y;

        y->right = z->right;
        y->right->p = y;

        y->color = z->color;
    }

    if (yc == RBT_BLACK)
        rbt_delete_fixup(t, x);

    t->size--;
}

void rbt_preorder(rbt_tree_t *t, rbt_node_t *r)
{
    if (r == t->nil)
        return;

    if (t->travel) {
        t->travel(t, NULL, r);
    }
    rbt_preorder(t, r->left);
    rbt_preorder(t, r->right);
}

void rbt_inorder(rbt_tree_t *t, rbt_node_t *r)
{
    if (r == t->nil)
        return;

    rbt_inorder(t, r->left);
    if (t->travel) {
        t->travel(t, NULL, r);
    }
    rbt_inorder(t, r->right);
}

void rbt_postorder(rbt_tree_t *t, rbt_node_t *r)
{
    if (r == t->nil)
        return;

    rbt_postorder(t, r->left);
    rbt_postorder(t, r->right);
    if (t->travel) {
        t->travel(t, NULL, r);
    }
}


void rbt_queue_init(rbt_node_t *queue)
{
    queue->next = queue;
    queue->prev = queue;
}

void rbt_enqueue(rbt_node_t *queue, rbt_node_t *n)
{
    rbt_node_t *prev = queue->prev;

    n->prev = prev;
    n->next = queue;

    prev->next = n;
    queue->prev = n;
}

rbt_node_t* rbt_dequeue(rbt_node_t *queue)
{
    rbt_node_t *next = queue->next;

    queue->next = next->next;
    next->next->prev = queue;

    return next;
}

int rbt_queue_empty(rbt_node_t *queue)
{
    return (queue->next == queue);
}

void rbt_bfs(rbt_tree_t *t)
{
    rbt_node_t queue;
    rbt_node_t *n;
    rbt_node_t *prev = NULL;

    rbt_queue_init(&queue);

    if (t->root != t->nil) {
        rbt_enqueue(&queue, t->root);
        t->root->layer = 0;
    }

    while (!rbt_queue_empty(&queue)) {
        n = rbt_dequeue(&queue);

        if (t->bfs) {
            if (!t->bfs(t, prev, n)) {
                break;
            }
        }

        if (n->left != rbt_end(t)) {
            rbt_enqueue(&queue, n->left);
            n->left->layer = n->layer + 1;
        } 

        if (n->right != rbt_end(t)) {
            rbt_enqueue(&queue, n->right);
            n->right->layer = n->layer + 1;
        } 

        prev = n;
    }
}


/* *
 * 
 * is the tree is a red black tree?
 * ret:
 *     0 -> is rbt
 *     1 -> color error
 *     2 -> root is not black
 *     3 -> nil  is not black
 *     4 -> red node own red child
 *     5 -> path's black nodes is not same
 * */
int rbt_check(rbt_tree_t *t)
{
    rbt_node_t queue;
    rbt_node_t *n, *p;
    rbt_node_t endsq;

    if (t->root->color != RBT_BLACK) {
        return 2;
    }

    if (t->nil->color != RBT_BLACK) {
        return 3;
    }

    rbt_queue_init(&queue);
    rbt_queue_init(&endsq);

    if (t->root != t->nil) {
        rbt_enqueue(&queue, t->root);
    }

    while (!rbt_queue_empty(&queue)) {
        n = rbt_dequeue(&queue);

        if (n->color != RBT_RED && n->color != RBT_BLACK) {
            return 1;
        }

        if (n->left != rbt_end(t)) {
            rbt_enqueue(&queue, n->left);
        } 

        if (n->right != rbt_end(t)) {
            rbt_enqueue(&queue, n->right);
        }

        if (n->left == rbt_end(t) || n->right == rbt_end(t)) {
            rbt_enqueue(&endsq, n);
        }
    }

    /* pick one path and caculate the black nodes number */
    int curr_black_size = 0;
    int last_black_size = 0;

    while (!rbt_queue_empty(&endsq)) {
        n = rbt_dequeue(&endsq);
        curr_black_size = 0;
        
        while (n != t->nil) {
            p = n->p;
            if (n->color == RBT_RED && p->color == RBT_RED) {
                return 4;
            }

            if (n->color == RBT_BLACK)
                curr_black_size++;

            n = p;
        }

        if (last_black_size != 0 && last_black_size != curr_black_size) {
            return 5;
        }

        last_black_size = curr_black_size;
    }

    return 0;
}


