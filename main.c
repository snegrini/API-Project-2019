#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRING_LENGTH   50

#define RED     0
#define BLACK   1

struct rb_node {
    char *key;
    int color;
    struct rb_node *nested;
    struct rb_node *parent;
    struct rb_node *left;
    struct rb_node *right;
};

struct list_node {
    char *id_ent;
    struct list_node *next;
};

/*
 * Global variables
 */
struct rb_node leaf;
struct rb_node *t_nil = &leaf;
int first_print = 1;
struct list_node *ent_list_head = NULL;
unsigned int max_size = 0;

void addent(struct rb_node **rb_root, char *id_ent);
void delent(struct rb_node **ent_rb_root, struct rb_node **rel_rb_root, char *id_ent);
void addrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel);
void delrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel);
void report(struct rb_node *rb_root);

/*
 * RB Tree functions
 */
int rb_insert(struct rb_node **rb_root, struct rb_node *new);
void rb_insert_fixup(struct rb_node **rb_root, struct rb_node *z);
struct rb_node *rb_delete(struct rb_node **rb_root, struct rb_node *node);
void rb_delete_fixup(struct rb_node **rb_root, struct rb_node *x);
struct rb_node *rb_search(struct rb_node **rb_root, char *key);
void rotate_left(struct rb_node **rb_root, struct rb_node *x);
void rotate_right(struct rb_node **rb_root, struct rb_node *x);
struct rb_node *tree_minimum(struct rb_node *node);
struct rb_node *tree_successor(struct rb_node *node);
struct rb_node *rb_create_insert_node(struct rb_node **rb_root, char *id_ent);
void rb_free(struct rb_node **rb_root, int clear_id_ent);
unsigned int rb_count_nodes(struct rb_node *rb_root);
void print_report(struct rb_node *rb_root);
void rb_visit_nested_inorder(struct rb_node *rb_root);
void rb_delete_ent_from_rel(struct rb_node **rel_rb_root, struct rb_node **curr_rb_root, char *id_ent);
void rb_delete_ent_from_rel_nested(struct rb_node **dest_rb_root, struct rb_node **curr_rb_root, char *id_ent);
void rb_print_tree(struct rb_node *rb_root);

/* 
 * List functions
 */
void list_node_insert(struct list_node **head, char *id_ent);
void list_node_free(struct list_node **head, int clear_id_ent);
void list_node_print(struct list_node *head);

/*
 * Input functions
 */
int readLine(char **str);

int main(int argc, char *argv[])
{
    char *line = NULL;
    
    char command[8];    
    char *id_ent  = NULL;
    char *id_orig = NULL;
    char *id_dest = NULL;
    char *id_rel  = NULL;
    
    int len;

    /* Set t_nil (leaf) color to BLACK */
    t_nil->color = BLACK;
    
    struct rb_node *ent_rb_root; /* Store all entities before relations are created. */
    struct rb_node *rel_rb_root;
        
    ent_rb_root = t_nil;
    rel_rb_root = t_nil;
    rel_rb_root->nested = t_nil;
    rel_rb_root->nested->nested = t_nil;
    
    do {
        line = malloc(sizeof(char) * (DEFAULT_STRING_LENGTH + 1));
        len = readLine(&line);
        sscanf(line, "%7s", command);
        //printf("%d\n", len);
                
        if (strncmp(command, "addent", 7) == 0) {
            id_ent = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s", id_ent);
            addent(&ent_rb_root, id_ent);
        } else if (strncmp(command, "delent", 7) == 0) {
            id_ent = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s", id_ent);
            delent(&ent_rb_root, &rel_rb_root, id_ent);
        } else if (strncmp(command, "addrel", 7) == 0) {
            id_orig = malloc(sizeof(char) * len);
            id_dest = malloc(sizeof(char) * len);
            id_rel = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            
            /* Verifico che le entità della relazione siano monitorate. */           
            if (rb_search(&ent_rb_root, id_orig) != t_nil && rb_search(&ent_rb_root, id_dest) != t_nil) {
                addrel(&rel_rb_root, id_orig, id_dest, id_rel);
            } else {
                free(id_orig);
                free(id_dest);
                free(id_rel);
            }         
        } else if (strncmp(command, "delrel", 7) == 0) {
            id_orig = malloc(sizeof(char) * len);
            id_dest = malloc(sizeof(char) * len);
            id_rel = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            delrel(&rel_rb_root, id_orig, id_dest, id_rel);
        } else if (strncmp(command, "report", 7) == 0) {
            report(rel_rb_root);
        }
        free(line);
    } while (strncmp(command, "end", 4) != 0);
    
    rb_free(&ent_rb_root, 1);
    rb_free(&rel_rb_root, 1);

    return 0;
}

void addent(struct rb_node **rb_root, char *id_ent)
{
    struct rb_node *new_node_ent;
    new_node_ent = malloc(sizeof(struct rb_node));
    new_node_ent->key = id_ent;
    new_node_ent->nested = t_nil;
    
    if (rb_insert(rb_root, new_node_ent) == 0) {
        /* Nessun inserimento, key già presente */
        free(id_ent);
        free(new_node_ent);
    }
}

void delent(struct rb_node **ent_rb_root, struct rb_node **rel_rb_root, char *id_ent)
{
    struct rb_node *node_tmp;
    
    node_tmp = rb_search(ent_rb_root, id_ent);
    if (node_tmp != t_nil) {
        free(node_tmp->key);
        node_tmp = rb_delete(ent_rb_root, node_tmp);
        free(node_tmp);
    }
    
    if (*rel_rb_root != t_nil)       
        rb_delete_ent_from_rel(rel_rb_root, rel_rb_root, id_ent);
    free(id_ent);
}

/*
 * Add a relation. If the relation is not existent, a new one is created.
 */
void addrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel)
{
    struct rb_node *node_rel;
    struct rb_node *node_ent;
    node_rel = rb_search(rb_root, id_rel);
    
    if (node_rel == t_nil) {
        node_rel = rb_create_insert_node(rb_root, id_rel);
        
        /* La relazione prima non esisteva. Sicuramente devo creare rb_dest e rb_orig */
        rb_create_insert_node(&node_rel->nested, id_dest);
        rb_create_insert_node(&node_rel->nested->nested, id_orig);
    } else {
        /* 
         * La relazione esiste.
         * Cerco se è già presente rb_dest dell'entità id_dest,
         * altrimenti la creo.
         */
        free(id_rel); /* Non viene utilizzato se la relazione esiste già. */
        node_ent = rb_search(&node_rel->nested, id_dest);
        
        if (node_ent == t_nil) {      
            /* 
             * Inserisco nuovo nodo per l'entità id_dest.
             */
            node_ent = rb_create_insert_node(&node_rel->nested, id_dest);
            /* 
             * Non essendoci rb_dest, non sarà presente neanche il nodo in rb_orig,
             * che va quindi creato.
             */
            rb_create_insert_node(&node_ent->nested, id_orig);
        } else {
            /*
             * Cerco, se esiste, il nodo dell'entità id_orig.
             * Se non esiste lo aggiungo nell'albero rb_orig.
             * Se esiste, libero lo spazio della stringa id_orig e non faccio altro.
             */
            free(id_dest); /* Il nodo rb_dest esiste già, non serve più */
            if (rb_search(&node_ent->nested, id_orig) == t_nil) {
                rb_create_insert_node(&node_ent->nested, id_orig);
            } else {
                free(id_orig);
            }
        }
    }
    
}

void delrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel)
{
    struct rb_node *node_rel, *node_dest, *node_orig;
    node_rel = rb_search(rb_root, id_rel);
    if (node_rel != t_nil) {
        node_dest = rb_search(&node_rel->nested, id_dest);
        if (node_dest != t_nil) {
            node_orig = rb_search(&node_dest->nested, id_orig);
            if (node_orig != t_nil) {
                free(node_orig->key);
                node_orig = rb_delete(&node_dest->nested, node_orig);
                free(node_orig);
                
                /* Se l'albero rb_orig è vuoto, elimino il nodo della entità destinazione */
                if (node_dest->nested == t_nil) {
                    free(node_dest->key);
                    node_dest = rb_delete(&node_rel->nested, node_dest);
                    free(node_dest);
                    
                    /* Se l'albero rb_dest è vuoto, elimino il nodo della relazione */
                    if (node_rel->nested == t_nil) {
                        free(node_rel->key);
                        node_rel = rb_delete(rb_root, node_rel);
                        free(node_rel);
                    }
                }
            }
        }
    }
    free(id_orig);
    free(id_dest);
    free(id_rel);
}

void report(struct rb_node *rb_root)
{
    if (rb_root == t_nil) {
        printf("none");
    } else {
        print_report(rb_root);
        if (first_print == 1) {
            printf("none");
        }
    }
    printf("\n");
    first_print = 1;
}

/*
 * @return:
 * 0 nessun inserimento, key già presente
 * 1 nuovo inserimento
 */
int rb_insert(struct rb_node **rb_root, struct rb_node *new)
{
    struct rb_node *x = *rb_root;
    struct rb_node *y = t_nil;
    
    while (x != t_nil) {
        y = x;
        
        if (strcmp(new->key, x->key) < 0)
            x = x->left;
        else if (strcmp(new->key, x->key) > 0)
            x = x->right;
        else
            return 0; // Avoid duplicates
    }
    new->parent = y;
    
    if (y == t_nil)
        *rb_root = new; // Empty tree
    else if (strcmp(new->key, y->key) < 0)
        y->left = new;
    else
        y->right = new;

    new->left  = t_nil;
    new->right = t_nil;
    new->color = RED;
    rb_insert_fixup(rb_root, new);

    return 1;
}

void rb_insert_fixup(struct rb_node **rb_root, struct rb_node *z)
{
    struct rb_node *x, *y;
    
    if (z == *rb_root) {
        (*rb_root)->color = BLACK;
    } else {
        x = z->parent;
        if (x->color == RED) {
            if (x == x->parent->left) {
                y = x->parent->right;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(rb_root, x->parent);
                } else {
                    if (z == x->right) {
                        z = x;
                        rotate_left(rb_root, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    rotate_right(rb_root, x->parent);
                }
            } else {
                 y = x->parent->left;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(rb_root, x->parent);
                } else {
                    if (z == x->left) {
                        z = x;
                        rotate_right(rb_root, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    rotate_left(rb_root, x->parent);
                }
            }
        }
    }
}

struct rb_node *rb_delete(struct rb_node **rb_root, struct rb_node *node)
{
    struct rb_node *x, *y;
    
    if (node->left == t_nil || node->right == t_nil)
        y = node;
    else
        y = tree_successor(node);
    if (y->left != t_nil)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;
    if (y->parent == t_nil)
        *rb_root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    
    if (y != node) {
        node->key = y->key;
        node->nested = y->nested;
    }
    if (y->color == BLACK)
        rb_delete_fixup(rb_root, x);
    return y;
}

void rb_delete_fixup(struct rb_node **rb_root, struct rb_node *x)
{
    struct rb_node *w;
    
    if (x->color == RED || x->parent == t_nil) {
        x->color = BLACK;
    } else if (x == x->parent->left) {
        w = x->parent->right;
        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            rotate_left(rb_root, x->parent);
            w = x->parent->right;
        } 
        if (w->left->color == BLACK && w->right->color == BLACK) {
            w->color = RED;
            rb_delete_fixup(rb_root, x->parent);
        } else {
            if (w->right->color == BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                rotate_right(rb_root, w);
                w = x->parent->right;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            w->right->color = BLACK;
            rotate_left(rb_root, x->parent);
        }
    } else {
         w = x->parent->left;
        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            rotate_right(rb_root, x->parent);
            w = x->parent->left;
        } 
        if (w->right->color == BLACK && w->left->color == BLACK) {
            w->color = RED;
            rb_delete_fixup(rb_root, x->parent);
        } else {
            if (w->left->color == BLACK) {
                w->right->color = BLACK;
                w->color = RED;
                rotate_left(rb_root, w);
                w = x->parent->left;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            w->left->color = BLACK;
            rotate_right(rb_root, x->parent);
        }
        
    }
}

struct rb_node *rb_search(struct rb_node **rb_root, char *key)
{
    if (*rb_root == t_nil || strcmp(key, (*rb_root)->key) == 0)
        return *rb_root;
    if (strcmp(key, (*rb_root)->key) < 0)
        return rb_search(&(*rb_root)->left, key);
    else
        return rb_search(&(*rb_root)->right, key);
}

void rotate_left(struct rb_node **rb_root, struct rb_node *x)
{
    struct rb_node *y = x->right;
    x->right = y->left;
    
    if (y->left != t_nil)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == t_nil)
        *rb_root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rotate_right(struct rb_node **rb_root, struct rb_node *x)
{
    struct rb_node *y = x->left;
    x->left = y->right;
    
    if (y->right != t_nil)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == t_nil)
        *rb_root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

struct rb_node *tree_minimum(struct rb_node *node) 
{
    while (node->left != t_nil)
        node = node->left;
    return node;
}

struct rb_node *tree_successor(struct rb_node *node)
{
    struct rb_node *y;
    
    if (node->right != t_nil)
        return tree_minimum(node->right);
    y = node->parent;
    while (y != t_nil && node == y->right) {
        node = y;
        y = y->parent;
    }
    return y;
}

struct rb_node *rb_create_insert_node(struct rb_node **rb_root, char *id_ent)
{
    struct rb_node *node_ent = malloc(sizeof(struct rb_node));
    node_ent->key = id_ent;
    node_ent->nested = t_nil;
    rb_insert(rb_root, node_ent);
    return node_ent;
}

void rb_free(struct rb_node **rb_root, int clear_id_ent)
{
    if (*rb_root == t_nil)
        return;
    
    rb_free(&(*rb_root)->left, clear_id_ent);
    rb_free(&(*rb_root)->right, clear_id_ent);
    if ((*rb_root)->nested != t_nil)
        rb_free(&(*rb_root)->nested, clear_id_ent);

    if (clear_id_ent == 1)
        free((*rb_root)->key);
    free(*rb_root);
    *rb_root = t_nil;
}

unsigned int rb_count_nodes(struct rb_node *rb_root)
{
    if (rb_root == t_nil)
        return 0;
    return 1 + rb_count_nodes(rb_root->left) + rb_count_nodes(rb_root->right);
}

void print_report(struct rb_node *rb_root)
{        
    if (rb_root == t_nil)
        return;
    
    print_report(rb_root->left);
    
    rb_visit_nested_inorder(rb_root->nested);
    
    if (max_size != 0) {
        if (first_print == 1) {
            printf("%s", rb_root->key);
            first_print = 0;
        } else {
            printf(" %s", rb_root->key);
        }
        
        list_node_print(ent_list_head);
        printf(" %d;", max_size);
        list_node_free(&ent_list_head, 0);
        max_size = 0;
    }
    print_report(rb_root->right);
}

void rb_visit_nested_inorder(struct rb_node *rb_root)
{
    unsigned int curr_size = 0;
    
    if (rb_root == t_nil)
        return;
    
    rb_visit_nested_inorder(rb_root->right);
    
    curr_size = rb_count_nodes(rb_root->nested);
    if (curr_size != 0) {
        /* Se ho un nuovo max_size, azzero la lista */
        if (curr_size == max_size) {
            list_node_insert(&ent_list_head, rb_root->key);
        } else if (curr_size > max_size) {
            list_node_free(&ent_list_head, 0);
            list_node_insert(&ent_list_head, rb_root->key);
            max_size = curr_size;
        }
    }
    
    rb_visit_nested_inorder(rb_root->left);
}

void rb_delete_ent_from_rel(struct rb_node **rel_rb_root,
                            struct rb_node **curr_rb_root, char *id_ent)
{
    struct rb_node *node_tmp;

    if (*curr_rb_root != t_nil) {
        rb_delete_ent_from_rel(rel_rb_root, &(*curr_rb_root)->left, id_ent);
        rb_delete_ent_from_rel(rel_rb_root, &(*curr_rb_root)->right, id_ent);
        /* 
         * Cerco l'entità nell'albero rb_dest di ogni relazione,
         * se la trovo la elimino ed elimino anche l'albero rb_orig associato.
         */
        node_tmp = rb_search(&(*curr_rb_root)->nested, id_ent);
        if (node_tmp != t_nil) {
            free(node_tmp->key);
            rb_free(&node_tmp->nested, 1);
            node_tmp = rb_delete(&(*curr_rb_root)->nested, node_tmp);
            free(node_tmp);
        }
        
        /* Cerco l'entità nell'albero rb_orig di ogni rb_dest */
        rb_delete_ent_from_rel_nested(&(*curr_rb_root)->nested,
                                      &(*curr_rb_root)->nested, id_ent);
        
        /* Se l'albero rb_dest è vuoto, elimino il nodo della relazione */
        /*if ((*curr_rb_root)->nested == t_nil) {
            free((*curr_rb_root)->key);
            node_tmp = rb_delete(rel_rb_root, *curr_rb_root);
            free(node_tmp);
        }
        
        if (*curr_rb_root != t_nil) {
            rb_delete_ent_from_rel(rel_rb_root, &(*curr_rb_root)->left, id_ent);
            rb_delete_ent_from_rel(rel_rb_root, &(*curr_rb_root)->right, id_ent);
        }*/
    }
}

void rb_delete_ent_from_rel_nested(struct rb_node **dest_rb_root, 
                                   struct rb_node **curr_rb_root, char *id_ent)
{
    struct rb_node *node_tmp;
    if (*curr_rb_root != t_nil) {
        rb_delete_ent_from_rel_nested(dest_rb_root, &(*curr_rb_root)->left, id_ent);
        rb_delete_ent_from_rel_nested(dest_rb_root, &(*curr_rb_root)->right, id_ent);

        /* Cerco l'entità nell'albero rb_orig, se la trovo la elimino */
        node_tmp = rb_search(&(*curr_rb_root)->nested, id_ent);
        if (node_tmp != t_nil) {
            free(node_tmp->key);
            node_tmp = rb_delete(&(*curr_rb_root)->nested, node_tmp);
            free(node_tmp);
        }
        
        /* Se l'albero rb_orig è vuoto, elimino il nodo della entità destinazione */
        /*if ((*curr_rb_root)->nested == t_nil) {
            free((*curr_rb_root)->key);
            node_tmp = rb_delete(dest_rb_root, *curr_rb_root);
            free(node_tmp);
        }
        
        if (*curr_rb_root != t_nil) {
            rb_delete_ent_from_rel_nested(dest_rb_root, &(*curr_rb_root)->left, id_ent);
            rb_delete_ent_from_rel_nested(dest_rb_root, &(*curr_rb_root)->right, id_ent);
        }*/
    }
}

/* TODO: remove*/
void rb_print_tree(struct rb_node *rb_root)
{
    if (rb_root != t_nil) {
        printf("%s, %d\n", rb_root->key, rb_root->color);
        rb_print_tree(rb_root->left);
        rb_print_tree(rb_root->right);
    } else {
        printf("t_nil\n");
    }
}


void list_node_insert(struct list_node **head, char *id_ent)
{
    struct list_node *new_node = malloc(sizeof(struct list_node));
    new_node->id_ent = id_ent;
    new_node->next = *head;
    *head = new_node;
}

void list_node_free(struct list_node **head, int clear_id_ent)
{
    struct list_node *curr;
    while (*head != NULL) {
        curr = *head;
        *head = (*head)->next;
        if (clear_id_ent == 1) {
            free(curr->id_ent);
        }
        free(curr);
    }
}

void list_node_print(struct list_node *head)
{
    struct list_node *curr = head;
    while (curr != NULL) {
        printf(" %s", curr->id_ent);
        curr = curr->next;
    }
}

int readLine(char **str)
{
    int ch, i = 0;
    
    while ((ch = getchar()) != '\n' && ch != EOF) {
        (*str)[i++] = ch;

        if (i % DEFAULT_STRING_LENGTH == 0) {
            *str = realloc(*str, sizeof(char) * (i + DEFAULT_STRING_LENGTH + 1));
        }
    }
    (*str)[i] = '\0';
    return i;
}
