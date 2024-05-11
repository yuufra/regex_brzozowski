#include "regex.h"

// 関数の宣言
Node* nullable_wrapper(Node* node);

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
        Node* ret = new_node(ND_OR,array[2],array[3]);
        // printf("dot_result\n");
        // print_tree(ret,0);
        return ret;
    } else if (node->kind == ND_STAR){
        // printf("star\n");
        array[0] = brzozowski_derivative(node->lhs,c);
        Node* ret = new_node(ND_DOT, array[0], node);
        // printf("star_result\n");
        // print_tree(ret,0);
        return ret;
    } else if (node->kind == ND_ALPHA){
        // printf("alpha\n");
        if (node->str[0] == '.'){  // 任意の一文字を受け付ける
            return new_node(DERIV_EPSILON, NULL, NULL);
        } 
        if (node->str[0] == c){
            return new_node(DERIV_EPSILON, NULL, NULL);
        } else {
            return new_node(DERIV_EMPTYSET, NULL, NULL);
        }
    } else if (node->kind == ND_OR){
        // printf("or\n");
        array[0] = brzozowski_derivative(node->lhs,c);
        array[1] = brzozowski_derivative(node->rhs,c);
        Node* ret = new_node(ND_OR,array[0],array[1]);
        // printf("or_result\n");
        // print_tree(ret,0);
        return ret;
    }
}

Node* nullable_wrapper(Node* node){
    // printf("nennotame\n");
    // print_tree(node,0);
    if (nullable(node)){
        // printf("nullable:true\n");
        return new_node(DERIV_EPSILON, NULL, NULL);
    } else {
        // printf("nullable:false\n");
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
    } else if (node->kind == ND_OR){
        return nullable(node->lhs) || nullable(node->rhs);
    } 
}