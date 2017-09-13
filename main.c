#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

#include "jhash.h"
#include "list.h"
#include "queue.h"
#include "avltree.h"
#include "sync.h"
#include "rbt.h"

typedef struct fucker_s {
    int id;
    int name;
    int age;

    avlnode_t node;
} fucker_t;

int fucker_cmp(avlnode_t *n1, avlnode_t *n2)
{
    fucker_t *f1, *f2;

    f1 = container_of(n1, fucker_t, node);
    f2 = container_of(n2, fucker_t, node);

    return (f1->id - f2->id);
}

int fucker_del(avlnode_t *n)
{
    fucker_t *f;
    f = container_of(n, fucker_t, node);

    free(f);

    return 0;
}

int fucker_travel(avlnode_t *n)
{
    fucker_t *f;
    f = container_of(n, fucker_t, node);

    printf("0x%d(h=%d) ", f->id, n->height);

    return 0;
}

avltree_t fuckertree;


#if 0
int main(int argc, char *argv[])
{
    uint32_t max = 1 << 21;
    int i;
    fucker_t *f;

    struct timeval stv, etv;
    double count, pass_ms, pass_s;

    //int arr [] = {3, 2, 1};
    //int arr [] = {1, 2, 3};
    //int arr [] = {8, 5, 6};
    int arr [] = {8, 10, 9, 12, 14, 80, 5, 60, 7};

    avltree_init(&fuckertree, fucker_cmp, fucker_del, fucker_travel);
    avltree_bfs(&fuckertree);
#if 0
    for (i = 0; i < sizeof(arr)/sizeof(int); i++) {
        f = malloc(sizeof(fucker_t));
        f->id = arr[i];
        printf("-----------------------------------------\n");
        printf("Insert: %d\n", f->id);
        free(avltree_insert(&fuckertree, &f->node));
        avltree_bfs(&fuckertree);
    }

    f = malloc(sizeof(fucker_t));

    //int arr1 [] = {12, 14, 9, 10};
    int arr1 [] = {12};
    for (i = 0; i < sizeof(arr1)/sizeof(int); i++) {
        printf("\n--------------DELETE---------------------------\n");
        f->id = arr1[i];
        f = avltree_delete(&fuckertree, &f->node);
        avltree_bfs(&fuckertree);

        if (f) {
            printf("DELETE:");
            fucker_travel(f);
        }
    }

#else


#if 0
    printf("-----------------------------------------\n");
    gettimeofday(&stv, NULL);
    for (i = 0; i < max; i++) {
        f = malloc(sizeof(fucker_t));
        f->id = i;
        avltree_insert(&fuckertree, &f->node);
    }
    gettimeofday(&etv, NULL);

    count = i;
    pass_ms = (etv.tv_sec * 1000 + etv.tv_usec / 1000) -
        (stv.tv_sec * 1000 + stv.tv_usec / 1000) + 0.00001;
    pass_s = pass_ms / 1000;
    printf("Total: %.1lf, Time: %.1lf, Speed: %.1lf\n", count, pass_s, count/pass_s);

#endif

    printf("-----------------------------------------\n");
    count = 0;
    gettimeofday(&stv, NULL);
    f = malloc(sizeof(fucker_t));
    for (i = 0; i < max; i++) {
        f->id = i;
        if (avltree_find(&fuckertree, &f->node) != NULL) {
            count++;
        }
    }
    free(f);
    gettimeofday(&etv, NULL);

    pass_ms = (etv.tv_sec * 1000 + etv.tv_usec / 1000) -
        (stv.tv_sec * 1000 + stv.tv_usec / 1000) + 0.00001;
    pass_s = pass_ms / 1000;
    printf("Total: %.1lf, Time: %.1lf, Speed: %.1lf\n", count, pass_s, count/pass_s);

    printf("-----------------------------------------\n");
    count = 0;
    gettimeofday(&stv, NULL);
    f = malloc(sizeof(fucker_t));
    for (i = 0; i < max; i++) {
        f->id = i;
        if (avltree_delete(&fuckertree, &f->node) != NULL) {
            count++;
        } else {
            //printf("delete: %x error\n", i);
        }
    }
    free(f);
    gettimeofday(&etv, NULL);

    pass_ms = (etv.tv_sec * 1000 + etv.tv_usec / 1000) -
        (stv.tv_sec * 1000 + stv.tv_usec / 1000) + 0.00001;
    pass_s = pass_ms / 1000;
    printf("Total: %.1lf, Time: %.1lf, Speed: %.1lf\n", count, pass_s, count/pass_s);
#endif


    printf("---------------after delete--------------------------\n");
    avltree_bfs(&fuckertree);

    avltree_destroy(&fuckertree);

    return 0;
}

#endif

#include "btree.h"

void test_btree_splite_child()
{
    btree_t tree;
    bnode_t root = {0};
    int i;

    for (i = 0; i < BTREE_MAX_KEY; i++) {
        root.key[i] = i;
    }

    root.leaf = true;
    root.n = BTREE_MAX_KEY;

    btree_init(&tree, NULL, NULL);
    tree.root = &root;

    printf("Init Root:\n");
    btree_bfs(&tree);

    printf("-------------------------------------------\n");
    bnode_t *pnode = calloc(1, sizeof(bnode_t));
    tree.root = pnode;

    pnode->child[0] = &root;
    pnode->n = 0;
    pnode->leaf = false;

    btree_splite_child(pnode, 0);

    btree_bfs(&tree);


}


void test_btree_insert()
{
    btree_t tree;
    int ok;
    int notok;
    int i;
    btree_init(&tree, NULL, NULL);

    int max = 50000000;
    int *data;

    data = calloc(max, sizeof(int));

    for (i = 0; i < max; i++) {
        data[i] = hashlittle(&i, sizeof(i), 0);
    }

    for (i = 0; i < max; i++) {
        btree_insert(&tree, data[i]);
        ok++;
    }
    printf("-------------------------------------------\n");

    bnode_t *retn;
    long idx;

    ok = 0;
    notok = 0;
    for (i = 0; i < max; i++) {
        btree_search(&tree, data[i], &retn, &idx);
        if (retn == NULL) {
            //printf("find error: %d\n", 2*i+1);
            notok++;
        } else {
            //printf("find %d : %d\n", i+1, retn->key[idx]);
            ok++;
        }
    }
    printf("Search after Insert: ok: %d, not ok: %d\n", ok, notok);
    printf("-------------------------------------------\n");


    for (i = 0; i < max; i++) {
        btree_delete(&tree, data[i]);
    }

    printf("-------------------------------------------\n");
    ok = 0;
    notok = 0;
    for (i = 0; i < max; i++) {
        btree_search(&tree, data[i], &retn, &idx);
        if (retn == NULL) {
            //printf("find error: %d\n", 2*i+1);
            notok++;
        } else {
            //printf("find %d : %d\n", i+1, retn->key[idx]);
            ok++;
        }
    }
    printf("Search after Delete: ok: %d, not ok: %d\n", ok, notok);
    printf("-------------------------------------------\n");
}

extern void test_rbt();
int test_skiplist(int argc, char *argv[]);
#include "skiplist.h"

int main(int argc, char *argv[])
{
    //test_btree_splite_child();
    //test_btree_insert();
    //test_rbt();
    test_skiplist(argc, argv);

    return 0;
}





























