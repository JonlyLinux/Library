#include "btree.h"

btree_t *btree_init(btree_t *t, bnode_key_cmp_t key_cmp, bnode_travle_t  travel_func)
{
    t->root = NULL;
    t->key_cmp = key_cmp == NULL ?
                default_bnode_key_cmp : key_cmp;
    t->travel_func = travel_func == NULL ?
                default_bnode_travle : travel_func;

    return t;
}

int btree_destroy(btree_t *t)
{
    return 0;
}

static int _btree_search(bnode_t *x, long k,
                         bnode_t **retn, long *idx)
{
    int i;

    for (i = 0; i < x->n && k > x->key[i]; i++);

    if (i < x->n && k == x->key[i]) {
        *retn = x;
        *idx = i;
        return 0;
    } else {
        if (x->leaf)
            return -1;
        else
            return _btree_search(x->child[i], k, retn, idx);
    }
}

int btree_search(btree_t *t,  long k,
                 bnode_t **retn,  long *idx)
{
    *retn = NULL;
    *idx = 0;

    if (t->root == NULL) {
        return -1;
    }

    return _btree_search(t->root, k, retn, idx);
}


/* make bnode not full */
/* x is not full, x.child[idx] is full */
bnode_t *btree_splite_child(bnode_t *x, long idx)
{
    bnode_t *y = x->child[idx];
    bnode_t *z = calloc(1, sizeof(bnode_t));
    int i = 0;

    /*finish z*//* [0, t-2][t-1][t, 2t-2] */
    for (i = 0; i < BTREE_MAX_DEGREE - 1; i++) {
        z->key[i] = y->key[i + BTREE_MAX_DEGREE];
    }

    if (!y->leaf) {
        for (i = 0; i < BTREE_MAX_DEGREE; i++) {
            z->child[i] = y->child[i + BTREE_MAX_DEGREE];
        }
    }

    z->leaf = y->leaf;
    z->n = BTREE_MAX_DEGREE - 1;

    /*finish y*/
    y->n = BTREE_MAX_DEGREE - 1;

    /*finish x*/
    for (i = x->n - 1; i >= idx; i--) {
        x->key[i+1] = x->key[i];
    }
    x->key[idx] = y->key[BTREE_MAX_DEGREE-1];

    for (i = x->n; i >= idx + 1; i--) {
        x->child[i+1] = x->child[i];
    }
    x->child[idx+1] = z;

    x->n++;
    return x;
}

/* insert k into a tree which is not full */
int btree_insert_not_full(bnode_t *n, long k)
{
    int i;

    if (n->leaf) {
        for (i = n->n - 1; i >= 0 && k < n->key[i]; i--) {
            n->key[i+1] = n->key[i];
        }
        i++;
        n->key[i] = k;
        n->n++;
    } else {
        for (i = 0; i < n->n && k > n->key[i]; i++);
        if (n->child[i]->n == BTREE_MAX_KEY) {
            btree_splite_child(n, i);
            if (k > n->key[i])
                i++;
        }
        btree_insert_not_full(n->child[i], k);
    }
}

/* insert k into a tree which maybe full */
int btree_insert(btree_t *t, long k)
{
    bnode_t *n;

    if (t->root == NULL) {
        t->root = calloc(1, sizeof(bnode_t));
        t->root->leaf = true;
        t->root->key[0] = k;
        t->root->n = 1;
        return 0;
    }
    else if (t->root->n == BTREE_MAX_KEY) {
        n = calloc(1, sizeof(bnode_t));
        n->n = 0;
        n->leaf = false;
        n->child[0] = t->root;
        t->root = n;
        btree_splite_child(n, 0);
    }

    btree_insert_not_full(t->root, k);
    return 0;
}

long btree_find_min(bnode_t *x)
{
    if (x == NULL) {
        return -1;
    }

    while (!x->leaf) {
        x = x->child[0];
    }

    return x->key[0];
}

long btree_find_max(bnode_t *x)
{
    if (x == NULL) {
        return -1;
    }

    while (!x->leaf) {
        x = x->child[x->n];
    }

    return x->key[x->n - 1];
}

/* x own enough keys which at least t,
 * x.c[idx] and x.c[idx+1] both have t-1 keys
 */
int btree_merge_child(bnode_t *x, long idx)
{
    bnode_t *y = x->child[idx];
    bnode_t *z = x->child[idx + 1];
    int i;

    /* modify y */
    for (i = 0; i < z->n + 1; i++) {
        y->child[y->n + 1 + i] = z->child[i];
    }

    y->key[y->n] = x->key[idx];
    y->n++;

    for (i = 0; i < z->n; i++) {
        y->key[y->n + i] = z->key[i];
    }
    y->n += z->n;

    /* mofify x */
    for (i = idx+1; i < x->n; i++) {
        x->key[i-1] = x->key[i];
    }

    for (i = idx + 2; i < x->n + 1; i++) {
        x->child[i-1] = x->child[i];
    }
    x->n--;

    /* free z */
    free(z);

    return 0;
}


int btree_stole_left(bnode_t *x, long idx)
{
    int i;
    bnode_t *c = x->child[idx];
    bnode_t *l = x->child[idx - 1];

    /*make space in c*/
    for (i = c->n-1; i >= 0; i--) {
        c->key[i+1] = c->key[i];
    }

    if (!c->leaf) {
        for (i = c->n; i >= 0; i--) {
            c->child[i+1] = c->child[i];
        }
    }

    c->key[0] = x->key[idx-1];
    c->child[0] = l->child[l->n];
    c->n++;

    x->key[idx-1] = l->key[l->n - 1];

    l->n--;

    return 0;
}

int btree_stole_right(bnode_t *x, long idx)
{
    int i;
    bnode_t *c = x->child[idx];
    bnode_t *r = x->child[idx + 1];

    /*make space in c*/
    c->key[c->n] = x->key[idx];
    c->child[c->n+1] = r->child[0];
    c->n++;

    x->key[idx] = r->key[0];

    for (i = 1; i < r->n; i++) {
        r->key[i-1] = r->key[i];
    }
    for (i = 1; i < r->n + 1; i++) {
        r->child[i-1] = r->child[i];
    }
    r->n--;

    return 0;
}

/* delete k from tree which own enough keys */
int btree_delete_enough(bnode_t *x, long k)
{
    int i;
    int j;
    long kk;
    int found = false;

    for (i = 0; i < x->n && k > x->key[i]; i++);

    found = (i < x->n && k == x->key[i]) ? true : false;

    if (found) {
        if (x->leaf) {
            for (j = i+1; j < x->n; j++) {
                x->key[j-1] = x->key[j];
            }
            x->n--;
        } else {
            if (x->child[i]->n >= BTREE_MAX_DEGREE) {
                kk = btree_find_max(x->child[i]);
                x->key[i] = kk;
                btree_delete_enough(x->child[i], kk);
            } else if (x->child[i+1]->n >= BTREE_MAX_DEGREE) {
                kk = btree_find_min(x->child[i+1]);
                x->key[i] = kk;
                btree_delete_enough(x->child[i+1], kk);
            } else {
                btree_merge_child(x, i);
                btree_delete_enough(x->child[i], k);
            }
        }
    } else if (!x->leaf) {
        if (x->child[i]->n == BTREE_MIN_KEY) {
            if (i >= 1 && x->child[i-1]->n >= BTREE_MAX_DEGREE) {
                //stole from left brother
                btree_stole_left(x, i);
                btree_delete_enough(x->child[i], k);
            } else if (i < x->n && x->child[i+1]->n >= BTREE_MAX_DEGREE) {
                //stole from right brother
                btree_stole_right(x, i);
                btree_delete_enough(x->child[i], k);
            } else {
                if (i == x->n)
                    i--;
                btree_merge_child(x, i);
                btree_delete_enough(x->child[i], k);
            }
        } else {
            btree_delete_enough(x->child[i], k);
        }
    }

    return 0;
}

/* delete k from tree which maybe not have enough keys */
int btree_delete(btree_t *t, long k)
{
    int i, j;
    bnode_t *x = t->root;
    int found;
    long kk;

    if (t->root == NULL) {
        return -1;
    } else if (t->root->n <= BTREE_MIN_KEY) {
        for (i = 0; i < x->n && k > x->key[i]; i++);
        found = (i < x->n && k == x->key[i]) ? true : false;
        if (found) {
            if (x->leaf) {
                for (j = i+1; j < x->n; j++) {
                    x->key[j-1] = x->key[j];
                }
                x->n--;
                if (x->n == 0) {
                    free(x);
                    t->root = NULL;
                }
            } else {
                if (x->child[i]->n >= BTREE_MAX_DEGREE) {
                    kk = btree_find_max(x->child[i]);
                    x->key[i] = kk;
                    btree_delete_enough(x->child[i], kk);
                } else if (x->child[i+1]->n >= BTREE_MAX_DEGREE) {
                    kk = btree_find_min(x->child[i+1]);
                    x->key[i] = kk;
                    btree_delete_enough(x->child[i+1], kk);
                } else {
                    btree_merge_child(x, i);
                    if (x->n == 0) {
                        t->root = x->child[0];
                        free(x);
                    }
                    btree_delete_enough(t->root, k);
                }
            }
        } else if (!x->leaf) {
            if (x->child[i]->n == BTREE_MIN_KEY) {
                if (i >= 1 && x->child[i-1]->n >= BTREE_MAX_DEGREE) {
                    //stole from left brother
                    btree_stole_left(x, i);
                    btree_delete_enough(x->child[i], k);
                } else if (i < x->n && x->child[i+1]->n >= BTREE_MAX_DEGREE) {
                    //stole from right brother
                    btree_stole_right(x, i);
                    btree_delete_enough(x->child[i], k);
                } else {
                    if (i == x->n)
                        i--;
                    btree_merge_child(x, i);
                    if (x->n == 0) {
                        t->root = x->child[0];
                        free(x);
                    }
                    btree_delete_enough(t->root, k);
                }
            } else {
                btree_delete_enough(x->child[i], k);
            }
        }
    } else {
        btree_delete_enough(t->root, k);
    }

    return 0;
}


/* queue with btree node */
int btinitqueue(bnode_t *queue)
{
    queue->next = queue;
    queue->prev = queue;
}

int btenqueue(bnode_t *queue, bnode_t *node)
{
    if (node) {
        node->next = queue->next;
        node->prev = queue;
        queue->next->prev = node;
        queue->next = node;
    }
}

bnode_t *btdequeue(bnode_t *queue)
{
    if (queue->next == queue) {
        return NULL;
    }

    bnode_t *node = queue->prev;
    queue->prev = node->prev;
    node->prev->next = queue;

    return node;
}

void btree_bfs(btree_t *tree)
{
    bnode_t tmp_queue, *queue, *node;
    queue = &tmp_queue;
    int i;

    btinitqueue(queue);

    int layer = 0;

    if (tree->root) {
        tree->root->layer = 0;
        btenqueue(queue, tree->root);
    }

    while ((node = btdequeue(queue)) != NULL) {
        if (node->layer != layer) {
            printf("\n");
            layer++;
        }
        tree->travel_func(node);

        for (i = 0; i <= node->n && !node->leaf; i++) {
            node->child[i]->layer = node->layer + 1;
            btenqueue(queue, node->child[i]);
        }
    }

    printf("\n");
}















