#include "avltree.h"

avltree_t *avltree_init(avltree_t *tree, avlnode_cmp_func_t cmp_func, 
        avlnode_del_func_t del_func,
        avlnode_travel_func_t travel_func)
{
    tree->root = NULL;
    tree->cmp_func = cmp_func;
    tree->del_func = del_func;
    tree->travel_func = travel_func;
    tree->spinlock = 0;
}

static void _avltree_destroy(avlnode_t *root, avlnode_del_func_t del_func)
{
    if (root) {
        _avltree_destroy(root->left, del_func);
        _avltree_destroy(root->right, del_func);
        root->parent = NULL;
        root->left   = NULL;
        root->right  = NULL;    
        root->height = 0;
        del_func(root); 
    }
}

avltree_t *avltree_destroy(avltree_t *tree)
{
    _avltree_destroy(tree->root, tree->del_func);
    tree->root = NULL;
    tree->cmp_func = NULL;
    tree->spinlock = 0;
}

static inline avlnode_t *_avltree_single_rotate_left(avlnode_t *top, avlnode_t *cent)
{
    top->left = cent->right;
    cent->right = top;
    top->height = CALHEIGHT(top);
    cent->height = CALHEIGHT(cent);

    return cent;
}
static inline avlnode_t *_avltree_single_rotate_right(avlnode_t *top, avlnode_t *cent)
{
    top->right = cent->left;
    cent->left = top;

    top->height = CALHEIGHT(top); 
    cent->height = CALHEIGHT(cent);

    return cent;
}
static avlnode_t *_avltree_double_rotate_left(avlnode_t *top, avlnode_t *cent, avlnode_t *bot)
{
    _avltree_single_rotate_right(cent, bot);
    _avltree_single_rotate_left(top, bot);

    return bot;
}
static avlnode_t *_avltree_double_rotate_right(avlnode_t *top, avlnode_t *cent, avlnode_t *bot)
{
    _avltree_single_rotate_left(cent, bot);
    _avltree_single_rotate_right(top, bot);

    return bot;
}
static avlnode_t *_avltree_insert(avlnode_t *root, avlnode_t *node, avlnode_cmp_func_t cmp_func, avlnode_t **retnode)
{
    int val;
    int hp, hl, hr;

    if (root == NULL) {
        return node;
    }

    node->parent = root;
    val = cmp_func(node, root);
    if (val < 0) {
        root->left = _avltree_insert(root->left, node, cmp_func, retnode);
        
        /* make balance */
        hl = HEIGHT(root->left);
        hr = HEIGHT(root->right);
        
        if (hl - hr == 2) {
            val = cmp_func(node, root->left);
            if (val < 0) {
                root = _avltree_single_rotate_left(root, root->left);
            } else {
                root = _avltree_double_rotate_left(root, root->left, root->left->right);
            }
        } else {
            /* caculate root's height */
            root->height = MAX(hl, hr) + 1;
        }
    } else if (val > 0) {
        root->right = _avltree_insert(root->right, node, cmp_func, retnode);
        
        /* make balance */
        hl = HEIGHT(root->left);
        hr = HEIGHT(root->right);
        
        if (hr -hl == 2) {
            val = cmp_func(node, root->right);
            if (val > 0) {
                root = _avltree_single_rotate_right(root, root->right);
            } else {
                root = _avltree_double_rotate_right(root, root->right, root->right->left);
            }
        } else {
            /* caculate root's height */
            root->height = MAX(hl, hr) + 1;
        }
    } else {
        node->height = root->height;
        node->left = root->left;
        node->right = root->right;
        node->parent = root->parent;
        if (root->parent && root->parent->left == root)
            root->parent->left = node;
        else if (root->parent && root->parent->right == root)
            root->parent->right = node;
        
        *retnode = root;
        root = node;
    }

    return root;
}

/*
 * in:  tree, node
 * out: compare the node with the node in the tree, 
 *      if node equal the old node. then insert node,
 *      return the old node.
 *      else return NULL
 * */
avlnode_t *avltree_insert(avltree_t *tree, avlnode_t *node)
{
    avlnode_t *retnode = NULL;

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->height = 0;
    tree->root = _avltree_insert(tree->root, node, tree->cmp_func, &retnode);

    return retnode;
}

avlnode_t *avltree_delete(avltree_t *tree, avlnode_t *node)
{
}


static int avlinitqueue(avlnode_t *queue)
{
    queue->next = queue;
    queue->prev = queue;
}
static int avlenqueue(avlnode_t *queue, avlnode_t *node)
{
    if (node) {
        node->next = queue->next;
        node->prev = queue;
        queue->next->prev = node;
        queue->next = node;
    }
}

static avlnode_t *avldequeue(avlnode_t *queue)
{
    if (queue->next == queue) {
        return NULL;
    }

    avlnode_t *node = queue->prev;
    queue->prev = node->prev;
    node->prev->next = queue;

    return node;
}

void avltree_bfs(avltree_t *tree)
{
    avlnode_t tmp_queue, *queue, *node;
    queue = &tmp_queue;

    avlinitqueue(queue);
    
    int layer = 0;
    tree->root->layer = 0;
    avlenqueue(queue, tree->root);

    while ((node = avldequeue(queue)) != NULL) {
        if (node->layer != layer) {
            printf("\n");
            layer++;
        }
        tree->travel_func(node);
        
        if (node->left) {
            node->left->layer = node->layer + 1;
            avlenqueue(queue, node->left);
        }

        if (node->right) {
            node->right->layer = node->layer + 1;
            avlenqueue(queue, node->right);
        }
    }

    printf("\n");
}

/*
 * On success return entry's address
 * or return NULL
 */

avlnode_t *avltree_find(avltree_t *tree, avlnode_t *node)
{
    int retval = 0;
    avlnode_t *root = NULL;

    root = tree->root;

    while (root) {
        retval = tree->cmp_func(node, root);
        if (retval == 0) {
            break;
        } else if (retval < 0) {
            root = root->left;
        } else {
            root = root->right;
        }
    }

    return root;
}

avlnode_t *avltree_find_min(avltree_t *tree)
{
    avlnode_t *root = NULL;
    root = tree->root;
    
    while (root && root->left) root = root->left;

    return root;
}

avlnode_t *avltree_find_max(avltree_t *tree)
{
    avlnode_t *root = NULL;
    root = tree->root;
    
    while (root && root->right) root = root->right;

    return root;
}


