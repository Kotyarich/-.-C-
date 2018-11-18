//Требуется написать программу, которая способна вычислять арифметические выражения.
//Выражения могут содержать:
//1) знаки операций '+', '-', '/', '*'
//2) Скобки '(', ')'
//3) Целые числа, в нотации '123', в том числе они достаточно большие (требуется реализовать длинную арифметику),
// все операции должны быть целочисленные
//4) необходимо учитывать приоритеты операций, и возможность унарного минуса, пробелы ничего не значат
//5) Если в выражении встретилась ошибка требуется вывести в стандартный поток вывода "[error]" (без кавычек)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define ADDITION_OP '+'
#define MULTIPLICATION_OP '*'
#define DIVISION_OP '/'
#define SUBTRACTION_OP '-'
#define OPEN_BRACKET '('
#define CLOSE_BRACKET ')'
#define OPERATORS "+*/-()"
#define ALLOCATION_ERR -1
#define TEST_FAILED -2

typedef struct
{
   int *digit;
   size_t len;
} long_int;

// Элемен стека
typedef struct node_t
{
    void *data;
    struct node_t *next;
} node_t;

// Извлечение элемента из стека
int pop(node_t **node)
{
    assert(node);

    node_t *prev = *node;
    (*node) = (*node)->next;

    free(prev);
    return 0;
}

// Добавление элемента в стек
int push(node_t **node, const void *value, size_t size)
{
    assert(node && value && size > 0);

    node_t *new_node = (node_t*) malloc(sizeof(node_t));
    if(!new_node)
    {
        return ALLOCATION_ERR;
    }

    new_node->data = malloc(size);
    if (!new_node->data)
    {
        free(new_node);
        return ALLOCATION_ERR;
    }
    memcpy(new_node->data, value, size);

    new_node->next = *node;
    (*node) = new_node;

    return 0;
}

// Удаление стека
void delete_stack(node_t **node)
{
    assert(node);

    while ((*node)->next)
    {
        free((*node)->data);
        pop(node);
    }
    free((*node)->data);
    free(*node);
    *node = NULL;
}

int max(int a, int b)
{
    return a > b? a : b;
}


int min(int a, int b)
{
    return a < b? a : b;
}

// Суммирование двух длинных чисел
long_int sum(const long_int a, const long_int b)
{
    assert(a.digit && b.digit);

    long_int res;
    res.len = max(a.len, b.len);
    res.digit = malloc(sizeof(int) * (res.len + 2));
    if (!res.digit)
    {
        return res;
    }
    res.digit[res.len + 1] = 0;
    size_t min_len = min(a.len, b.len);

    for (size_t i = 1; i <= min_len; i++)
    {
        res.digit[i] = a.digit[i] + b.digit[i];
    }
    for (size_t i = min_len + 1; i <= a.len; i++)
    {
        res.digit[i] = a.digit[i];
    }
    for (size_t i = min_len + 1; i <= b.len; i++)
    {
        res.digit[i] = b.digit[i];
    }

    for (size_t i = 1; i <= res.len; i++)
    {
        if (res.digit[i] > 9)
        {
            res.digit[i] -= 10;
            res.digit[i + 1]++;
        }
    }
    if (res.digit[res.len + 1] != 0)
    {
        res.len++;
    }

    return res;
}

// Вычитание двух длинных чисел, a > b
void subtraction(long_int *a, const long_int b)
{
    assert(a && a->digit && b.digit);

    for (size_t i = 1; i <= b.len; i++)
    {
        a->digit[i] -= b.digit[i];
        if (a->digit[i] < 0)
        {
            a->digit[i] += 10;
            a->digit[i + 1]--;
        }
    }
    if (a->digit[a->len] == 0 && a->len > 1)
    {
        a->len--;
    }
}

// Умножение двух длинных чисел
long_int multiplication(const long_int a, const long_int b)
{
    assert(a.digit && b.digit);

    long_int res;
    res.len = a.len + b.len;
    res.digit = calloc(a.len + b.len + 1, sizeof(int));
    if (!res.digit)
    {
        return res;
    }

    for (size_t i = 1; i <= b.len; i++)
    {
        for (size_t j = 1; j <= a.len; j++)
        {
            res.digit[i + j - 1] += a.digit[j] * b.digit[i];
        }
    }
    for (size_t i = 1; i < res.len; i++)
    {
        res.digit[i + 1] += res.digit[i] / 10;
        res.digit[i] %= 10;
    }
    // Удаление незначащих нулей
    while (!res.digit[res.len])
    {
        res.len--;
    }

    return res;
}

/*
 * Сравнение 2ух длинных чисел
 * Возвращает положительное число, если первое больше, 0 - если числа равны,
 * отрицательное число - если первое меньше.
*/
int compare(const long_int a, const long_int b)
{
    assert(a.digit && b.digit);

    if (a.len != b.len)
    {
        return a.len - b.len;
    }

    for (size_t i = a.len; i > 0; i--)
    {
        if (a.digit[i] != b.digit[i])
        {
            return a.digit[i] - b.digit[i];
        }
    }

    return 0;
}

// Деление 2ух длинных чисел
long_int division(long_int *a, const long_int b)
{
    assert(a && a->digit && b.digit);

    long_int res, zero;
    res.len = 0;
    res.digit = calloc(a->len + 1, sizeof(int));
    if (!res.digit)
    {
        return res;
    }
    zero.len = 1;
    zero.digit = calloc(2, sizeof(int));
    if (!zero.digit)
    {
        free(res.digit);
        return zero;
    }
    assert(compare(b, zero));
    if (!compare(*a, zero))
    {
        free(res.digit);
        return zero;
    }

    int cmp = compare(*a, b);
    if (cmp == 0)
    {
        free(zero.digit);
        res.digit[1] = 1;
        res.len = 1;
        return res;
    }
    if (cmp < 0)
    {
        free(res.digit);
        return zero;
    }

    long_int tmp_b;
    tmp_b.len = a->len;
    tmp_b.digit = calloc(a->len + 1, sizeof(int));
    if (!tmp_b.digit)
    {
        free(zero.digit);
        free(res.digit);
        return tmp_b;
    }
    memcpy(tmp_b.digit, b.digit, sizeof(int) * (b.len + 1));
    int q = 0;
    if (compare(*a, tmp_b) < 0)
    {
        tmp_b.len--;
    }
    int diff = tmp_b.len - b.len;
    for (int i = a->len - diff; i > 0; i--)
    {
        tmp_b.digit[i + diff] = tmp_b.digit[i];
        if (diff)
        {
            tmp_b.digit[i] = 0;
        }
    }

    while (tmp_b.len > 0 && compare(b, tmp_b) <= 0)
    {
        // Вычисление текущей цифры результата
        while (compare(*a, tmp_b) >= 0)
        {
            q++;
            subtraction(a, tmp_b);
        }
        // Удаление незначащих нулей
        for (size_t i = a->len; i > 0 && a->len > 1 && !a->digit[i]; i--, a->len--)
            ;
        res.len++;
        res.digit[res.len] = q;
        q = 0;
        for (size_t i = 1; i < tmp_b.len; i++)
        {
            tmp_b.digit[i] = tmp_b.digit[i + 1];
        }
        tmp_b.len--;        
    }

    for (size_t i = 1; i <= res.len / 2; i++)
    {
        int tmp = res.digit[res.len + 1 - i];
        res.digit[res.len + 1 - i] = res.digit[i];
        res.digit[i] = tmp;
    }
    // Удаление незначащих нулей
    int shift = 0;
    for (size_t i = res.len; i > 1 && !res.digit[i]; i--)
    {
        shift++;
    }
    res.len -= shift;
    free(zero.digit);
    free(tmp_b.digit);

    return res;
}

// Вывод длинного числа
void print_long_int(long_int num)
{
    assert(num.digit);

    if (num.digit[0] && !(num.digit[1] == 0 && num.len == 1))
    {
        printf("-");
    }
    for (size_t i = num.len; i > 0; i--)
    {
        printf("%d", num.digit[i]);
    }
    printf("\n");
}

// Проверка, является ли символ оператором
bool is_operator(char symbol)
{
    return strchr(OPERATORS, symbol) != NULL;
}

// Проверка, является ли символ числом
bool is_digit(char symbol)
{
    return symbol >= '0' && symbol <= '9';
}

// Получение приоритета оператора
int get_priority(char operator)
{
    switch (operator)
    {
        case OPEN_BRACKET: return 0;
        case CLOSE_BRACKET: return 1;
        case ADDITION_OP: return 2;
        case SUBTRACTION_OP: return 3;
        case MULTIPLICATION_OP: return 4;
        case DIVISION_OP: return 4;
        default: return 5;
    }
}

// Проверка, являтся ли символ под номером start унарным минусом
bool check_minus(char *expression, int start)
{
    assert(expression && start >= 0);

    if (expression[start] != '-')
    {
        return false;
    }
    // Проверка, является ли минус первым непробельным символом в строке
    bool is_first = true;
    for (int i = start; i > 0; i--)
    {
        if (expression[i] != ' ')
        {
            is_first = false;
        }
    }
    if (is_first)
    {
        return true;
    }

    // Проверка, стоит ли минус после открывающейся скобки
    int bracket_pos = start - 1;
    for (; bracket_pos > 0 && expression[bracket_pos] == ' '; bracket_pos--)
        ;
    if (expression[bracket_pos] == '(')
    {
        return true;
    }
    // Проверка, стоит ли минус после числа
    for (int i = start; i > 0; i--)
    {
        if (is_digit(expression[i]))
        {
            return false;
        }
    }
    return false;
}

// Преобразование выражения в инфиксной форме в постфиксную
char* infix_to_postfix(char *input, int len)
{
    assert(len > 0 && input);

    char *output = malloc(sizeof(char) * (len * 2 + 1));
    if (!output)
    {
        return NULL;
    }

    int out_len = 0;
    node_t *op_stack = NULL;

    for (int i = 0; i < len; i++)
    {
        if (is_digit(input[i]) || check_minus(input, i))
        {
            output[out_len++] = input[i];
            i++;
            while (i < len && is_digit(input[i]))
            {
                output[out_len++] = input[i];
                i++;
            }
            output[out_len++] = ' ';
            i--;
        }
        else if (is_operator(input[i]))
        {
            if (input[i] == OPEN_BRACKET)
            {
                push(&op_stack, (const void*) &input[i], sizeof(char));
            }
            else if (input[i] == CLOSE_BRACKET)
            {
                // Вытолкнуть из стека все операторы до открывающей скобки
                char s = *((char*) op_stack->data);
                free(op_stack->data);
                pop(&op_stack);
                while (s != OPEN_BRACKET)
                {
                    output[out_len++] = s;
                    output[out_len++] = ' ';
                    if (!op_stack)
                        return NULL;
                    s = *((char*) op_stack->data);
                    free(op_stack->data);
                    pop(&op_stack);
                }
            }
            else
            {
                if (op_stack)
                {
                    if (get_priority(input[i]) <= get_priority(*(char*) op_stack->data))
                    {
                       output[out_len++] = *(char*) op_stack->data;
                       output[out_len++] = ' ';
                       free(op_stack->data);
                       pop(&op_stack);
                    }
                }
                push(&op_stack, (const void*) &input[i], sizeof(char));
            }
        }
        else if (input[i] != ' ' && input[i] != '\n')
        {
            if (op_stack)
            {
                delete_stack(&op_stack);
            }
            free(output);
            return NULL;
        }
    }
    while (op_stack)
    {
        output[out_len++] = *(char*) op_stack->data;
        output[out_len++] = ' ';
        free(op_stack->data);
        pop(&op_stack);
    }
    if (op_stack)
    {
        delete_stack(&op_stack);
    }
    output[out_len] = '\0';

    return output;
}

// Считывание длинного числа из строки
long_int read_digit(char *input, int start)
{
    assert(input && start >= 0);

    bool sign = false;
    size_t k = 0;
    if (input[start] == '-')
    {
        start++;
        sign = true;
    }

    for (; is_digit(input[k + start]); k++)
        ;

    long_int result;
    result.digit = malloc(sizeof(int) * (k + 1));
    if (!result.digit)
    {
        return result;
    }
    result.len = k;
    if (sign)
    {
        result.digit[0] = 1;
    }
    else
    {
        result.digit[0] = 0;
    }

    for (int i = start; is_digit(input[i]); i++)
    {
        result.digit[k - (i - start)] = input[i] - '0';
    }

    return result;
}

// Произвести сумму двух длинных чисел
void do_sum(long_int *a, long_int *b, long_int *result)
{
    assert(a && b && result);
    assert(a->digit && b->digit);

    if (a->digit[0] == b->digit[0])
    {
        *result = sum(*a, *b);
        result->digit[0] = a->digit[0];
    }
    else
    {        
        if (compare(*a, *b) > 0)
        {
            subtraction(a, *b);
            *result = *a;
            result->digit[0] = a->digit[0];
            a->digit = NULL;
        }
        else
        {
            subtraction(b, *a);
            *result = *b;
            result->digit[0] = b->digit[0];
            b->digit = NULL;
        }
    }
}

// Произвести вычитание двух длинных чисел
void do_subtraction(long_int *a, long_int *b, long_int *result)
{
    assert(a && b && result);
    assert(a->digit && b->digit);

    if (a->digit[0] != b->digit[0])
    {
        *result = sum(*a, *b);
        result->digit[0] = a->digit[0];
    }
    else if (compare(*a, *b) > 0)
    {
        subtraction(a, *b);
        *result = *a;
        result->digit[0] = a->digit[0];
        a->digit = NULL;
    }
    else
    {
        subtraction(b, *a);
        *result = *b;
        result->digit[0] = !b->digit[0];
        b->digit = NULL;
    }

}

// Произвести произведение двух длинных чисел
void do_multiplication(long_int a, long_int b, long_int *result)
{
    assert(result);
    assert(a.digit && b.digit);

    *result = multiplication(a, b);
    // Определение знака результата
    result->digit[0] = a.digit[0] == b.digit[0]? 0 : 1;
}

// Произвести деление двух длинных чисел
void do_division(long_int a, long_int b, long_int *result)
{
    assert(result);
    assert(a.digit && b.digit);

    *result = division(&a, b);
    // Определение знака результата
    result->digit[0] = a.digit[0] == b.digit[0]? 0 : 1;
}

// Вычисление выражения input, записанного в постфиксной форме
long_int counting(char *input)
{
    assert(input);

    long_int a, b, result;
    node_t *stack = NULL;

    for (int i = 0; input[i] != 0; i++)
    {
        if (is_digit(input[i]) || (input[i + 1] != 0 && input[i] == '-' && input[i + 1] != ' '))
        {
            a = read_digit(input, i);
            push(&stack, &a, sizeof(long_int));
            i += a.len;
        }
        else if (is_operator(input[i]))
        {
            // Выталкиваем из стека 2 числа
            if (!stack)
            {
                result.digit = NULL;
                return result;
            }
            b = *(long_int*) stack->data;
            free(stack->data);
            pop(&stack);
            if (!stack)
            {
                result.digit = NULL;
                return result;
            }
            a = *(long_int*) stack->data;
            free(stack->data);
            pop(&stack);            

            switch (input[i])
            {
                case ADDITION_OP:
                    do_sum(&a, &b, &result);
                    break;
                case SUBTRACTION_OP:
                    do_subtraction(&a, &b, &result);
                    break;
                case MULTIPLICATION_OP :
                    do_multiplication(a, b, &result);
                    break;
                case DIVISION_OP :
                    if (b.len == 1 && b.digit[1] == 0)
                    {
                        result.digit = NULL;
                        return result;
                    }
                    do_division(a, b, &result);
            }
            if (a.digit)
            {
                free(a.digit);
            }
            if (b.digit)
            {
                free(b.digit);
            }

            push(&stack, &result, sizeof(long_int));
        }
    }
    if (!stack)
    {
        result.digit = NULL;
        return result;
    }
    result = *(long_int*) stack->data;
    delete_stack(&stack);

    return result;
}

// Считать выражение из стандартного потока ввода
char *get_input(void)
{
    const size_t buffer_size = 30;

    char *result, *buffer, *tmp;
    size_t size = buffer_size;
    size_t n = 0;
    result = buffer = malloc(buffer_size);
    if (!result)
    {
        return NULL;
    }

    result[0] = '\0';

    while (true)
    {
        if (!fgets(buffer, buffer_size, stdin))
        {
            break;
        }

        n += strlen(buffer);
        if (result[n - 1] == '\n')
        {
            break;
        }

        size += buffer_size;
        tmp = realloc(result, size);
        if (!tmp)
        {
            break;
        }

        result = tmp;
        buffer = result + n;
    }

    if (!result[0])
    {
        free(result);
        result = NULL;
    }


    return result;
}

int test_addition(void)
{
    char expression[] = "999999999999999999999999999999999999999999999999999999"
                        "999999999999999999999999999999999999999999999999999999"
                        "999999999999999999999999 10000000000000000000000000000"
                        "000000000000000000000000000000000000000000000000 +";
    long_int res = counting(expression);
    if (!res.digit || res.digit[0] != 0)
    {
        free(res.digit);
        return TEST_FAILED;
    }
    for (size_t i = 1; i <= 76; i++)
    {
        if (res.digit[i] != 9)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }
    for (size_t i = 78; i <= 131; i++)
    {
        if (res.digit[i] != 0)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }
    if (res.digit[133] != 1)
    {
        free(res.digit);
        return TEST_FAILED;
    }

    free(res.digit);

    return 0;
}

int test_subtraction(void)
{
    char expression[] = "222222222222222222222222222222222222222222222222222222"
                        "30000000000000000000000000000000000000 444444444444444"
                        "444444444444444444444444444444444444444444444444444444"
                        "44444444444444444444444444444444444444444444444 -";
    long_int res = counting(expression);
    if (!res.digit || res.digit[0] != 1)
    {
        free(res.digit);
        return TEST_FAILED;
    }
    for (size_t i = 1; i <= 37; i++)
    {
        if (res.digit[i] != 4)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }
    if (res.digit[38] != 1)
    {
        free(res.digit);
        return TEST_FAILED;
    }
    for (size_t i = 39; i <= 92; i++)
    {
        if (res.digit[i] != 2)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }
    for (size_t i = 93; i <= 116; i++)
    {
        if (res.digit[i] != 4)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }

    free(res.digit);

    return 0;
}

int test_multiplication(void)
{
    char expression[] = "222222222222222222222222222222222222222222222222222222"
                        "22222222222222222222222222222222222222 900000000000000"
                        "0000000000000000000000000000000000000000000000000000000 *";
    long_int res = counting(expression);
    if (!res.digit || res.digit[0] != 0)
    {
        free(res.digit);
        return TEST_FAILED;
    }
    for (size_t i = 1; i <= 69; i++)
    {
        if (res.digit[i] != 0)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }
    if (res.digit[70] != 8)
    {
        free(res.digit);
        return TEST_FAILED;
    }
    for (size_t i = 73; i <= 161; i++)
    {
        if (res.digit[i] != 9)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }
    if (res.digit[162] != 1)
    {
        free(res.digit);
        return TEST_FAILED;
    }

    free(res.digit);

    return 0;
}

int test_division(void)
{
    char expression[] = "888888888888888888888888888888888888888888888888888888"
                        "88888888888888888888888888888888888888 200000000000000"
                        "0000000000000000000000000000000000000000000000000000000 /";
    long_int res = counting(expression);
    if (!res.digit || res.digit[0] != 0)
    {
        free(res.digit);
        return TEST_FAILED;
    }
    for (size_t i = 1; i <= 23; i++)
    {
        if (res.digit[i] != 4)
        {
            free(res.digit);
            return TEST_FAILED;
        }
    }

    free(res.digit);

    return 0;
}

int main(void)
{
    if (test_addition())
    {
        printf("test_addition failed\n");
    }
    if (test_subtraction())
    {
        printf("test_subtraction failed\n");
    }
    if (test_multiplication())
    {
        printf("test_multiplication failed\n");
    }
    if (test_division())
    {
        printf("test_division failed\n");
    }

    char *inp = get_input();
    if (!inp)
    {
        printf("[error]\n");
        free(inp);
        return 0;
    }

    char *exp = infix_to_postfix(inp, strlen(inp));
    if (!exp)
    {
        printf("[error]\n");
        free(inp);
        return 0;
    }

    long_int res = counting(exp);
    if (!res.digit)
    {
        free(inp);
        free(exp);
        printf("[error]\n");
        return 0;
    }
    print_long_int(res);

    free(res.digit);
    free(inp);
    free(exp);

    return 0;
}
