#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#define MAX_SYMBOLS 1024
#define MAX_CODE_LEN 64
typedef struct{
    wchar_t symbol;
    long double prob;
    long long freq;
    int code_len;
    wchar_t code[MAX_CODE_LEN];
} Symbol;
int compare_prob(const void *a,const void *b){
    long double diff=((Symbol*)b)->prob-((Symbol*)a)->prob;
    return (diff>0)-(diff<0);
}
int Med(Symbol symbols[], int L, int R) {
    long double SL = 0;
    long double SR;
    int m;
    for (int i = L; i < R; i++) {
        SL += symbols[i].prob;
    }
    SR = symbols[R].prob;
    m = R;
    while (SL >= SR) {
        m = m - 1;
        SL = SL - symbols[m].prob;
        SR = SR + symbols[m].prob;
    }
    return m;
}
int MedA2(Symbol symbols[], int L, int R) {
    long double wes = 0.0L;
    for (int i = L; i <= R; i++) {
        wes += symbols[i].prob;
    }
    long double half = wes / 2.0L;
    long double summa = 0.0L;
    for (int i = L; i <= R; i++) {
        if (summa < half && summa + symbols[i].prob >= half) {
            return i;
        }
        summa += symbols[i].prob;
    }
    return R;
}
void Fano_coding(Symbol symbols[], int L, int R, int k) {
    if (L < R) {
        k = k + 1;
        int m = Med(symbols, L, R);
        for (int i = L; i <= R; i++) {
            if (i <= m) {
                symbols[i].code[k-1] = L'0';
                symbols[i].code_len = symbols[i].code_len + 1;
            }
            else {
                symbols[i].code[k-1] = L'1';
                symbols[i].code_len = symbols[i].code_len + 1;
            }
        }
        for (int i = L; i <= R; i++) {
            symbols[i].code[k] = L'\0';
        }
        Fano_coding(symbols, L, m, k);
        Fano_coding(symbols, m+1, R, k);
    }
    else if (L == R) {
        symbols[L].code[k] = L'\0';
    }
}
void Fano_codingA2(Symbol symbols[], int L, int R, int k) {
    if (L < R) {
        k = k + 1;
        int m = MedA2(symbols, L, R);
        for (int i = L; i <= R; i++) {
            if (i <= m) {
                symbols[i].code[k-1] = L'0';
                symbols[i].code_len = symbols[i].code_len + 1;
            }
            else {
                symbols[i].code[k-1] = L'1';
                symbols[i].code_len = symbols[i].code_len + 1;
            }
        }
        for (int i = L; i <= R; i++) {
            symbols[i].code[k] = L'\0';
        }
       
        Fano_codingA2(symbols, L, m, k);
        Fano_codingA2(symbols, m+1, R, k);
    }
    else if (L == R) {
        symbols[L].code[k] = L'\0';
    }
}
int Up(int n, long double q, long double P[]) {
    int i = n - 1;
    int j;
    while (i >= 2 && P[i - 1] <= q) {
        P[i] = P[i - 1];
        i--;
    }
    j = i;
    P[j] = q;
    return j;
}
void Down(int n, int j, wchar_t C[][MAX_CODE_LEN], int L[]) {
    wchar_t S[MAX_CODE_LEN];
    wcscpy(S, C[j]);
    int L_var = L[j];
    for (int i = j; i <= n - 2; i++) {
        wcscpy(C[i], C[i + 1]);
        L[i] = L[i + 1];
    }
    wcscpy(C[n - 1], S);
    wcscpy(C[n], S);
    C[n - 1][L_var] = L'0';
    C[n - 1][L_var + 1] = L'\0';
    C[n][L_var] = L'1';
    C[n][L_var + 1] = L'\0';
    L[n - 1] = L_var + 1;
    L[n] = L_var + 1;
}
void Huffman(int n, long double P[], wchar_t C[][MAX_CODE_LEN], int L[]) {
    if (n == 2) {
        C[1][0] = L'0';
        C[1][1] = L'\0';
        L[1] = 1;
        C[2][0] = L'1';
        C[2][1] = L'\0';
        L[2] = 1;
    } else {
        long double q = P[n - 1] + P[n];
        int j = Up(n, q, P);
        Huffman(n - 1, P, C, L);
        Down(n, j, C, L);
    }
}


void Mur(Symbol symbols[], int n) {
    double Q[MAX_SYMBOLS]; 
    double P[MAX_SYMBOLS]; 
    
    for (int i = 0; i < n; i++) {
        P[i] = symbols[i].prob;
    }
    
    for (int i = 0; i < n; i++) {
        Q[i] = 0.0;
        
        for (int j = 0; j < i; j++) {
            Q[i] += P[j];
        }
        
        Q[i] += P[i] / 2.0;
        
        for (int j = 0; j < i; j++) {
        }
    }
    wprintf(L"\n");
    
    for (int i = 0; i < n; i++) {
        int l_i = (int)ceil(-log2(P[i])) + 1;
        symbols[i].code_len = l_i;
    }
    wprintf(L"\n");
    
    for (int i = 0; i < n; i++) {
        double temp_Q = Q[i];
        int l_i = symbols[i].code_len;
        for (int j = 0; j < l_i; j++) {
            temp_Q *= 2;
            int bit = (int)floor(temp_Q);
            symbols[i].code[j] = (bit == 1) ? '1' : '0';
            if (temp_Q >= 1.0) {
                temp_Q -= 1.0;
            }
        }
        symbols[i].code[l_i] = '\0';
    }
}

void shannon_coding(Symbol symbols[], int n) {
    double P[MAX_SYMBOLS + 1];
    double Q[MAX_SYMBOLS + 1];
    int L[MAX_SYMBOLS + 1];

    P[0] = 0.0;
    Q[0] = 0.0;

    for (int i = 1; i <= n; i++) {
        P[i] = symbols[i - 1].prob;
        Q[i] = Q[i - 1] + P[i];
        L[i] = (int)ceil(-log2(P[i]));
    }

    wprintf(L"\nКумулятивные вероятности Q (Шеннон):\n");
    wprintf(L"Q0 = 0.0000\n");
    for (int i = 1; i <= n; i++) {
        wprintf(L"Q%d = Q%d + p%d = %.4f + %.4f = %.4f\n", i, i - 1, i, Q[i - 1], P[i], Q[i]);
    }
    wprintf(L"\n");

    for (int i = 1; i <= n; i++) {
        double temp_Q = Q[i - 1];
        for (int j = 0; j < L[i]; j++) {
            temp_Q *= 2;
            int bit = (int)floor(temp_Q);
            symbols[i - 1].code[j] = bit ? '1' : '0';
            if (temp_Q >= 1.0) {
                temp_Q -= 1.0;
            }
        }
        symbols[i - 1].code[L[i]] = '\0';
        symbols[i - 1].code_len = L[i];
    }
}



int main(void){
    system("clear");
    setlocale(LC_ALL, "");
    FILE *f=fopen("text.txt", "r");
    if (!f){
        perror("Ошибка открытия файла");
        return 1;
    }
    Symbol base[MAX_SYMBOLS];
    int n_symbols=0;
    long long total=0;
    wint_t c;
    struct{
        wchar_t sym;
        long long freq;
    } freq[MAX_SYMBOLS];
    int freq_count=0;
    while ((c=fgetwc(f)) != WEOF){
        total++;
        int found=0;
        for (int i=0; i < freq_count; i++){
            if (freq[i].sym == c){
                freq[i].freq++;
                found=1;
                break;
            }
        }
        if (!found){
            freq[freq_count].sym=c;
            freq[freq_count].freq=1;
            freq_count++;
        }
    }
    fclose(f);
    for (int i=0; i < freq_count; i++){
        base[n_symbols].symbol=freq[i].sym;
        base[n_symbols].freq=freq[i].freq;
        base[n_symbols].prob=(long double)freq[i].freq / total;
        base[n_symbols].code_len=0;
        base[n_symbols].code[0]=L'\0';
        n_symbols++;
    }
    qsort(base, n_symbols, sizeof(Symbol), compare_prob);
    
    
    
    Symbol symbols_shannon[MAX_SYMBOLS];
    memcpy(symbols_shannon, base, sizeof(Symbol)*n_symbols);
    shannon_coding(symbols_shannon,n_symbols);
    wprintf(L"\n=======================Shannon=======================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");
    wprintf(L"-------------------------------------------------------------\n");
    for(int i=0;i<n_symbols;i++){
        wprintf(L"%-8lc %-10lld %-12.4Lf %-8d %-10ls\n",
                symbols_shannon[i].symbol,
                symbols_shannon[i].freq,
                symbols_shannon[i].prob,
                symbols_shannon[i].code_len,
                symbols_shannon[i].code);
    }
    long double kraft0=0, H0=0, L0=0;
    for(int i=0;i<n_symbols;i++){
        kraft0 += powl(2.0L, -symbols_shannon[i].code_len);
        H0 += -symbols_shannon[i].prob * log2l(symbols_shannon[i].prob);
        L0 += symbols_shannon[i].prob * symbols_shannon[i].code_len;
    }
    long double R0 = L0 - H0;
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22ls ║ %-22ls ║ %-26ls ║ %-22ls ║\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22.6Lf ║ %-22.6Lf ║ %-26.6Lf ║ %-22.6Lf ║\n", kraft0, H0, L0, R0);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    
      
    Symbol symbols_classic[MAX_SYMBOLS];
    Symbol symbols_a2[MAX_SYMBOLS];
    memcpy(symbols_classic, base, sizeof(Symbol)*n_symbols);
    memcpy(symbols_a2, base, sizeof(Symbol)*n_symbols);
    for(int i=0;i<n_symbols;i++){
        symbols_classic[i].code_len=0;
        symbols_classic[i].code[0]=L'\0';
        symbols_a2[i].code_len=0;
        symbols_a2[i].code[0]=L'\0';
    }
    Fano_coding(symbols_classic, 0, n_symbols-1, 0);
    wprintf(L"\n================фано================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");
    wprintf(L"-------------------------------------------------------------\n");
    for(int i=0;i<n_symbols;i++){
        wprintf(L"%-8lc %-10lld %-12.4Lf %-8d %-10ls\n",
                symbols_classic[i].symbol,
                symbols_classic[i].freq,
                symbols_classic[i].prob,
                symbols_classic[i].code_len,
                symbols_classic[i].code);
    }
    long double kraft1=0, H1=0, L1=0;
    for(int i=0;i<n_symbols;i++){
        kraft1 += powl(2.0L, -symbols_classic[i].code_len);
        H1 += -symbols_classic[i].prob * log2l(symbols_classic[i].prob);
        L1 += symbols_classic[i].prob * symbols_classic[i].code_len;
    }
    long double R1 = L1 - H1;
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"| %-22ls | %-22ls | %-26ls | %-22ls |\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"| %-22.6Lf | %-22.6Lf | %-26.6Lf | %-22.6Lf |\n", kraft1, H1, L1, R1);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    Fano_codingA2(symbols_a2, 0, n_symbols-1, 0);
    Symbol symbols_huff[MAX_SYMBOLS];
    memcpy(symbols_huff, base, sizeof(Symbol)*n_symbols);
    long double P[MAX_SYMBOLS + 1];
    int HL[MAX_SYMBOLS + 1];
    wchar_t HC[MAX_SYMBOLS + 1][MAX_CODE_LEN];
    for (int i = 1; i <= n_symbols; i++) {
        P[i] = symbols_huff[i - 1].prob;
        HL[i] = 0;
        HC[i][0] = L'\0';
    }
    Huffman(n_symbols, P, HC, HL);
    for (int i = 1; i <= n_symbols; i++) {
        wcscpy(symbols_huff[i - 1].code, HC[i]);
        symbols_huff[i - 1].code_len = HL[i];
    }
    wprintf(L"\n=======================Huffman=======================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");
    wprintf(L"-------------------------------------------------------------\n");
    for(int i=0;i<n_symbols;i++){
        wprintf(L"%-8lc %-10lld %-12.4Lf %-8d %-10ls\n",
                symbols_huff[i].symbol,
                symbols_huff[i].freq,
                symbols_huff[i].prob,
                symbols_huff[i].code_len,
                symbols_huff[i].code);
    }
    long double kraft3=0, H3=0, L3=0;
    for(int i=0;i<n_symbols;i++){
        kraft3 += powl(2.0L, -symbols_huff[i].code_len);
        H3 += -symbols_huff[i].prob * log2l(symbols_huff[i].prob);
        L3 += symbols_huff[i].prob * symbols_huff[i].code_len;
    }
    long double R3 = L3 - H3;
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22ls ║ %-22ls ║ %-26ls ║ %-22ls ║\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22.6Lf ║ %-22.6Lf ║ %-26.6Lf ║ %-22.6Lf ║\n", kraft3, H3, L3, R3);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    
    
    //---------------------------------------------------------------------------++++++++++++++++++++++++++++++
    Symbol symbols_mur[MAX_SYMBOLS];
    memcpy(symbols_mur, base, sizeof(Symbol)*n_symbols);
    Mur(symbols_mur,n_symbols);
    wprintf(L"\n=======================Gilbert-Moor=======================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");
    wprintf(L"-------------------------------------------------------------\n");
    for(int i=0;i<n_symbols;i++){
        wprintf(L"%-8lc %-10lld %-12.4Lf %-8d %-10ls\n",
                symbols_mur[i].symbol,
                symbols_mur[i].freq,
                symbols_mur[i].prob,
                symbols_mur[i].code_len,
                symbols_mur[i].code);
    }
    long double kraft4=0, H4=0, L4=0;
    for(int i=0;i<n_symbols;i++){
        kraft4 += powl(2.0L, -symbols_mur[i].code_len);
        H4 += -symbols_mur[i].prob * log2l(symbols_mur[i].prob);
        L4 += symbols_mur[i].prob * symbols_mur[i].code_len;
    }
    long double R4 = L4 - H4;
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22ls ║ %-22ls ║ %-26ls ║ %-22ls ║\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22.6Lf ║ %-22.6Lf ║ %-26.6Lf ║ %-22.6Lf ║\n", kraft4, H4, L4, R4);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    
    wprintf(L"\n=======================Shannon=======================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");
    wprintf(L"-------------------------------------------------------------\n");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22ls ║ %-22ls ║ %-26ls ║ %-22ls ║\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22.6Lf ║ %-22.6Lf ║ %-26.6Lf ║ %-22.6Lf ║\n", kraft0, H0, L0, R0);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n\n");
    
    
    wprintf(L"\n================Fano================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");
    wprintf(L"-------------------------------------------------------------\n");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"| %-22ls | %-22ls | %-26ls | %-22ls |\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"| %-22.6Lf | %-22.6Lf | %-26.6Lf | %-22.6Lf |\n", kraft1, H1, L1, R1);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n\n");
    
    
    
    wprintf(L"\n=======================Huffman=======================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");

    wprintf(L"-------------------------------------------------------------\n");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22ls ║ %-22ls ║ %-26ls ║ %-22ls ║\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22.6Lf ║ %-22.6Lf ║ %-26.6Lf ║ %-22.6Lf ║\n", kraft3, H3, L3, R3);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n\n");
    
    
    
    wprintf(L"\n=======================Gilbert-Moor=======================\n");
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-10ls\n",
            L"Символ", L"Частота", L"Вероятность", L"Длина", L"Код");
    wprintf(L"-------------------------------------------------------------\n");
    
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22ls ║ %-22ls ║ %-26ls ║ %-22ls ║\n",
            L"Крафт", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    wprintf(L"║ %-22.6Lf ║ %-22.6Lf ║ %-26.6Lf ║ %-22.6Lf ║\n", kraft4, H4, L4, R4);
    wprintf(L"---------------------------------------------------------------------------------------------------------\n");
    
    return 0;
}
