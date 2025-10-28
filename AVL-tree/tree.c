#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int key;
    struct Node *left;
    struct Node *right;
    int height; // для AVL
} Node;

int max(int a, int b) { return (a > b) ? a : b; }
int height(Node *n) { return n ? n->height : 0; }
Node* new_node(int key) {
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->key = key;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

Node* rotate_right(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    // Поворот
    x->right = y;
    y->left = T2;
    // Обновление высот
    y->height = 1 + max(height(y->left), height(y->right));
    x->height = 1 + max(height(x->left), height(x->right));
    return x;
}

Node* rotate_left(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    // Поворот
    y->left = x;
    x->right = T2;
    // Обновление высот
    x->height = 1 + max(height(x->left), height(x->right));
    y->height = 1 + max(height(y->left), height(y->right));
    return y;
}

int get_balance(Node* n) {
    if (!n) return 0;
    return height(n->left) - height(n->right);
}

Node* avl_insert(Node* node, int key) {
    if (!node) return new_node(key);
    if (key < node->key)
        node->left = avl_insert(node->left, key);
    else if (key > node->key)
        node->right = avl_insert(node->right, key);
    else
        return node; // дубликаты не вставляем

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = get_balance(node);

    // LL
    if (balance > 1 && key < node->left->key)
        return rotate_right(node);
    // RR
    if (balance < -1 && key > node->right->key)
        return rotate_left(node);
    // LR
    if (balance > 1 && key > node->left->key) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    // RL
    if (balance < -1 && key < node->right->key) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}

Node* bst_insert(Node* node, int key) {
    if (!node) return new_node(key);
    if (key < node->key) node->left = bst_insert(node->left, key);
    else if (key > node->key) node->right = bst_insert(node->right, key);
    return node;
}

void inorder_print(Node* node) {
    if (!node) return;
    inorder_print(node->left);
    printf("%d ", node->key);
    inorder_print(node->right);
}

int tree_size(Node* node) {
    if (!node) return 0;
    return 1 + tree_size(node->left) + tree_size(node->right);
}

long long tree_checksum(Node* node) {
    if (!node) return 0LL;
    return (long long)node->key + tree_checksum(node->left) + tree_checksum(node->right);
}

int tree_height(Node* node) {
    if (!node) return 0;
    return 1 + max(tree_height(node->left), tree_height(node->right));
}

long long sum_depths(Node* node, int depth) {
    if (!node) return 0LL;
    return depth + sum_depths(node->left, depth + 1) + sum_depths(node->right, depth + 1);
}

double average_depth(Node* node) {
    int n = tree_size(node);
    if (n == 0) return 0.0;
    long long sumd = sum_depths(node, 1); // считаем глубины начиная с 1
    return (double)sumd / (double)n;
}

void shuffle(int *arr, int n) {
    for (int i = n-1; i > 0; --i) {
        int j = rand() % (i+1);
        int t = arr[i]; arr[i] = arr[j]; arr[j] = t;
    }
}

int main(void) {
    srand((unsigned)time(NULL));
    int n = 100;
    int domain = 1000; // диапазон для случайных чисел
    int *pool = (int*)malloc(domain * sizeof(int));
    if (!pool) { perror("malloc"); return 1; }
    for (int i = 0; i < domain; ++i) pool[i] = i+1;
    shuffle(pool, domain);

    Node *root_avl = NULL;
    Node *root_bst = NULL;

    // возьмём первые n уникальных чисел из перемешанного pool
    int *values = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) values[i] = pool[i];

    for (int i = 0; i < n; ++i) {
        root_avl = avl_insert(root_avl, values[i]);
        root_bst = bst_insert(root_bst, values[i]);
    }

    printf("\nОбход (in-order) AVL-дерева (слева направо):\n");
    inorder_print(root_avl);
    printf("\n");

    int size_avl = tree_size(root_avl);
    long long sum_avl = tree_checksum(root_avl);
    int height_avl = tree_height(root_avl);
    double avg_depth_avl = average_depth(root_avl);

    int size_bst = tree_size(root_bst);
    long long sum_bst = tree_checksum(root_bst);
    int height_bst = tree_height(root_bst);
    double avg_depth_bst = average_depth(root_bst);
    printf("| %-10s | %-6s | %-12s | %-6s | %-14s |\n", "n = 100", "Размер", "Контр. сумма", "Высота", "Средн. высота");
    printf("|------------|--------|--------------|--------|---------------|\n");
    printf("| %-10s | %-6d | %-12lld | %-6d | %-14.2f|\n","ИСДП (BST)", size_bst, sum_bst, height_bst, avg_depth_bst);
    printf("| %-10s | %-6d | %-12lld | %-6d | %-14.2f|\n","АВЛ (AVL) ", size_avl, sum_avl, height_avl, avg_depth_avl);
    free(pool);
    free(values);

    return 0;
}

