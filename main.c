#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_GRAPH_SIZE      50
#define DEFAULT_STRING_LENGTH   50

#define RED     0
#define BLACK   1

struct adj_list_node {
    char *vertex;
    struct adj_list_node *next;
};

struct adj_list {
    char *name;
    unsigned int size;
    struct adj_list *next_list;
    struct adj_list_node *node;
};

struct graph {
    int vertices_num;
    struct adj_list *adj_list_ent;
};

struct rb_node {
    char *key;
    int color;
    struct graph *ent_graph;
    struct rb_node *parent;
    struct rb_node *left;
    struct rb_node *right;
};

void addent(struct rb_node *rb_root, char *id_ent);
void delent(struct rb_node *rb_root, char *id_ent);
void addrel(struct rb_node *rb_root, char *id_orig, char *id_dest, char *id_rel);
void delrel(struct rb_node *rb_root, char *id_orig, char *id_dest, char *id_rel);
void report(void);

/*
 * RB Tree functions
 */
void rb_insert(struct rb_node *rb_root, struct rb_node *new);
void rb_insert_fixup(struct rb_node *rb_root, struct rb_node *new);
struct rb_node *rb_delete(struct rb_node *rb_root, struct rb_node *node);
void rb_delete_fixup(struct rb_node *rb_root, struct rb_node *x);
struct rb_node *rb_search(struct rb_node *rb_root, char *key);
void rotate_left(struct rb_node *rb_root, struct rb_node *x);
void rotate_right(struct rb_node *rb_root, struct rb_node *x);
struct rb_node *tree_minimum(struct rb_node *node);
struct rb_node *tree_successor(struct rb_node *node);
void rb_free(struct rb_node *rb_root);

/*
 * Graph functions
 */

/*
 * Input functions
 */
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
    
    struct rb_node *ent_rb_root; // Store all entities before relations are created
    struct rb_node *rel_rb_root;
        
    ent_rb_root = malloc(sizeof(struct rb_node));
    rel_rb_root = malloc(sizeof(struct rb_node));
    
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
            addent(ent_rb_root, id_ent);
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
    
    free(ent_rb_root);
    free(rel_rb_root);
    
    return 0;
}


void addent(struct rb_node *rb_root, char *id_ent)
{
    struct rb_node *new_node_ent;
    
    new_node_ent = malloc(sizeof(struct rb_node));
    new_node_ent->key = id_ent;
    rb_insert(rb_root, new_node_ent);
}

void delent(struct rb_node *rb_root, char *id_ent)
{
    struct rb_node *node_ent;
    
    node_ent = rb_search(rb_root, id_ent);
    rb_delete(rb_root, node_ent);
    free(node_ent);
}

/*
 * Add a relation. If the relation is not existent, a new one is created.
 */
void addrel(struct rb_node *rb_root, char *id_orig, char *id_dest, char *id_rel)
{
    struct rb_node *node_rel;
    struct adj_list_node *new_node_ent;
    node_rel = rb_search(rb_root, id_rel);
    
    if (node_rel == NULL) {
        node_rel = malloc(sizeof(struct rb_node));
        node_rel->key = id_rel;
        
        // La relazione prima non esisteva. Sicuramente devo creare il grafo e le liste di adiacenza
        node_rel->ent_graph = malloc(sizeof(struct graph));
        node_rel->ent_graph->vertices_num = DEFAULT_GRAPH_SIZE;
        node_rel->ent_graph->adj_list_ent = malloc(sizeof(struct adj_list) * node_rel->ent_graph->vertices_num);
        
        node_rel->ent_graph->adj_list_ent->name = id_orig;
        node_rel->ent_graph->adj_list_ent->next_list = NULL;
        
        new_node_ent = malloc(sizeof(struct adj_list_node));
        new_node_ent->vertex = id_dest;
        new_node_ent->next = NULL;
        
        node_rel->ent_graph->adj_list_ent->node = new_node_ent;
        node_rel->ent_graph->adj_list_ent->size = node_rel->ent_graph->adj_list_ent->size + 1; 
        
        rb_insert(rb_root, node_rel);
    } else {
        /* 
         * TODO: implementare un metodo per cercare una lista di adiacenza esistente e aggiungere quindi
         * un nodo.
         * 
         * TODO: Se la lista di adiacenza non esiste, controllare che ci sia abbastanza spazio per aggiungere 
         * una nuova liste di adiacenza.
         * Effettuare una realloc delle liste di adiacenza per aumentare lo spazio.
         * Come controllare lo spazio rimasto? Aggiungendo un contatore nel grafo oltre a vertices_num (questo
         * indica il numero massimo di vertici per cui è stato stanziato il grafo all'inizio)?
         * 
         * TODO: i nodi delle liste di adiacenza vengono allocati singolarmente? Cioè per ogni nuovo nodo 
         * viene effettuata una malloc e vengono collegati i puntatori alla lista.
         * Controllareche l'inserimento del nodo all'interno della lista di adiacenza venga effettuato in testa,
         * in modo da avere complessità temporale O(1) (costante).
         * 
         * TODO: vedere se è possibile snellire il codice della funzione addrel(), magari mettendo in comune
         * del codice dell'if-else.
         */
    }
    
}

void delrel(struct rb_node *rb_root, char *id_orig, char *id_dest, char *id_rel)
{

}

void report(void)
{

}

void rb_insert(struct rb_node *rb_root, struct rb_node *new)
{
    struct rb_node *x = rb_root;
    struct rb_node *y = NULL;
    
    while (x != NULL) {
        y = x;
        
        if (strcmp(new->key, x->key) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    new->parent = y;
    
    if (y == NULL)
        rb_root = new; // empty tree
    else if (strcmp(new->key, y->key) < 0)
        y->left = new;
    else
        y->right = new;
    
    new->left  = NULL;
    new->right = NULL;
    new->color = RED;  
    rb_insert_fixup(rb_root, new);
}

void rb_insert_fixup(struct rb_node *rb_root, struct rb_node *new)
{
    struct rb_node *x, *y;
    
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
                    rb_insert_fixup(rb_root, x->parent);
                } else {
                    if (new == x->right) {
                        new = x;
                        rotate_left(rb_root, new);
                        x = new->parent;
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
                    if (new == x->left) {
                        new = x;
                        rotate_right(rb_root, new);
                        x = new->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    rotate_left(rb_root, x->parent);
                }
            }
        }
    }
}

struct rb_node *rb_delete(struct rb_node *rb_root, struct rb_node *node)
{
    struct rb_node *x, *y;
    
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
        rb_delete_fixup(rb_root, x);
    return y;
}

void rb_delete_fixup(struct rb_node *rb_root, struct rb_node *x)
{
    struct rb_node *w;
    
    if (x->color == RED || x->parent == NULL) {
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
        if (w->left->color == BLACK && w->right->color == BLACK) {
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
            rotate_right(rb_root, x->parent);
        }
        
    }
}

struct rb_node *rb_search(struct rb_node *rb_root, char *key)
{
    if (rb_root == NULL || strcmp(key, rb_root->key) == 0)
        return rb_root;
    if (strcmp(key, rb_root->key) < 0)
        return rb_search(rb_root->left, key);
    else
        return rb_search(rb_root->right, key);
}

void rotate_left(struct rb_node *rb_root, struct rb_node *x)
{
    struct rb_node *y = x->right;
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

void rotate_right(struct rb_node *rb_root, struct rb_node *x)
{
    struct rb_node *y = x->left;
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

struct rb_node *tree_minimum(struct rb_node *node) 
{
    while (node->left != NULL)
        node = node->left;
    return node;
}

struct rb_node *tree_successor(struct rb_node *node)
{
    struct rb_node *y;
    
    if (node->right != NULL)
        return tree_minimum(node->right);
    y = node->parent;
    while (y != NULL && node == y->right) {
        node = y;
        y = y->parent;
    }
    return y;
}

void rb_free(struct rb_node *rb_root)
{
    if (rb_root == NULL)
        return;

    rb_free(rb_root->left);
    rb_free(rb_root->right);

    free(rb_root);
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

