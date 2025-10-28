#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#define N 100

int VR = 1, HR = 1;

typedef struct Vertex {
    int Data;
    int balance;
    struct Vertex *Left;
    struct Vertex *Right;
} Vertex;

void Left_to_Right(Vertex *p) {
    if (p != NULL) {
        Left_to_Right(p->Left);
        printf("%d ", p->Data);
        Left_to_Right(p->Right);
    }
}

int Size(Vertex *p) {
    if (p == NULL) return 0;
    return 1 + Size(p->Left) + Size(p->Right);
}

int CheckSum(Vertex *p) {
    if (p == NULL) return 0;
    return p->Data + CheckSum(p->Left) + CheckSum(p->Right);
}

int Height(Vertex *p) {
    if (p == NULL) return 0;
    int lh = Height(p->Left);
    int rh = Height(p->Right);
    return 1 + (lh > rh ? lh : rh);
}

int PathLengthSum(Vertex *p, int level) {
    if (p == NULL) return 0;
    return level + PathLengthSum(p->Left, level + 1) + PathLengthSum(p->Right, level + 1);
}

float AverageHeight(Vertex *root) {
    if (root == NULL) return 0.0f;
    return (float)PathLengthSum(root, 1) / Size(root);
}

void generator(int arr[], int n) {
    srand(time(NULL));
    printf("Генерация случ последовательности:\n");
    int count = 0;
    while (count < n) {
        int num = rand() % 1000;
        int unique = 1;
        for (int i = 0; i < count; i++) {
            if (arr[i] == num) {
                unique = 0;
                break;
            }
        }
        if (unique) {
            arr[count] = num;
            printf("%d ", num);
            count++;
        }
    }
    printf("\n");
}

void ll_rotation(Vertex **p) {
    Vertex *q = (*p)->Left;
    (*p)->Left = q->Right;
    q->Right = *p;
    (*p)->balance = 0;
    q->balance = 0;
    *p = q;
}

void lr_rotation(Vertex **p) {
    Vertex *q = (*p)->Left;
    Vertex *r = q->Right;

    if (r->balance < 0) (*p)->balance = 1;
    else (*p)->balance = 0;

    if (r->balance > 0) q->balance = -1;
    else q->balance = 0;

    r->balance = 0;

    q->Right = r->Left;
    (*p)->Left = r->Right;
    r->Left = q;
    r->Right = *p;
    *p = r;
}

void rl_rotation(Vertex **p) {
    Vertex *q = (*p)->Right;
    Vertex *r = q->Left;

    if (r->balance > 0) (*p)->balance = -1;
    else (*p)->balance = 0;

    if (r->balance < 0) q->balance = 1;
    else q->balance = 0;

    r->balance = 0;

    q->Left = r->Right;
    (*p)->Right = r->Left;
    r->Left = *p;
    r->Right = q;
    *p = r;
}

void rr_rotation(Vertex **p) {
    Vertex *q = (*p)->Right;
    (*p)->Right = q->Left;
    q->Left = *p;
    (*p)->balance = 0;
    q->balance = 0;
    *p = q;
}

int insert_AVL(int D, Vertex **p) {
    int growth = 0;
    if (*p == NULL) {
        *p = (Vertex *)malloc(sizeof(Vertex));
        (*p)->Data = D;
        (*p)->balance = 0;
        (*p)->Left = (*p)->Right = NULL;
        return 1;
    }

    if (D < (*p)->Data) {
        growth = insert_AVL(D, &(*p)->Left);
        if (growth) {
            if ((*p)->balance > 0) {
                (*p)->balance = 0;
                return 0;
            } else if ((*p)->balance == 0) {
                (*p)->balance = -1;
                return 1;
            } else {
                if ((*p)->Left->balance < 0)
                    ll_rotation(p);
                else
                    lr_rotation(p);
                return 0;
            }
        }
    } else if (D > (*p)->Data) {
        growth = insert_AVL(D, &(*p)->Right);
        if (growth) {
            if ((*p)->balance < 0) {
                (*p)->balance = 0;
                return 0;
            } else if ((*p)->balance == 0) {
                (*p)->balance = 1;
                return 1;
            } else {
                if ((*p)->Right->balance > 0)
                    rr_rotation(p);
                else
                    rl_rotation(p);
                return 0;
            }
        }
    }
    return 0;
}

void B2INSERT(int D, Vertex **p) {
    if (*p == NULL) {
        *p = (Vertex *)malloc(sizeof(Vertex));
        (*p)->Data = D;
        (*p)->Left = (*p)->Right = NULL;
        (*p)->balance = 0;
        VR = 1;
        return;
    }

    else if (D < (*p)->Data) {
        B2INSERT(D, &(*p)->Left);
        if (VR == 1) {
            if ((*p)->balance == 0) {
                Vertex *q = (*p)->Left;
                (*p)->Left = q->Right;
                q->Right = *p;
                q->balance = 1;
                *p = q;
                VR = 0;
                HR = 1;
            } else {
                (*p)->balance = 0;
                VR = 1;
                HR = 0;
            }
        } else {
            HR = 0;
        }
    } else if (D > (*p)->Data) {
        B2INSERT(D, &(*p)->Right);
        if (VR == 1) {
            (*p)->balance = 1;
            HR = 1;
            VR = 0;
        } else if (HR == 1) {
            if ((*p)->balance == 1) {
                Vertex *q = (*p)->Right;
                (*p)->balance = 0;
                q->balance = 0;
                (*p)->Right = q->Left;
                q->Left = *p;
                *p = q;
                VR = 1;
                HR = 0;
            } else {
                HR = 0;
            }
        }
    }
}
void PrintStatString(const char *name, Vertex *root) {
    printf(" %-20s  %-10d  %-15d  %-10d  %-15.2f \n", name, Size(root), CheckSum(root), Height(root), AverageHeight(root));
}

int main() {
    system("chcp 65001 > nul");

    int initial[N];

    generator(initial, N);

    Vertex *AVL = NULL;
    Vertex *B2 = NULL;

    for (int i = 0; i < N; i++){
        insert_AVL(initial[i], &AVL);
        B2INSERT(initial[i], &B2);
    }

    printf("\nОбход ДБД:\n");
    printf("\n");
    Left_to_Right(B2);

    printf("\nОбход AVL:\n");
    Left_to_Right(AVL);
    printf("\n\n");

    printf(" %-20s  %-10s  %-15s  %-10s  %-15s \n", "Type", "Size", "CheckSum", "Height", "Avg Height");
    PrintStatString("AVL", AVL);
    PrintStatString("B-Tree", B2);
    return 0;
}
