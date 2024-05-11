// *** EBNFによる文法 ***
// expr = or ("|" or)*
// or = star ("." star)*
// star = prim ("*" | "+" | "?")?
// prim = string | "(" expr ")"

#include "regex.h"

// 関数の宣言
// Node* parse_expr();
Node* parse_or();
Node* parse_star();
Node* parse_primary();

int consume(char);
int consume_dot();
void expect(char);
char* expect_str();


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
        } else if (*p == '*' || *p == ')' || *p == '+' || *p == '?'){
            cur = new_token(TK_RESERVED, cur, p);
            // fprintf(stderr, "p: %s\n", p);
            if (*(p+1) != 0 && *(p+1) != '*' && *(p+1) != '|' && *(p+1) != ')' && *(p+1) != '+' && *(p+1) != '?'){
                cur = new_token(TK_DOT, cur, p);
            }
            p++;
            continue;
        } else if (*p == '|' || *p == '('){
            cur = new_token(TK_RESERVED, cur, p);
            p++;
            continue;
        } else if (isalpha(*p)){
            cur = new_token(TK_ALPHA, cur, p);
            // fprintf(stderr, "p: %s\n", p);
            if (*(p+1) != 0 && *(p+1) != '*' && *(p+1) != '|' && *(p+1) != ')' && *(p+1) != '+' && *(p+1) != '?'){
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

// int at_eof(){
//     return token->kind == TK_EOF;
// }

// 構文木とノードの関数
void print_tree(Node* node, int depth){
    char type[6][10] = {"ALPHA","DOT","STAR","OR","EMPTYSET","EPSILON"};
    fprintf(stderr, "- type:%s", type[node->kind]);
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
    } else if (node->kind == DERIV_EMPTYSET || node->kind == DERIV_EPSILON) {
        fprintf(stderr, "\n");
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
        if (lhs->kind == DERIV_EMPTYSET)
            node = rhs;
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
    Node* node = parse_or();

    for(;;){
        if(consume('|')){
            node = new_node(ND_OR, node, parse_or());
            // print_tree(node, 0);
            continue;
        }
        return node;
    }
}

Node* parse_or(){
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
        } else if(consume('+')){
            // a+はaa*に置き換えられる
            Node *temp = new_node(ND_STAR, node, NULL);
            node = new_node(ND_DOT, node, temp);
            // print_tree(node, 0);
            continue;
        } else if(consume('?')){
            // a?は(a|ε)に置き換えられる
            Node *temp = new_node(DERIV_EPSILON, NULL, NULL);
            node = new_node(ND_OR, node, temp);
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