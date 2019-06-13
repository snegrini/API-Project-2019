#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINE_LENGTH 50

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

void rb_insert(char *id_ent)
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

void readLine(char **str)
{
    int ch, i = 0;
    
    while((ch = getchar()) != '\n' && ch != EOF) {
        (*str)[i++] = ch;

        if (i % DEFAULT_LINE_LENGTH == 0) {
            *str = realloc(*str, sizeof(char) * (i + DEFAULT_LINE_LENGTH + 1));
        }
    }
    (*str)[i] = '\0';
}

int main(int argc, char *argv[])
{
    char *line = malloc(sizeof(char) * (DEFAULT_LINE_LENGTH + 1));
    char command[7];
    char *id_ent = malloc(sizeof(char) * DEFAULT_LINE_LENGTH);
    char *id_orig, *id_dest, *id_rel;

    do {
        readLine(&line);
        sscanf(line, "%7s", command);
        
        if (strncmp(command, "addent", 7) == 0) {
            sscanf(line, "%*s %s", id_ent);
            printf("ADDENT %s", id_ent);
        } else if (strncmp(command, "delent", 7) == 0) {
            sscanf(line, "%*s %s", id_ent);
            printf("DELENT %s", id_ent);
        } else if (strncmp(command, "addrel", 7) == 0) {
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            printf("ADDREL %s %s %s", id_orig, id_dest, id_rel);
        } else if (strncmp(command, "delrel", 7) == 0) {
            sscanf(line, "%*s %s %s %s", id_orig, id_dest, id_rel);
            printf("DELREL %s %s %s", id_orig, id_dest, id_rel);
        } else if (strncmp(command, "report", 7) == 0) {
            printf("REPORT");
        }
        printf("%s\n", command);
    } while (strncmp(command, "end", 4) != 0);
    
    free(command);

    return 0;
}
