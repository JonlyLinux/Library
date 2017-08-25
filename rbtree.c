#include "rbtree.h"

/*
 * The red-black tree code is based on the algorithm described in
 * the "Introduction to Algorithms" by Cormen, Leiserson and Rivest.
 */


static inline void rbtree_left_rotate(rbtree_node_t **root,
    rbtree_node_t *sentinel, rbtree_node_t *node);
static inline void rbtree_right_rotate(rbtree_node_t **root,
    rbtree_node_t *sentinel, rbtree_node_t *node);


void
rbtree_insert(rbtree_t *tree,
    rbtree_node_t *node)
{
    rbtree_node_t  **root, *temp, *sentinel;

    /* a binary tree insert */

    root = (rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;

    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        rbt_black(node);
        *root = node;

        return;
    }

    tree->insert(*root, node, sentinel);

    /* re-balance tree */

    while (node != *root && rbt_is_red(node->parent)) {

        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;

            if (rbt_is_red(temp)) {
                rbt_black(node->parent);
                rbt_black(temp);
                rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rbtree_left_rotate(root, sentinel, node);
                }

                rbt_black(node->parent);
                rbt_red(node->parent->parent);
                rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else {
            temp = node->parent->parent->left;

            if (rbt_is_red(temp)) {
                rbt_black(node->parent);
                rbt_black(temp);
                rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rbtree_right_rotate(root, sentinel, node);
                }

                rbt_black(node->parent);
                rbt_red(node->parent->parent);
                rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    rbt_black(*root);
}


void
rbtree_insert_value(rbtree_node_t *temp, rbtree_node_t *node,
    rbtree_node_t *sentinel)
{
    rbtree_node_t  **p;

    for ( ;; ) {

        p = (node->key < temp->key) ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    rbt_red(node);
}


void
rbtree_insert_timer_value(rbtree_node_t *temp, rbtree_node_t *node,
    rbtree_node_t *sentinel)
{
    rbtree_node_t  **p;

    for ( ;; ) {

        /*
         * Timer values
         * 1) are spread in small range, usually several minutes,
         * 2) and overflow each 49 days, if milliseconds are stored in 32 bits.
         * The comparison takes into account that overflow.
         */

        /*  node->key < temp->key */

        p = ((rbtree_key_int_t) (node->key - temp->key) < 0)
            ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    rbt_red(node);
}


void
rbtree_delete(rbtree_t *tree,
    rbtree_node_t *node)
{
    unsigned char        red;
    rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

    /* a binary tree delete */

    if(node->left == NULL && node->right == NULL && node->parent == NULL)
    	return ;

    root = (rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;

    if (node->left == sentinel) {
        temp = node->right;
        subst = node;

    } else if (node->right == sentinel) {
        temp = node->left;
        subst = node;

    } else {
        subst = rbtree_min(node->right, sentinel);

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    if (subst == *root) {
        *root = temp;
        rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    red = rbt_is_red(subst);

    if (subst == subst->parent->left) {
        subst->parent->left = temp;

    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {

        temp->parent = subst->parent;

    } else {

        if (subst->parent == node) {
            temp->parent = subst;

        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        rbt_copy_color(subst, node);

        if (node == *root) {
            *root = subst;

        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) {
        return;
    }

    /* a delete fixup */

    while (temp != *root && rbt_is_black(temp)) {

        if (temp == temp->parent->left) {
            w = temp->parent->right;

            if (rbt_is_red(w)) {
                rbt_black(w);
                rbt_red(temp->parent);
                rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            if (rbt_is_black(w->left) && rbt_is_black(w->right)) {
                rbt_red(w);
                temp = temp->parent;

            } else {
                if (rbt_is_black(w->right)) {
                    rbt_black(w->left);
                    rbt_red(w);
                    rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                rbt_copy_color(w, temp->parent);
                rbt_black(temp->parent);
                rbt_black(w->right);
                rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }

        } else {
            w = temp->parent->left;

            if (rbt_is_red(w)) {
                rbt_black(w);
                rbt_red(temp->parent);
                rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (rbt_is_black(w->left) && rbt_is_black(w->right)) {
                rbt_red(w);
                temp = temp->parent;

            } else {
                if (rbt_is_black(w->left)) {
                    rbt_black(w->right);
                    rbt_red(w);
                    rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                rbt_copy_color(w, temp->parent);
                rbt_black(temp->parent);
                rbt_black(w->left);
                rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    rbt_black(temp);
}


static inline void
rbtree_left_rotate(rbtree_node_t **root, rbtree_node_t *sentinel,
    rbtree_node_t *node)
{
    rbtree_node_t  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}


static inline void
rbtree_right_rotate(rbtree_node_t **root, rbtree_node_t *sentinel,
    rbtree_node_t *node)
{
    rbtree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->right) {
        node->parent->right = temp;

    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}

void rbtree_iterator_init(const rbtree_t *tree, rbtree_iterator_t *iter)  
{  
    // 获取二叉树头节点  
    rbtree_node_t* node = tree->root;  

    // 移动指针，指向整个二叉树最左边（值最小）的节点  
    if(node == tree->sentinel)
    	iter->cur = NULL;
    else
    	iter->cur = rbtree_min(node, tree->sentinel);
    iter->root = tree->root;
    iter->sentinel = tree->sentinel;
}  

/** 
 * 令迭代器指向下一个位置 
 * @param iter 指向迭代器结构体变量的指针 
 * @param value 输出一个值 
 */  
int rbtree_iterator_next(rbtree_iterator_t *iter, rbtree_node_t **cur)  
{  
    rbtree_node_t* node = iter->cur;  
    // 判断迭代是否结束  
    if (!node)  
        return 0;  
  
    /* 
        节点的迭代 
            对于一个二叉树来说，总有一种方法可以依次访问树中的所有节点，但和线性结构不同，要遍历树中所有节点，必须按照一种规则和步骤： 
        1. 在开始遍历前，先用指针指向整个树中最左边的节点（即树中值最小的节点），以此作为遍历的起点； 
        2. 每次总以当前节点右孩子的左支的最末节点作为迭代的下一个节点，该节点必然为比当前节点值大的最小值节点； 
        3. 如果当前节点没有右孩子，则访问其父节点，并将不以当前节点为右孩子的父节点作为下一个节点 
        4. 如果在第3步得到NULL值，表示整个遍历结束 
        遍历流程参考[图3] 
     */  
  
    // 保存节点值  
    *cur = node;
  
    // 判断当前节点是否有右孩子  
    if (node->right && node->right != iter->sentinel) {   // 有右孩子的情况  
      
        // 令指针指向当前节点的右孩子（如果该节点没有左支，则该节点就作为迭代的下一个节点）  
        node = node->right;  
        // 通过循环令指针指向该节点左支的最末节点，该节点为迭代的下一个节点  
        while (node->left && node->left != iter->sentinel)  
            node = node->left;  
    } else {               // 没有右孩子的情况  
      
        rbtree_node_t *temp;  
        // 向上访问当前节点的父节点  
        do  {
            temp = node;  
            if(node == iter->root)
            	node = NULL;
            else
            	node = node->parent;  
        } while (node && temp == node->right);  // 依次访问当前节点的父节点，直到没有父节点（到达头节点）或者当前节点不是其父节点的右孩子  
    }  
    // 将当前迭代到的节点保存在迭代器中  
    iter->cur = node;  
    return 1;  
}  


