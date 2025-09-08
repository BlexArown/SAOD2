#include <stdlib.h>
#include <stdio.h>

typedef struct Vertex
{
    int Data;
    struct Vertex *Left;
    struct Vertex *Right;
} Vertex;

void Top_to_bot(Vertex *p)
{
    if (p != NULL)
    {
        printf("%d ", p->Data);
        Top_to_bot(p->Left);
        Top_to_bot(p->Right);
    }
}

void Left_to_right(Vertex *p)
{
    if (p != NULL)
    {
        Left_to_right(p->Left);
        printf("%d ", p->Data);
        Left_to_right(p->Right);
    }
}

void Bot_to_top(Vertex *p)
{
    if (p != NULL)
    {
        Bot_to_top(p->Left);
        Bot_to_top(p->Right);
        printf("%d ", p->Data);
    }
}

Vertex *create_vertex(int data)
{
    Vertex *newVertex = (Vertex *)malloc(sizeof(Vertex));
    newVertex->Data = data;
    newVertex->Left = NULL;
    newVertex->Right = NULL;
    return newVertex;
}

Vertex *add_to_left(Vertex *p, int data)
{
    Vertex *leftVetex = create_vertex(data);
    p->Left = leftVetex;
    return leftVetex;
}

Vertex *add_to_right(Vertex *p, int data)
{
    Vertex *rightVetex = create_vertex(data);
    p->Right = rightVetex;
    return rightVetex;
}

int Size(Vertex *p)
{
    if (p == NULL)
        return 0;
    else
        return 1 + Size(p->Left) + Size(p->Right);
}

int CheckSum(Vertex *p)
{
    if (p == NULL)
        return 0;
    else
        return p->Data + CheckSum(p->Left) + CheckSum(p->Right);
}

int Height(Vertex *p)
{
    if (p == NULL)
        return 0;
    else
    {
        int leftHeight = Height(p->Left);
        int rightHeight = Height(p->Right);
        return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }
}

int PathLengthSum(Vertex *p, int level)
{
    if (p == NULL)
        return 0;
    else
        return level +
               PathLengthSum(p->Left, level + 1) +
               PathLengthSum(p->Right, level + 1);
}

float AverageHeight(Vertex *root)
{
    if (root == NULL)
        return 0.0f;
    int total_path_length = PathLengthSum(root, 1);
    int tree_size = Size(root);
    return (float)total_path_length / tree_size;
}

int main()
{
    // Построение дерева из примера
    Vertex *root = create_vertex(1);       // корень
    Vertex *v2 = add_to_left(root, 2);     // левый ребёнок 1
    Vertex *v3 = add_to_left(v2, 3);       // левый от 2
    add_to_right(v2, 4);                   // правый от 2
    Vertex *v5 = add_to_right(v3, 5);      // правый от 3
    add_to_left(v5, 6);                    // левый от 5

    printf("Обход сверху вниз: ");
    Top_to_bot(root);
    printf("\nОбход слева направо: ");
    Left_to_right(root);
    printf("\nОбход снизу вверх: ");
    Bot_to_top(root);

    printf("\nРазмер дерева: %d", Size(root));
    printf("\nКонтрольная сумма дерева: %d", CheckSum(root));
    printf("\nВысота дерева: %d", Height(root));
    printf("\nСредняя высота дерева: %.2f\n", AverageHeight(root));
}

