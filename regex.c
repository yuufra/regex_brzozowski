// *** EBNFによる文法 ***
// expr = star ("." star)*
// star = prim "*"?
// prim = string

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

// 関数の宣言
Node* parse_expr();
Node* parse_star();
Node* parse_primary();

int consume(char);
int consume_dot();
void expect(char);
char* expect_str();

Node* nullable_wrapper(Node* node);
bool nullable(Node* node);


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

    // print_list(head->next);
    cur = new_token(TK_EOF, cur, p);
    return head->next;
}

int at_eof(){
    return token->kind == TK_EOF;
}

// 構文木とノードの関数
void print_tree(Node* node, int depth){
    fprintf(stderr, "- type:%d", node->kind);
    if (node->kind == ND_ALPHA){
        fprintf(stderr, ", str:%s\n", node->str);
    } else if (node->kind == ND_STAR){
        fprintf(stderr, "\n");
        fprintf(stderr, "%*s", 2*depth, " ");
        print_tree(node->lhs, depth+1);
    } else if (node->kind == ND_DOT || node->kind == ND_OR) {
        fprintf(stderr, "\n");
        fprintf(stderr, "%*s", 2*depth, " ");
        print_tree(node->lhs, depth+1);
        fprintf(stderr, "%*s", 2*depth, " ");
        print_tree(node->rhs, depth+1);
    } 
}

Node* new_node(NodeKind kind, Node* lhs, Node* rhs){
    // fprintf(stderr, "type %d registered\n", kind);
    Node* node = (Node*)calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    if (kind == ND_DOT){
        if(lhs->kind == DERIV_EPSILON)
            node = rhs;
        else if (lhs->kind == DERIV_EMPTYSET)
            node = new_node(DERIV_EMPTYSET, NULL, NULL);
    } else if (kind == ND_OR) {
        if(lhs->kind == DERIV_EPSILON)
            node = new_node(DERIV_EPSILON, NULL, NULL);
        else if (lhs->kind == DERIV_EMPTYSET)
            node = rhs;
        else if(rhs->kind == DERIV_EPSILON)
            node = new_node(DERIV_EPSILON, NULL, NULL);
        else if (rhs->kind == DERIV_EMPTYSET)
            node = lhs;
    }
    return node;
}

Node* new_node_alpha(char* str){
    // fprintf(stderr, "number registered(value:%d)\n", val);
    Node* node = (Node*)calloc(1, sizeof(Node));
    node->kind = ND_ALPHA;
    node->str = str;
    return node;
}

Node* parse_expr(){
    // fprintf(stderr, "parse_expr called\n");
    Node* node = parse_star();

    for(;;){
        if(consume_dot()){
            node = new_node(ND_DOT, node, parse_star());
            // print_tree(node, 0);
            continue;
        }
        return node;
    }
}

Node* parse_star(){
    // fprintf(stderr, "parse_mul called\n");
    Node* node = parse_primary();

    for(;;){
        if(consume('*')){
            node = new_node(ND_STAR, node, NULL);
            // print_tree(node, 0);
            continue;
        }
        return node;
    }
}

Node* parse_primary(){
    // fprintf(stderr, "parse_primary called\n");
    Node* node;
    if(consume('(')){
        node = parse_expr();
        expect(')');
    } else {
        char* str = expect_str();
        node = new_node_alpha(str);
        // print_tree(node, 0);
    }
    return node;
}

int consume(char c){
    if (token->kind != TK_RESERVED || token->str[0] != c){
        return false;
    }
    token = token->next;
    return true;
}

int consume_dot(){
    if (token->kind != TK_DOT){
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

char* expect_str(){
    if (token->kind != TK_ALPHA){
        error("got unexpexted value\n");
    }
    char* c = token->str;
    token = token->next;
    return c;
}

Node* brzozowski_derivative(Node* node, char c){
    if (node->kind == DERIV_EMPTYSET){
        return new_node(DERIV_EMPTYSET, NULL, NULL);
    } else if (node->kind == DERIV_EPSILON){
        return new_node(DERIV_EMPTYSET, NULL, NULL);
    }

    // print_tree(node, 0);
    // printf("%c\n", c);

    Node** array = malloc(4 * sizeof(Node*));

    if (node->kind == ND_DOT){
        // printf("dot\n");
        array[0] = brzozowski_derivative(node->lhs,c);
        array[1] = brzozowski_derivative(node->rhs,c);
        array[2] = new_node(ND_DOT,array[0],node->rhs);
        array[3] = new_node(ND_DOT,nullable_wrapper(node->lhs),array[1]);
        return new_node(ND_OR,array[2],array[3]);
    } else if (node->kind == ND_STAR){
        // printf("star\n");
        array[0] = brzozowski_derivative(node->lhs,c);
        return new_node(ND_DOT, array[0], node);
    } else if (node->kind == ND_ALPHA){
        // printf("alpha\n");
        if (node->str[0] == c){
            return new_node(DERIV_EPSILON, NULL, NULL);
        } else {
            return new_node(DERIV_EMPTYSET, NULL, NULL);
        }
    } else if (node->kind == ND_OR){
        printf("or\n");
        array[0] = brzozowski_derivative(node->lhs,c);
        array[1] = brzozowski_derivative(node->rhs,c);
        return new_node(ND_OR,array[0],array[1]);
    }
}

Node* nullable_wrapper(Node* node){
    if (nullable(node)){
        return new_node(DERIV_EPSILON, NULL, NULL);
    } else {
        return new_node(DERIV_EMPTYSET, NULL, NULL);
    }
}

bool nullable(Node* node){  // 空文字列の照合関数
    if (node->kind == DERIV_EPSILON){
        return true;
    } else if (node->kind == DERIV_EMPTYSET){
        return false;
    } else if (node->kind == ND_DOT){
        return nullable(node->lhs) && nullable(node->rhs);
    } else if (node->kind == ND_STAR){
        return true;
    } else if (node->kind == ND_ALPHA){
        return false;
    } 
}

int main(int argc, char** argv){    
    
    if (argc != 3){
        error("引数の個数が正しくありません");
        return 1;
    }

    token = tokenize(argv[1]);
    Node* node = parse_expr();
    // print_tree(node, 0);

    // printf("\n\n");
    char* input = argv[2];
    for (int i = 0; input[i] != '\0'; i++) {
        // printf("***i:%d***\n",i);
        node = brzozowski_derivative(node, input[i]);
        // print_tree(node, 0);
    }

    if(nullable(node)){
        return 0;
    } else {
        return 1;
    };
    
}