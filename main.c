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

int readLine(char *str) {
    int ch, i = 0;
    
    while((ch = getchar()) != '\n') {
        str[i++] = ch;
        
        if (i % DEFAULT_LINE_LENGTH == 0) {
            int b = DEFAULT_LINE_LENGTH * (i / DEFAULT_LINE_LENGTH);
            str = realloc(str, sizeof(char) * b);
        }
    }
    str[i] = '\0';

    return 0;
}

int main(int argc, char *argv[])
{
    char *command = malloc(sizeof(char) * DEFAULT_LINE_LENGTH);
    //char *command, *id_ent, *id_orig, *id_dest, *id_rel;

    do {
        int a = readLine(command);
        printf("%c", a);
        
        if (strncmp(command, "addent", 7) == 0) {
            //scanf("%ms", &id_ent);
        } else if (strncmp(command, "delent", 7) == 0) {
            //scanf("%ms", &id_ent);
        } else if (strncmp(command, "addrel", 7) == 0) {
            //scanf("%ms", &id_orig);
            //scanf("%ms", &id_dest);
            //scanf("%ms", &id_rel);
        } else if (strncmp(command, "delrel", 7) == 0) {
            //scanf("%ms", &id_orig);
            //scanf("%ms", &id_dest);
            //scanf("%ms", &id_rel);
        } else if (strncmp(command, "report", 7) == 0) {
            
        }
        printf("%s\n", command);
    } while (strncmp(command, "end", 4) != 0);
    
    free(command);

    return 0;
}
