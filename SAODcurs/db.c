#include "db.h"
#include <string.h>
#include <locale.h>
#include <math.h>
#include <ctype.h>

/// ======== Загрузка базы данных ========
void LoadBase(struct record **base, list **lbase) {
    FILE *f = fopen("testBase1.dat", "rb");
    if (!f) {
        perror("Ошибка открытия файла");
        exit(1);
    }

    *base = (struct record*)malloc(N * sizeof(struct record));
    fread(*base, sizeof(struct record), N, f);
    fclose(f);

    *lbase = (list*)malloc(sizeof(list));
    list *curr = *lbase;
    curr->data = &(*base)[0];
    curr->prior = NULL;
    for (int i = 1; i < N; i++) {
        curr->next = (list*)malloc(sizeof(list));
        curr->next->prior = curr;
        curr->next->data = &(*base)[i];
        curr = curr->next;
    }
    curr->next = NULL;
}

/// ===== Сортировка по году и автору =====
void DigitalSort(list **head) {
    if (!head || !*head) return;

    // 1. Считаем элементы
    int len = 0;
    list *p = *head;
    while (p) { len++; p = p->next; }

    list **arr = malloc(len * sizeof(list*));
    p = *head;
    for (int i = 0; i < len; i++, p = p->next) arr[i] = p;

    // 2. LSD сортировка: сначала по author[11] … author[0], потом по году (2 байта)
    for (int j = 11; j >= 0; j--) {
        list *bucket[257] = {0}, *tail[257] = {0};
        for (int i = 0; i < len; i++) {
            unsigned char d = (unsigned char)arr[i]->data->author[j];
            if (!bucket[d]) bucket[d] = tail[d] = arr[i];
            else { tail[d]->next = arr[i]; tail[d] = arr[i]; }
        }
        int idx = 0;
        for (int k = 0; k < 257; k++) {
            if (bucket[k]) {
                list *cur = bucket[k];
                while (cur) {
                    arr[idx++] = cur;
                    if (cur == tail[k]) break;
                    cur = cur->next;
                }
            }
        }
    }

    // теперь по году (2 байта)
    for (int j = 0; j < 2; j++) {
        list *bucket[257] = {0}, *tail[257] = {0};
        for (int i = 0; i < len; i++) {
            unsigned char d = (arr[i]->data->year >> (j * 8)) & 0xFF;
            if (!bucket[d]) bucket[d] = tail[d] = arr[i];
            else { tail[d]->next = arr[i]; tail[d] = arr[i]; }
        }
        int idx = 0;
        for (int k = 0; k < 257; k++) {
            if (bucket[k]) {
                list *cur = bucket[k];
                while (cur) {
                    arr[idx++] = cur;
                    if (cur == tail[k]) break;
                    cur = cur->next;
                }
            }
        }
    }

    // 3. Перепривязываем список
    for (int i = 0; i < len; i++) {
        arr[i]->next  = (i + 1 < len) ? arr[i + 1] : NULL;
        arr[i]->prior = (i > 0) ? arr[i - 1] : NULL;
    }
    *head = arr[0];

    free(arr);
}


/// ======== Бинарный поиск по году (возвращает первый (левый) индекс или -1) ========
int BSearch(struct record **A, int year) {
    int l = 0, r = N - 1;
    int result = -1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (A[m]->year < year) {
            l = m + 1;
        } else if (A[m]->year > year) {
            r = m - 1;
        } else {
            result = m;     // нашли совпадение
            r = m - 1;      // продолжаем искать в левой половине
        }
    }
    return result;
}

#include <ctype.h>

/// ======== Постраничный вывод с навигацией (N / P / TO n / BACK n / Q) ========
void PrintPages(list *head) {
    if (!head) return;

    const int pageSize = 20;

    // считаем общее количество записей
    int total = 0;
    list *t = head;
    while (t) { total++; t = t->next; }

    if (total == 0) {
#ifdef _WIN32
        system("chcp 65001 > nul");
#endif
        printf("База пуста.\n");
        return;
    }

    // создаем массив указателей для быстрого доступа к страницам
    list **arr = malloc(total * sizeof(list*));
    t = head;
    for (int i = 0; i < total && t; i++, t = t->next) arr[i] = t;

    int totalPages = (total + pageSize - 1) / pageSize;
    int currentPage = 0;

    char input[64];

    while (1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

#ifdef _WIN32
        system("chcp 65001 > nul");
#endif
        printf("\nСтраница %d из %d (всего записей: %d)\n",
               currentPage + 1, totalPages, total);
        printf("====================================================================================\n");
        printf("%-4s %-15s %-25s %-20s %-6s %-12s\n",
               "№", "     Автор", "               Заглавие", "              Издательство", "        Стр.", "  Год");
        printf("====================================================================================\n\n");
#ifdef _WIN32
        system("chcp 866 > nul");
#endif

        // выводим текущую страницу
        int start = currentPage * pageSize;
        int end = start + pageSize;
        if (end > total) end = total;

        for (int i = start; i < end; i++) {
            printf("%-4d %-15s %-25s %-20s %-6d %-12d\n",
                   i + 1,
                   arr[i]->data->author,
                   arr[i]->data->title,
                   arr[i]->data->publisher,
                   arr[i]->data->num_of_page,
                   arr[i]->data->year);
        }

#ifdef _WIN32
        system("chcp 65001 > nul");
#endif
        printf("\n===== Навигация =====\n");
        printf("[N] - следующая страница\n");
        printf("[P] - предыдущая страница\n");
        printf("[TO n] - перелистнуть вперёд на n страниц\n");
        printf("[BACK n] - вернуться назад на n страниц\n");
        printf("[Q] - выход\n");
        printf("------------------------------\n");
        printf("Выбор: ");

        // очищаем ввод
        if (!fgets(input, sizeof(input), stdin)) break;

        // убираем перевод строки
        input[strcspn(input, "\n")] = 0;

        // пропускаем пробелы
        char *cmd = input;
        while (*cmd == ' ' || *cmd == '\t') cmd++;

        // переводим в верхний регистр первую часть
        for (char *p = cmd; *p; ++p) {
            if (*p >= 'a' && *p <= 'z') *p = *p - 32;
        }

        if (strcmp(cmd, "N") == 0) {
            if (currentPage < totalPages - 1) currentPage++;
        } else if (strcmp(cmd, "P") == 0) {
            if (currentPage > 0) currentPage--;
        } else if (strncmp(cmd, "TO", 2) == 0) {
            int n = 1;
            sscanf(cmd + 2, "%d", &n);
            if (n < 1) n = 1;
            currentPage += n;
            if (currentPage >= totalPages) currentPage = totalPages - 1;
        } else if (strncmp(cmd, "BACK", 4) == 0) {
            int n = 1;
            sscanf(cmd + 4, "%d", &n);
            if (n < 1) n = 1;
            currentPage -= n;
            if (currentPage < 0) currentPage = 0;
        } else if (strcmp(cmd, "Q") == 0) {
            break;
        }
    }

    free(arr);
}

/// ======== Формирование очереди поиска ========
queue SearchQueue(struct record **A, int year) {
    queue result = {NULL, NULL};
    int pos = BSearch(A, year);
    if (pos == -1) return result;

    while (pos < N && A[pos]->year == year) {
        list *node = (list*)malloc(sizeof(list));
        node->data = A[pos];
        node->next = NULL;
        node->prior = NULL;
        node->pos = pos + 1; // порядковый номер в отсорт массиве

        if (!result.head) result.head = result.tail = node;
        else {
            result.tail->next = node;
            result.tail = node;
        }
        pos++;
    }
    return result;
}

/// ======== Вывод очереди + порядковый номер в отсорт массиве ========
void PrintQueue(queue q) {
    list *p = q.head;

    system("chcp 65001 > nul");
    printf("\n%-4s %-15s %-25s %-20s %-6s %-12s\n",
           "№", "Автор", "Заглавие", "Издательство", "Стр.", "Год");
    system("chcp 866 > nul");

    while (p) {
        printf("%-4d %-15s %-25s %-20s %-6d %-12d\n",
               p->pos, // позиция записи в отсорт массиве
               p->data->author,
               p->data->title,
               p->data->publisher,
               p->data->num_of_page,
               p->data->year);

        p = p->next;
    }
}

/// ======== Вставка записи в двоичное дерево по числу страниц ========
Node* InsertTree(Node *root, struct record *rec) {
    if (!rec) return root;
    if (root == NULL) {
        Node *node = (Node*)malloc(sizeof(Node));
        node->data = rec;
        node->left = node->right = NULL;
        return node;
    }
    if (rec->num_of_page < root->data->num_of_page)
        root->left = InsertTree(root->left, rec);
    else
        root->right = InsertTree(root->right, rec);
    return root;
}

/// ======== Симметричный обход дерева (in-order) ========
void PrintTreeInOrder(Node *root) {
    if (!root) return;
    PrintTreeInOrder(root->left);
    printf("%-15s %-25s %-20s %-6d %-12d\n",
           root->data->author,
           root->data->title,
           root->data->publisher,
           root->data->num_of_page,
           root->data->year);
    PrintTreeInOrder(root->right);
}

/// ======== Поиск записи в дереве по числу страниц ========
struct record* SearchInTree(Node *root, int pages) {
    if (!root) return NULL;
    if (pages == root->data->num_of_page) return root->data;
    if (pages < root->data->num_of_page)
        return SearchInTree(root->left, pages);
    else
        return SearchInTree(root->right, pages);
}

/// ======== Освобождение памяти дерева ========
void FreeTree(Node *root) {
    if (!root) return;
    FreeTree(root->left);
    FreeTree(root->right);
    free(root);
}