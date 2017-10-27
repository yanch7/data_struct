#ifndef __RBTREE_H_
#define __RBTREE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct rbtree_node_t {
    int value;
    struct rbtree_node_t *parent;
    struct rbtree_node_t *left;
    struct rbtree_node_t *right;
    int color;
};

struct rbtree_t {
    struct rbtree_node_t *root;
};

struct rbtree_t* new_rbtree();
void free_rbtree(struct rbtree_t *tree);

struct rbtree_node_t* new_rbtree_node(int v);
void free_rbtree_node(struct rbtree_node_t *node);

struct rbtree_node_t* rbtree_insert(struct rbtree_t *tree, int v);
struct rbtree_node_t* rbtree_find(struct rbtree_t *tree, int v);
void rbtree_delete(struct rbtree_t *tree, struct rbtree_node_t *n);
void rbtree_display(struct rbtree_t *tree);

#ifdef __cplusplus
}
#endif

#endif