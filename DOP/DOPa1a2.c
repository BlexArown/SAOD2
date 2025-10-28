#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#define N 100

typedef struct Vertex
{
    int Data;
    int balance;
    int weight;
    bool use;
    struct Vertex *Left;
    struct Vertex *Right;
} Vertex;


Vertex *AddVertex(Vertex *root, int data, int weight)
{
    if (root == NULL)
    {
        root = (Vertex *)malloc(sizeof(Vertex));
        root->Data = data;
        root->weight = weight;
        root->use = false;
        root->Right = NULL;
        root->Left = NULL;
    }
    else if (data < root->Data)
    {
        root->Left = AddVertex(root->Left, data, weight);
    }
    else if (data > root->Data)
    {
        root->Right = AddVertex(root->Right, data, weight);
    }
    return root;
}

Vertex* BuildTreeByWeightA1(int data[], int weights[], int n)
{
    Vertex *Root = NULL;
    Vertex V[N];
    for (int i = 0; i < n; i++) {
        V[i].Data = data[i];
        V[i].weight = weights[i];
        V[i].use = false;
        V[i].Left = NULL;
        V[i].Right = NULL;
    }
    for (int i = 0; i < n; i++) {
        int max = 0;
        int Index = 0;
        for (int j = 0; j < n; j++) {
            if (V[j].weight > max && V[j].use == false) {
                max = V[j].weight;
                Index = j;
            }
        }
        V[Index].use = true;
        Root = AddVertex(Root, V[Index].Data, V[Index].weight);
    }
    return Root;
}

Vertex* BuildA2Tree(int L, int R, int data[], int weights[], Vertex *root)
{
    if (L <= R)
    {
        int wes = 0;
        int summa = 0;
        for (int i = L; i <= R; i++)
        {
            wes += weights[i];
        }
        int median_index = L;
        for (int i = L; i <= R; i++)
        {
            if (summa < wes/2 && summa + weights[i] >= wes/2)
            {
                median_index = i;
                break;
            }
            summa += weights[i];
        }
        root = AddVertex(root, data[median_index], weights[median_index]);
        root = BuildA2Tree(L, median_index - 1, data, weights, root);
        root = BuildA2Tree(median_index + 1, R, data, weights, root);
    }
    return root;
}

Vertex* CreateTree(int L, int R, int data[], int weights[], int AR[][N+1], Vertex *root)
{
    if (L < R)
    {
        int k = AR[L][R];
        root = AddVertex(root, data[k], weights[k]);
        root = CreateTree(L, k-1, data, weights, AR, root);
        root = CreateTree(k, R, data, weights, AR, root);
    }
    return root;
}

void CalculateAW(int n, int weights[], int AW[][N+1])
{
    for (int i = 0; i <= n; i++)
    {
        for (int j = i+1; j <= n; j++)
        {
            AW[i][j] = AW[i][j-1] + weights[j];
        }
    }
}

void CalculateAPAR(int n, int AW[][N+1], int AP[][N+1], int AR[][N+1])
{
    for (int i = 0; i <= n-1; i++)
    {
        int j = i + 1;
        AP[i][j] = AW[i][j];
        AR[i][j] = j;
    }

    for (int h = 2; h <= n; h++)
    {
        for (int i = 0; i <= n - h; i++)
        {
            int j = i + h;
            int m = AR[i][j-1];
            int min_val = AP[i][m-1] + AP[m][j];
            for (int k = m+1; k <= AR[i+1][j]; k++)
            {
                int x = AP[i][k-1] + AP[k][j];
                if (x < min_val)
                {
                    m = k;
                    min_val = x;
                }
            }
            AP[i][j] = min_val + AW[i][j];
            AR[i][j] = m;
        }
    }
}

void Left_to_Right(Vertex *p)
{
    if (p != NULL)
    {
        Left_to_Right(p->Left);
        printf("%d(w:%d) ", p->Data, p->weight);
        Left_to_Right(p->Right);
    }
}

int Size(Vertex *p)
{
    if (p == NULL)
        return 0;
    return 1 + Size(p->Left) + Size(p->Right);
}

int CheckSum(Vertex *p)
{
    if (p == NULL)
        return 0;
    return p->Data + CheckSum(p->Left) + CheckSum(p->Right);
}

int Height(Vertex *p)
{
    if (p == NULL)
        return 0;
    int lh = Height(p->Left);
    int rh = Height(p->Right);
    return 1 + (lh > rh ? lh : rh);
}

int PathLengthSum(Vertex *p, int level)
{
    if (p == NULL)
        return 0;
    return level + PathLengthSum(p->Left, level + 1) + PathLengthSum(p->Right, level + 1);
}

int TotalWeight(Vertex *p)
{
    if (p == NULL)
        return 0;
    return p->weight + TotalWeight(p->Left) + TotalWeight(p->Right);
}

int WeightedPathSum(Vertex *p, int level)
{
    if (p == NULL)
        return 0;
    return (p->weight * level) + WeightedPathSum(p->Left, level + 1) + WeightedPathSum(p->Right, level + 1);
}

float WeightedAverageHeight(Vertex *root)
{
    if (root == NULL)
        return 0.0f;
    int total_weight = TotalWeight(root);
    if (total_weight == 0)
        return 0.0f;
    int weighted_path_sum = WeightedPathSum(root, 1);
    return (float)weighted_path_sum / total_weight;
}

void generator(int data[], int weights[], int n)
{
    srand(time(NULL));
    printf("Генерация случайной последовательности с весами:\n");
    int count = 0;
    while (count < n)
    {
        int num = rand() % 1000;
        int unique = 1;
        for (int i = 0; i < count; i++)
        {
            if (data[i] == num)
            {
                unique = 0;
                break;
            }
        }
        if (unique)
        {
            data[count] = num;
            weights[count] = rand() % 100 + 1;
            printf("%d(w:%d) ", data[count], weights[count]);
            count++;
        }
    }
    printf("\n");
}

int insertSort(int data[], int weights[], int n)
{
    int C = 0;
    int M = 0;

    for (int i = 1; i < n; i++)
    {
        int temp_data = data[i];
        int temp_weight = weights[i];
        int j = i - 1;
        M++;

        while (j >= 0 && temp_data < data[j])
        {
            C++;
            data[j + 1] = data[j];
            weights[j + 1] = weights[j];
            M++;
            j--;
        }
        if (j >= 0)
        {
            C++;
        }

        data[j + 1] = temp_data;
        weights[j + 1] = temp_weight;
        M++;
    }
    int trud = M + C;
    return trud;
}

void PrintStatString(const char *name, Vertex *root)
{
    printf("| %-20s | %-10d | %-15d | %-10d | %-15.2f |\n", name, Size(root), CheckSum(root), Height(root), WeightedAverageHeight(root));
}

int main()
{
    system("chcp 65001 > nul");

    int data[N];
    int weights[N];
    Vertex *optimalBST = NULL;
    Vertex *A1_tree = NULL;
    Vertex *A2_tree = NULL;

    generator(data, weights, N);
    insertSort(data, weights, N);
    A1_tree = BuildTreeByWeightA1(data, weights, N);

    int optimalData[N+1];
    int optimalWeights[N+1];
    for (int i = 0; i < N; i++) {
        optimalData[i+1] = data[i];
        optimalWeights[i+1] = weights[i];
    }

    int AW[N+1][N+1] = {0};
    int AP[N+1][N+1] = {0};
    int AR[N+1][N+1] = {0};

    CalculateAW(N, optimalWeights, AW);
    CalculateAPAR(N, AW, AP, AR);
    double calculated_weighted_height = (double)AP[0][N] / AW[0][N];
    optimalBST = CreateTree(0, N, optimalData, optimalWeights, AR, optimalBST);
    A2_tree = BuildA2Tree(0, N-1, data, weights, A2_tree);

    printf("\nОбход оптимального дерева поиска:\n");
    Left_to_Right(optimalBST);
    printf("\n");

    printf("\nОбход дерева A1:\n");
    Left_to_Right(A1_tree);
    printf("\n");

    printf("\nОбход дерева A2:\n");
    Left_to_Right(A2_tree);
    printf("\n");
    printf("| %-20s | %-10s | %-15s | %-10s | %-15s |\n","Type", "Size", "CheckSum", "Height", "Avg Height");
    PrintStatString("DOP", optimalBST);
    PrintStatString("A1 Tree", A1_tree);
    PrintStatString("A2 Tree", A2_tree);
    printf("\nAP[0,n]/AW[0,n] = %.2f\n", calculated_weighted_height);
    return 0;
}
