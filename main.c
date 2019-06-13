#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRING_LENGTH 50

#define RED 0
#define BLACK 1

struct RB_node {
    char *key;
    int color;
    struct RB_node *parent;
    struct RB_node *left;
    struct RB_node *right;
};

struct RB_node *rb_root;


void addent(char *id_ent);
void delent(char *id_ent);
void addrel(char *id_orig, char *id_dest, char *id_rel);
void delrel(char *id_orig, char *id_dest, char *id_rel);
void report(void);

void rb_insert(struct RB_node *new);
void rb_insert_fixup(struct RB_node *new);
struct RB_node *rb_delete(struct RB_node *node);
void rb_delete_fixup(struct RB_node *x);
void left_rotate(struct RB_node *x);
void right_rotate(struct RB_node *x);
struct RB_node *tree_minimum(struct RB_node *node);
struct RB_node *tree_successor(struct RB_node *node);

void readLine(char **str);


int main(int argc, char *argv[])
{
    char *line = malloc(sizeof(char) * (DEFAULT_STRING_LENGTH + 1));
    
    char command[7];    
    char *id_ent  = NULL;
    char *id_orig = NULL;
    char *id_dest = NULL;
    char *id_rel  = NULL;
    
    size_t len;
    
    do {
        readLine(&line);
        sscanf(line, "%7s", command);
        
        len = strlen(line);
        
        if (strncmp(command, "addent", 7) == 0) {
            if (!id_ent) {
                id_ent = malloc(sizeof(char) * len);
            }
            sscanf(line, "%*s %s", id_ent);
            //printf("ADDENT %s\n", id_ent);
            
        } else if (strncmp(command, "delent", 7) == 0) {
            if (!id_ent) {
                id_ent = malloc(sizeof(char) * len);
            }
            sscanf(line, "%*s %s", id_ent);
            printf("DELENT %s\n", id_ent);
        } else if (strncmp(command, "addrel", 7) == 0) {
            if (!id_orig) {
                id_orig = malloc(sizeof(char) * len);
            }
            if (!id_dest) {
                id_dest = malloc(sizeof(char) * len);
            }
            if (!id_rel) {
                id_rel = malloc(sizeof(char) * len);
            }
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            printf("ADDREL %s %s %s\n", id_orig, id_dest, id_rel);
        } else if (strncmp(command, "delrel", 7) == 0) {
            if (!id_orig) {
                id_orig = malloc(sizeof(char) * len);
            }
            if (!id_dest) {
                id_dest = malloc(sizeof(char) * len);
            }
            if (!id_rel) {
                id_rel = malloc(sizeof(char) * len);
            }
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            printf("DELREL %s %s %s\n", id_orig, id_dest, id_rel);
        } else if (strncmp(command, "report", 7) == 0) {
            printf("REPORT\n");
        }
        printf("\n");
    } while (strncmp(command, "end", 4) != 0);
    
    free(line);
    free(id_ent);
    free(id_orig);
    free(id_dest);
    free(id_rel);

    return 0;
}


void addent(char *id_ent)
{
    
}

void delent(char *id_ent)
{

}

void addrel(char *id_orig, char *id_dest, char *id_rel)
{

}

void delrel(char *id_orig, char *id_dest, char *id_rel)
{

}

void report(void)
{

}

void rb_insert(struct RB_node *new)
{
    struct RB_node *x = rb_root;
    struct RB_node *y = NULL;
    
    while (x != NULL) {
        y = x;
        
        if (new->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    new->parent = y;
    
    if (y == NULL)
        rb_root = new; // albero vuoto
    else if (new->key < y->key)
        y->left = new;
    else
        y->right = new;
    
    new->left  = NULL;
    new->right = NULL;
    new->color = RED;  
    rb_insert_fixup(new);
}

void rb_insert_fixup(struct RB_node *new)
{
    struct RB_node *x, *y;
    
    if (new == rb_root) {
        rb_root->color = BLACK;
    } else {
        x = new->parent;
        if (x->color == RED) {
            if (x == x->parent->left) {
                y = x->parent->right;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(x->parent);
                } else if (new == x->right) {
                    new = x;
                    left_rotate(new);
                    x = new->parent;
                    
                    x->color = BLACK;
                    x->parent->color = RED;
                    right_rotate(x->parent);
                }
            } else {
                 y = x->parent->left;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(x->parent);
                } else if (new == x->left) {
                    new = x;
                    right_rotate(new);
                    x = new->parent;
                    
                    x->color = BLACK;
                    x->parent->color = RED;
                    left_rotate(x->parent);
                }
            }
        }
    }
}

struct RB_node *rb_delete(struct RB_node *node)
{
    struct RB_node *x, *y;
    
    if (node->left == NULL || node->right == NULL)
        y = node;
    else
        y = tree_successor(node);
    if (y->left != NULL)
        x = y->left;
    else
        x = y->right;
    
    x->parent = y->parent;
    if (y->parent == NULL)
        rb_root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    
    if (y != node)
        node->key = y->key;
    if (y->color == BLACK)
        rb_delete_fixup(x);
    return y;
}

void rb_delete_fixup(struct RB_node *x)
{
    struct RB_node *w;
    
    if (x->color == RED || x->parent == NULL) {
        x->color = BLACK;
    } else if (x == x->parent->left) {
        w = x->parent->right;
        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            left_rotate(x->parent);
            w = x->parent->right;
        } 
        if (w->left->color == BLACK && w->right->color == BLACK) {
            w->color = RED;
            rb_delete_fixup(x->parent);
        } else if (w->right->color == BLACK) {
            w->left->color = BLACK;
            w->color = RED;
            right_rotate(w);
            w = x->parent->right;
            
            w->color = x->parent->color;
            x->parent->color = BLACK;
            left_rotate(x->parent);
        }
    } else {
         w = x->parent->left;
        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            right_rotate(x->parent);
            w = x->parent->left;
        } 
        if (w->left->color == BLACK && w->right->color == BLACK) {
            w->color = RED;
            rb_delete_fixup(x->parent);
        } else if (w->left->color == BLACK) {
            w->right->color = BLACK;
            w->color = RED;
            left_rotate(w);
            w = x->parent->left;
            
            w->color = x->parent->color;
            x->parent->color = BLACK;
            right_rotate(x->parent);
        }
    }
}

void left_rotate(struct RB_node *x)
{
    struct RB_node *y = x->right;
    x->right = y->left;
    
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        rb_root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void right_rotate(struct RB_node *x)
{
    struct RB_node *y = x->left;
    x->left = y->right;
    
    if (y->right != NULL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        rb_root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

struct RB_node *tree_minimum(struct RB_node *node) 
{
    while (node->left != NULL)
        node = node->left;
    return node;
}

struct RB_node *tree_successor(struct RB_node *node)
{
    struct RB_node *y;
    
    if (node->right != NULL)
        return tree_minimum(node->right);
    y = node->parent;
    while (y != NULL && node == y->right) {
        node = y;
        y = y->parent;
    }
    return y;
}

void readLine(char **str)
{
    int ch, i = 0;
    
    while ((ch = getchar()) != '\n' && ch != EOF) {
        (*str)[i++] = ch;

        if (i % DEFAULT_STRING_LENGTH == 0) {
            *str = realloc(*str, sizeof(char) * (i + DEFAULT_STRING_LENGTH + 1));
        }
    }
    (*str)[i] = '\0';
}

