#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <criterion/criterion.h>

#include "a_star.h"
#include "util.h"

#define EPSILON (0.000001)
#define ERR_MSG_LEN (1000)

/* helper_graph_create
 *
 * num_nodes: number of nodes in graph
 * test_name: test name in error messages
 */
void helper_graph_create(int num_nodes, char *test_name)
{
    graph_t *actual = graph_create(num_nodes);
    char err_msg[ERR_MSG_LEN];

    snprintf(err_msg, ERR_MSG_LEN-1,
             ("\n  Function called in failed test:"
              "\n   -> graph_create(%d);\n"
              "\n  The filter to run this specific test is: --filter %s"), 
              num_nodes, test_name);

    // check graph nodes
    for(int i = 0; i < num_nodes; i++) {
        cr_assert_null(actual->nodes[i], 
                    " %s"
                    "\n      Actual nodes[%d] != NULL", err_msg, i);
    }
    
    // check graph fields
    cr_assert_eq(actual->num_nodes, num_nodes, 
                    " %s"
                    "\n      Actual num_nodes: %d"      
                    "\n      Expected num_nodes: %d ", err_msg, actual->num_nodes, num_nodes);

    graph_free(actual);
}

TestSuite(graph_create, .timeout=60);

Test(graph_create, testA) 
{
    helper_graph_create(2, "graph_create/testA");
}

Test(graph_create, testB) 
{
    helper_graph_create(5, "graph_create/testB");
}

/* helper_node_create
 *
 * num_nodes: number of graph nodes
 * node_num: node number
 * city_name: the city
 * latitude: city latitude
 * longitude: city longitude
 * test_name: test name in error messages
 */
void helper_node_create(int num_nodes, int node_num, char *city_name, 
                         double latitude, double longitude, char *test_name)
{
    graph_t *actual = graph_create(num_nodes);
    node_create(actual, node_num, city_name, latitude, longitude);
    char err_msg[ERR_MSG_LEN];

    snprintf(err_msg, ERR_MSG_LEN-1,
             ("\n  Functions called in failed test:"
              "\n      graph_t *g = graph_create(%d);"
              "\n   -> node_create(g, %d, '%s', %.1f, %.1f);\n"
              "\n  The filter to run this specific test is: --filter %s"), 
              num_nodes, node_num, city_name, latitude, longitude, test_name);

    node_t *node = actual->nodes[node_num];

    // check node fields
    cr_assert_not_null(node, 
                " %s"
                "\n      Actual nodes[%d]: NULL", err_msg, node_num);

    cr_assert_eq(node->node_num, node_num, 
                " %s"
                "\n      Actual node_num: %d"      
                "\n      Expected node_num: %d ", err_msg, node->node_num, node_num);
    cr_assert_str_eq(node->city_name, city_name, 
                " %s"
                "\n      Actual city_name: %d"      
                "\n      Expected city_name: %d ", err_msg, node->node_num, node_num);
    cr_assert_float_eq(node->latitude, latitude, 0.01, 
                " %s"
                "\n      Actual latitude: %.1f"      
                "\n      Expected latitude: %.1f ", err_msg, node->latitude, latitude);
    cr_assert_float_eq(node->longitude, longitude, 0.01, 
                " %s"
                "\n      Actual longitude: %.1f"      
                "\n      Expected longitude: %.1f ", err_msg, node->longitude, longitude);

    // check A* fields
    cr_assert_null(node->neighbors, 
                " %s"
                "\n      Actual neighbors != NULL", err_msg);
    cr_assert_null(node->parent, 
                " %s"
                "\n      Actual parent != NULL", err_msg);
    cr_assert_float_eq(node->f_cost, 0, 0.01, 
                " %s"
                "\n      Actual f_cost: %.1f"      
                "\n      Expected f_cost: 0 ", err_msg, node->f_cost);
    cr_assert_float_eq(node->g_cost, 0, 0.01, 
                " %s"
                "\n      Actual g_cost: %.1f"      
                "\n      Expected g_cost: 0 ", err_msg, node->g_cost);
    cr_assert_float_eq(node->h_cost, 0, 0.01,
                " %s"
                "\n      Actual h_cost: %.1f"      
                "\n      Expected h_cost: 0 ", err_msg, node->h_cost);

    graph_free(actual);
}

TestSuite(node_create, .timeout=60);

Test(node_create, testA) 
{
    helper_node_create(2, 0, "A", 0, 0, "node_create/testA");
}

Test(node_create, testB) 
{
    helper_node_create(2, 1, "B", 0, 1, "node_create/testB");
}

Test(node_create, testC) 
{
    helper_node_create(5, 0, "C", 1, 0, "node_create/testC");
}

/* helper_add_edge
 *
 * graph: the graph
 * node_num1: first node
 * node_num2: second node
 * test_string: string representation of graph call
 * test_name: test name in error messages
 */
void helper_add_edge(graph_t *graph, int node_num1, int node_num2, char *test_string, char *test_name)
{
    add_edge(graph, node_num1, node_num2);
    char err_msg[ERR_MSG_LEN];

    snprintf(err_msg, ERR_MSG_LEN-1,
             ("\n  Functions called in failed test:\n%s\n   -> add_edge(g, %d, %d);\n"
              "\n  The filter to run this specific test is: --filter %s"), test_string, node_num1, node_num2, test_name);

    node_t *node1 = graph->nodes[node_num1];
    node_t *node2 = graph->nodes[node_num2];

    bool found1 = false;
    bool found2 = false;

    intlist_t *neighbor = node1->neighbors;
    while(neighbor != NULL) {
        if(neighbor->num == node_num2) {
            found1 = true;
            break;
        }
        neighbor = neighbor->next;
    }

    neighbor = node2->neighbors;
    while(neighbor != NULL) {
        if(neighbor->num == node_num1) {
            found2 = true;
            break;
        }
        neighbor = neighbor->next;
    }

    cr_assert_eq(found1, true, " %s\n      Not found in node %d neighbors: %d", err_msg, node_num1, node_num2);
    cr_assert_eq(found2, true, " %s\n      Not found in node %d neighbors: %d", err_msg, node_num2, node_num1);
}

TestSuite(add_edge, .timeout=60);

Test(add_edge, testA) 
{   
    graph_t *graph = graph_create(2);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 1, 1);

    char *test = "      graph_t *g = graph_create(2);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 1, 1);";
    helper_add_edge(graph, 0, 1, test, "add_edge/testA");

    graph_free(graph);
}

Test(add_edge, testB) 
{   
    graph_t *graph = graph_create(5);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 1, 1);

    char *test = "      graph_t *g = graph_create(5);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 1, 1);";
    helper_add_edge(graph, 0, 1, test, "add_edge/testB");

    graph_free(graph);
}

Test(add_edge, testC) 
{   
    graph_t *graph = graph_create(5);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 1, 1);
    node_create(graph, 2, "C", 0, 2);
    add_edge(graph, 0, 1);

    char *test = "      graph_t *g = graph_create(5);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 1, 1);\n"
                 "      node_create(g, 2, 'C', 2, 2);\n"
                 "      add_edge(g, 0, 1);";
    helper_add_edge(graph, 0, 2, test, "add_edge/testC");

    graph_free(graph);
}

Test(add_edge, testD) 
{   
    graph_t *graph = graph_create(5);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 1, 1);
    node_create(graph, 2, "C", 0, 2);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);

    char *test = "      graph_t *g = graph_create(5);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 1, 1);\n"
                 "      node_create(g, 2, 'C', 2, 2);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);";
    helper_add_edge(graph, 1, 2, test, "add_edge/testD");

    graph_free(graph);
}

Test(add_edge, testE) 
{   
    graph_t *graph = graph_create(5);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 1, 1);
    node_create(graph, 2, "C", 0, 2);
    node_create(graph, 4, "D", 2, 0);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 2);

    char *test = "      graph_t *g = graph_create(5);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 1, 1);\n"
                 "      node_create(g, 2, 'C', 2, 2);\n"
                 "      node_create(g, 4, 'D', 2, 0);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 0, 2);\n"
                 "      add_edge(g, 1, 2);";
    helper_add_edge(graph, 1, 4, test, "add_edge/testE");

    graph_free(graph);
}

/* helper_a_star
 *
 * graph: the graph
 * start_node: start node
 * end_node: send node
 * expected: expected output
 * test_string: string representation of graph call
 * test_name: test name in error messages
 */
void helper_a_star(graph_t *graph, int start_node, int end_node, double expected, char *test_string, char *test_name)
{
    double actual = a_star(graph, start_node, end_node);
    char err_msg[ERR_MSG_LEN];

    snprintf(err_msg, ERR_MSG_LEN-1,
             ("\n  Functions called in failed test:\n%s\n   -> a_star(g, %d, %d);\n"
              "\n  The filter to run this specific test is: --filter %s"), test_string, start_node, end_node, test_name);

    cr_assert_float_eq(actual, expected, 0.01, " %s\n      Actual: %.1f\n      Expected: %.1f ", err_msg, actual, expected);
}

TestSuite(a_star, .timeout=60);

Test(a_star, testA) 
{   
    graph_t *graph = graph_create(2);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 0, 1);
    add_edge(graph, 0, 1);

    char *test = "      graph_t *g = graph_create(2);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 0, 1);\n"
                 "      add_edge(g, 0, 1);";
    helper_a_star(graph, 0, 1, 1, test, "a_star/testA");

    graph_free(graph);
}

Test(a_star, testB) 
{   
    graph_t *graph = graph_create(3);
    node_create(graph, 0, "A", 1, 0);
    node_create(graph, 1, "B", 0, 0);
    node_create(graph, 2, "C", 0, 1);
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);

    char *test = "      graph_t *g = graph_create(3);\n"
                 "      node_create(g, 0, 'A', 1, 0);\n"
                 "      node_create(g, 1, 'B', 0, 0);\n"
                 "      node_create(g, 2, 'C', 0, 1);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);";
    helper_a_star(graph, 0, 2, 2, test, "a_star/testB");

    graph_free(graph);
}

Test(a_star, testC) 
{   
    graph_t *graph = graph_create(3);
    node_create(graph, 0, "A", 0, 1);
    node_create(graph, 1, "B", 0, 0);
    node_create(graph, 2, "C", 1, 0);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 2);

    char *test = "      graph_t *g = graph_create(3);\n"
                 "      node_create(g, 0, 'A', 1, 0);\n"
                 "      node_create(g, 1, 'B', 0, 0);\n"
                 "      node_create(g, 2, 'C', 0, 1);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);\n"
                 "      add_edge(g, 0, 2);";
    helper_a_star(graph, 0, 2, 1.41421, test, "a_star/testC");

    graph_free(graph);
}

Test(a_star, testD) 
{   
    graph_t *graph = graph_create(4);
    node_create(graph, 0, "A", 0, 1);
    node_create(graph, 1, "B", 0, 0);
    node_create(graph, 2, "C", 1, 0);
    node_create(graph, 3, "D", 1, -1);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 3);


    char *test = "      graph_t *g = graph_create(4);\n"
                 "      node_create(g, 0, 'A', 1, 0);\n"
                 "      node_create(g, 1, 'B', 0, 0);\n"
                 "      node_create(g, 2, 'C', 0, 1);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);\n"
                 "      add_edge(g, 0, 2);\n"
                 "      add_edge(g, 2, 3);";
    helper_a_star(graph, 0, 3, 2.41421, test, "a_star/testD");

    graph_free(graph);
}

Test(a_star, testE) 
{   
    graph_t *graph = graph_create(4);
    node_create(graph, 0, "A", 0, 1);
    node_create(graph, 1, "B", 0, 0);
    node_create(graph, 2, "C", 1, 0);
    node_create(graph, 3, "D", 1, -1);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 3);


    char *test = "      graph_t *g = graph_create(4);\n"
                 "      node_create(g, 0, 'A', 1, 0);\n"
                 "      node_create(g, 1, 'B', 0, 0);\n"
                 "      node_create(g, 2, 'C', 0, 1);\n"
                 "      node_create(g, 3, 'D', 1, -1);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);\n"
                 "      add_edge(g, 0, 2);\n"
                 "      add_edge(g, 2, 3);";
    helper_a_star(graph, 3, 0, 2.41421, test, "a_star/testE");

    graph_free(graph);
}

Test(a_star, testF) 
{   
    graph_t *graph = graph_create(9);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 1, 0);
    node_create(graph, 2, "C", 2, 0);
    node_create(graph, 3, "D", 3, 0);
    node_create(graph, 4, "E", 3, 1);
    node_create(graph, 5, "F", 3, 2);
    node_create(graph, 6, "G", 2, 2);
    node_create(graph, 7, "H", 1, 2);
    node_create(graph, 8, "I", 0, 2);
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 3);
    add_edge(graph, 3, 4);
    add_edge(graph, 4, 5);
    add_edge(graph, 5, 6);
    add_edge(graph, 6, 7);
    add_edge(graph, 7, 8);

    char *test = "      graph_t *g = graph_create(9);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 1, 0);\n"
                 "      node_create(g, 2, 'C', 2, 0);\n"
                 "      node_create(g, 3, 'D', 3, 0);\n"
                 "      node_create(g, 4, 'E', 3, 1);\n"
                 "      node_create(g, 5, 'F', 3, 2);\n"
                 "      node_create(g, 6, 'G', 2, 2);\n"
                 "      node_create(g, 7, 'H', 1, 2);\n"
                 "      node_create(g, 8, 'I', 0, 2);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);\n"
                 "      add_edge(g, 2, 3);\n"
                 "      add_edge(g, 3, 4);\n"
                 "      add_edge(g, 4, 5);\n"
                 "      add_edge(g, 5, 6);\n"
                 "      add_edge(g, 6, 7);\n"
                 "      add_edge(g, 7, 8);";
    helper_a_star(graph, 2, 8, 6, test, "a_star/testF");

    graph_free(graph);
}

Test(a_star, testG) 
{   
    graph_t *graph = graph_create(3);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 0, 1);
    node_create(graph, 2, "C", 0, 2);
    add_edge(graph, 0, 1);

    char *test = "      graph_t *g = graph_create(3);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 0, 1);\n"
                 "      node_create(g, 2, 'C', 0, 2);\n"
                 "      add_edge(g, 0, 1);";
    helper_a_star(graph, 0, 2, -1, test, "a_star/testG");

    graph_free(graph);
}

Test(a_star, testH) 
{   
    graph_t *graph = graph_create(8);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 1, 0);
    node_create(graph, 2, "C", 2, 0);
    node_create(graph, 3, "D", 3, 0);
    node_create(graph, 4, "E", 1, 1);
    node_create(graph, 5, "F", 2, 1);
    node_create(graph, 6, "G", 3, 1);
    node_create(graph, 7, "H", 3, 2);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 4);
    add_edge(graph, 1, 2);
    add_edge(graph, 1, 4);
    add_edge(graph, 2, 3);
    add_edge(graph, 2, 4);
    add_edge(graph, 2, 5);
    add_edge(graph, 4, 5);
    add_edge(graph, 3, 6);
    add_edge(graph, 6, 7);

    char *test = "      graph_t *g = graph_create(8);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 1, 0);\n"
                 "      node_create(g, 2, 'C', 2, 0);\n"
                 "      node_create(g, 3, 'D', 3, 0);\n"
                 "      node_create(g, 4, 'E', 1, 1);\n"
                 "      node_create(g, 5, 'F', 2, 1);\n"
                 "      node_create(g, 6, 'G', 3, 1);\n"
                 "      node_create(g, 7, 'H', 3, 2);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 0, 4);\n"
                 "      add_edge(g, 1, 2);\n"
                 "      add_edge(g, 1, 4);\n"
                 "      add_edge(g, 2, 3);\n"
                 "      add_edge(g, 2, 4);\n"
                 "      add_edge(g, 4, 5);\n"
                 "      add_edge(g, 3, 6);\n"
                 "      add_edge(g, 6, 7);";
    helper_a_star(graph, 0, 7, 5, test, "a_star/testH");

    graph_free(graph);
}

void helper_a_star_parent(graph_t *graph, int start_node, int end_node, int expected_parent, char *test_string, char *test_name)
{
    a_star(graph, start_node, end_node);
    int actual_parent = graph->nodes[end_node]->parent->node_num;
    char err_msg[ERR_MSG_LEN];

    snprintf(err_msg, ERR_MSG_LEN-1,
             ("\n  Functions called in failed test:\n%s\n   -> a_star(g, %d, %d);\n"
              "\n  The filter to run this specific test is: --filter %s"), test_string, start_node, end_node, test_name);

    cr_assert_eq(actual_parent, expected_parent, " %s\n      Actual parent: %d\n      Expected parent: %d ", err_msg, actual_parent, expected_parent);
}

TestSuite(a_star_parent, .timeout=60);

Test(a_star_parent, testA) 
{   
    graph_t *graph = graph_create(2);
    node_create(graph, 0, "A", 0, 0);
    node_create(graph, 1, "B", 0, 1);
    add_edge(graph, 0, 1);

    char *test = "      graph_t *g = graph_create(2);\n"
                 "      node_create(g, 0, 'A', 0, 0);\n"
                 "      node_create(g, 1, 'B', 0, 1);\n"
                 "      add_edge(g, 0, 1);";
    helper_a_star_parent(graph, 0, 1, 0, test, "a_star_parent/testA");

    graph_free(graph);
}

Test(a_star_parent, testB) 
{   
    graph_t *graph = graph_create(3);
    node_create(graph, 0, "A", 1, 0);
    node_create(graph, 1, "B", 0, 0);
    node_create(graph, 2, "C", 0, 1);
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);

    char *test = "      graph_t *g = graph_create(3);\n"
                 "      node_create(g, 0, 'A', 1, 0);\n"
                 "      node_create(g, 1, 'B', 0, 0);\n"
                 "      node_create(g, 2, 'C', 0, 1);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);";
    helper_a_star_parent(graph, 0, 2, 1, test, "a_star_parent/testB");

    graph_free(graph);
}

Test(a_star_parent, testC) 
{   
    graph_t *graph = graph_create(3);
    node_create(graph, 0, "A", 0, 1);
    node_create(graph, 1, "B", 0, 0);
    node_create(graph, 2, "C", 1, 0);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 2);

    char *test = "      graph_t *g = graph_create(3);\n"
                 "      node_create(g, 0, 'A', 1, 0);\n"
                 "      node_create(g, 1, 'B', 0, 0);\n"
                 "      node_create(g, 2, 'C', 0, 1);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);\n"
                 "      add_edge(g, 0, 2);";
    helper_a_star_parent(graph, 0, 2, 0, test, "a_star_parent/testC");

    graph_free(graph);
}

Test(a_star_parent, testD) 
{   
    graph_t *graph = graph_create(4);
    node_create(graph, 0, "A", 0, 1);
    node_create(graph, 1, "B", 0, 0);
    node_create(graph, 2, "C", 1, 0);
    node_create(graph, 3, "D", 1, -1);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 3);


    char *test = "      graph_t *g = graph_create(4);\n"
                 "      node_create(g, 0, 'A', 1, 0);\n"
                 "      node_create(g, 1, 'B', 0, 0);\n"
                 "      node_create(g, 2, 'C', 0, 1);\n"
                 "      add_edge(g, 0, 1);\n"
                 "      add_edge(g, 1, 2);\n"
                 "      add_edge(g, 0, 2);\n"
                 "      add_edge(g, 2, 3);";
    helper_a_star_parent(graph, 0, 3, 2, test, "a_star_parent/testD");

    graph_free(graph);
}