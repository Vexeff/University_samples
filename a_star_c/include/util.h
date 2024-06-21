/********* SET AND PRIORITY QUEUE TYPES *********/

typedef struct set set_t;
typedef struct queue queue_t;

/********* SET *********/

/* set_create: create a set
 *
 * Returns: a set
 */ 
set_t *set_create();

/* set_add: add a number to the set
 *
 * set: the set
 * num: the number to add to the set
 */ 
void set_add(set_t *set, int num);

/* set_query: determines whether a number is in the set
 *
 * set: the set
 * num: the number to query
 * 
 * Returns: true if the number is in the set, false otherwise
 */ 
bool set_query(set_t *set, int num);

/* set_free: free a set
 *
 * set: the set
 * 
 */ 
void set_free(set_t *set);

/* set_print: print the elements in a set
 *
 * set: the set
 * 
 */ 
void set_print(set_t *set);

/********* PRIORITY QUEUE *********/

/* queue_create: create a priority queue
 *
 * Returns: a priority queue
 */ 
queue_t *queue_create();

/* queue_add: add an element to the priority queue
 * 
 * q: the priority queue
 * num: the number to add to the queue
 * priority: priority of num
 */ 
void queue_add(queue_t *q, int num, double priority);

/* queue_remove: remove an element from the priority queue
 * 
 * q: the priority queue
 * 
 * Returns: the number removed from the priority queue
 */ 
int queue_remove(queue_t *q);

/* queue_query: determines whether a number is in the priority queue
 * 
 * q: the priority queue
 * num: the number to query
 * 
 * Returns: true if num is in the queue, false otherwise
 */ 
bool queue_query(queue_t *q, int num);

/* queue_is_empty: determines whether or not the queue is empty
 * 
 * q: the priority queue
 * 
 * Returns: true if the queue is empty, false otherwise
 */ 
bool queue_is_empty(queue_t *q);

/* queue_change_priority: change the priority of an element 
 *     in the priority queue
 * 
 * q: the priority queue
 * num: the number to change
 * new_priority: new priority of num
 * 
 */ 
void queue_change_priority(queue_t *q, int num, double new_priority);

/* queue_free: free a priority queue and its elements
 * 
 * q: the priority queue
 */ 
void queue_free(queue_t *q);

/* queue_print: print the elements in a priority queue
 * 
 * q: the priority queue
 */ 
void queue_print(queue_t *q);