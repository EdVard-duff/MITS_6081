// A simple program to observe ASLR (Address Space Layout Randomization) in action.

#include <stdio.h>

int main(int argc, char *argv[]) {
    int stack = 0;
    printf("%p\n", &stack);
    
    return 0;
}