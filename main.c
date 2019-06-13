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

void left_rotate(struct RB_node *x) {
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

void right_rotate(struct RB_node *x) {
    // TODO
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

void readStr(char *str)
{
    char ch;
    int i = 0;
        
    while (sscanf(str, "%c", &ch)) {
        str[i++] = ch;

        if (i % DEFAULT_STRING_LENGTH == 0) {
            str = realloc(str, sizeof(char) * (i + DEFAULT_STRING_LENGTH + 1));
        }
    }
    str[i] = '\0';
}

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
