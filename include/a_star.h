/********* INTEGER LIST *********/

typedef struct intlist intlist_t;

struct intlist {
   int num;
   intlist_t *next;
};

/********* GRAPH STRUCTS *********/

typedef struct node node_t;
typedef struct graph graph_t;

struct node {
    int node_num;

    // represents a city
    char *city_name;
    double latitude;
    double longitude;

    // neighbors
    intlist_t *neighbors;

    // fields for A*
    node_t *parent;
    double f_cost;
    double g_cost;
    double h_cost;
};

struct graph {
    int num_nodes;
    node_t **nodes;
};

/********* GRAPH *********/

/* graph_create: create a graph
 *
 * Returns: a graph
 */ 
graph_t *graph_create(int num_nodes);

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
                 double latitude, double longitude);

/* add_edge: add an edge between two nodes
 *
 * graph: the graph
 * node_num1: the first node
 * node_num2: the second node
 * 
 */ 
void add_edge(graph_t* graph, int node_num1, int node_num2);

/* graph_free: free a graph and its nodes
 *
 * graph: the graph
 */ 
void graph_free(graph_t* graph);

/********* A* SEARCH *********/

/* a_star: performs A* search
 *
 * graph: the graph
 * start_node_num: the staring node number
 * end_node_num: the ending node number
 * 
 * Returns: the distance of the path between the start node and end node
 */ 
double a_star(graph_t *graph, int start_node_num, int end_node_num);