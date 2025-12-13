#ifndef DB_H
#define DB_H

#include <stdio.h>
#include <stdlib.h>

#define N 4000
#define BTREE_T 2

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
    int pos; 
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

typedef struct BTreeNode {
    int n;
    int leaf;
    struct record *keys[2*BTREE_T - 1];
    struct BTreeNode *child[2*BTREE_T];
} BTreeNode;

BTreeNode* BTreeCreateNode(int leaf);
void BTreeSplitChild(BTreeNode *x, int i);
void BTreeInsert(BTreeNode **root, struct record *rec);
void BTreeInsertNonFull(BTreeNode *x, struct record *rec);
struct record* BTreeSearch(BTreeNode *x, int pages);
void BTreePrint(BTreeNode *root, int depth);
void BTreeFree(BTreeNode *x);
void BTreePrintInOrder(BTreeNode *root, int depth);
queue BTreeSearchAll(BTreeNode *root, int pages);

void EncodeDatabase_GilbertMoore();
void AnalyzeDatabase_GilbertMoore();

#endif