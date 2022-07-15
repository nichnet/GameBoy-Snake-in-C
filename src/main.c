#include <stdio.h>
#include <stdlib.h>

struct Position {
    int x;
    int y;
};

int tailLength = 10;

struct Position *tail;

void increaseSize(int);
void decreaseSize(int);
void printArr();


int main() {
    //default
    tail = (struct Position*) malloc(tailLength * sizeof(struct Position));
    
    int i;
    for(i = 0; i < tailLength; i++) {
        tail[i].x = i;
        tail[i].y = i;
    }

    printArr();
    //reassign size
    increaseSize(1);
    
    tail[tailLength - 1].x = 10;
    tail[tailLength - 1].y = 10;

    printArr();
}

void increaseSize(int add){
    if(add <= 0){
        return;
    }
    
    tailLength = tailLength + add;
    //create new size array pointer
    struct Position *temp = (struct Position*) malloc(size * sizeof(struct Position));
   
    //copy existing values
    int i;
    for(i = 0; i < tailLength; i++) {
        temp[i].x = tail[i].x;
        temp[i].y = tail[i].y;
    }
    
    //free memory and assign new array
    free(tail);
    tail = temp;
    temp = NULL;
}

void decreaseSize(int sub) {
    if(sub <= 0){
        return;
    }
    
    tailLength = tailLength - sub;
    //create new size array pointer
    struct Position *temp = (struct Position*) malloc(tailLength * sizeof(struct Position));
   
    //copy existing values
    int i;
    for(i = 0; i < size; i++) {
        temp[i].x = tail[i].x;
        temp[i].y = tail[i].y;
    }

    //free memory and assign new array
    free(tail);
    tail = temp;
    temp = NULL;
}

void printArr() {
    printf("new size; %i", tailLength);
    int i;
    printf("----------\n");
   for(i = 0; i < tailLength; i++) {
        printf("(%d, %d)\n", tail[i].x, tail[i].y);
    }
}
