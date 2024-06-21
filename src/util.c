#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "util.h"

/********* SET *********/

#define SIZE 20

struct set {
    bool elements[SIZE];
};

/* set_create: create a set
 *
 * Returns: a set
 */ 
set_t *set_create()
{
    set_t *set = (set_t*)malloc(sizeof(set_t));
    for(int i = 0; i < SIZE; i++) {
        set->elements[i] = false;
    }

    return set;
}

/* set_add: add a number to the set
 *
 * set: the set
 * num: the number to add to the set
 */ 
void set_add(set_t *set, int num)
{
    assert(set != NULL);
    assert(num < SIZE);
    set->elements[num] = true;
}

/* set_query: determines whether a number is in the set
 *
 * set: the set
 * num: the number to query
 * 
 * Returns: true if the number is in the set, false otherwise
 */ 
bool set_query(set_t *set, int num)
{
    assert(set != NULL);
    assert(num < SIZE);
    return set->elements[num];
}

/* set_free: free a set
 *
 * set: the set
 * 
 */ 
void set_free(set_t *set)
{
    assert(set != NULL);
    free(set);
}

/* set_print: print the elements in a set
 *
 * set: the set
 * 
 */ 
void set_print(set_t *set)
{
    printf("set:\n");
    for(int i = 0; i < SIZE; i++) {
        if(set->elements[i]) {
            printf("    %d\n", i);
        }
    }
}

/********* PRIORITY QUEUE *********/

typedef struct queue_element qelement_t;

struct queue_element {
    int num;
    double priority;
    qelement_t *next;
};

struct queue {
    qelement_t *front;
};

/* queue_create: create a priority queue
 *
 * Returns: a priority queue
 */ 
queue_t *queue_create()
{
    queue_t *q = (queue_t*)malloc(sizeof(queue_t));
    q->front = NULL;
    return q;
}

/* new_element: create a new element in the priority queue
 *
 * num: the number to add to the queue
 * priority: the priority of num
 * 
 * Returns: the new element
 */ 
qelement_t *new_element(int num, double priority)
{
    qelement_t *element = (qelement_t*)malloc(sizeof(qelement_t));
    element->num = num;
    element->priority = priority;
    element->next = NULL;

    return element;
}

/* queue_add: add an element to the priority queue
 * 
 * q: the priority queue
 * num: the number to add to the queue
 * priority: priority of num
 */ 
void queue_add(queue_t *q, int num, double priority)
{
    assert(q != NULL);

    qelement_t *element = new_element(num, priority);

    // the queue is empty
    if(q->front == NULL) {
        q->front = element;
        return;
    }

    // the queue is not empty
    if(q->front->priority > priority) { // new front
        element->next = q->front;
        q->front = element;
    } else {
        qelement_t *curr = q->front;
        while((curr->next != NULL) && (curr->next->priority < priority)){ // find position
            curr = curr->next;
        }
        element->next = curr->next;
        curr->next = element;
    }
}

/* queue_remove: remove an element from the priority queue
 * 
 * q: the priority queue
 * 
 * Returns: the number removed from the priority queue
 */ 
int queue_remove(queue_t *q)
{
    assert(q != NULL);
    assert(q->front != NULL); // queue is empty

    // previous front
    qelement_t *temp = q->front;
    q->front = q->front->next;

    // return numue of first 
    int num = temp->num;
    free(temp);
    return num;
}

/* queue_query: determines whether a number is in the priority queue
 * 
 * q: the priority queue
 * num: the number to query
 * 
 * Returns: true if num is in the queue, false otherwise
 */ 
bool queue_query(queue_t *q, int num)
{
    assert(q != NULL);
    qelement_t *curr = q->front;
    while(curr != NULL){ 
        if(curr->num == num) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

/* queue_is_empty: determines whether or not the queue is empty
 * 
 * q: the priority queue
 * 
 * Returns: true if the queue is empty, false otherwise
 */ 
bool queue_is_empty(queue_t *q)
{
    assert(q != NULL);
    return q->front == NULL;
}

/* queue_change_priority: change the priority of an element 
 *     in the priority queue
 * 
 * q: the priority queue
 * num: the number to change
 * new_priority: new priority of num
 * 
 */ 
void queue_change_priority(queue_t *q, int num, double new_priority)
{
    assert(q != NULL);
    qelement_t *prev;
    qelement_t *curr = q->front;

    // first element
    if(curr->num == num) {
        queue_remove(q);
        queue_add(q, num, new_priority);
        return;
    }

    prev = curr;
    curr = curr->next;
    while(curr != NULL){ 
        if(curr->num == num){

            // found the element
            prev->next = curr->next;
            free(curr);
            queue_add(q, num, new_priority);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

/* queue_free: free a priority queue and its elements
 * 
 * q: the priority queue
 * 
 */ 
void queue_free(queue_t *q)
{
    assert(q != NULL);
    qelement_t *curr = q->front;
    qelement_t *next;
    while(curr != NULL){ 
        next = curr->next;
        free(curr);
        curr = next;
    }
    free(q);
}

/* queue_print: print the elements in a priority queue
 * 
 * q: the priority queue
 * 
 */ 
void queue_print(queue_t *q)
{
    assert(q != NULL);
    printf("queue:\n");
    if(q->front == NULL) {
        printf("    empty\n");
    } else {
        qelement_t *curr = q->front;
        while(curr != NULL){ 
            printf("    (%d, %.2f)\n", curr->num, curr->priority);
            curr = curr->next;
        }
    }
}