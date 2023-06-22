#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARENT(i) (i - 1) / 2
#define LEFT_CHILD(i) 2 * (i) + 1
#define RIGHT_CHILD(i) 2 * (i) + 2

#define CHAR_BITSIZE 8
#define ASCII_SIZE 256
#define PRIORITY_QUEUE_CAPACITY ASCII_SIZE


typedef struct Node {
    char letter;
    int frequence;

    int isLeaf;
    struct Node* leftChild;
    struct Node* rightChild;
} Node;

typedef struct PriorityQueue {
    Node* data[PRIORITY_QUEUE_CAPACITY];
    int size;
} PriorityQueue;


Node* new_node(char letter, int frequence, int isLeaf, Node* leftChild, Node* rightChild);
void delete_node(Node* n);
PriorityQueue* new_priority_queue();
void push(PriorityQueue* priority_queue, Node* data);
Node* pop(PriorityQueue* priority_queue);

Node* create_haffman_tree(PriorityQueue* priority_queue);
void encode(Node* haffman_tree_root, char* string, char haffman_tree_dictionary[ASCII_SIZE][CHAR_BITSIZE + 1]);
void decode(Node* haffman_tree_root);


/************************************** DEBUG OUTPUT *******************************************/

void print_node(Node* node);
void print_priority_queue(PriorityQueue* priority_queue);
void print_haffman_tree_dictionary(char haffman_tree_dictionary[ASCII_SIZE][CHAR_BITSIZE + 1]);

/****************************************** MAIN ***********************************************/


int main(int argc, char* argv[])
{
    // DEBUG INPUT
    FILE* input;
    input = fopen("tests/input.txt", "r");
    char* string;
    fscanf(input, "%m[^\n]*", &string);
    fclose(input);

    // frequency table initialization
    int frequencies[ASCII_SIZE] = { 0 };
    for (int i = 0; i < strlen(string); ++i)
        ++frequencies[string[i]];

    // priority queue initialization
    PriorityQueue* priority_queue = new_priority_queue();
    for (int i = 0; i < ASCII_SIZE; ++i) {
        if (frequencies[i] != 0) 
            push(priority_queue, new_node(i, frequencies[i], 1, NULL, NULL));
    }
    print_priority_queue(priority_queue);

    // building a huffman tree
    Node* haffman_tree_root = create_haffman_tree(priority_queue);

    // huffman tree dictionary initialization
    char haffman_tree_dictionary[ASCII_SIZE][CHAR_BITSIZE + 1] = { 0 };
    encode(haffman_tree_root, string, haffman_tree_dictionary);
    print_haffman_tree_dictionary(haffman_tree_dictionary);

    // DEBUG OUTPUT
    FILE* output = fopen("tests/output.txt", "w");
    for (int i = 0; i < strlen(string); ++i)
        fprintf(output, "%s", haffman_tree_dictionary[string[i]]);
    fclose(output);
    output = fopen("tests/huffman_tree_dictionary.txt", "w");
    for (int i = 0; i < ASCII_SIZE; ++i)
        if (haffman_tree_dictionary[i][0] != '\0')
            fprintf(output, "%c %s\n", i, haffman_tree_dictionary[i]);
    fclose(output);
    
    // MEMORY CLEARING
    free(string);

    return 0;
}

/***********************************************************************************************/


void swap(Node** node1, Node** node2)
{
    Node* temp = *node1;
    *node1 = *node2;
    *node2 = temp;
}


PriorityQueue* new_priority_queue()
{
    PriorityQueue* priority_queue = malloc(sizeof(PriorityQueue));
    priority_queue->size = 0;
    for (int i = 0; i < PRIORITY_QUEUE_CAPACITY; ++i)
        priority_queue->data[i] = NULL;

    return priority_queue;
}


Node* new_node(char letter, int frequence, int isLeaf, Node* leftChild, Node* rightChild)
{
    Node* n = malloc(sizeof(Node));
    n->letter = letter;
    n->frequence = frequence;
    n->isLeaf = isLeaf;
    n->leftChild = leftChild;
    n->rightChild = rightChild;

    return n;
}


void heapify(PriorityQueue* priority_queue, int i)
{
    int least = i;
    int left = LEFT_CHILD(i);
    int right = RIGHT_CHILD(i);

    if (left < priority_queue->size && priority_queue->data[left]->frequence < priority_queue->data[least]->frequence) {
        least = left;
    }
    if (right < priority_queue->size && priority_queue->data[right]->frequence < priority_queue->data[least]->frequence) {
        least = right;
    }
    if (least != i) {
        swap(&priority_queue->data[i], &priority_queue->data[least]);
        heapify(priority_queue, least);
    }
}


void push(PriorityQueue* priority_queue, Node* new_node)
{
    int i = priority_queue->size;        
    int parent = PARENT(i);
    priority_queue->data[priority_queue->size++] = new_node;
    while (priority_queue->data[i]->frequence < priority_queue->data[parent]->frequence) {
        swap(&priority_queue->data[i], &priority_queue->data[parent]);
        i = parent;
        parent = PARENT(i);
    }
}


void delete_node(Node* n)
{
    free(n);
}


Node* pop(PriorityQueue* priority_queue)
{
    Node* top = priority_queue->data[0];
    priority_queue->data[0] = priority_queue->data[--priority_queue->size];
    priority_queue->data[priority_queue->size] = NULL;
    heapify(priority_queue, 0);

    return top;
}


Node* create_haffman_tree(PriorityQueue* priority_queue)
{
    while (priority_queue->size > 1) {
        Node* top1 = pop(priority_queue);
        Node* top2 = pop(priority_queue);
        if (top1->frequence > top2->frequence)
            push(priority_queue, new_node(0, top1->frequence + top2->frequence, 0, top1, top2));
        else
            push(priority_queue, new_node(0, top1->frequence + top2->frequence, 0, top2, top1));
    }

    return priority_queue->data[0];
}


void recursive_encoding(Node* haffman_tree_node, char* string, char haffman_tree_dictionary[ASCII_SIZE][CHAR_BITSIZE + 1], 
                        char buffer[CHAR_BITSIZE + 1], int depth)
{
    if (haffman_tree_node->isLeaf) {
        buffer[depth] = '\0';
        for (int i = 0; i <= CHAR_BITSIZE; ++i) {
            haffman_tree_dictionary[haffman_tree_node->letter][i] = buffer[i];
        }
        return;
    }
    buffer[depth] = '0';
    recursive_encoding(haffman_tree_node->leftChild, string, haffman_tree_dictionary, buffer, depth + 1);
    buffer[depth] = '1';
    recursive_encoding(haffman_tree_node->rightChild, string, haffman_tree_dictionary, buffer, depth + 1);
}


void encode(Node* haffman_tree_root, char* string, char haffman_tree_dictionary[ASCII_SIZE][CHAR_BITSIZE + 1])
{
    char buffer[CHAR_BITSIZE + 1] = { 0 };
    recursive_encoding(haffman_tree_root, string, haffman_tree_dictionary, buffer, 0);
}


void print_node(Node* node)
{
    if (!node) {
        printf("NULL\n");
        return;
    }

    if (node->isLeaf)
        printf("'%c' : %i\n", node->letter, node->frequence);
    else
        printf("NODE : %i\n", node->frequence);
}


void print_priority_queue(PriorityQueue* priority_queue) 
{
    printf("\nPRIORITY QUEUE\n");
    printf("SIZE = %i\n", priority_queue->size);
    for (int i = 0; i < priority_queue->size; ++i)
        print_node(priority_queue->data[i]);
}


void print_haffman_tree_dictionary(char haffman_tree_dictionary[ASCII_SIZE][CHAR_BITSIZE + 1])
{
    printf("\nHAFFMAN TREE DICTIONARY\n");
    for (int i = 0; i < ASCII_SIZE; ++i)
        if (haffman_tree_dictionary[i][0] != '\0')
            printf("'%c' : %s\n", i, haffman_tree_dictionary[i]);
}