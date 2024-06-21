#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <math.h>

/* Structure definition for the linked list queue */
struct node
{
    int index;
    struct node* next;
    struct node* prev;
};

typedef struct node node_t;

/* Structure definition for the queue manager */
struct manager
{
    int id;
    node_t* head;
    node_t* tail;
};

typedef struct manager manager_t;

int hits, misses, evictions = 0;
int MAX_LEN = 20; //used as a max length for tracefile names and trace lines
int ADDRESS_BITS = 64; //address size
int counter = 0;
int b = 0;
int e = 0;
int s = 0;
int vflag = 0;
int hflag = 0;
long long tag = 0;
long long validbit = ((long long) 1) << 63;
// 1- process command-line commands
// 2- set up cache 'array' based on values of e,b, and s
// 3- parse tracefile (fgets? adds \n to the end. Goes one by one)
// 4- individually process each element of the tracefile and change the array 
// 5- count reads, misses, and evictions (prob pointers are best choice)

/* changes string to a segment ranging from given character position to the end
 * Inputs: 
    - string (char*)
    - n (int)
*/
char* segstring(char* string, int n)
{
    int len = strlen(string);
    int newlen = len - (n + 1);
    char* newline = (char*)malloc(sizeof(char)*(newlen + 1));
    if(newline == NULL){
        fprintf(stderr, "segstring: malloc failed\n");
        exit(8);
    }

    for(int i = 0; i <= newlen; i++){
        newline[i] = string[n]; 
        newline[i+1] = '\0';
        n++;
    }
    return newline;
}

// gets tag bits of given number. First makes sure valid bit is off, 
// then right shifts by s + b bits to get tag bits
long long gettag(long long num){
    //printf("This is validbit %llx, ")
    return ((~validbit) & num) >> (s + b);
}

//gets set bits of given number. Uses a setmask to isolate b + s bits, then right shifts by b
long long getset(long long num){
    long long setmask = ~(((long long) -1) << (b + s));
    return (num & setmask) >> b;
}

//returns true if number has valid bit on. Does this by rightshifting by 63
bool isvalid(long long num){
    return num >> 63;
}

//returns true if two given numbers have the same tagbits
bool checktag(long long num1, long long num2){
    return gettag(num1) == gettag(num2);
}

//creates node for linked list
node_t* create_node(int n)
{
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if(node == NULL){
        fprintf(stderr, "create_node: malloc failed\n");
        exit(10);
    }
    node->index = n;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

//frees node
void free_node(node_t* node)
{
    free(node);
}

//creates node manager with pointers to head and tail of list
manager_t* create_manager()
{
    manager_t* manager = (manager_t*)malloc(sizeof(manager_t));
    if(manager == NULL){
        fprintf(stderr, "create_manager: malloc failed\n");
        exit(11);
    }

    node_t* head = create_node(0);
    manager->head = head;

    node_t* node = head;
    node_t* next = NULL;

    for(int i = 1; i < e; i++){
        node->next = create_node(i);
        next = node->next;
        next->prev = node;
        node = next;
    }

    manager->tail = node;

    return manager;
}

//frees manager
void free_manager(manager_t* manager)
{   
    node_t* head = manager->head;
    node_t* next = head->next;
    while(head){
        free_node(head);
        head = next;
        if(next){
        next = head->next;
        }else{
            break;
        }
    }
    free(manager);
}

// returns the correct line index choice using head of priority queue
// rearranges queue
int toppriority(int setnum, manager_t** master)
{
    manager_t* manager = master[setnum];

    int ret = manager->head->index;

    if(e > 1){
        node_t* first = manager->head;
        node_t* second = first->next;
        manager->tail->next = first;
        first->next = NULL;
        first->prev = manager->tail;
        second->prev = NULL;
        manager->head = second;
        manager->tail = first; 
    }

    return ret;
}

//returns pointer to node with given index in given manager
node_t* searcher(int index, manager_t* manager)
{
    node_t* temp = manager->head;

    while(temp){
        if(temp->index == index){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}


//Given set number, index of used node, and a pointer to managers, 
// updates priority queue after hits. Moves hit node to tail
void updatepriority(int setnum, int index, manager_t** master)
{
    manager_t* manager = master[setnum];
    node_t* newtail = searcher(index, manager);
    if(newtail->index == manager->head->index){
        toppriority(setnum, master);
    }else if(newtail->index == manager->tail->index){
        return;
    }else{
    newtail->prev->next = newtail->next;
    newtail->next->prev = newtail->prev;

    newtail->next = NULL;
    newtail->prev = manager->tail;
    manager->tail->next = newtail;
    manager->tail = newtail;
    }
}


/* given trace line, memory array, and pointer to memory, 
 * 'checks' cache and records valid bit and tag bits accordingly. 
 * updates number of hits, misses, and evictions. 
 * follows LRU replacement policy.
 */ 
void lineman(char* line, long long** mem, manager_t** master)
{
    long long num;
    char* newline = segstring(line, 3);
    num = strtol(newline, NULL, 16);
    long long setbits = getset(num);

    for(int i = 0; i < e; i++){
        if(isvalid(mem[setbits][i])){
            if(checktag(mem[setbits][i], num)){
                if(vflag){
                    printf("%s hit\n", line);
                }
                if(e > 1){
                updatepriority(setbits, i, master);
                }
                hits++;
                counter++;
                return;
            }
            continue;
        }else{
            mem[setbits][toppriority(setbits, master)] =  num^validbit;
            if(vflag){
                 printf("%s miss\n", line);
            }
            misses++;
            counter++;
            return;
        } 
    }
    mem[setbits][toppriority(setbits, master)] =  num^validbit;
    if(vflag){
    printf("%s miss evict\n", line);
    }
    misses++;
    evictions++;
    return;
}

//parses file with tracelines and processes each line one by one
void parser(char* filename, long long** mem, manager_t** master)
{
    FILE* tracefile = NULL;
    tracefile = fopen(filename, "r");
    if(!tracefile){
        fprintf(stderr, "error loading file");
        exit(4);
    }
    int i = 0;
    char* line = (char*) malloc(sizeof(char)*MAX_LEN);
    if(line == NULL){
        fprintf(stderr, "main: malloc failed\n");
        exit(5);
    }
    while(fgets(line, MAX_LEN, tracefile)){
        i++;
        line = strtok(line, ",");
        if(line[0] != 32){ //32 is the ASCII code for a space
            continue;
        }
        char action = line[1];
        if(action == 73){
            continue;
        }
        if(action ==  77){ //77 is the ASCII code for M
            lineman(line, mem, master);
            }
        lineman(line, mem, master);
    }
}

// prints usage info when optional -h flag is set
void hprint()
{
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n  -h         Print this help message.\n"
    "  -v         Optional verbose flag.\n"
    "  -s <num>   Number of set index bits.\n"
    "  -E <num>   Number of lines per set.\n"
    "  -b <num>   Number of block offset bits.\n"
    "  -t <file>  Trace file.\n");
    printf("Examples:\n  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
    "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char* argv[])
{
   int option;
   char* filename = (char*) malloc(sizeof(char)*MAX_LEN);
    if(filename == NULL){
        fprintf(stderr, "main - filename: malloc failed\n");
        exit(6);
    };

    while((option = getopt(argc, argv, "-::-s:-E:-b:-t:")) != -1){
        switch (option)
        {
            case '?':
            if(optopt == 'h'){
                hflag = 1;
            }else if(optopt == 'v'){
                vflag = 1;
            }else{
                fprintf(stderr, "Invalid option input: %c\n", optopt);
                exit(2);
            }
            break;
            case 's':
                s = strtol(optarg, NULL, 10);
                break;
            case 'E':
                e = strtol(optarg, NULL, 10);
                break;
            case 'b':
                b = strtol(optarg, NULL, 10);
                break;
            case 't':
                filename = optarg;
                break;
            default:
                fprintf(stderr, "Invalid arguements\n");
                exit(3);
        }
    }

    if(hflag){
        hprint();
    }

    int setnums = pow(2, s);

    tag = ADDRESS_BITS - (b + s);
    long long** memory = (long long**)malloc(sizeof(long long*)*setnums);
    if(memory == NULL){
        fprintf(stderr, "main - memory: malloc failed\n");
        exit(7);
    }

    for(int i = 0; i < setnums; i++){
        memory[i] = (long long*)malloc(sizeof(long long)*e);
        if(memory[i] == NULL){
        fprintf(stderr, "main - memory: malloc failed\n");
        exit(9);
        }
    }

    manager_t** master = (manager_t**)malloc(sizeof(manager_t*)*setnums);
    if(master == NULL){
        fprintf(stderr, "main - master: malloc failed\n");
        exit(12);
    }
    
    for(int i = 0; i < setnums; i++){
        master[i] = create_manager();
    }

    for(int i = 0; i < s; i++){
        for(int j = 0; j < e; j++){
            memory[i][j] = ((long long) 0);
        }
    }
    parser(filename, memory, master);

    for(int i = 0; i < setnums; i++){
        free_manager(master[i]);
    }
    free(master);
    for(int i = 0; i < setnums; i++){
        free(memory[i]);
    }
    free(memory);

    printSummary(hits, misses, evictions);
    return 0;
}
