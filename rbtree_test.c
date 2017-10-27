#include <stdio.h>
#include "rbtree.h"

int main() {
    struct rbtree_t *tree = new_rbtree();
    int i;
    struct rbtree_node_t *node = NULL;
    if(!tree) {
        printf("cannot create rbtree\n");
        return 1;
    }

    for(i = 0; i < 10; i++) {
        rbtree_insert(tree, i);
    }

    rbtree_display(tree);
    printf("---------------\n");

    rbtree_delete(tree, rbtree_find(tree, 4));
    rbtree_display(tree);
    printf("---------------\n");

    rbtree_delete(tree, rbtree_find(tree, 9));
    rbtree_display(tree);
    printf("---------------\n");
    
    free_rbtree(tree);
    return 0;
}
