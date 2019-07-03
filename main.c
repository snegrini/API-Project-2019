#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_GRAPH_SIZE      50
#define DEFAULT_STRING_LENGTH   50

#define RED     0
#define BLACK   1

struct adj_list_node {
    char *id_ent;
    struct adj_list_node *next;
};

/*
 * L'attributo name delle liste di adiacenza è il nome dell'entità id_dest.
 */
struct adj_list {
    char *name;
    unsigned int size;
    struct adj_list *next_list;
    struct adj_list_node *node;
};

struct graph {
    unsigned int max_size;
    unsigned int curr_size;
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

struct rb_node node;
struct rb_node *t_nil = &node;

void addent(struct rb_node **rb_root, char *id_ent);
void delent(struct rb_node **rb_root, char *id_ent);
void addrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel);
void delrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel);
void report(struct rb_node **rb_root);
void print_report(struct adj_list_node *ent_list_head);

/*
 * RB Tree functions
 */
void rb_insert(struct rb_node **rb_root, struct rb_node *new);
void rb_insert_fixup(struct rb_node **rb_root, struct rb_node *new);
struct rb_node *rb_delete(struct rb_node **rb_root, struct rb_node *node);
void rb_delete_fixup(struct rb_node **rb_root, struct rb_node *x);
struct rb_node *rb_search(struct rb_node **rb_root, char *key);
void rotate_left(struct rb_node **rb_root, struct rb_node *x);
void rotate_right(struct rb_node **rb_root, struct rb_node *x);
struct rb_node *tree_minimum(struct rb_node *node);
struct rb_node *tree_successor(struct rb_node *node);
void rb_free(struct rb_node **rb_root);
void rb_visit_inorder(struct rb_node *rb_root, struct adj_list_node **ent_list_head);

/*
 * Graph functions
 */
struct adj_list *adj_list_search(struct adj_list **head, char *name);
void adj_list_node_insert(struct adj_list_node **head, char *id_ent);
struct adj_list_node *adj_list_node_search(struct adj_list_node **head, char *id_ent);
void adj_list_node_free(struct adj_list_node *head);

/*
 * Input functions
 */
int readLine(char **str);

int main(int argc, char *argv[])
{
    char *line = malloc(sizeof(char) * (DEFAULT_STRING_LENGTH + 1));
    
    char command[8];    
    char *id_ent  = NULL;
    char *id_orig = NULL;
    char *id_dest = NULL;
    char *id_rel  = NULL;
    
    int len;
    
    struct rb_node *ent_rb_root; // Store all entities before relations are created
    struct rb_node *rel_rb_root;
        
    ent_rb_root = t_nil;
    rel_rb_root = t_nil;
    
    do {
        len = readLine(&line);
        sscanf(line, "%7s", command);
                
        if (strncmp(command, "addent", 7) == 0) {
            id_ent = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s", id_ent);
            addent(&ent_rb_root, id_ent);
        } else if (strncmp(command, "delent", 7) == 0) {
            id_ent = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s", id_ent);
            delent(&ent_rb_root, id_ent);
        } else if (strncmp(command, "addrel", 7) == 0) {
            id_orig = malloc(sizeof(char) * len);
            id_dest = malloc(sizeof(char) * len);
            id_rel = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            
            
            /*
             * Verifico che le entità della relazione siano monitorate.
             */
            struct rb_node *tmp_orig = rb_search(&ent_rb_root, id_orig);
            struct rb_node *tmp_dest = rb_search(&ent_rb_root, id_dest);
            *tmp_orig = *tmp_orig;
            *tmp_dest = *tmp_dest;
            
            if (rb_search(&ent_rb_root, id_orig) != t_nil && rb_search(&ent_rb_root, id_dest) != t_nil) {
                addrel(&rel_rb_root, id_orig, id_dest, id_rel);
            }            
        } else if (strncmp(command, "delrel", 7) == 0) {
            id_orig = malloc(sizeof(char) * len);
            id_dest = malloc(sizeof(char) * len);
            id_rel = malloc(sizeof(char) * len);
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            delrel(&rel_rb_root, id_orig, id_dest, id_rel);
        } else if (strncmp(command, "report", 7) == 0) {
            report(&rel_rb_root);
        }
    } while (strncmp(command, "end", 4) != 0);
    
    /*free(line);
    free(id_ent);
    free(id_orig);
    free(id_dest);
    free(id_rel);*/
    
    rb_free(&ent_rb_root);
    rb_free(&rel_rb_root);
    
    return 0;
}

void addent(struct rb_node **rb_root, char *id_ent)
{
    struct rb_node *new_node_ent;
    new_node_ent = malloc(sizeof(struct rb_node));
    new_node_ent->key = id_ent;
    
    rb_insert(rb_root, new_node_ent);
}

void delent(struct rb_node **rb_root, char *id_ent)
{
    struct rb_node *node_ent;
    
    node_ent = rb_search(rb_root, id_ent);
    rb_delete(rb_root, node_ent);
    free(node_ent);
}

/*
 * Add a relation. If the relation is not existent, a new one is created.
 */
void addrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel)
{
    struct rb_node *node_rel;
    struct adj_list_node *node_ent;
    node_rel = rb_search(rb_root, id_rel);
    
    if (node_rel == t_nil) {
        node_rel = malloc(sizeof(struct rb_node));
        node_rel->key = id_rel;
        
        /* La relazione prima non esisteva. Sicuramente devo creare il grafo e le liste di adiacenza */
        node_rel->ent_graph = malloc(sizeof(struct graph));
        node_rel->ent_graph->max_size = DEFAULT_GRAPH_SIZE;
        node_rel->ent_graph->adj_list_ent = malloc(sizeof(struct adj_list) * node_rel->ent_graph->max_size);
        
        node_rel->ent_graph->adj_list_ent->name = id_dest;
        node_rel->ent_graph->adj_list_ent->next_list = NULL;
        node_rel->ent_graph->curr_size = 1;
        
        node_ent = malloc(sizeof(struct adj_list_node));
        node_ent->id_ent = id_orig;
        node_ent->next = NULL;
        
        node_rel->ent_graph->adj_list_ent->node = node_ent;
        node_rel->ent_graph->adj_list_ent->size = 1; 
        
        rb_insert(rb_root, node_rel);
    } else {
        /* 
         * La relazione esiste, quindi esiste anche il grafo associato ad essa.
         * Cerco se è già presente la lista di adiacenza dell'entità id_dest,
         * altrimenti la creo.
         *
         * TODO: forse è meglio creare tutte le liste di adiacenza con nome entità e puntatore
         * al primo nodo NULL (sarebbe un vero grafo. In tal caso, ricordarsi di aggiungere/togliere
         * le occorrenze in tutti i grafi delle relazioni ad ogni aggiunta/rimozione di una
         * nuova entità (operazioni effettuate tramite addent()/delent().
         */
        struct adj_list *adj_list_ent;
        adj_list_ent = adj_list_search(&node_rel->ent_graph->adj_list_ent, id_dest);
        if (adj_list_ent == NULL) {
            /* Rialloco spazio se necessario */
            if ((node_rel->ent_graph->curr_size + 1) == node_rel->ent_graph->max_size) {
                node_rel->ent_graph->max_size = node_rel->ent_graph->max_size + DEFAULT_GRAPH_SIZE;
                node_rel->ent_graph->adj_list_ent = realloc(node_rel->ent_graph->adj_list_ent,
                                                            sizeof(struct adj_list) * node_rel->ent_graph->max_size);
            }
            
            /* 
             * Inserisco in testa la nuova lista di adiacenza per l'entità id_dest.
             */
            adj_list_ent->name = id_dest;
            adj_list_ent->size = 0;
            adj_list_ent->next_list = node_rel->ent_graph->adj_list_ent;
            node_rel->ent_graph->adj_list_ent = adj_list_ent->next_list;
            node_rel->ent_graph->curr_size = node_rel->ent_graph->curr_size + 1;
            
            /* 
             * Non essendoci la lista di adiacenza, non sarà presente in lista neanche l'entità id_orig, che
             * va quindi creata.
             */
             adj_list_node_insert(&adj_list_ent->node, id_orig);
             node_rel->ent_graph->adj_list_ent->size = node_rel->ent_graph->adj_list_ent->size + 1;
        } else {
            /*
             * Cerco, se esiste, il nodo dell'entità id_orig.
             * Se non esiste lo aggiungo in testa alla lista di adiacenza.
             * Se esiste, non faccio nulla.
             */
            node_ent = adj_list_node_search(&adj_list_ent->node, id_orig);
            if (node_ent == NULL) {
                adj_list_node_insert(&adj_list_ent->node, id_orig);
                adj_list_ent->size = adj_list_ent->size + 1;
            }
        }
        
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
         * Controllare che l'inserimento del nodo all'interno della lista di adiacenza venga effettuato in testa,
         * in modo da avere complessità temporale O(1) (costante).
         * 
         * TODO: vedere se è possibile snellire il codice della funzione addrel(), magari mettendo in comune
         * del codice dell'if-else.
         */
    }
    
}

void delrel(struct rb_node **rb_root, char *id_orig, char *id_dest, char *id_rel)
{
    
}

void report(struct rb_node **rb_root)
{
    struct adj_list_node *ent_list_head = NULL;
    rb_visit_inorder(*rb_root, &ent_list_head);
    print_report(ent_list_head);
}

void print_report(struct adj_list_node *ent_list_head)
{
    while (ent_list_head != NULL) {
        printf("%s", ent_list_head->id_ent);
        ent_list_head = ent_list_head->next;
    }
}

void rb_insert(struct rb_node **rb_root, struct rb_node *new)
{
    struct rb_node *x = *rb_root;
    struct rb_node *y = t_nil;
    
    while (x != t_nil) {
        y = x;
        
        if (strcmp(new->key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
        /*} else if (strcmp(new->key, x->key) > 0) {
            x = x->right;
        } else {
            return; // TODO: Avoid duplicates
        }*/
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
}

void rb_insert_fixup(struct rb_node **rb_root, struct rb_node *new)
{
    struct rb_node *x, *y;
    
    if (new == *rb_root) {
        (*rb_root)->color = BLACK;
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
    
    if (y != node)
        node->key = y->key;
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
        y->left->parent = x;
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

void rb_free(struct rb_node **rb_root)
{
    if (*rb_root == t_nil)
        return;

    rb_free(&(*rb_root)->left);
    rb_free(&(*rb_root)->right);

    free(*rb_root);
}

void rb_visit_inorder(struct rb_node *rb_root, struct adj_list_node **ent_list_head)
{
    unsigned int curr_max_size;
    struct adj_list_node *tmp_list_node;
    
    curr_max_size = 0;
    
    if (rb_root != t_nil) {
        rb_visit_inorder(rb_root->left, ent_list_head);

        while (rb_root->ent_graph->adj_list_ent != NULL) {
            if (rb_root->ent_graph->adj_list_ent->size == curr_max_size) {
                tmp_list_node = malloc(sizeof(struct adj_list_node));
                tmp_list_node->id_ent = rb_root->ent_graph->adj_list_ent->name;
                tmp_list_node->next = *ent_list_head;
                *ent_list_head = tmp_list_node;
            } else if (rb_root->ent_graph->adj_list_ent->size > curr_max_size) {
                if (*ent_list_head != NULL) {
                    adj_list_node_free(*ent_list_head);
                }
                tmp_list_node = malloc(sizeof(struct adj_list_node));
                tmp_list_node->id_ent = rb_root->ent_graph->adj_list_ent->name;
                tmp_list_node->next = NULL;
                *ent_list_head = tmp_list_node;
                curr_max_size = rb_root->ent_graph->adj_list_ent->size;
            }
            rb_root->ent_graph->adj_list_ent = rb_root->ent_graph->adj_list_ent->next_list;
        }
        tmp_list_node = malloc(sizeof(struct adj_list_node));
        tmp_list_node->id_ent = ";";
        tmp_list_node->next = NULL;
        //strcpy(tmp_list_node->id_ent, ";");
        *ent_list_head = tmp_list_node;
        
        rb_visit_inorder(rb_root->right, ent_list_head);
    }
}

struct adj_list *adj_list_search(struct adj_list **head, char *name)
{
    struct adj_list *curr = *head;
    while (curr != NULL && strcmp(curr->name, name) != 0) {
        curr = curr->next_list;
    }
    return curr;
}

void adj_list_node_insert(struct adj_list_node **head, char *id_ent)
{
    struct adj_list_node *new_node = malloc(sizeof(struct adj_list_node));
    new_node->id_ent = id_ent;
    new_node->next = *head; // TODO: sostituibile con = NULL?
    *head = new_node;
}

struct adj_list_node *adj_list_node_search(struct adj_list_node **head, char *id_ent)
{
    struct adj_list_node *curr = *head;
    while (curr != NULL && strcmp(curr->id_ent, id_ent) != 0) {
        curr = curr->next;
    }
    return curr;
}

void adj_list_node_free(struct adj_list_node *head)
{
    struct adj_list_node *curr;
    while (head != NULL) {
        curr = head;
        head = head->next;
        free(curr);
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

