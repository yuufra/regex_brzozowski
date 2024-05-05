#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){    
    
    char *char1 = argv[1];
    char *char2 = argv[2];

    if(strcmp(char1,char2) == 0){
        return 0;
    } else {
        return 1;
    };
    
}