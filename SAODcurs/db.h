#ifndef DB_H
#define DB_H

#include <stdio.h>
#include <stdlib.h>

#define N 4000

struct record {
    char author[12];
    char title[32];
    char publisher[16];
    short int year;
    short int num_of_page;
};

typedef struct list {
    struct record *data;
    struct list *next;
    struct list *prior;
    int pos; // позиция в отсорт массиве (для вывода результатов поиска)
} list;

typedef struct queue {
    list *head;
    list *tail;
} queue;

void LoadBase(struct record **base, list **lbase);
void DigitalSort(list **head);
void PrintPages(list *head);
queue SearchQueue(struct record **A, int year);
void PrintQueue(queue q);

// ======== Двоичное Б-дерево (по num_of_page) ========
typedef struct Node {
    struct record *data;    // указатель на запись
    struct Node *left;
    struct Node *right;
} Node;

Node* InsertTree(Node *root, struct record *rec);
void PrintTreeInOrder(Node *root);
struct record* SearchInTree(Node *root, int pages);
void FreeTree(Node *root);

#endif