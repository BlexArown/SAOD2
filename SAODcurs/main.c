#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include "db.h"

int main() {
    system("chcp 65001 > nul");
    setlocale(LC_ALL, "C");

    struct record *base = NULL;
    list *lbase = NULL;
    struct record **mas = NULL;

    int isLoaded = 0;
    int isSorted = 0;

    
    LoadBase(&base, &lbase);
    isLoaded = 1;

    char choice;
    do {
        system("chcp 65001 > nul");
        printf("\n===== МЕНЮ =====\n");
        printf("1 - Показать базу данных ДО сортировки\n");
        printf("2 - Сортировка по году и автору\n");
        printf("3 - Поиск по году (только после сортировки по году)\n");
	printf("4 - Анализ и кодирование базы методом Гильберта–Мура\n");
        printf("0 - Выход\n");
        printf("Выбор: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
    		if (isLoaded) {
        	    if (isSorted) {
            		printf("Ошибка: база уже отсортирована — показать \"ДО сортировки\" нельзя.\n");
        	    } else {
            		system("chcp 866 > nul");
            		PrintPages(lbase);
            		system("chcp 65001 > nul");
        	    }
    		}
    		break;

            case '2':
                if (isLoaded) {
                    DigitalSort(&lbase);
                    
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
			    BTreeNode *btree = NULL;
			    list *qptr = q.head;
			    while (qptr) {
    			    	BTreeInsert(&btree, qptr->data);
    				qptr = qptr->next;
			    }
	                    system("chcp 65001 > nul");
			    printf("\n\n===== B-ДЕРЕВО по числу страниц =====\n");
			    printf("%-15s %-25s %-20s %-6s %-12s\n", "Автор", "Заглавие", "Издательство", "Стр.", "Год");
			    system("chcp 866 > nul");
			    BTreePrintInOrder(btree, 0);
			    system("chcp 65001 > nul");
			    int pages;
			    printf("\nВведите количество страниц для поиска: ");
			    scanf("%d", &pages);
			    queue found = BTreeSearchAll(btree, pages);
			    if (!found.head) {
    				printf("Записей с таким числом страниц не найдено.\n");
			    } else {
    				system("chcp 65001 > nul");
    				printf("\nНайденные записи:\n");
    				printf("%-4s %-15s %-25s %-20s %-6s %-12s\n", "№", "Автор", "Заглавие", "Издательство", "Стр.", "Год");
    			    system("chcp 866 > nul");
    			    list *p = found.head;
    			    int i = 1;
    			    while (p) {
        		    	printf("%-4d %-15s %-25s %-20s %-6d %-12d\n", i++, p->data->author, p->data->title, p->data->publisher, p->data->num_of_page, p->data->year);
        		 	p = p->next;
    			    }
    			    
    			    p = found.head;
     			    while (p) {
        			list *tmp = p;
        			p = p->next;
        			free(tmp);
    			    }
			}
			BTreeFree(btree);
			    system("chcp 65001 > nul");
                     }
                 }
                 break;

	    case '4':
		AnalyzeDatabase_GilbertMoore();
    		EncodeDatabase_GilbertMoore();
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