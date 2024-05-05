// *** EBNFによる文法 ***
// expr = star ("." star)*
// star = prim "*"?
// prim = string

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef enum {
    TK_RESERVED, // 予約された記号
    TK_ALPHA,
    TK_DOT,  // 省略されている連結記号
    TK_EOF
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    // int val;
    char* str;
    Token* next; // 連結リストを作る
};

Token* token;

void error(char* msg){
    fprintf(stderr, "%s", msg);
    exit(1);
}

void print_list(Token* token){
    while (token != NULL) {
        fprintf(stderr, "- type:%d", token->kind);
        if (token->kind == TK_RESERVED || token->kind == TK_ALPHA){
            fprintf(stderr, ", str:%s", token->str);
        }
        fprintf(stderr, "\n");
        token = token->next;
    }
    // fprintf(stderr,"\n\n");
}

Token* new_token(TokenKind kind, Token* cur, char* p){
    // fprintf(stderr, "type %d registered\n", kind);
    cur->next = (Token*)calloc(1, sizeof(Token));
    cur = cur->next;

    cur->kind = kind;
    cur->str = (char*)calloc(1, sizeof(char));
    strncpy(cur->str, p, 1);
    return cur;
}

Token* tokenize(char* p){
    Token* head = (Token*)calloc(1, sizeof(Token));
    Token* cur = head;

    int num;

    while(*p){
        // print_list(head->next);
        if (isspace(*p)){
            p++;
            continue;
        } else if (*p == '*'){
            cur = new_token(TK_RESERVED, cur, p);
            // fprintf(stderr, "p: %s\n", p);
            if (*(p+1) != 0 && *(p+1) != '*'){
                cur = new_token(TK_DOT, cur, p);
            }
            p++;
            continue;
        } else if (isalpha(*p)){
            cur = new_token(TK_ALPHA, cur, p);
            // fprintf(stderr, "p: %s\n", p);
            if (*(p+1) != 0 && *(p+1) != '*'){
                cur = new_token(TK_DOT, cur, p);
            }
            p++;
            continue;
        }

        error("invalid input\n");
    }

    print_list(head->next);
    cur = new_token(TK_EOF, cur, p);
    return head->next;
}

int at_eof(){
    return token->kind == TK_EOF;
}

int consume(char c){
    if (token->kind != TK_RESERVED || token->str[0] != c){
        return false;
    }
    token = token->next;
    return true;
}

void expect(char c){
    if (token->kind != TK_RESERVED || token->str[0] != c){
        error("got unexpexted value\n");
    }
    token = token->next;
}


int main(int argc, char** argv){    
    
    if (argc != 3){
        error("引数の個数が正しくありません");
        return 1;
    }

    token = tokenize(argv[1]);
    

    char *char1 = argv[1];
    char *char2 = argv[2];

    if(strcmp(char1,char2) == 0){
        return 0;
    } else {
        return 1;
    };
    
}