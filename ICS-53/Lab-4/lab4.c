#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2
#define MAXBLOCKSZ 127
#define MAXALLOCATABLEBLOCK 125

static unsigned char blocknumber = 0;
static unsigned char * MainHeap = NULL;
static unsigned char* start = NULL;
static unsigned char * end = NULL;

static unsigned char get_blocksz(char s) // use first fit
{
    unsigned char temp = s;
    temp = temp >> 1;
    return temp;
} 
static void set_allocated_bit(unsigned char * lower_byte)
{
    SET_BIT(*lower_byte, 0);
}
static void remove_allocation_bit(unsigned char * lower_byte)
{
    CLR_BIT(*lower_byte, 0);
}
static char get_allocation_bit (unsigned char lower_byte)
{
    return GET_BIT(lower_byte, 0);
}
static char * findblock(unsigned char blocknum)
{
    unsigned char * temp = start;
    while(temp < end)
    {
        if(temp[0] == blocknum)
        {
            return temp;
        }
        temp += get_blocksz(temp[1]);
    }
    return NULL;
}
static void init_heap()
{
    MainHeap = (unsigned char*)malloc(MAXBLOCKSZ);
    start = MainHeap;
    end = MainHeap + MAXBLOCKSZ;

    MainHeap[0] = blocknumber;
    MainHeap[1] = MAXBLOCKSZ;
    MainHeap[1] = MainHeap[1] << 1;
}
static void free_heap()
{
    free(MainHeap);
    MainHeap = NULL;
    start = NULL;
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
static unsigned char * find_free_block(int reqsz)
{
    // dont split off if block found is 2 more than requested size
    // only split is block found is 3 or more than requested size.
    unsigned char * temp = start;
    while(temp < end)
    {
        if(!get_allocation_bit(temp[1]) && get_blocksz(temp[1]) >= reqsz)
        {
            return temp;
        }
        temp += get_blocksz(temp[1]);
    }
    return NULL;
}
static void set_size(char * lower_byte, unsigned int sz)
{
    *lower_byte = sz;
    *lower_byte = *lower_byte << 1;
}
static void allocate(unsigned int blocksz)
{
    unsigned int realsz = blocksz +2;
    unsigned int bsz = 0;
    if(realsz > MAXBLOCKSZ)
    {
        printf("Cannot allocate more than %i blocks in a single allocation\n", MAXBLOCKSZ);
        return;
    }
    unsigned char * newblk = find_free_block(realsz);
    if(newblk == NULL)
    {
        printf("No more space on heap available, MAX amount is already allocated.\n");
        return;
    }
    bsz = get_blocksz(newblk[1]);
    if(get_blocksz(newblk[1])-realsz <3)
    {
        
        newblk[0] = ++blocknumber;
        printf("%i\n", newblk[0]);
        set_size(&newblk[1], get_blocksz(newblk[1]));
        set_allocated_bit(&newblk[1]);
    }
    else
    {
        unsigned char oldblocknum = newblk[0];
        newblk[0] = ++blocknumber;
        printf("%i\n", newblk[0]);
        set_size(&newblk[1], realsz);
        set_allocated_bit(&newblk[1]);
        newblk += realsz;
        newblk[0] = oldblocknum;
        set_size(&newblk[1], bsz - realsz);
        remove_allocation_bit(&newblk[1]);
    }
    
}
static void free_block(unsigned int blocknum)
{
    unsigned char * oldblk = findblock(blocknum);
    if(oldblk == NULL)
    {
        printf("Block not found. Try again.\n");
        return;
    }
    remove_allocation_bit(&oldblk[1]);
}
static void print_blocklist()
{
    unsigned char * temp = start;
    unsigned char st = 0;
    printf("SIZE        ALLOCATED       START       END\n");
    while(temp < end)
    {
    if(get_allocation_bit(temp[1]))
    {
    printf(" %3i           %3s           %3i         %3i\n", get_blocksz(temp[1]), "yes",st, st+get_blocksz(temp[1]) -1);
    }
    else
    {
    printf(" %3i           %3s           %3i         %3i\n", get_blocksz(temp[1]), "no",st, st+get_blocksz(temp[1]) -1);
    }
    st = st+ get_blocksz(temp[1]);
    temp+=get_blocksz(temp[1]);
    }
}
static void write_heap(unsigned char blocknum, char c, unsigned char copies)
{
    unsigned char * temp = findblock(blocknum);

    if(temp == NULL)
    {
        printf("Block not found, try again\n");
        return;
    }
    if(copies > (get_blocksz(temp[1])-2))
    {
        printf("Not enough space to print\n");
        return;
    }
    if(!get_allocation_bit(temp[1]))
    {
        printf("Block not valid/allocated\n");
    }
    temp+=2;
    int i;
    for(i=0; i<copies; i++)
    {
        temp[i] = c;
    }

}
static void print_heap(unsigned char blocknum, unsigned char sz)
{
    unsigned char * temp = findblock(blocknum);

    if(temp == NULL)
    {
        printf("Block not found, try again\n");
        return;
    }
    if(!get_allocation_bit(temp[1]))
    {
        printf("Block not valid/allocated\n");
    }
    temp+=2;
    int i;
    for(i=0; i<sz; i++)
    {
        printf("%c",temp[i]);
    }
    printf("\n");
}
static void print_header(unsigned char blocknum)
{
       unsigned char * temp = findblock(blocknum);

    if(temp == NULL)
    {
        printf("Block not found, try again\n");
        return;
    }
    printf("%02x%02x\n", temp[0], temp[1]);
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
