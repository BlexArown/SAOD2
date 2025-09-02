#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

char initial_string[] = "notebka canvas of unimaginable depth, always has a way of humbling me. Looking up, I see a tapestry woven with countless stars, each a distant sun, perhaps with its own orbiting worlds. It’s a profound reminder of our place in the universe, a tiny speck in an ocean of cosmic wonders. A9!pkj3X%C cat dot she he carThe sheer A9!pkj3X%C  scale of it all is mind-boggling. When we think about the billions of galaxies, each containing billions of stars, the sheer number of possibilities for life becomes almost overwhelming. Are we truly alone? It’s a question that has captivated humanity for centuries, fueling our curiosity and driving our scientific A9!pkj3X%C endeavors. Perhaps someday, through advances in technology and our insatiable quest for knowledge, we’ll find an answer. A9!pkj3X%C The moon, a faithful companion to our planet, casts a soft, ethereal glow, transforming the familiar landscape into something mysterious and magical. Shadows lengthen and deepen, creating an atmosphere of quiet enchantment.";
char initial_substring[] = "way of humbling";

bool compare(const char *haystack, int start, const char *needle, int m, int *comp)
{
    for (int i = 0; i < m; i++)
    {
        (*comp)++;
        if (haystack[start + i] != needle[i])
        {
            return false;
        }
    }
    return true;
}

void print_positions(const char *method_name, int positions[], int count, int comparisons)
{
    printf("%-25s", method_name);
    printf("Найдено: ");
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            printf("%d", positions[i]);
            if (i < count - 1) {
                printf(", ");
            }
        }
        printf(" Сравнений: %d", comparisons);
    } else {
        printf("Нет совпадений.");
    }
    printf("\n");
}

int direct_search(char *string, char *substring, int *comparisons)
{
    int string_len = strlen(string);
    int substring_len = strlen(substring);

    int found_count = 0;
    int positions[100];

    *comparisons = 0;

    for (int i = 0; i <= string_len - substring_len; i++)
    {
        if (compare(string, i, substring, substring_len, comparisons))
        {
            positions[found_count++] = i;
        }
    }

    print_positions("Прямой поиск", positions, found_count, *comparisons);
    return found_count;
}

int hash_string(const char *str, int start, int end, int q)
{
    int hash = 0;
    for (int i = start; i <= end; i++)
    {
        hash = (hash * 256 + (unsigned char)str[i]) % q;
    }
    return hash;
}

int rabin_karp_search(char *haystack, char *needle, int q, int *comparisons)
{
    int m = strlen(needle);
    int n = strlen(haystack);
    int found_count = 0;
    int positions[100];

    *comparisons = 0;

    int hash_needle = hash_string(needle, 0, m - 1, q);
    int hash_haystack = hash_string(haystack, 0, m - 1, q);

    for (int i = 0; i <= n - m; i++)
    {
        if (hash_haystack == hash_needle)
        {
            if (compare(haystack, i, needle, m, comparisons))
            {
                positions[found_count++] = i;
            }
        }
        if (i < n - m)
        {
            hash_haystack = hash_string(haystack, i + 1, i + m, q);
        }
    }

    char method_name[50];
    snprintf(method_name, sizeof(method_name), "RabinKarp (q=%d)", q);
    print_positions(method_name, positions, found_count, *comparisons);

    return found_count;
}

int main()
{
    int q[2] = {61, 67};

    printf("Text: %s\n", initial_string);
    printf("Подстрока: %s\n\n", initial_substring);

    printf("Длина текста: %d символов\n", strlen(initial_string));
    printf("Длина подстроки: %d символов\n\n", strlen(initial_substring));

    int direct_comparisons;
    direct_search(initial_string, initial_substring, &direct_comparisons);

    for (int i = 0; i < 2; i++)
    {
        int rk_comparisons;
        rabin_karp_search(initial_string, initial_substring, q[i], &rk_comparisons);
    }

    return 0;
}
