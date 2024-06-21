#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "util.h"
#include "a_star.h"

/********* GRAPH *********/

/* graph_create: create a graph
 *
 * Returns: a graph
 */ 
graph_t *graph_create(int num_nodes)
{
    node_t** nodes = (node_t**)malloc(sizeof(node_t*) * num_nodes);
    if(nodes == NULL){
        fprintf(stderr, "graph_create - nodes: malloc failed\n");
        exit(1);
    }

    for(int i = 0; i < num_nodes; i++){
        nodes[i] = NULL;
    }

    graph_t* graph = (graph_t*)malloc(sizeof(graph_t));
    if(graph == NULL){
        fprintf(stderr, "graph_create - graph: malloc failed\n");
        exit(2);
    }
    graph->num_nodes = num_nodes;
    graph->nodes = nodes;

    return graph;
}

/* node_create: create a graph node
 *
 * graph: the graph
 * node_num: node number
 * city_name: the city
 * latitude: city latitude
 * longitude: city longitude
 * 
 */ 
void node_create(graph_t *graph, int node_num, char *city_name, 
                 double latitude, double longitude)
{    
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if(node == NULL){
        fprintf(stderr, "node_create: malloc failed\n");
        exit(3);
    }

    node->node_num = node_num;
    node->city_name = city_name;
    node->latitude = latitude;
    node->longitude = longitude;
    node->neighbors = NULL;
    node->parent = NULL;
    node->f_cost = 0;
    node->g_cost = 0;
    node->h_cost = 0;
    
    graph->nodes[node_num] = node;
}

/* Helper for add_edge below:
 *
 * edge_help: adds a neighbor to given node
 *
 * Inputs: 
 * - node: pointer to node (node_t*)
 * - edgenum: node number of neighbor node (int)
 * 
 * Output: none. function is void.
 */
void edge_help(node_t* node, int edgenum)
{
    intlist_t* neighbor = (intlist_t*)malloc(sizeof(intlist_t));
    if(neighbor == NULL){
        fprintf(stderr, "edge_help: malloc failed\n");
        exit(4);
    }

    neighbor->num = edgenum;
    neighbor->next = NULL;

    if(!node->neighbors){
        node->neighbors = neighbor;
    }else{
        intlist_t* lst = node->neighbors->next;
        intlist_t* prev = node->neighbors;
        while(lst){
            prev = lst;
            lst = lst->next;
        }
        prev->next = neighbor;
    }   
}

/* add_edge: add an edge between two nodes
 *
 * graph: the graph
 * node_num1: the first node
 * node_num2: the second node
 * 
 */ 
void add_edge(graph_t* graph, int node_num1, int node_num2)
{
    node_t* node1 = graph->nodes[node_num1];
    node_t* node2 = graph->nodes[node_num2];

    edge_help(node1, node_num2);
    edge_help(node2, node_num1);
}

/* graph_free: free a graph and its nodes
 *
 * graph: the graph
 */ 
void graph_free(graph_t* graph)
{
    for(int i = 0; i < graph->num_nodes; i++){
        if(graph->nodes[i]){
            intlist_t* curr = graph->nodes[i]->neighbors;
            intlist_t* next = NULL;
            while(curr){
                next = curr->next;
                free(curr);
                curr = next;
            }
            free(graph->nodes[i]);
        }
    }
    free(graph->nodes);
    free(graph);
}

/********* A* SEARCH *********/

/* Helpers for a_star below:
 *
 * h_calc: calculates h_cost of node
 * 
 * Inputs: 
 * - root: pointer to base node (node_t*) 
 * - end: pointer to end node (node_t*)
 * 
 * Output: Estimate of distance from base node to end node (double)
 */
double h_calc(node_t* root, node_t* end)
{
    double root_lat = root->latitude;
    double root_lon = root->longitude;
    double end_lat = end->latitude;
    double end_lon = end->longitude;

    return sqrt(pow((root_lon - end_lon), 2) + pow((root_lat - end_lat), 2));
} 

/* neighbor_help: calculates distance-related info for a given node
 * 
 * Inputs: 
 * - node: pointer to (neighbor) node (node_t*)
 * - curr: pointer to current (parent) node (node_t*)
 * - fin: pointer to final node (node_t*)
 * - q: pointer to priority queue (queue_t*)
 * - s: pointer to closed set (set_t*) 
 * 
 * Output: returns true if function meets exit conditions, 
 * false otherwise (bool)
 * exit conditions: 
 *  - if given node exists in priority queue with lower f_cost
 *  - if given node exists in closed set 
 */
bool dist(node_t* node, node_t* curr, node_t* fin, queue_t* q, set_t* s)
{
    double h_cost = h_calc(node, fin);
    double g_cost = h_calc(curr, node) + curr->g_cost;
    double f_cost = h_cost + g_cost;

    if(queue_query(q, node->node_num)){
        if(node->f_cost < f_cost){
            return true;
        }else{
            queue_change_priority(q, node->node_num, f_cost);
        }
    }

    if(set_query(s, node->node_num)){
        return true;
    }
    
    node->h_cost = h_cost;
    node->g_cost = g_cost;
    node->f_cost = f_cost;

    return false;
}

/* a_star: performs A* search
 *
 * graph: the graph
 * start_node_num: the staring node number
 * end_node_num: the ending node number
 * 
 * Returns: the distance of the path between the start node and end node
 */ 
double a_star(graph_t *graph, int start_node_num, int end_node_num)
{
    queue_t* q = queue_create();
    set_t* s = set_create();
   
    bool breakbool = false;
    intlist_t* neighbor_pt = NULL;
    node_t* neighbor = NULL;
    node_t* curr = graph->nodes[start_node_num];
    node_t* final = graph->nodes[end_node_num];

    queue_add(q, start_node_num, h_calc(curr, final));

    while(!queue_is_empty(q)){
        curr = graph->nodes[queue_remove(q)];
        if(curr->node_num == end_node_num){
            breakbool = true;
            break;
        }

        neighbor_pt = curr->neighbors;

        while(neighbor_pt){
            neighbor = graph->nodes[neighbor_pt->num];
            if(dist(neighbor, curr, final, q, s)){
                neighbor_pt = neighbor_pt->next;
                continue;
            }

            neighbor->parent = curr;
            queue_add(q, neighbor_pt->num, neighbor->f_cost);
            neighbor_pt = neighbor_pt->next;
        }
        
        set_add(s, curr->node_num);
    }

    set_free(s);
    queue_free(q);

    if(breakbool){
        return curr->g_cost;
    }else{    
        return -1;
    }
}