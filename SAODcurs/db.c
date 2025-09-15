#include "db.h"
#include <string.h>
#include <locale.h>
#include <math.h>

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


/// ======== Постраничный вывод (с нумерацией и шапкой) ========
void PrintPages(list *head) {
    int count = 0;
    int index = 1;
    char choice;

    system("chcp 65001 > nul");
    printf("\n%-4s %-15s %-25s %-20s %-6s %-12s\n",
           "№", "     Автор", "               Заглавие", "              Издательство", "        Стр.", "  Год");
    printf("\n====================================================================================\n\n");
    system("chcp 866 > nul");

    while (head) {
        printf("%-4d %-15s %-25s %-20s %-6d %-12d\n",
               index,
               head->data->author,
               head->data->title,
               head->data->publisher,
               head->data->num_of_page,
               head->data->year);

        head = head->next;
        count++;
        index++;

        if (count % 20 == 0 && head != NULL) {
            system("chcp 65001 > nul");
            printf("\nПоказано 20 записей. Продолжить? (y/n): ");
            scanf(" %c", &choice);
            system("chcp 866 > nul");

            if (choice != 'y' && choice != 'Y') break;
        }
    }
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
