#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define NODE_COUNT 100

typedef struct Node {
    int key;        // исходный ключ (1..100)
    int number;     // номер после нумерации
    struct Node *left;
    struct Node *right;
} Node;

Node *create_node(int key) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->key = key;
    new_node->number = 0;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Построение идеально сбалансированного дерева
Node *build_balanced_bst(int left, int right, int array[]) {
    if (left > right) return NULL;
    int mid = (left + right) / 2;

    Node *node = create_node(array[mid]);
    node->left = build_balanced_bst(left, mid - 1, array);
    node->right = build_balanced_bst(mid + 1, right, array);
    return node;
}

// Нумерация in-order
void numerate_inorder(Node *node, int *counter) {
    if (!node) return;
    numerate_inorder(node->left, counter);
    node->number = ++(*counter);
    numerate_inorder(node->right, counter);
}

// Очередь для обхода по уровням
typedef struct Queue {
    Node* data[NODE_COUNT];
    int front, rear;
} Queue;

void init_queue(Queue *q) { q->front = q->rear = 0; }
bool is_empty(Queue *q) { return q->front == q->rear; }
void enqueue(Queue *q, Node *node) { q->data[q->rear++] = node; }
Node* dequeue(Queue *q) { return q->data[q->front++]; }

// Нумерация level-order (слева направо, 1 в середине дерева)
void numerate_levelorder(Node *root) {
    Queue q;
    init_queue(&q);
    enqueue(&q, root);
    int counter = 0;

    while (!is_empty(&q)) {
        Node *cur = dequeue(&q);
        cur->number = ++counter;

        if (cur->left) enqueue(&q, cur->left);
        if (cur->right) enqueue(&q, cur->right);
    }
}

// Печать in-order (номера)
void inorder_traversal(Node *node) {
    if (node != NULL) {
        inorder_traversal(node->left);
        printf("%d ", node->number);
        inorder_traversal(node->right);
    }
}

// Печать level-order (номера)
void level_order_traversal(Node *root) {
    Queue q;
    init_queue(&q);
    enqueue(&q, root);

    while (!is_empty(&q)) {
        Node *cur = dequeue(&q);
        printf("%d ", cur->number);

        if (cur->left) enqueue(&q, cur->left);
        if (cur->right) enqueue(&q, cur->right);
    }
}

// Служебные функции
int tree_size(Node *node) {
    if (node == NULL) return 0;
    return 1 + tree_size(node->left) + tree_size(node->right);
}

int checksum(Node *node) {
    if (node == NULL) return 0;
    return node->number + checksum(node->left) + checksum(node->right);
}

int tree_height(Node *node) {
    if (node == NULL) return 0;
    int lh = tree_height(node->left);
    int rh = tree_height(node->right);
    return 1 + (lh > rh ? lh : rh);
}

int total_path_length(Node *node, int level) {
    if (node == NULL) return 0;
    return level + total_path_length(node->left, level + 1) +
                  total_path_length(node->right, level + 1);
}

float average_depth(Node *root) {
    if (root == NULL) return 0.0f;
    int total = total_path_length(root, 1);
    int size = tree_size(root);
    return (float)total / size;
}

void fill_incremental_array(int n, int array[]) {
    for (int i = 0; i < n; i++) {
        array[i] = i + 1;
    }
}

int main() {
    int values[NODE_COUNT];
    fill_incremental_array(NODE_COUNT, values);

    // === Вариант 1: обычная нумерация in-order ===
    Node *root_in = build_balanced_bst(0, NODE_COUNT - 1, values);
    int counter_in = 0;
    numerate_inorder(root_in, &counter_in);

    printf("=== In-order numbering (классический ИСДП) ===\n");
    printf("In-order traversal:\n");
    inorder_traversal(root_in);
    printf("\nSize: %d\n", tree_size(root_in));
    printf("Checksum: %d\n", checksum(root_in));
    printf("Height: %d\n", tree_height(root_in));
    printf("Average Depth: %.2f\n", average_depth(root_in));

    printf("\nLevel-order traversal:\n");
    level_order_traversal(root_in);
    printf("\nSize: %d\n", tree_size(root_in));
    printf("Checksum: %d\n", checksum(root_in));
    printf("Height: %d\n", tree_height(root_in));
    printf("Average Depth: %.2f\n", average_depth(root_in));


    // === Вариант 2: нумерация по уровням (1 в середине) ===
    Node *root_lvl = build_balanced_bst(0, NODE_COUNT - 1, values);
    numerate_levelorder(root_lvl);

    printf("\n\n=== Level-order numbering (1 в середине) ===\n");
    printf("In-order traversal:\n");
    inorder_traversal(root_lvl);
    printf("\nSize: %d\n", tree_size(root_lvl));
    printf("Checksum: %d\n", checksum(root_lvl));
    printf("Height: %d\n", tree_height(root_lvl));
    printf("Average Depth: %.2f\n", average_depth(root_lvl));

    printf("\nLevel-order traversal:\n");
    level_order_traversal(root_lvl);
    printf("\nSize: %d\n", tree_size(root_lvl));
    printf("Checksum: %d\n", checksum(root_lvl));
    printf("Height: %d\n", tree_height(root_lvl));
    printf("Average Depth: %.2f\n", average_depth(root_lvl));

    return 0;
}

