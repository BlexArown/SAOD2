#include "db.h"
#include <string.h>
#include <locale.h>
#include <math.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>

#ifndef MAX_SYMBOLS
#define MAX_SYMBOLS 2048
#endif

#ifndef MAX_CODE_LEN
#define MAX_CODE_LEN 64
#endif

typedef struct {
    unsigned char symbol;              
    long long freq;                    
    double prob;                       
    int code_len;                      
    char code[MAX_CODE_LEN];           
} Symbol;

int symbol_cmp_prob(const void *a, const void *b) {
    const Symbol *A = (const Symbol*)a;
    const Symbol *B = (const Symbol*)b;
    if (B->prob > A->prob) return 1;
    if (B->prob < A->prob) return -1;
    return 0;
}

void gilbert_moor_coding(Symbol symbols[], int n) {
    if (n <= 0) return;

    double Q[MAX_SYMBOLS];
    double P[MAX_SYMBOLS];

    
    if (n > MAX_SYMBOLS) n = MAX_SYMBOLS;

    for (int i = 0; i < n; i++) P[i] = symbols[i].prob;

    for (int i = 0; i < n; i++) {
        
        Q[i] = 0.0;
        for (int j = 0; j < i; j++) Q[i] += P[j];
        Q[i] += P[i] / 2.0;

        
        double pi = P[i];
        if (!(pi > 0.0)) {
            
            symbols[i].code_len = 1;
            symbols[i].code[0] = '0';
            symbols[i].code[1] = '\0';
            continue;
        }

        
        double lg = -log2(pi);
        if (!isfinite(lg) || lg < 0.0) lg = 0.0;
        int len = (int)ceil(lg) + 1;

        
        if (len >= MAX_CODE_LEN) len = MAX_CODE_LEN - 1;
        if (len < 1) len = 1;

        symbols[i].code_len = len;

        double temp = Q[i];

        
        char tmp[MAX_CODE_LEN];
        for (int j = 0; j < len; j++) {
            temp *= 2.0;
            int bit = (int)floor(temp);
            tmp[j] = bit ? '1' : '0';
            if (temp >= 1.0) temp -= 1.0;
        }
        tmp[len] = '\0';

        
        strncpy(symbols[i].code, tmp, MAX_CODE_LEN - 1);
        symbols[i].code[MAX_CODE_LEN - 1] = '\0';
    }
}


static int find_symbol(Symbol symbols[], int n, unsigned char c)
{
    for (int i = 0; i < n; ++i) {
        if (symbols[i].symbol == c)
            return i;
    }
    return -1;
}


static void print_symbol_char(unsigned char c)
{
    if (c == '\n')      printf("\\n");
    else if (c == '\r') printf("\\r");
    else if (c == '\t') printf("\\t");
    else if (c == '\'') printf("\\'");
    else                printf("%c", c);
}


static int build_gm_code(const char *filename, Symbol symbols[], int *n_symbols, long long *total_bytes)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Ошибка: не удалось открыть файл '%s'\n", filename);
        return 0;
    }

    *n_symbols = 0;
    *total_bytes = 0;

    unsigned char buf[4096];
    size_t read_bytes;

    while ((read_bytes = fread(buf, 1, sizeof(buf), f)) > 0) {
        for (size_t i = 0; i < read_bytes; ++i) {
            unsigned char c = buf[i];

            int idx = find_symbol(symbols, *n_symbols, c);
            if (idx == -1) {
                if (*n_symbols >= MAX_SYMBOLS) {
                    fclose(f);
                    printf("Слишком много различных символов (>%d)\n", MAX_SYMBOLS);
                    return 0;
                }
                symbols[*n_symbols].symbol   = c;
                symbols[*n_symbols].freq     = 0;
                symbols[*n_symbols].prob     = 0.0;
                symbols[*n_symbols].code_len = 0;
                symbols[*n_symbols].code[0]  = '\0';
                idx = (*n_symbols)++;
            }

            symbols[idx].freq++;
            (*total_bytes)++;
        }
    }

    fclose(f);

    if (*total_bytes == 0 || *n_symbols == 0) {
        printf("Файл пуст или в нём нет символов.\n");
        return 0;
    }

    
    for (int i = 0; i < *n_symbols; ++i) {
        symbols[i].prob = (double)symbols[i].freq / (double)(*total_bytes);
    }

    qsort(symbols, *n_symbols, sizeof(Symbol), symbol_cmp_prob);

    gilbert_moor_coding(symbols, *n_symbols);

    return 1;
}

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


void DigitalSort(list **head) {
    if (!head || !*head) return;

    
    int len = 0;
    list *p = *head;
    while (p) { len++; p = p->next; }

    list **arr = malloc(len * sizeof(list*));
    p = *head;
    for (int i = 0; i < len; i++, p = p->next) arr[i] = p;

    
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

    
    for (int i = 0; i < len; i++) {
        arr[i]->next  = (i + 1 < len) ? arr[i + 1] : NULL;
        arr[i]->prior = (i > 0) ? arr[i - 1] : NULL;
    }
    *head = arr[0];

    free(arr);
}

int BSearch(struct record **A, int year) {
    int L = 0, R = N - 1;
    int m;

    while (L < R) {
        m = (L + R) / 2;

        if (A[m]->year < year) {
            L = m + 1;
        } else {
            R = m;
        }
    }

    if (A[R]->year == year)
        return R;
    else
        return -1;
}

void PrintPages(list *head) {
    if (!head) return;

    const int pageSize = 20;

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
        printf("\nСтраница %d из %d (всего записей: %d)\n", currentPage + 1, totalPages, total);
        printf("====================================================================================\n");
        printf("%-4s %-15s %-25s %-20s %-6s %-12s\n", "№", "     Автор", "               Заглавие", "              Издательство", "        Стр.", "  Год");
        printf("====================================================================================\n\n");
#ifdef _WIN32
        system("chcp 866 > nul");
#endif
        int start = currentPage * pageSize;
        int end = start + pageSize;
        if (end > total) end = total;

        for (int i = start; i < end; i++) {
            printf("%-4d %-15s %-25s %-20s %-6d %-12d\n", i + 1, arr[i]->data->author, arr[i]->data->title, arr[i]->data->publisher, arr[i]->data->num_of_page, arr[i]->data->year);
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

        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = 0;

        char *cmd = input;
        while (*cmd == ' ' || *cmd == '\t') cmd++;

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

queue SearchQueue(struct record **A, int year) {
    queue result = {NULL, NULL};
    int pos = BSearch(A, year);
    if (pos == -1) return result;

    while (pos < N && A[pos]->year == year) {
        list *node = (list*)malloc(sizeof(list));
        node->data = A[pos];
        node->next = NULL;
        node->prior = NULL;
        node->pos = pos + 1;

        if (!result.head) result.head = result.tail = node;
        else {
            result.tail->next = node;
            result.tail = node;
        }
        pos++;
    }
    return result;
}

void PrintQueue(queue q) {
    list *p = q.head;

    system("chcp 65001 > nul");
    printf("\n%-4s %-15s %-25s %-20s %-6s %-12s\n", "№", "Автор", "Заглавие", "Издательство", "Стр.", "Год");
    system("chcp 866 > nul");
    while (p) {
        printf("%-4d %-15s %-25s %-20s %-6d %-12d\n", p->pos, p->data->author, p->data->title, p->data->publisher, p->data->num_of_page, p->data->year);
        p = p->next;
    }
}

BTreeNode* BTreeCreateNode(int leaf) {
    BTreeNode *node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->leaf = leaf;
    node->n = 0;
    for (int i = 0; i < 2*BTREE_T; i++)
        node->child[i] = NULL;
    return node;
}

void BTreeSplitChild(BTreeNode *x, int i) {
    int t = BTREE_T;

    BTreeNode *y = x->child[i];
    BTreeNode *z = BTreeCreateNode(y->leaf);

    z->n = t - 1;

    for (int j = 0; j < t - 1; j++)
        z->keys[j] = y->keys[j + t];

    if (!y->leaf) {
        for (int j = 0; j < t; j++)
            z->child[j] = y->child[j + t];
    }

    y->n = t - 1;

    for (int j = x->n; j >= i + 1; j--)
        x->child[j + 1] = x->child[j];

    x->child[i + 1] = z;

    for (int j = x->n - 1; j >= i; j--)
        x->keys[j + 1] = x->keys[j];

    x->keys[i] = y->keys[t - 1];
    x->n += 1;
}

void BTreeInsertNonFull(BTreeNode *x, struct record *rec) {
    int i = x->n - 1;

    if (x->leaf) {
        
        while (i >= 0 && rec->num_of_page < x->keys[i]->num_of_page) {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i + 1] = rec;
        x->n += 1;
    }
    else {
        
        while (i >= 0 && rec->num_of_page < x->keys[i]->num_of_page)
            i--;

        i++;

        if (x->child[i]->n == 2*BTREE_T - 1) {
            BTreeSplitChild(x, i);

            if (rec->num_of_page > x->keys[i]->num_of_page)
                i++;
        }

        BTreeInsertNonFull(x->child[i], rec);
    }
}

void BTreeInsert(BTreeNode **root, struct record *rec) {
    if (*root == NULL) {
        *root = BTreeCreateNode(1);
        (*root)->keys[0] = rec;
        (*root)->n = 1;
        return;
    }

    BTreeNode *r = *root;

    if (r->n == 2*BTREE_T - 1) {
        BTreeNode *s = BTreeCreateNode(0);
        *root = s;
        s->child[0] = r;

        BTreeSplitChild(s, 0);

        int i = 0;
        if (rec->num_of_page > s->keys[0]->num_of_page)
            i++;

        BTreeInsertNonFull(s->child[i], rec);
    } else {
        BTreeInsertNonFull(r, rec);
    }
}

struct record* BTreeSearch(BTreeNode *x, int pages) {
    int i = 0;

    while (i < x->n && pages > x->keys[i]->num_of_page)
        i++;

    if (i < x->n && pages == x->keys[i]->num_of_page)
        return x->keys[i];

    if (x->leaf)
        return NULL;

    return BTreeSearch(x->child[i], pages);
}

void BTreePrint(BTreeNode *root, int depth) {
    if (!root) return;

    for (int d = 0; d < depth; d++)
        printf("   ");

    printf("[ ");
    for (int i = 0; i < root->n; i++)
        printf("%d ", root->keys[i]->num_of_page);
    printf("]\n");

    if (!root->leaf) {
        for (int i = 0; i <= root->n; i++)
            BTreePrint(root->child[i], depth + 1);
    }
}

void BTreePrintInOrder(BTreeNode *root, int depth) {
    if (!root) return;
    for (int i = 0; i < root->n; i++) {
        if (!root->leaf)
            BTreePrintInOrder(root->child[i], depth);
        printf("%-15s %-25s %-20s %-6d %-12d\n",root->keys[i]->author,root->keys[i]->title,root->keys[i]->publisher,root->keys[i]->num_of_page,root->keys[i]->year);
    }
    if (!root->leaf)
        BTreePrintInOrder(root->child[root->n], depth);
}

static void _bts_search_rec(BTreeNode *node, int pages, queue *out) {
    if (!node) return;

    for (int i = 0; i < node->n; i++) {

        if (!node->leaf)
            _bts_search_rec(node->child[i], pages, out);

        if (node->keys[i]->num_of_page == pages) {
            list *ln = (list*)malloc(sizeof(list));
            ln->data = node->keys[i];
            ln->next = NULL;
            ln->prior = NULL;
            ln->pos = 0;

            if (!out->head) out->head = out->tail = ln;
            else {
                out->tail->next = ln;
                out->tail = ln;
            }
        }
    }

    if (!node->leaf)
        _bts_search_rec(node->child[node->n], pages, out);
}

queue BTreeSearchAll(BTreeNode *root, int pages) {
    queue q = {NULL, NULL};
    _bts_search_rec(root, pages, &q);
    return q;
}

void BTreeFree(BTreeNode *x) {
    if (!x) return;

    if (!x->leaf) {
        for (int i = 0; i <= x->n; i++)
            BTreeFree(x->child[i]);
    }
    free(x);
}

void AnalyzeDatabase_GilbertMoore()
{
    const char *filename = "testBase1.dat";

    Symbol symbols[MAX_SYMBOLS];
    int n_symbols = 0;
    long long total_bytes = 0;

    if (!build_gm_code(filename, symbols, &n_symbols, &total_bytes)) {
        return;
    }

    
    system("chcp 65001 > nul");
    printf("\nКоличество уникальных символов: %d\n", n_symbols);
    printf("\n===== ТАБЛИЦА КОДОВ ГИЛБЕРТА–МУРА =====\n");
    printf("%-8s %-10s %-12s %-5s %-s\n", "Символ", "Частота", "Prob", "L", "Код");
    printf("---------------------------------------------------------------------\n");
    
    system("chcp 866 > nul");
    for (int i = 0; i < n_symbols; ++i) {
        printf("'");
        print_symbol_char(symbols[i].symbol);
        printf("' %-10lld %-12.6f %-5d %s\n", symbols[i].freq, symbols[i].prob, symbols[i].code_len, symbols[i].code);
    }
    
    system("chcp 65001 > nul");
    
    double H = 0.0, Lavg = 0.0, kraft = 0.0;
    for (int i = 0; i < n_symbols; ++i) {
        double p = symbols[i].prob;
        if (p > 0.0) {
            H += -p * log2(p);
            Lavg += p * symbols[i].code_len;
            kraft += pow(2.0, -symbols[i].code_len);
        }
    }

    long long original_bits = total_bytes * 8;
    long long encoded_bits  = 0;
    for (int i = 0; i < n_symbols; ++i) {
        encoded_bits += symbols[i].freq * (long long)symbols[i].code_len;
    }

    double compression_ratio = (double)encoded_bits / (double)original_bits * 100.0;

    printf("\nЭнтропия H = %.6f бит\n", H);
    printf("Средняя длина L = %.6f бит\n", Lavg);
    printf("Избыточность R = L - H  = %.6f бит\n", Lavg - H);
    printf("Сумма Крафта = %.6f\n", kraft);
    printf("Исходный размер = %lld байт (%lld бит)\n", total_bytes, original_bits);
    printf("Теоретический размер кода = %lld бит\n", encoded_bits);
    printf("Теоретический коэфф. сжатия = %.2f%%\n", compression_ratio);
}

void EncodeDatabase_GilbertMoore()
{
    const char *in_name  = "testBase1.dat";
    const char *out_name = "gm_encoded.dat";

    Symbol symbols[MAX_SYMBOLS];
    int n_symbols = 0;
    long long total_bytes = 0;
    
    if (!build_gm_code(in_name, symbols, &n_symbols, &total_bytes)) {
        return;
    }

    FILE *fin = fopen(in_name, "rb");
    if (!fin) {
        printf("Ошибка: не удалось открыть '%s' для чтения\n", in_name);
        return;
    }

    FILE *fout = fopen(out_name, "wb");
    if (!fout) {
        printf("Ошибка: не удалось открыть '%s' для записи\n", out_name);
        fclose(fin);
        return;
    }

    unsigned char in_buf[4096];
    size_t read_bytes;
    unsigned char out_byte = 0;
    int bits_in_byte = 0;
    long long encoded_bytes = 0;

    while ((read_bytes = fread(in_buf, 1, sizeof(in_buf), fin)) > 0) {
        for (size_t i = 0; i < read_bytes; ++i) {
            unsigned char c = in_buf[i];
            int idx = find_symbol(symbols, n_symbols, c);
            if (idx == -1) {
                
                continue;
            }

            char *code = symbols[idx].code;
            for (int j = 0; code[j] != '\0'; ++j) {
                int bit = (code[j] == '1') ? 1 : 0;
                out_byte = (out_byte << 1) | bit;
                bits_in_byte++;

                if (bits_in_byte == 8) {
                    fputc(out_byte, fout);
                    encoded_bytes++;
                    out_byte = 0;
                    bits_in_byte = 0;
                }
            }
        }
    }
    
    if (bits_in_byte > 0) {
        out_byte <<= (8 - bits_in_byte);
        fputc(out_byte, fout);
        encoded_bytes++;
    }

    fclose(fin);
    fclose(fout);

    long long original_bits = total_bytes * 8;
    long long encoded_bits  = encoded_bytes * 8;

    double compression_ratio = (double)encoded_bits / (double)original_bits * 100.0;

    printf("\n=== Кодирование базы (Гилберт–Мур) ===\n");
    printf("Исходный файл:      %s\n", in_name);
    printf("Упакованный файл:   %s\n", out_name);
    printf("Исходный размер:    %lld байт (%lld бит)\n", total_bytes, original_bits);
    printf("Записано:           %lld байт (%lld бит)\n", encoded_bytes, encoded_bits);
    printf("Фактический коэфф. сжатия: %.2f%%\n", compression_ratio);
}