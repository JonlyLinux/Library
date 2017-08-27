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
    // ��ȡ������ͷ�ڵ�  
    rbtree_node_t* node = tree->root;  

    // �ƶ�ָ�룬ָ����������������ߣ�ֵ��С���Ľڵ�  
    if(node == tree->sentinel)
    	iter->cur = NULL;
    else
    	iter->cur = rbtree_min(node, tree->sentinel);
    iter->root = tree->root;
    iter->sentinel = tree->sentinel;
}  

/** 
 * �������ָ����һ��λ�� 
 * @param iter ָ��������ṹ�������ָ�� 
 * @param value ���һ��ֵ 
 */  
int rbtree_iterator_next(rbtree_iterator_t *iter, rbtree_node_t **cur)  
{  
    rbtree_node_t* node = iter->cur;  
    // �жϵ����Ƿ����  
    if (!node)  
        return 0;  
  
    /* 
        �ڵ�ĵ��� 
            ����һ����������˵������һ�ַ����������η������е����нڵ㣬�������Խṹ��ͬ��Ҫ�����������нڵ㣬���밴��һ�ֹ���Ͳ��裺 
        1. �ڿ�ʼ����ǰ������ָ��ָ��������������ߵĽڵ㣨������ֵ��С�Ľڵ㣩���Դ���Ϊ��������㣻 
        2. ÿ�����Ե�ǰ�ڵ��Һ��ӵ���֧����ĩ�ڵ���Ϊ��������һ���ڵ㣬�ýڵ��ȻΪ�ȵ�ǰ�ڵ�ֵ�����Сֵ�ڵ㣻 
        3. �����ǰ�ڵ�û���Һ��ӣ�������丸�ڵ㣬�������Ե�ǰ�ڵ�Ϊ�Һ��ӵĸ��ڵ���Ϊ��һ���ڵ� 
        4. ����ڵ�3���õ�NULLֵ����ʾ������������ 
        �������̲ο�[ͼ3] 
     */  
  
    // ����ڵ�ֵ  
    *cur = node;
  
    // �жϵ�ǰ�ڵ��Ƿ����Һ���  
    if (node->right && node->right != iter->sentinel) {   // ���Һ��ӵ����  
      
        // ��ָ��ָ��ǰ�ڵ���Һ��ӣ�����ýڵ�û����֧����ýڵ����Ϊ��������һ���ڵ㣩  
        node = node->right;  
        // ͨ��ѭ����ָ��ָ��ýڵ���֧����ĩ�ڵ㣬�ýڵ�Ϊ��������һ���ڵ�  
        while (node->left && node->left != iter->sentinel)  
            node = node->left;  
    } else {               // û���Һ��ӵ����  
      
        rbtree_node_t *temp;  
        // ���Ϸ��ʵ�ǰ�ڵ�ĸ��ڵ�  
        do  {
            temp = node;  
            if(node == iter->root)
            	node = NULL;
            else
            	node = node->parent;  
        } while (node && temp == node->right);  // ���η��ʵ�ǰ�ڵ�ĸ��ڵ㣬ֱ��û�и��ڵ㣨����ͷ�ڵ㣩���ߵ�ǰ�ڵ㲻���丸�ڵ���Һ���  
    }  
    // ����ǰ�������Ľڵ㱣���ڵ�������  
    iter->cur = node;  
    return 1;  
}  

