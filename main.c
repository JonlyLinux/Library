#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

#include "jhash.h"
#include "list.h"
#include "queue.h"
#include "avltree.h"

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

    printf("%d(h=%d) ", f->id, n->height);

    return 0;
}




avltree_t fuckertree;

int main(int argc, char *argv[])
{
    uint32_t max = 1 << 20;
    int i;
    fucker_t *f;

    struct timeval stv, etv;
    double count, pass_ms, pass_s;

    //int arr [] = {3, 2, 1};
    //int arr [] = {1, 2, 3};
    //int arr [] = {8, 5, 6};
    //int arr [] = {8, 10, 9};

    avltree_init(&fuckertree, fucker_cmp, fucker_del, fucker_travel);

#if 0
    for (i = 0; i < sizeof(arr)/sizeof(int); i++) {
        f = malloc(sizeof(fucker_t));
        f->id = arr[i];
        printf("-----------------------------------------\n");
        printf("Insert: %d\n", f->id);
        avltree_insert(&fuckertree, &f->node);
        avltree_bfs(&fuckertree);
    }
#else
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




    count = 0;
    gettimeofday(&stv, NULL);
    for (i = 0; i < max; i++) {
        f->id = i;
        if (avltree_find(&fuckertree, &f->node) != NULL) {
            count++;
        }
    }
    gettimeofday(&etv, NULL);

    pass_ms = (etv.tv_sec * 1000 + etv.tv_usec / 1000) -
        (stv.tv_sec * 1000 + stv.tv_usec / 1000) + 0.00001;
    pass_s = pass_ms / 1000;
    printf("Total: %.1lf, Time: %.1lf, Speed: %.1lf\n", count, pass_s, count/pass_s);


    avlnode_t *p;
    f->id = 0x12345;
    if ((p = avltree_find(&fuckertree, &f->node)) != NULL) {
        printf("p: %p, %d\n", p, p->height);
    }


    f = malloc(sizeof(fucker_t));
    f->id = 0x12345;
    if ((p = avltree_insert(&fuckertree, &f->node)) != NULL) {
        printf("p: %p, %d\n", p, p->height);
    }

    f->id = 0x12345;
    if ((p = avltree_find(&fuckertree, &f->node)) != NULL) {
        printf("p: %p, %d\n", p, p->height);
    }


#endif


    avltree_destroy(&fuckertree);
    return 0;
}



