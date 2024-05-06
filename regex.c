#include "regex.h"

Token* token;

int main(int argc, char** argv){    
    
    if (argc != 3){
        fprintf(stderr, "usage: ./compiler code\n");
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