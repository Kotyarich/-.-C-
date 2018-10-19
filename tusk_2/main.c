//Требуется написать программу, которая способна вычислять арифметические выражения.
//Выражения могут содержать:
//1) знаки операций '+', '-', '/', '*'
//2) Скобки '(', ')'
//3) Целые числа, в нотации '123', в том числе они достаточно большие (требуется реализовать длинную арифметику), все операции должны быть целочисленные
//4) необходимо учитывать приоритеты операций, и возможность унарного минуса, пробелы ничего не значат
//5) Если в выражении встретилась ошибка требуется вывести в стандартный поток вывода "[error]" (без кавычек)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#define ALLOCATION_ERR -1

typedef struct
{
   int *digit;
   size_t len;
} long_int;

typedef struct node_t
{
    void *data;
    struct node_t *next;
} node_t;

int pop(node_t **node)
{
    if (!node)
        return -1;

    node_t *prev = NULL;
    prev = *node;
    (*node) = (*node)->next;

    free(prev);
    return 0;
}

int push(node_t **node, const void *value, size_t size)
{
    assert(value != NULL);
    node_t *new_node = (node_t*) malloc(sizeof(node_t));
    if(!new_node)
        return ALLOCATION_ERR;

    new_node->data = malloc(size);
    if (!new_node->data)
        return ALLOCATION_ERR;
    memcpy(new_node->data, value, size);

    new_node->next = *node;
    (*node) = new_node;

    return 0;
}

void delete_stack(node_t **node)
{
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

long_int sum(const long_int a, const long_int b)
{
    long_int res;
    res.len = max(a.len, b.len);
    res.digit = malloc(sizeof(int) * (res.len + 2));
    res.digit[res.len + 1] = 0;
    size_t min_len = min(a.len, b.len);

    for (int i = 1; i <= min_len; i++)
        res.digit[i] = a.digit[i] + b.digit[i];
    for (int i = min_len + 1; i <= a.len; i++)
        res.digit[i] = a.digit[i];
    for (int i = min_len + 1; i <= b.len; i++)
        res.digit[i] = b.digit[i];

    for (int i = 1; i <= res.len; i++)
        if (res.digit[i] > 9)
        {
            res.digit[i] -= 10;
            res.digit[i + 1]++;
        }
    if (res.digit[res.len + 1] != 0)
        res.len++;

    return res;
}

void subtraction(long_int *a, const long_int b)
{
    for (int i = 1; i <= b.len; i++)
    {
        a->digit[i] -= b.digit[i];
        if (a->digit[i] < 0)
        {
            a->digit[i] += 10;
            a->digit[i + 1]--;
        }
    }
    if (a->digit[a->len] == 0 && a->len > 1)
        a->len--;
}

long_int multiplication(const long_int a, const long_int b)
{
    long_int res;
    res.len = a.len + b.len;
    res.digit = calloc(a.len + b.len + 1, sizeof(int));

    for (int i = 1; i <= b.len; i++)
        for (int j = 1; j <= a.len; j++)
            res.digit[i + j - 1] += a.digit[j] * b.digit[i];

    for (int i = 1; i < res.len; i++)
    {
        res.digit[i + 1] += res.digit[i] / 10;
        res.digit[i] %= 10;
    }

    while (!res.digit[res.len])
        res.len--;

    return res;
}

int compare(const long_int a, const long_int b)
{
    if (a.len > b.len)
        return 1;
    if (b.len > a.len)
        return -1;

    for (int i = a.len; i > 0; i--)
    {
        if (a.digit[i] > b.digit[i])
            return 1;
        if (b.digit[i] > a.digit[i])
            return -1;
    }

    return 0;
}

long_int division(long_int *a, const long_int b)
{
    long_int res, zero;
    res.len = 0;
    res.digit = calloc(a->len + 1, sizeof(int));
    zero.len = 1;
    zero.digit = calloc(2, sizeof(int));
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
    memcpy(tmp_b.digit, b.digit, sizeof(int) * (b.len + 1));
    int q = 0;
    if (compare(*a, tmp_b) < 0)
        tmp_b.len--;
    int diff = tmp_b.len - b.len;
    for (int i = a->len - diff; i > 0; i--)
    {
        tmp_b.digit[i + diff] = tmp_b.digit[i];
        if (diff)
            tmp_b.digit[i] = 0;
    }

    do
    {
        while (compare(*a, tmp_b) >= 0)
        {
            q++;
            subtraction(a, tmp_b);
        }
        for (int i = a->len; i > 0 && a->len > 1 && !a->digit[i]; i--, a->len--)
            ;
        res.len++;
        res.digit[res.len] = q;
        q = 0;
        for (int i = 1; i < tmp_b.len; i++)
            tmp_b.digit[i] = tmp_b.digit[i + 1];
        tmp_b.len--;        
    } while (tmp_b.len > 0 && compare(b, tmp_b) <= 0);

    int tmp;
    for (int i = 1; i <= res.len / 2; i++)
    {
        tmp = res.digit[res.len + 1 - i];
        res.digit[res.len + 1 - i] = res.digit[i];
        res.digit[i] = tmp;
    }
    int shift = 0;
    for (int i = res.len; i > 1 && !res.digit[i]; i--)
        shift++;
    res.len -= shift;
    free(zero.digit);
    free(tmp_b.digit);

    return res;
}

void print_long_int(long_int num)
{
    if (num.digit[0] && !(num.digit[1] == 0 && num.len == 1))
        printf("-");
    for (int i = num.len; i > 0; i--)
        printf("%d", num.digit[i]);
    printf("\n");
}

bool is_operator(char symbol)
{
    return strchr("()+-*/", symbol) != NULL;
}

bool is_digit(char symbol)
{
    return symbol >= '0' && symbol <= '9';
}

int get_priority(char operator)
{
    switch (operator)
    {
        case '(': return 0;
        case ')': return 1;
        case '+': return 2;
        case '-': return 3;
        case '*': return 4;
        case '/': return 4;
        default: return 5;
    }
}

bool check_minus(char *expression, int start)
{
    if (expression[start] != '-')
        return false;

    bool is_first = true;
    for (int i = start; i > 0; i--)
        if (expression[i] != ' ')
            is_first = false;
    if (is_first)
        return true;

    bool is_last = true;
    for (int i = start + 1; expression[i] != 0; i++)
        if (expression[i] != ' ')
            is_last = false;
    if (!is_last)
        return false;

    for (int i = start; i > 0; i--)
        if (is_digit(expression[i]))
            return false;

    return false;
}

char* infix_to_postfix(char *input, int len)
{
    assert(len > 0 && input);
    char *output = malloc(sizeof(char) * (len * 2 + 1));
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
            if (input[i] == '(')
                push(&op_stack, (const void*) &input[i], sizeof(char));
            else if (input[i] == ')')
            {
                char s = *((char*) op_stack->data);
                free(op_stack->data);
                pop(&op_stack);
                while (s != '(')
                {
                    output[out_len++] = s;
                    output[out_len++] = ' ';
                    s = *((char*) op_stack->data);
                    free(op_stack->data);
                    pop(&op_stack);
                }
            }
            else
            {
                if (op_stack)
                    if (get_priority(input[i]) <= get_priority(*(char*) op_stack->data))
                    {
                       output[out_len++] = *(char*) op_stack->data;
                       output[out_len++] = ' ';
                       free(op_stack->data);
                       pop(&op_stack);
                    }
                push(&op_stack, (const void*) &input[i], sizeof(char));
            }
        }
        else if (input[i] != ' ' && input[i] != '\n')
        {
            if (op_stack)
                delete_stack(&op_stack);
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
        delete_stack(&op_stack);
    output[out_len++] = '\0';

    return output;
}

long_int read_digit(char *input, int start)
{
    bool sign = false;
    int k = 0;
    if (input[start] == '-')
    {
        start++;
        sign = true;
    }

    for (; is_digit(input[k + start]); k++)
        ;

    long_int result;
    result.digit = malloc(sizeof(int) * (k + 1));
    result.len = k;
    if (sign)
        result.digit[0] = 1;
    else
        result.digit[0] = 0;

    for (int i = start; is_digit(input[i]); i++)
        result.digit[k - (i - start)] = input[i] - '0';

    return result;
}

void do_sum(long_int *a, long_int *b, long_int *result)
{
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

void do_subtraction(long_int *a, long_int *b, long_int *result)
{    
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

void do_multiplication(long_int a, long_int b, long_int *result)
{
    *result = multiplication(a, b);
    if (a.digit[0] == b.digit[0])
        result->digit[0] = 0;
    else
        result->digit[0] = 1;
}

void do_division(long_int a, long_int b, long_int *result)
{
    *result = division(&a, b);
    if (a.digit[0] == b.digit[0])
        result->digit[0] = 0;
    else
        result->digit[0] = 1;    
}

long_int counting(char *input)
{
    long_int a, b, result;
    a.digit = NULL;
    b.digit = NULL;
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
            if (!stack)
            {
                result.digit = NULL;
                free(input);
                return result;
            }
            b = *(long_int*) stack->data;
            free(stack->data);
            pop(&stack);
            if (!stack)
            {
                result.digit = NULL;
                free(input);
                return result;
            }
            a = *(long_int*) stack->data;
            free(stack->data);
            pop(&stack);            

            switch (input[i])
            {
                case '+': do_sum(&a, &b, &result);
                          break;
                case '-': do_subtraction(&a, &b, &result);
                          break;
                case '*' : do_multiplication(a, b, &result);
                           break;
                case '/' : if (b.len == 1 && b.digit[0] == 0)
                           {
                               result.digit = NULL;
                               free(input);
                               return result;
                           }
                           do_division(a, b, &result);
            }
            if (a.digit)
                free(a.digit);
            if (b.digit)
                free(b.digit);

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
    free(input);

    return result;
}

char *get_input(void)
{
    const size_t buffer_size = 30;

    char *result, *buffer, *tmp;
    size_t size = buffer_size;
    size_t n = 0;
    result = buffer = malloc(buffer_size);
    if (!result)
        return NULL;

    result[0] = '\0';

    while (true)
    {
        if (!fgets(buffer, buffer_size, stdin))
            break;

        n += strlen(buffer);
        if (result[n - 1] == '\n')
            break;

        size += buffer_size;
        tmp = realloc(result, size);
        if (!tmp)
            break;

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

int main(void)
{    
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
        printf("[error]\n");
        return 0;
    }
    print_long_int(res);

    free(res.digit);
    free(inp);

    return 0;
}
