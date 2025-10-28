#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "db.h"

int main() {
    system("chcp 65001 > nul");
    setlocale(LC_ALL, "C");

    struct record *base = NULL;
    list *lbase = NULL;
    struct record **mas = NULL;

    int isLoaded = 0;
    int isSorted = 0;

    // Загружаем базу в память один раз
    LoadBase(&base, &lbase);
    isLoaded = 1;

    char choice;
    do {
        system("chcp 65001 > nul");
        printf("\n===== МЕНЮ =====\n");
        printf("1 - Показать базу данных ДО сортировки\n");
        printf("2 - Сортировка по году и автору\n");
        printf("3 - Поиск по году (только после сортировки по году)\n");
        printf("0 - Выход\n");
        printf("Выбор: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                if (isLoaded) {
                    system("chcp 866 > nul");
                    PrintPages(lbase);
                    system("chcp 65001 > nul");
                }
                break;

            case '2':
                if (isLoaded) {
                    DigitalSort(&lbase);
                    // пересобираем массив для поиска
                    mas = (struct record**)malloc(N * sizeof(struct record*));
                    list *ptr = lbase;
                    for (int i = 0; i < N && ptr != NULL; i++) {
                        mas[i] = ptr->data;
                        ptr = ptr->next;
                    }
                    isSorted = 1;
                    PrintPages(lbase);
                }
                break;

             case '3':
                 if (!isSorted) {
                     printf("Ошибка: база данных ещё не отсортирована по году!\n");
                 } else {
                     int year;
                     printf("Введите год: ");
                     scanf("%d", &year);
                     queue q = SearchQueue(mas, year);
                     if (!q.head) {
                            printf("Записей с таким годом нет.\n");
                     } else {
                            system("chcp 866 > nul");
                            PrintQueue(q);
                            system("chcp 65001 > nul");
			    // --- Новый блок: построение дерева из очереди ---
			    Node *treeRoot = NULL;
			    list *qptr = q.head;
			    // строим дерево по числу страниц
			    while (qptr) {
    			    	treeRoot = InsertTree(treeRoot, qptr->data);
    				qptr = qptr->next;
			    }
	                    system("chcp 65001 > nul");
			    printf("\n\n===== Дерево по числу страниц (in-order) =====\n");
			    printf("%-15s %-25s %-20s %-6s %-12s\n",
       			    "Автор", "Заглавие", "Издательство", "Стр.", "Год");
			    system("chcp 866 > nul");
			    PrintTreeInOrder(treeRoot);
			    // --- Поиск в дереве ---
			    system("chcp 65001 > nul");
			    int pages;
			    printf("\nВведите количество страниц для поиска: ");
			    scanf("%d", &pages);
			    struct record *found = SearchInTree(treeRoot, pages);
			    if (found) {
    				printf("\nНайдена запись:\n");
    				system("chcp 866 > nul");
    				printf("%-15s %-25s %-20s %-6d %-12d\n",
           			found->author, found->title, found->publisher,
           			found->num_of_page, found->year);
			    } else {
    				printf("Запись с таким числом страниц не найдена.\n");
			    }
			    FreeTree(treeRoot);
			    system("chcp 65001 > nul");
                     }
                 }
                 break;
            case '0':
                printf("Выход...\n");
                break;

            default:
                printf("Неверный выбор.\n");
        }

    } while (choice != '0');

    free(base);
    free(mas);
    return 0;
}