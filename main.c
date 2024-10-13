#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum
{
    INT_VAL,
    STRING_VAL,
    NULL_VAL
} ValueType;

typedef enum
{
    TERM,
    PLUS,
    MINUS,
    END
} TokenType;

typedef union
{
    int int_val;
    char *string_val;
    void *null_val;
} Value;

typedef struct
{
    TokenType type;
    ValueType val_t;
    Value val;
} Token;

Token *CreateToken(TokenType type, ValueType val_t, Value val)
{
    Token *t = (Token *)malloc(sizeof(Token));
    t->type = type;
    t->val_t = val_t;
    t->val = val;
    return t;
}

char *ReprToken(Token t)
{
    char *result = malloc(120);
    char *token_t = malloc(20);

    switch (t.type)
    {
    case TERM:
        strncpy(token_t, "TERM", 5);
        break;
    case PLUS:
        strncpy(token_t, "PLUS", 5);
        break;
    case MINUS:
        strncpy(token_t, "MINUS", 6);
        break;
    case END:
        strncpy(token_t, "END", 4);
        break;
    }

    if (t.type == TERM)
        sprintf(result, "Token(type=%s, val=%d)", token_t, t.val.int_val);
    else if (t.type == PLUS || t.type == MINUS)
        sprintf(result, "Token(type=%s, val=%s)", token_t, t.val.string_val);
    else if (t.type == END)
    {
        sprintf(result, "Token(type=%s, val=%s)", token_t, "NULL");
    }
    else
        return NULL;
    free(token_t);
    return result;
}

typedef struct
{
    int pos;
    char *text;
    Token *current_token;
} Interpreter;

Interpreter CreateInterpreter(char *text)
{
    Interpreter i;
    i.pos = 0;
    i.text = (char *)malloc(sizeof(char) * (strlen(text) + 1));
    strncpy(i.text, text, strlen(text) + 1);
    i.current_token = NULL;
    return i;
}

Token *getNextToken(Interpreter *i)
{
    Token t;
    if (i->pos > strlen(i->text) - 1 || i->text[i->pos] == '\n')
    {
        Value val;
        val.null_val = NULL;
        return CreateToken(END, NULL_VAL, val);
    }
    else if (i->text[i->pos] == '+')
    {
        i->pos++;
        Value val;
        val.string_val = "+";
        return CreateToken(PLUS, STRING_VAL, val);
    }
    else if (i->text[i->pos] == '-')
    {
        i->pos++;
        Value val;
        val.string_val = "-";
        return CreateToken(MINUS, STRING_VAL, val);
    }

    else if (isdigit(i->text[i->pos]))
    {
        int startpos = i->pos;
        int count = 0;
        while (isdigit(i->text[i->pos]))
        {
            i->pos++;
            count++;
        }
        char *intstr = (char *)malloc(sizeof(char) * (count + 1));
        memcpy(intstr, i->text + startpos, count);
        intstr[count] = '\0';

        Value val;
        val.int_val = atoi(intstr);
        return CreateToken(TERM, INT_VAL, val);
    }
    else if (i->text[i->pos] == ' ')
    {
        i->pos++;
        return getNextToken(i);
    }
    else
    {
        printf("char: %c\n", i->text[i->pos]);
        perror("Unvalid type error\n");
        exit(EXIT_FAILURE);
    }
}

bool eat(Interpreter *i, TokenType type)
{
    if (i->current_token->type == type)
    {
        i->current_token = getNextToken(i);
        return true;
    }
    else
    {
        return false;
    }
}

int term(Interpreter *i)
{
    Token *result = i->current_token;
    if (eat(i, TERM) == false)
    {
        perror("Wrong TOKEN\n");
        exit(EXIT_FAILURE);
    }
    return result->val.int_val;
}

int expr(Interpreter *i)
{
    i->current_token = getNextToken(i);
    int result = term(i);

    while (i->current_token->type == PLUS || i->current_token->type == MINUS)
    {
        Token token = *i->current_token;
        if (token.type == PLUS)
        {
            if (eat(i, PLUS) == false)
            {
                perror("Wrong TOKEN\n");
                exit(EXIT_FAILURE);
            }
            result += term(i);
        }
        else if (token.type == MINUS)
        {
            if (eat(i, MINUS) == false)
            {
                perror("Wrong TOKEN\n");
                exit(EXIT_FAILURE);
            }
            result -= term(i);
        }
    }
    return result;
}

int main()
{
    char text[100];
    Interpreter i;
    int result;

    printf("calc> ");
    fgets(text, sizeof(text), stdin);
    do
    {
        i = CreateInterpreter(text);
        result = expr(&i);
        printf("%d\n", result);
        printf("calc> ");
        fgets(text, sizeof(text), stdin);

    } while (strcmp(text, "quit") != 0);
    return 0;
}
