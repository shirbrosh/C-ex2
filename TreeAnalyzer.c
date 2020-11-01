#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

#define FAIL -1
#define SUCCESS 0
#define N_LINE 1
#define SPACE " "
#define NO_ENTER "%[^\n]\n"
#define NO_R "%[^\r]\n"
#define HYPHEN "-"
#define HYPHEN_ENTER "-\n"
#define HYPHEN_ENTER_R "-\r\n"
#define HYPHEN_EOF "-eof"

#define ARGUMENT_MSG "USAGE:    TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n"
#define FILE_MSG "Cannot open the file"
#define INVALID_INPUT_MSG "Invalid input\n"

#define MAX_LINE 1024
#define MAX_NEIGHBORS 512
#define PRINT_ROOT "Root Vertex: %d\n"
#define PRINT_VER "Vertices Count: %d\n"
#define PRINT_EDGE "Edges Count: %d\n"
/**
 * The node struct
 */
typedef struct Node
{
    int children_amount;
    int parent;
    int key;
    bool is_leaf;
    int dist;
    int prev;
    int *children_arr;
} Node;

/**
 * The tree struct
 */
typedef struct Tree
{
    int root;
    int max_height;
    int node_amount;
    Node *node_arr;
} Tree;


Node initializeNode()
{
    Node new_node;
    new_node.children_amount = 0;
    new_node.parent = -1;
    new_node.is_leaf = false;
    return new_node;
}


int convertToNumber(char *line)
{
    for (size_t i = 0; i < strlen(line); i++)
    {
        if (isdigit(line[i]) == 0)
        {
            return FAIL;
        }
    }
    char *ptr;
    int n = (int) strtod(line, &ptr);
    return n;
}

void removeEnter(char *line, char *line_without_enter)
{
    sscanf(line, NO_ENTER, line_without_enter);
    sscanf(line, NO_R, line_without_enter);

}


/**
 * This function checks if the n input is valid - if not print a matching message and return -1,
 * if valid calculates the given number of vertexes and return it
 * @param line - the first line of the file containing the number of vertexes
 * @return -1 if not valid and the given number of vertexes if valid
 */
int initializeN(char *line)
{
    int n = convertToNumber(line);
    if (n <= 0)
    {
        fprintf(stderr, INVALID_INPUT_MSG);
        return FAIL;
    }
    return n;
}


/**
 * This function checks that the received file is not empty
 * @param fp - the pointer to the received file
 * @return -1 if thee file is empty or 0 if not
 */
int fileNotEmpty(FILE *fp)
{
    long f_size = 0;

    // Go to to the end of the file
    fseek(fp, 0, SEEK_END);
    f_size = ftell(fp);
    if (f_size == 0)
    {
        return FAIL;
    }
    return SUCCESS;
}


int checkRowUpdateNode(char *token, Tree *myTree, int key_of_line)
{
    if (strcmp(token, HYPHEN) == 0 || strcmp(token, HYPHEN_ENTER) == 0 ||
        strcmp(token, HYPHEN_ENTER_R) == 0 ||
        strcmp(token, HYPHEN_EOF) == 0)
    {
        myTree->node_arr[key_of_line].children_amount = 0;
        myTree->node_arr[key_of_line].children_arr = NULL;
        myTree->node_arr[key_of_line].is_leaf = true;
        return SUCCESS;
    }
    int num = convertToNumber(token);

    //check if the received num is in the right range and not the key_of_line itself
    if (num < 0 || num > myTree->node_amount - 1 || num == key_of_line)
    {
        fprintf(stderr, INVALID_INPUT_MSG);
        return FAIL;
    }

    // check if a node already have a parent
    if (myTree->node_arr[num].parent != -1)
    {
        fprintf(stderr, INVALID_INPUT_MSG);
        return FAIL;
    }
    myTree->node_arr[num].parent = key_of_line;
    myTree->node_arr[key_of_line].children_amount += 1;
    if (myTree->node_arr[key_of_line].children_amount == 1)
    {
        myTree->node_arr[key_of_line].children_arr = (int *) malloc(sizeof(int));
    }
    else
    {
        myTree->node_arr[key_of_line].children_arr = (int *) realloc(myTree->node_arr[key_of_line]
                                                                             .children_arr,
                                                                     myTree->node_arr[key_of_line].children_amount *
                                                                     sizeof(int));
    }
    myTree->node_arr[key_of_line].children_arr[myTree->node_arr[key_of_line].children_amount -
                                               1] = num;
    return SUCCESS;
}

int parsing(Tree *myTree, char *token, bool initialize_first_line, int line_counter)
{
    while (token != NULL)
    {
        //checks if the tree had been already initialized
        if (initialize_first_line && line_counter == N_LINE)
        {
            fprintf(stderr, INVALID_INPUT_MSG);
            return FAIL;
        }

        //check if the n input is valid and initialize it
        if (line_counter == N_LINE)
        {
            int n = initializeN(token);
            initialize_first_line = true;
            if (n != FAIL)
            {
                myTree->node_amount = n;
                myTree->node_arr = (Node *) malloc(myTree->node_amount * sizeof(Node));
                for (int i = 0; i < n; i++)
                {
                    Node new_node = initializeNode();
                    myTree->node_arr[i] = new_node;
                }
            }
            else
            {
                return FAIL;
            }
        }
        else
        {
            int key_of_line = line_counter - 2;
            myTree->node_arr[key_of_line].key = key_of_line;
            int valid = checkRowUpdateNode(token, myTree, key_of_line);
            if (valid == FAIL)
            {
                return FAIL;
            }
        }
        token = strtok(NULL, SPACE);
    }
    return SUCCESS;
}

int findRoot(Tree *myTree)
{
    for (int i = 0; i < myTree->node_amount; i++)
    {
        if (myTree->node_arr[i].parent == -1)
        {
            myTree->root = myTree->node_arr[i].key;
            printf(PRINT_ROOT, myTree->node_arr[i].key);
            return SUCCESS;
        }
    }
    //if the function could'nt find a root- there a loop in the tree
    fprintf(stderr, INVALID_INPUT_MSG);
    return FAIL;
}

void printNumNodes(Tree *myTree)
{
    printf(PRINT_VER, myTree->node_amount);
}

void printNumEdges(Tree *myTree)
{
    printf(PRINT_EDGE, myTree->node_amount - 1);
}

void createNeighbors(Node *u, int *neighbors_arr)
{
    neighbors_arr[u->children_amount] = u->parent;
    for (int i = 0; i < u->children_amount; i++)
    {
        neighbors_arr[i] = u->children_arr[i];
    }
}

void BFS(Tree *myTree, Node *start)
{
    for (int i = 0; i < myTree->node_amount; i++)
    {
        myTree->node_arr[i].dist = -1;
    }
    start->dist = 0;
    start->prev = -1;
    Queue *qu = allocQueue();
    enqueue(qu, start->key);
    while (!queueIsEmpty(qu))
    {
        Node u = myTree->node_arr[dequeue(qu)];
        int neighbors_num = u.children_amount + 1;
        int neighbors_arr[MAX_NEIGHBORS];
        createNeighbors(&u, neighbors_arr);
        for (int i = 0; i < neighbors_num; i++)
        {
            if (myTree->node_arr[neighbors_arr[i]].dist == -1)
            {
                enqueue(qu, neighbors_arr[i]);
                myTree->node_arr[neighbors_arr[i]].prev = u.key;
                myTree->node_arr[neighbors_arr[i]].dist = u.dist + 1;
            }
        }
    }
    freeQueue(&qu);
}

void findMinMaxBranch(Tree *myTree)
{
    BFS(myTree, &myTree->node_arr[myTree->root]);
    int min_dist = myTree->node_amount;
    int max_dist = 0;
    for (int i = 0; i < myTree->node_amount; i++)
    {
        if (myTree->node_arr[i].is_leaf)
        {
            if (myTree->node_arr[i].dist < min_dist)
            {
                min_dist = myTree->node_arr[i].dist;
            }
            if (myTree->node_arr[i].dist > max_dist)
            {
                max_dist = myTree->node_arr[i].dist;
                myTree->max_height = myTree->node_arr[i].key;
            }
        }
    }
    printf("Length of Minimal Branch: %d\n", min_dist);
    printf("Length of Maximal Branch: %d\n", max_dist);
}

void findDiameter(Tree *myTree)
{
    BFS(myTree, &myTree->node_arr[myTree->max_height]);
    int diam = 0;
    for (int i = 0; i < myTree->node_amount; i++)
    {
        if (myTree->node_arr[i].dist > diam)
        {
            diam = myTree->node_arr[i].dist;
        }
    }
    printf("Diameter Length: %d\n", diam);
}


void findShortestPath(Node *u, Node *v, Tree *myTree)
{
    BFS(myTree, v);
    int dist_u_v = myTree->node_arr[u->key].dist;
    printf("Shortest Path Between %d and %d: %d", u->key, v->key, u->key);
    do
    {
        *u = myTree->node_arr[u->prev];
        printf(" %d", u->key);
    } while (myTree->node_arr[u->key].prev != v->key);
    printf(" %d", v->key);
}

void deleteMallocs(Tree *myTree)
{
    for (int i = 0; i < myTree->node_amount; i++)
    {
        free(&myTree->node_arr[i].children_arr);
        myTree->node_arr[i].children_arr = NULL;
    }
    free(myTree->node_arr);
    myTree->node_arr = NULL;
}

int main(int argc, char *argv[])
{
    //check that the user entered the exact amount of arguments
    if (argc != 4)
    {
        fprintf(stderr, ARGUMENT_MSG);
        exit(EXIT_FAILURE);
    }
    FILE *fp;
    fp = fopen(argv[1], "r");

    //checks that there is a file
    if (fp == NULL)
    {
        fclose(fp);
        fprintf(stderr, FILE_MSG);
        exit(EXIT_FAILURE);
    }

    //checks that the file is not empty
    if (fileNotEmpty(fp) == FAIL)
    {
        fclose(fp);
        fprintf(stderr, INVALID_INPUT_MSG);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    fp = fopen(argv[1], "r");

    //initial check that the received arguments u,v are numbers and bigger then zero
    int u_key = convertToNumber(argv[2]);
    int v_key = convertToNumber(argv[3]);
    if (u_key < 0 || v_key < 0)
    {
        fprintf(stderr, INVALID_INPUT_MSG);
        exit(EXIT_FAILURE);
    }

    Tree myTree;
    char line[MAX_LINE];
    int line_counter = N_LINE;
    bool initialize_first_line = false;

    //loops while there's another line to read from the file
    while (fgets(line, MAX_LINE, fp) != NULL)
    {

        char line_without_enter[MAX_LINE];
        removeEnter(line, line_without_enter);
        char *token = strtok(line_without_enter, SPACE);
        if (parsing(&myTree, token, initialize_first_line, line_counter) == FAIL)
        {
            deleteMallocs(&myTree);
            exit(EXIT_FAILURE);
        }
        line_counter++;
    }
    if (myTree.node_amount != line_counter - 2)
    {
        deleteMallocs(&myTree);
        fprintf(stderr, INVALID_INPUT_MSG);
        exit(EXIT_FAILURE);
    }

    //check if the received arguments u,v are indeed part of the tree
    if (u_key > myTree.node_amount - 1 || v_key > myTree.node_amount - 1)
    {
        deleteMallocs(&myTree);
        fprintf(stderr, INVALID_INPUT_MSG);
        exit(EXIT_FAILURE);
    }

    if (findRoot(&myTree) == -1)
    {
        deleteMallocs(&myTree);
        exit(EXIT_FAILURE);
    }
    printNumNodes(&myTree);
    printNumEdges(&myTree);
    findMinMaxBranch(&myTree);
    findDiameter(&myTree);
    findShortestPath(&myTree.node_arr[u_key], &myTree.node_arr[v_key], &myTree);
    fclose(fp);
}





