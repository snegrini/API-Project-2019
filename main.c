#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRING_LENGTH   35

#define RED     0
#define BLACK   1

struct str_node {
    char str[DEFAULT_STRING_LENGTH];
    unsigned char size;
};

struct rb_node {
    char *key;
    unsigned int size;
    int color;
    struct rb_node *nested;
    struct rb_node *parent;
    struct rb_node *left;
    struct rb_node *right;
};

/*
 * Global variables
 */
struct rb_node leaf;
struct rb_node *t_nil;
int first_print;
int need_report_update;
struct str_node tokens[4];
struct rb_node *ent_rb_root; /* Store all entities before relations are created. */
struct rb_node *rel_rb_root;
struct rb_node *report_rb_root;

/*
 * Default functions
 */
void addent(char *id_ent);
void delent(char *id_ent);
void addrel(char *id_orig, char *id_dest, char *id_rel);
void delrel(char *id_orig, char *id_dest, char *id_rel);
void report();

/*
 * Utility functions
 */
void build_report(struct rb_node *rb_root);
void build_report_nested(struct rb_node *node_rep, struct rb_node *node_dest);
void update_report(struct rb_node *node_rep, char *id_dest, unsigned int size);
void rb_delete_ent_from_rel(struct rb_node *curr_rb_root, char *id_ent);
void rb_delete_ent_from_rel_nested(struct rb_node *curr_rb_root, char *id_ent);
void print_report(struct rb_node *rb_root);
void print_report_nested(struct rb_node *rb_root);
void fputui(unsigned int num);

/*
 * Input functions
 */
int readline();

/*
 * RB Tree functions
 */
int rb_insert(struct rb_node **rb_root, struct rb_node *new);
void rb_insert_fixup(struct rb_node **rb_root, struct rb_node *z);
struct rb_node *rb_delete(struct rb_node **rb_root, struct rb_node *node);
void rb_delete_fixup(struct rb_node **rb_root, struct rb_node *x);
struct rb_node *rb_search(struct rb_node *rb_root, char *key);
void rotate_left(struct rb_node **rb_root, struct rb_node *x);
void rotate_right(struct rb_node **rb_root, struct rb_node *x);
struct rb_node *tree_minimum(struct rb_node *node);
struct rb_node *tree_successor(struct rb_node *node);
struct rb_node *rb_create_insert_node(struct rb_node **rb_root, char *key);
void rb_free(struct rb_node **rb_root, int clear_id_ent);


int main(void)
{
    char command[8];
    char *id_ent  = NULL;
    char *id_orig = NULL;
    char *id_dest = NULL;
    char *id_rel  = NULL;
    char *tmp_id_orig = NULL;
    char *tmp_id_dest = NULL;

    /* Initialize global vars */
    t_nil = &leaf;
    t_nil->color = BLACK; /* Set t_nil (leaf) color to BLACK */
    first_print = 1;
    need_report_update = 0;
    ent_rb_root = t_nil;
    rel_rb_root = t_nil;
    rel_rb_root->nested = t_nil;
    rel_rb_root->nested->nested = t_nil;
    report_rb_root = t_nil;

    for (unsigned int i = 0; i < 4; i++)
        tokens[i].size = 0;

    do {
        readline();
        memcpy(command, tokens[0].str, tokens[0].size);

        if (strncmp(command, "addent", 7) == 0) {
            id_ent = malloc(sizeof(char) * tokens[1].size);
            strcpy(id_ent, tokens[1].str);
            addent(id_ent);
        } else if (strncmp(command, "delent", 7) == 0) {
            id_ent = malloc(sizeof(char) * tokens[1].size);
            strcpy(id_ent, tokens[1].str);
            delent(id_ent);
        } else if (strncmp(command, "addrel", 7) == 0) {
            id_orig = malloc(sizeof(char) * tokens[1].size);
            id_dest = malloc(sizeof(char) * tokens[2].size);
            id_rel = malloc(sizeof(char) * tokens[3].size);
            strcpy(id_orig, tokens[1].str);
            strcpy(id_dest, tokens[2].str);
            strcpy(id_rel, tokens[3].str);

            /* Checking that the entities of the relationship are monitored. */
            tmp_id_orig = rb_search(ent_rb_root, id_orig)->key;
            tmp_id_dest = rb_search(ent_rb_root, id_dest)->key;
            if (tmp_id_orig != NULL && tmp_id_dest != NULL) {
                addrel(tmp_id_orig, tmp_id_dest, id_rel);
            } else {
                free(id_rel);
            }
            free(id_orig);
            free(id_dest);
        } else if (strncmp(command, "delrel", 7) == 0) {
            id_orig = malloc(sizeof(char) * tokens[1].size);
            id_dest = malloc(sizeof(char) * tokens[2].size);
            id_rel = malloc(sizeof(char) * tokens[3].size);
            strcpy(id_orig, tokens[1].str);
            strcpy(id_dest, tokens[2].str);
            strcpy(id_rel, tokens[3].str);

            delrel(id_orig, id_dest, id_rel);
        } else if (strncmp(command, "report", 7) == 0) {
            report();
        }
    } while (strncmp(command, "end", 4) != 0);
    
    rb_free(&ent_rb_root, 1);
    rb_free(&rel_rb_root, 1);  
    rb_free(&report_rb_root, 0);

    return 0;
}

/*
 * Adds an entity to the entity tree.
 * Entity names are unique.
 *
 * @param   id_ent  name of the entity to be added
 */
void addent(char *id_ent)
{
    if (rb_create_insert_node(&ent_rb_root, id_ent) == t_nil)
        free(id_ent); /* No entry, key already existent */
}

/*
 * Deletes an entity from the entity tree.
 * If an entity is deleted, a full report update is required
 * and the need_report_update flag is set.
 *
 * @param   id_ent  name of the entity to be deleted
 */
void delent(char *id_ent)
{
    struct rb_node *node_tmp;
     
    if (rel_rb_root != t_nil) {
        rb_delete_ent_from_rel(rel_rb_root, id_ent);
        need_report_update = 1; /* Setting flag for report update */
    }
    node_tmp = rb_search(ent_rb_root, id_ent);
    if (node_tmp != t_nil) {
        free(node_tmp->key);
        node_tmp = rb_delete(&ent_rb_root, node_tmp);
        free(node_tmp);        
    }
    free(id_ent);
}

/*
 * Adds a relation in the relationship tree.
 * If the node relation is not existent, a new one is created.
 * If a relation is added, the report tree gets updated.
 *
 * @param   id_orig     name of the origin entity
 * @param   id_dest     name of the destination entity
 * @param   id_rel      name of the relationship to be added
 */
void addrel(char *id_orig, char *id_dest, char *id_rel)
{
    struct rb_node *node_ent;
    struct rb_node *node_rel;
    struct rb_node *node_rep;

    node_rel = rb_search(rel_rb_root, id_rel);

    if (node_rel == t_nil) {
        node_rel = rb_create_insert_node(&rel_rb_root, id_rel);

        /* The relation did not exist before. Have to create rb_dest and rb_orig. */
        node_ent = rb_create_insert_node(&node_rel->nested, id_dest);
        ++node_ent->size;
        rb_create_insert_node(&node_rel->nested->nested, id_orig);

        if (need_report_update == 0) {
            /* Report update */
            node_rep = rb_create_insert_node(&report_rb_root, node_rel->key);
            node_rep->size = node_ent->size;
            rb_create_insert_node(&node_rep->nested, node_ent->key);
        }
    } else {
        /*
         * The relation already exists.
         * Checking if rb_dest exists, otherwise a new one is created.
         */
        free(id_rel); /* No more needed, relationship already exists. */
        node_ent = rb_search(node_rel->nested, id_dest);

        if (node_ent == t_nil) {
            /* Inserting new node for entity id_dest and increasing rb_orig size. */
            node_ent = rb_create_insert_node(&node_rel->nested, id_dest);
            ++node_ent->size;
            /* Since there is no rb_dest, the node in rb_orig will not be present, so it must be created. */
            rb_create_insert_node(&node_ent->nested, id_orig);

            if (need_report_update == 0) {
                /* Report update */
                node_rep = rb_search(report_rb_root, node_rel->key);
                update_report(node_rep, node_ent->key, node_ent->size);
            }
        } else {
            /*
             * Checking if the id_orig node exists:
             * - if it does not exist, it is added to the rb_orig tree.
             * - else, id_orig string is freed.
             */
            if (rb_create_insert_node(&node_ent->nested, id_orig) != t_nil) {
                ++node_ent->size;
                if (need_report_update == 0) {
                    /* Report update */
                    node_rep = rb_search(report_rb_root, node_rel->key);
                    update_report(node_rep, node_ent->key, node_ent->size);
                }
            }
        }
    }

}

/*
 * Deletes a relation from the relationship tree.
 * If a relation is deleted, a full report update is required
 * and the need_report_update flag is set.
 *
 * @param   id_orig     name of the origin entity
 * @param   id_dest     name of the destination entity
 * @param   id_rel      name of the relationship to be added
 */
void delrel(char *id_orig, char *id_dest, char *id_rel)
{
    struct rb_node *node_rel, *node_dest, *node_orig;
    node_rel = rb_search(rel_rb_root, id_rel);
    if (node_rel != t_nil) {
        node_dest = rb_search(node_rel->nested, id_dest);
        if (node_dest != t_nil) {
            node_orig = rb_search(node_dest->nested, id_orig);
            if (node_orig != t_nil) {
                node_orig = rb_delete(&node_dest->nested, node_orig);
                node_dest->size = node_dest->size - 1;
                free(node_orig);
                need_report_update = 1; /* Setting flag for report update */

                /* If rb_orig tree is empty, rb_dest node is deleted */
                if (node_dest->nested == t_nil) {
                    node_dest = rb_delete(&node_rel->nested, node_dest);
                    free(node_dest);

                    /* If rb_dest tree is empty, rb_rel node is deleted */
                    if (node_rel->nested == t_nil) {
                        free(node_rel->key);
                        node_rel = rb_delete(&rel_rb_root, node_rel);
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

/*
 * Updates the report tree if the need_report_update flag is set.
 * Prints the report tree.
 */
void report()
{
    if (rel_rb_root == t_nil) {
        fputs("none", stdout);
    } else {
        if (need_report_update == 1) {
            rb_free(&report_rb_root, 0);
            build_report(rel_rb_root);
            need_report_update = 0;
        }
        print_report(report_rb_root);
        if (first_print == 1) {
            fputs("none", stdout);
        }
        first_print = 1;
    }
    fputc('\n', stdout);
}

/*
 * Builds a new report tree.
 * Calls a function to build nested rb_orig tree.
 *
 * @param   rb_root     root of the tree to be builded
 */
void build_report(struct rb_node *rb_root)
{
    struct rb_node *node_rep;

    if (rb_root == t_nil)
        return;
    build_report(rb_root->left);

    if (rb_root != t_nil) {
        node_rep = rb_create_insert_node(&report_rb_root, rb_root->key);
        build_report_nested(node_rep, rb_root->nested);
    }

    build_report(rb_root->right);
}

/*
 * Builds rb_orig report tree.
 *
 * @param   rb_root     root of the rb_orig tree to be builded
 */
void build_report_nested(struct rb_node *node_rep, struct rb_node *node_dest)
{
    if (node_dest == t_nil)
        return;

    build_report_nested(node_rep, node_dest->right);

    if (node_dest->size != 0) {
        /* Se ho un nuovo max_size, azzero l'albero */
        if (node_dest->size == node_rep->size) {
            rb_create_insert_node(&node_rep->nested, node_dest->key);
        } else if (node_dest->size > node_rep->size) {
            rb_free(&node_rep->nested, 0);
            rb_create_insert_node(&node_rep->nested, node_dest->key);
            node_rep->size = node_dest->size;
        }
    }

    build_report_nested(node_rep, node_dest->left);
}

/*
 * Updates report tree without rebuilding the whole tree.
 *
 * @param   node_rep    node of the report tree to be updated
 * @param   id_dest     name of the new entity destination to be added
 * @param   size        new size of the report node
 */
void update_report(struct rb_node *node_rep, char *id_dest, unsigned int size)
{
    if (size == node_rep->size) {
        rb_create_insert_node(&node_rep->nested, id_dest);
    } else if (size > node_rep->size) {
        rb_free(&node_rep->nested, 0);
        rb_create_insert_node(&node_rep->nested, id_dest);
        node_rep->size = size;
    }
}

/*
 * Deletes an entity from relation rb_dest tree.
 * Calls a function to deletes entity in the nested rb_orig tree.
 *
 * @param   curr_rb_root    node of the relation tree to be searched
 * @param   id_ent     		name of the entity to be deleted
 */
void rb_delete_ent_from_rel(struct rb_node *curr_rb_root, char *id_ent)
{
    struct rb_node *node_tmp;

    if (curr_rb_root == t_nil)
        return;
    rb_delete_ent_from_rel(curr_rb_root->left, id_ent);
    rb_delete_ent_from_rel(curr_rb_root->right, id_ent);
    /*
     * Searches for entity in rb_dest of every relation,
     * on hit, nested rb_orig are deleted.
     */
    node_tmp = rb_search(curr_rb_root->nested, id_ent);
    if (node_tmp != t_nil) {
        rb_free(&node_tmp->nested, 0);
        node_tmp = rb_delete(&curr_rb_root->nested, node_tmp);
        free(node_tmp);
    }
    rb_delete_ent_from_rel_nested(curr_rb_root->nested, id_ent);
}

/*
 * Deletes an entity in rb_orig of every rb_dest (relation) nodes,
 *
 * @param   curr_rb_root    node of the rb_dest tree to be searched
 * @param   id_ent     		name of the entity to be deleted
 */
void rb_delete_ent_from_rel_nested(struct rb_node *curr_rb_root, char *id_ent)
{
    struct rb_node *node_tmp;

    if (curr_rb_root == t_nil)
        return;
    rb_delete_ent_from_rel_nested(curr_rb_root->left, id_ent);
    /* Searches for entity in rb_dest of every relation,
     * on hit, the node is deleted.
     */
    node_tmp = rb_search(curr_rb_root->nested, id_ent);
    if (node_tmp != t_nil) {
        node_tmp = rb_delete(&curr_rb_root->nested, node_tmp);
        curr_rb_root->size = curr_rb_root->size - 1;
        free(node_tmp);
    }
    rb_delete_ent_from_rel_nested(curr_rb_root->right, id_ent);
}

/*
 * Prints the report tree as follows:
 * id_rel1 id_ent1_1 id_ent1_2 n_rel1; id_rel2 id_ent2_1 id_ent2_2 n_rel2; ...
 *
 * Calls a function to print the nested rb_orig tree.
 *
 * id_rel  name of the relation
 * id_ent  name of the entity receiving the relation
 * n_rel   number of relations received
 *
 * @param   rb_root     root of the tree to be printed
 */
void print_report(struct rb_node *rb_root)
{
    if (rb_root == t_nil)
        return;
    print_report(rb_root->left);

    if (rb_root->size != 0) {
        if (first_print == 1) {
            fputs(rb_root->key, stdout);
            first_print = 0;
        } else {
            fputc(' ', stdout);
            fputs(rb_root->key, stdout);
        }
        print_report_nested(rb_root->nested);
        fputc(' ', stdout);
        fputui(rb_root->size);
        fputc(';', stdout);
    }

    print_report(rb_root->right);
}

/*
 * Print the rb_orig report tree.
 *
 * @param   rb_root     root of the rb_orig tree to be printed
 */
void print_report_nested(struct rb_node *rb_root)
{
    if (rb_root == t_nil)
        return;
    print_report_nested(rb_root->left);
    fputc(' ', stdout);
    fputs(rb_root->key, stdout);
    print_report_nested(rb_root->right);
}

/*
 * Prints the given number via fputc() method.
 * This results in a faster printing method than
 * the common printf().
 *
 * @param   num     number to be printed
 */
void fputui(unsigned int num)
{
    if (num >= 10)
        fputui(num / 10);
    fputc('0' + num % 10, stdout);
}

/*
 * Reads input lines from stdin.
 * A single whitespace ' ' is used as string separator.
 * This allows to save string sizes (for a later dynamic memory allocation)
 * without calling strlen().
 */
int readline()
{
    int ch, i = 0, j = 0;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        if (ch == ' ') {
            tokens[i].str[j++] = '\0';
            tokens[i++].size = j;
            j = 0;
        } else {
            tokens[i].str[j++] = ch;
        }
    }
    tokens[i].str[j++] = '\0';
    tokens[i].size     = j;
    return i;
}

/*
 * Inserts a new node into the given RBT.
 * @return 	int     0 no entry, key already existing
 *                  1 new entry
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
            return 0; /* Avoid key duplicates */
    }
    new->parent = y;

    if (y == t_nil)
        *rb_root = new; /* Empty tree */
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


/*
 * Fixes the tree so that RBT properties are preserved.
 * @param   rb_root
 * @param   z
 */
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

/*
 * Deletes a node from the given RBT.
 * @param   rb_root
 * @param   node
 * @return  rb_node     the deleted node
 */
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
        node->size = y->size;
        node->nested = y->nested;
    }
    if (y->color == BLACK)
        rb_delete_fixup(rb_root, x);
    return y;
}

/*
 * Fixes the tree so that RBT properties are preserved.
 * @param   rb_root
 * @param   x
 */
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

/*
 * Searches the key in the given RBT.
 * @param   rb_root
 * @param   node
 * @return  rb_node     the found node (with the corresponding key)
 */
struct rb_node *rb_search(struct rb_node *rb_root, char *key)
{
    while (rb_root != t_nil) {
        if (strcmp(key, rb_root->key) == 0)
            return rb_root;
        else if (strcmp(key, rb_root->key) < 0)
            rb_root = rb_root->left;
        else
            rb_root = rb_root->right;
    }
    return t_nil;
}

/*
 * Rotates node x to left.
 * @param   rb_root
 * @param   x
 */
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

/*
 * Rotates node x to right.
 * @param   rb_root
 * @param   x
 */
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

/*
 * Finds the tree minimum 
 * @param   node
 * @return  struct rb_node
 */
struct rb_node *tree_minimum(struct rb_node *node)
{
    while (node->left != t_nil)
        node = node->left;
    return node;
}

/*
 * Finds the given node successor 
 * @param   node
 * @return  struct rb_node
 */
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

/*
 * Creates a new node with the given key and inserts it in the given RBT.
 * @param   rb_root  
 * @param   key              
 * @return  struct rb_node      the created node on success, t_nil on fail
 */
struct rb_node *rb_create_insert_node(struct rb_node **rb_root, char *key)
{
    struct rb_node *node_ent = malloc(sizeof(struct rb_node));
    node_ent->key = key;
    node_ent->size = 0;
    node_ent->nested = t_nil;
    if (rb_insert(rb_root, node_ent) == 1)
        return node_ent;
    free(node_ent);
    return t_nil;
}

/*
 * Frees the given tree and the nested trees. If clear_id_int flag is set
 * also strings (keys) of the root tree are freed. Nested keys will not be freed.
 * @param   rb_root  
 * @param   clear_id_ent        set to 1 to free strings, 0 otherwise              
 */
void rb_free(struct rb_node **rb_root, int clear_id_ent)
{
    if (*rb_root == t_nil)
        return;
    rb_free(&(*rb_root)->left, clear_id_ent);
    rb_free(&(*rb_root)->right, clear_id_ent);

    if ((*rb_root)->nested != t_nil)
        rb_free(&(*rb_root)->nested, 0);
    if (clear_id_ent == 1 && (*rb_root)->key != NULL)
        free((*rb_root)->key);
    free(*rb_root);
    *rb_root = t_nil;
}
