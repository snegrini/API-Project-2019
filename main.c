#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[])
{
    char *command, *id_ent, *id_orig, *id_dest, *id_rel;

    do {
        scanf("%ms", &command);

        if (strncmp(command, "addent", 7) == 0) {
            scanf("%ms", &id_ent);
        } else if (strncmp(command, "delent", 7) == 0) {
            scanf("%ms", &id_ent);
        } else if (strncmp(command, "addrel", 7) == 0) {
            scanf("%ms", &id_orig);
            scanf("%ms", &id_dest);
            scanf("%ms", &id_rel);
        } else if (strncmp(command, "delrel", 7) == 0) {
            scanf("%ms", &id_orig);
            scanf("%ms", &id_dest);
            scanf("%ms", &id_rel);
        } else if (strncmp(command, "report", 7) == 0) {
            
        }

    } while (strncmp(command, "end", 4) != 0);

    return 0;
}