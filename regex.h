#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// トークンの定義
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


// ノードの定義
typedef enum {
    ND_ALPHA,
    ND_DOT,
    ND_STAR,
    ND_OR,
    DERIV_EMPTYSET,
    DERIV_EPSILON
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    // int val;
    char* str;
    Node* lhs; // 木構造を作る
    Node* rhs;
};

// 変数・関数宣言
extern Token* token;

Token* tokenize(char* p);
Node* parse_expr();
Node* new_node(NodeKind kind, Node* lhs, Node* rhs);

Node* brzozowski_derivative(Node* node, char c);
bool nullable(Node* node);

void print_tree(Node* node, int depth);