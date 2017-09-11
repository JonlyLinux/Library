#include "rbt.h"



void swap(int *a, int *b)
{
    int p = *a;
    *a = *b;
    *b = p;
}

void test_rbt()
{
    rbt_tree_t rbt;
    rbt_node_t *n;
    rbt_node_t tmp;

    int ret;
    int i;

    rbt_init(&rbt, rbt_cmp_func, free, NULL, NULL);

    int size = 10000;
    int *data = calloc(size, sizeof(int));
    
    for (i = 0; i < size; i++)
        data[i] = hashlittle(&i, sizeof(int), 0) % 10000;

    for (i = 0; i < size; i++) {
        n = calloc(1, sizeof(rbt_node_t));
        n->key = data[i];

        rbt_insert(&rbt, n);
        ret = rbt_check(&rbt);
        if (ret != 0) {
            printf("Delete error. key = %d, ret = %d\n", n->key, ret);
        }
    }

    for (i = 0; i < size; i++) {
        tmp.key = data[i];
        n = rbt_find(&rbt, &tmp);

        if (n != rbt_end(&rbt)) {
            rbt_delete(&rbt, n);
        } else {
            printf("Find %d error\n", data[i]);
        }
        
        ret = rbt_check(&rbt);
        if (ret != 0) {
            printf("Delete error. key = %d, ret = %d\n", n->key, ret);
        }
    }

    ret = rbt_check(&rbt);
    printf("check ret = %d, size = %d\n", ret, rbt.size);

    rbt_destroy(&rbt);
}

