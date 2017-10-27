#include <stdlib.h>
#include <memory.h>
#include "rbtree.h"

#define RED 0
#define BLACK 1

#define COLOR_OF(node) (((node) == NULL)? BLACK : (node)->color)
#define LEFT_OF(node) (((node) == NULL)? NULL : (node)->left)
#define RIGHT_OF(node) (((node) == NULL)? NULL : (node->right))
#define PARENT_OF(node) (((node) == NULL)? NULL : (node->parent))

static void destroy_rbtree(struct rbtree_node_t *root);
static void set_color(struct rbtree_node_t *node, int color);
static void rotate_left(struct rbtree_t *tree, struct rbtree_node_t *n);
static void rotate_right(struct rbtree_t *tree, struct rbtree_node_t *n);
static void fix_after_insertion(struct rbtree_t *tree, struct rbtree_node_t *n);
static struct rbtree_node_t* successor(struct rbtree_t *tree, struct rbtree_node_t *n);
static void fix_after_deletion(struct rbtree_t *tree, struct rbtree_node_t *n);

struct rbtree_t* new_rbtree() {
    struct rbtree_t *tree = (struct rbtree_t *)malloc(sizeof(struct rbtree_t));
    if(!tree) return NULL;
    memset(tree, 0, sizeof(struct rbtree_t));
    tree->root = NULL;
    return tree;
}

void free_rbtree(struct rbtree_t *tree) {
    if(tree) {
        destroy_rbtree(tree->root);
        free(tree);
    }
}

struct rbtree_node_t* new_rbtree_node(int v) {
    struct rbtree_node_t *node = (struct rbtree_node_t*)malloc(sizeof(struct rbtree_node_t));
    if(!node) return NULL;
    memset(node, 0, sizeof(struct rbtree_node_t));
    node->value = v;
    node->parent = node->left = node->right = NULL;
    node->color = RED;
    return node;
}

void free_rbtree_node(struct rbtree_node_t *node) {
    if(node) {
        if(node->left || node->right) {
            return;
        }

        free(node);
    }
}

void destroy_rbtree(struct rbtree_node_t *root) {
    if(root) {
        destroy_rbtree(root->left);
        destroy_rbtree(root->right);
        free_rbtree_node(root);
    }
}

void set_color(struct rbtree_node_t *node, int color) {
    if(node) node->color = color;
}

void rotate_left(struct rbtree_t *tree, struct rbtree_node_t *n) {
    if(tree && n) {
        struct rbtree_node_t *r = n->right;
        if(!r) return;
        n->right = r->left;
        if(r->left) r->left->parent = n;

        r->parent = n->parent;
        if(n->parent == NULL) {
            tree->root = r;
        }
        else {
            if(LEFT_OF(PARENT_OF(n)) == n) {
                n->parent->left = r;
            }
            else {
                n->parent->right = r;
            }
        }

        r->left = n;
        n->parent = r;
    }
}

void rotate_right(struct rbtree_t *tree, struct rbtree_node_t *n) {
    if(tree && n) {
        struct rbtree_node_t *l = n->left;
        if(!l) return;

        n->left = l->right;
        if(l->right) l->right->parent = n;

        l->parent = n->parent;
        if(n->parent == NULL) {
            tree->root = l;
        }
        else {
            if(LEFT_OF(PARENT_OF(n)) == n) {
                n->parent->left = l;
            }
            else {
                n->parent->right = l;
            }
        }

        l->right = n;
        n->parent = l;
    }
}

struct rbtree_node_t* rbtree_insert(struct rbtree_t *tree, int v) {
    if(tree) {
        struct rbtree_node_t *node;
        if(tree->root == NULL) {
            node = new_rbtree_node(v);
            if(!node) return NULL;
            tree->root = node;
        }
        else {
            struct rbtree_node_t *parent, *ptr = tree->root;
            do {
                parent = ptr;
                if(v < ptr->value) {
                    ptr = ptr->left;
                }
                else if(v > ptr->value) {
                    ptr = ptr->right;
                }
                else return ptr;
            } while(ptr);

            node = new_rbtree_node(v);
            if(!node) return NULL;

            if(v > parent->value) {
                parent->right = node;
            } else {
                parent->left = node;
            }
            node->parent = parent;
        }

        fix_after_insertion(tree, node);
        return node;
    }

    return NULL;
}

void fix_after_insertion(struct rbtree_t *tree, struct rbtree_node_t *n) {
    if(tree && n) {
        set_color(n, RED);
        while(n && n != tree->root && COLOR_OF(PARENT_OF(n)) == RED) {
            struct rbtree_node_t *uncle = NULL;
            if(LEFT_OF(PARENT_OF(PARENT_OF(n))) == PARENT_OF(n)) {
                uncle = RIGHT_OF(PARENT_OF(PARENT_OF(n)));
                if(RIGHT_OF(PARENT_OF(n)) == n) {
                    rotate_left(tree, PARENT_OF(n));
                    n = LEFT_OF(n);
                }

                if(COLOR_OF(uncle) == BLACK) {
                    set_color(PARENT_OF(n), BLACK);
                    set_color(PARENT_OF(PARENT_OF(n)), RED);
                    rotate_right(tree, PARENT_OF(PARENT_OF(n)));
                } else {
                    set_color(uncle, BLACK);
                    set_color(PARENT_OF(n), BLACK);
                    set_color(PARENT_OF(PARENT_OF(n)), RED);
                    n = PARENT_OF(PARENT_OF(n));
                }
            }
            else {
                uncle = LEFT_OF(PARENT_OF(PARENT_OF(n)));
                if(LEFT_OF(PARENT_OF(n)) == n) {
                    rotate_right(tree, PARENT_OF(n));
                    n = RIGHT_OF(n);
                }

                if(COLOR_OF(uncle) == BLACK) {
                    set_color(PARENT_OF(n), BLACK);
                    set_color(PARENT_OF(PARENT_OF(n)), RED);
                    rotate_left(tree, PARENT_OF(PARENT_OF(n)));
                } else {
                    set_color(PARENT_OF(n), BLACK);
                    set_color(uncle, BLACK);
                    set_color(PARENT_OF(PARENT_OF(n)), RED);
                    n = PARENT_OF(PARENT_OF(n));
                }
            }
        }

        set_color(tree->root, BLACK);
    }
}

struct rbtree_node_t* rbtree_find(struct rbtree_t *tree, int v) {
    if(tree) {
        struct rbtree_node_t *ptr = tree->root;
        while(ptr) {
            if(ptr->value == v) return ptr;
            else if(v < ptr->value) ptr = ptr->left;
            else ptr = ptr->right;
        }
    }

    return NULL;
}

void rbtree_delete(struct rbtree_t *tree, struct rbtree_node_t *n) {
    if(tree && n) {
        struct rbtree_node_t *replacement;
        if(LEFT_OF(n) && RIGHT_OF(n)) {
            replacement = successor(tree, n);
            n->value = replacement->value;
            n = replacement;
        }

        replacement = (LEFT_OF(n) == NULL)? RIGHT_OF(n) : LEFT_OF(n);
        if(replacement == NULL) {
            if(n->color == BLACK) {
                fix_after_deletion(tree, n);
            }

            if(PARENT_OF(n)) {
                if(LEFT_OF(PARENT_OF(n)) == n) {
                    n->parent->left = NULL;
                }
                else {
                    n->parent->right = NULL;
                }                
            }
            else {
                tree->root = NULL;
            }
        }
        else {
            replacement->parent = PARENT_OF(n);
            if(PARENT_OF(n) == NULL) {
                tree->root = replacement;
            } else {
                if(LEFT_OF(PARENT_OF(n)) == n) {
                    n->parent->left = replacement;
                }
                else {
                    n->parent->right = replacement;
                }
            }

            if(n->color == BLACK) {
                fix_after_deletion(tree, replacement);
            }
        }

        n->left = n->right = n->parent = NULL;
        free_rbtree_node(n);
    }
}

struct rbtree_node_t* successor(struct rbtree_t *tree, struct rbtree_node_t *n) {
    if(tree && n) {
        if(RIGHT_OF(n)) {
            for(n = n->right; LEFT_OF(n); n = LEFT_OF(n));
            return n;
        }
        else {
            struct rbtree_node_t *p;
            for(p = PARENT_OF(n); p != NULL && RIGHT_OF(p) == n; n = p, p = PARENT_OF(n));
            return p;
        }
    }

    return NULL;
}

void fix_after_deletion(struct rbtree_t *tree, struct rbtree_node_t *n) {
    if(tree && n) {
        struct rbtree_node_t *bro;
        while(n && n != tree->root && COLOR_OF(n) == BLACK) {
            if(LEFT_OF(PARENT_OF(n)) == n) {
                bro = RIGHT_OF(PARENT_OF(n));
                if(COLOR_OF(bro) == RED) {
                    set_color(PARENT_OF(n), RED);
                    set_color(bro, BLACK);
                    rotate_left(tree, PARENT_OF(bro));
                    bro = RIGHT_OF(PARENT_OF(n));
                }

                if(COLOR_OF(RIGHT_OF(bro)) == BLACK && COLOR_OF(LEFT_OF(bro)) == BLACK) {
                    set_color(bro, RED);
                    n = n->parent;
                }
                else {
                    if(COLOR_OF(RIGHT_OF(bro)) == BLACK) {
                        set_color(bro, RED);
                        set_color(LEFT_OF(bro), BLACK);
                        rotate_right(tree, bro);
                        bro = RIGHT_OF(PARENT_OF(n));
                    }

                    set_color(bro, COLOR_OF(PARENT_OF(bro)));
                    set_color(PARENT_OF(bro), BLACK);
                    set_color(RIGHT_OF(bro), BLACK);
                    rotate_left(tree, PARENT_OF(n));
                }
            }
            else {
                bro = LEFT_OF(PARENT_OF(n));
                if(COLOR_OF(bro) == RED) {
                    set_color(bro, BLACK);
                    set_color(PARENT_OF(bro), RED);
                    rotate_right(tree, PARENT_OF(bro));
                    bro = LEFT_OF(PARENT_OF(n));
                }

                if(COLOR_OF(LEFT_OF(bro)) == BLACK && COLOR_OF(RIGHT_OF(bro)) == BLACK) {
                    set_color(bro, RED);
                    n = PARENT_OF(n);
                }
                else {
                    if(COLOR_OF(LEFT_OF(bro)) == BLACK) {
                        set_color(bro, RED);
                        set_color(RIGHT_OF(bro), BLACK);
                        rotate_left(tree, bro);
                        bro = LEFT_OF(PARENT_OF(n));
                    }

                    set_color(bro, COLOR_OF(PARENT_OF(n)));
                    set_color(PARENT_OF(n), BLACK);
                    set_color(LEFT_OF(bro), BLACK);
                    rotate_right(tree, PARENT_OF(n));
                }
            }
        }

        set_color(n, BLACK);
    }
}

void rbtree_node_display(struct rbtree_node_t *node) {
    if(node) {
        printf("val: %d, parent: %d, color: %s", node->value, (node->parent == NULL)? -1 : node->parent->value, (node->color == RED)? "red" : "black");
        if(node->parent) {
            if(LEFT_OF(PARENT_OF(node)) == node) {
                printf(",LEFT");
            } 
            else printf(",RIGHT");
        }
        printf("\n");

        rbtree_node_display(node->left);
        rbtree_node_display(node->right);
    }
}

void rbtree_display(struct rbtree_t *tree) {
    if(tree) {
        rbtree_node_display(tree->root);
    }
}