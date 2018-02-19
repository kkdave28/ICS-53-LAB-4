#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2
#define MAXBLOCKSZ 128

static char * MainHeap = NULL;
static void init_heap()
{
    MainHeap = (char*)malloc(MAXBLOCKSZ);
}
static void free_heap()
{
    free(MainHeap);
    MainHeap = NULL;
}
static int customgetLine (char *buff, size_t sz) {
    int ch, extra;

    printf("> ");
    if (fgets (buff, sz, stdin) == NULL)
        return NO_INPUT;

    if (buff[strlen(buff)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? TOO_LONG : OK;
    }

    buff[strlen(buff)-1] = '\0';
    return OK;
}

static char** str_split(char* a_str, const char a_delim)
{
    char** result = NULL;
    int count = 0;
    char* tmp = a_str;
    char* last_comma = NULL;
    char delim[2];
    
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);
    count++;
    result = malloc(sizeof(char*) * count);
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}
static void allocate(unsigned int blocksz)
{
    printf("allocating block of size %i\n", blocksz);
}
static void free_block(unsigned int blocknum)
{
    printf("Freeing block number %i\n", blocknum);
}
static void print_blocklist()
{
    printf("Printing blocklist\n");
}
static void write_heap(unsigned int blocknum, char c, unsigned int copies)
{
    printf("Writing %i copies of %c to block number %i\n", copies, c, blocknum);
}
static void print_heap(unsigned int blocknum, unsigned int sz)
{
    printf("Printing %i bytes of block number %i\n", sz, blocknum);
}
static void print_header(unsigned int blocknum)
{
    printf("Printing the header of block number %i\n", blocknum);
}

static int read_command(char * s)
{
    // 1 - allocate
    // 2 - free
    // 3 - blocklist
    // 4 - writeheap
    // 5 - printheap
    // 6 - printheader
    // 7 - quit
    // -1 - default
    if(strcmp(s, "allocate") == 0){

        return 1;
    }
    else if(strcmp(s, "free") == 0){

        return 2;
    }
    else if(strcmp(s, "blocklist") == 0){

        return 3;
    }
    else if(strcmp(s, "writeheap") == 0){

        return 4;
    }
    else if(strcmp(s, "printheap") == 0){

        return 5;
    }
    else if(strcmp(s, "printheader") == 0){

        return 6;
    }
    else if(strcmp(s, "quit") == 0)
    {
        return 7;
    }
    else{

        return -1;
    }

}
static void cleanup(char ** s){
    
    int i;
    for(i = 0; *(s+i); i++){
        free(*(s + i));
    }
    free(s);
}

static void shell()
{
    char command[1024];
    char ** temp = NULL;
    int cmd;
    //printf("Initializing Memory, Page Table and Disk\n");
    init_heap();
    //printf("Ready for operations\n");    
    // printf("Hello There!\n");
    // printf("GENERAL KENOBI REEEEEEE!!!!\n");
    while(customgetLine(command, sizeof(command)) != NO_INPUT)
    {
        printf("\n");
        
        temp = str_split(command, ' ');
        cmd = read_command(*(temp));
        switch(cmd)
        {
                // 1 - allocate
                // 2 - free
                // 3 - blocklist
                // 4 - writeheap
                // 5 - printheap
                // 6 - printheader
                // 7 - quit
                // -1 - default
            default:{
                printf("Invalid command, please enter a valid command\n");
                cleanup(temp);
                temp=NULL;
                break;
            }
            case 1:{
                allocate(atoi(*(temp+1)));
                cleanup(temp);
                temp=NULL;
                break;
            }
            case 2:{
                free_block(atoi(*(temp+1)));
                cleanup(temp);
                temp=NULL;
                break;
            }
            case 3:{
                print_blocklist();
                cleanup(temp);
                temp=NULL;
                break;
            }
            case 4:{
                write_heap(atoi(*(temp+1)), **(temp+2), atoi(*(temp+3)));
                cleanup(temp);
                temp=NULL;
                break;
            }
            case 5:{
                print_heap(atoi(*(temp+1)), atoi(*(temp+2)));
                cleanup(temp);
                temp=NULL;
                break;
            }
            case 6:{
                print_header(atoi(*(temp+1)));
                cleanup(temp);
                temp=NULL;
                break;
            }
            case 7:
            {
                cleanup(temp);
                temp=NULL;
                return;
            }



        }
    }

}
int main()
{
    atexit(free_heap);
    shell();
    return 0;
}
