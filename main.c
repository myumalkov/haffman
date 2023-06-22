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


PriorityQueue* new_priority_queue();
Node* new_node(char letter, int frequence, int isLeaf, Node* leftChild, Node* rightChild);
void delete_node(Node* n);
void push(PriorityQueue* pq, const Node* data);
Node* pop(PriorityQueue* pq);
Node* create_haffman_tree(PriorityQueue* pq);

/***************************************** DEBUG ***********************************************/

void print_node(Node* node);
void print_queue(PriorityQueue* pq);
void print_haffman_tree(Node* haffman_tree_root);

void create_haffman_table(Node* haffman_tree_node, char buffer[CHAR_BITSIZE + 1], char haffman_table[ASCII_SIZE][CHAR_BITSIZE + 1], int depth)
{
    if (!(haffman_tree_node->isLeaf)) {
        buffer[depth] = '0';
        create_haffman_table(haffman_tree_node->leftChild, buffer, haffman_table, depth + 1);
        buffer[depth] = '1';
        create_haffman_table(haffman_tree_node->rightChild, buffer, haffman_table, depth + 1);
        return;
    }
    buffer[depth] = '\0';
    for (int i = 0; i <= CHAR_BITSIZE; ++i)
        haffman_table[haffman_tree_node->letter][i] = buffer[i];
}

void unzip(char* unzipped_string, char* zipped_string, Node* haffman_tree_root)
{
    int j = 0;
    Node current_haffman_tree_node = *haffman_tree_root;
    for (int i = 0; i < strlen(zipped_string); ++i) {
        if (current_haffman_tree_node.isLeaf) {
            unzipped_string[j++] = current_haffman_tree_node.letter;
            current_haffman_tree_node = *haffman_tree_root;
        } 
        else
            if (zipped_string[i] == '0')
                current_haffman_tree_node = *current_haffman_tree_node.leftChild;
            else
                current_haffman_tree_node = *current_haffman_tree_node.rightChild;
    }
    zipped_string[j] = '\0';
}

/****************************************** MAIN ***********************************************/

int main()
{
    char* string;
    scanf("%m[^\n]*", &string);

    int frequencies[ASCII_SIZE] = { 0 };
    for (int i = 0; i < strlen(string); ++i)
        ++frequencies[string[i]];

    PriorityQueue* pq = new_priority_queue();

    for (int i = 0; i < ASCII_SIZE; ++i) {
        if (frequencies[i] != 0) 
            push(pq, new_node(i, frequencies[i], 1, NULL, NULL));
    }
    print_queue(pq);

    Node* haffman_tree_root = create_haffman_tree(pq);
    print_haffman_tree(haffman_tree_root);

    char buffer[CHAR_BITSIZE + 1];
    char haffman_table[ASCII_SIZE][CHAR_BITSIZE + 1];
    create_haffman_table(haffman_tree_root, buffer, haffman_table, 0);

    printf("\n%s = ", string);
    char* zipped_string = malloc(strlen(string) * CHAR_BITSIZE + 1);
    int k = 0;
    for (int i = 0; i < strlen(string); ++i)
        //printf("%s", haffman_table[string[i]]);
        for (int j = 0; j < strlen(haffman_table[string[i]]); ++j)
            zipped_string[k++] = haffman_table[string[i]][j];
    zipped_string[k] = '\0';
    printf("%s\n", zipped_string);

    char* unzipped_string = malloc(strlen(string) + 1);
    unzip(unzipped_string, zipped_string, haffman_tree_root);
    printf("unzipped string: %s", unzipped_string);


    return 0;
}

/***********************************************************************************************/

void swap(Node** n1, Node** n2)
{
    Node* temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}


PriorityQueue* new_priority_queue()
{
    PriorityQueue* pq = malloc(sizeof(PriorityQueue));
    pq->size = 0;
    for (int i = 0; i < PRIORITY_QUEUE_CAPACITY; ++i)
        pq->data[i] = NULL;

    return pq;
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


void heapify(PriorityQueue* pq, int i)
{
    int least = i;
    int left = LEFT_CHILD(i);
    int right = RIGHT_CHILD(i);

    if (left < pq->size && pq->data[left]->frequence < pq->data[least]->frequence) {
        least = left;
    }
    if (right < pq->size && pq->data[right]->frequence < pq->data[least]->frequence) {
        least = right;
    }
    if (least != i) {
        swap(&pq->data[i], &pq->data[least]);
        heapify(pq, least);
    }
}


void push(PriorityQueue* pq, const Node* new_node)
{
    int i = pq->size;        
    int parent = PARENT(i);
    pq->data[pq->size++] = new_node;
    while (pq->data[i]->frequence < pq->data[parent]->frequence) {
        swap(&pq->data[i], &pq->data[parent]);
        i = parent;
        parent = PARENT(i);
    }
}


void delete_node(Node* n)
{
    free(n);
}


Node* top(PriorityQueue* pq)
{
    return pq->data[0];
}


Node* pop(PriorityQueue* pq)
{
    Node* top = pq->data[0];
    pq->data[0] = pq->data[--pq->size];
    pq->data[pq->size] = NULL;
    heapify(pq, 0);

    return top;
}


Node* create_haffman_tree(PriorityQueue* pq)
{
    while (pq->size > 1) {
        Node* top1 = pop(pq);
        Node* top2 = pop(pq);
        if (top1->frequence > top2->frequence)
            push(pq, new_node(0, top1->frequence + top2->frequence, 0, top1, top2));
        else
            push(pq, new_node(0, top1->frequence + top2->frequence, 0, top2, top1));
    }

    return pq->data[0];
}


void print_node(Node* node)
{
    if (!node) {
        printf("NULL\n");
        return;
    }

    if (node->isLeaf)
        printf("%c' : %i\n", node->letter, node->frequence);
    else
        printf("NODE : %i\n", node->frequence);
}


void print_queue(PriorityQueue* pq) 
{
    printf("\nSIZE = %i\n", pq->size);
    for (int i = 0; i < pq->size; ++i)
        print_node(pq->data[i]);
}


void print_ht(Node* haffman_tree_node, char buffer[CHAR_BITSIZE + 1], int depth)
{
    if (!(haffman_tree_node->isLeaf)) {
        buffer[depth] = '0';
        print_ht(haffman_tree_node->leftChild, buffer, depth + 1);
        buffer[depth] = '1';
        print_ht(haffman_tree_node->rightChild, buffer, depth + 1);
        return;
    }
    buffer[depth] = '\0';
    printf("'%c' : %s\n", haffman_tree_node->letter, buffer, haffman_tree_node->letter);
}


void print_haffman_tree(Node* haffman_tree_root)
{
    printf("\nHaffman Tree:\n");
    char buffer[CHAR_BITSIZE + 1];
    print_ht(haffman_tree_root, buffer, 0);
}