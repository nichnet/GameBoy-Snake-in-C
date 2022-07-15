#include <stdio.h>
#include <stdlib.h>

void init();
void loop();
void checkInput();
void updateSwitches();

void tickPlayer();
void respawnFood();
void moveSprite(int, int, int);
void movePlayer(int, int);

void increaseTailLength(int);
void decreaseTailLength(int);
void printArr();

const int TILE_SIZE = 8;
const int WIDTH = 20;
const int HEIGHT = 18;

struct Position {
    int x;
    int y;
};

enum Direction {
    UP, 
    LEFT, 
    RIGHT,
    DOWN
};




struct Position *tail;

struct Position foodPos;

int tailLength = 1;
lastDirection enum Direction = RIGHT;

void main() {
    init();
    loop();
}

void init() {
    //default
    tail = (struct Position*) malloc(tailLength * sizeof(struct Position));
    
    tail[0].x = (int) WIDTH / 2;
    tail[0].y = (int) HEIGHT / 2;
    
    respawnFood();
   /* 
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
*/
}

void loop() {
    while(1) {
        tickPlayer();
        checkInput();
        updateSwitches();
        //wait until VBLANK to avoid disrupting the visual memory.
        wait_vbl_done();
        delay(200);
    }
}

void checkInput() {
    if (joypad() & J_LEFT) {
        lastDirection = LEFT;
    } else if (joypad() & J_UP) {
        lastDirection = UP;
    } else if (joypad() & J_DOWN) {
        lastDirection = DOWN;
    } else if (joypad() & J_RIGHT) {
        lastDirection = RIGHT;
    }
}

void updateSwitches() {
    HDIE_WIN;
    SHOW_SPRITES;
    SHOW_BKG;
}

void tickPlayer() {
    movePlayer(0, lastDirection); 
    
    if(foodPos.x == tail[0].x && 
       foodPos.y == tail[0].y) {
        increaseTailLength(1);
        respawnFood();
    }
}

void respawnFood() {
    foodPos.x = 0;
    foodPos.y = 0;
    
    //TODO random location
    
    moveSprite(0, foodPos.x, foodPos.y); // 0 is ID for food.
}

void movePlayer(Direction direction) {
    int x = tail[0].x;
    int y = tail[0].y;
    switch(direction) {
        case LEFT:
            x -= 1;
            break;
        case RIGHT:
            x += 1;
            break;
        case UP:
            y -= 1;
            break;
        case DOWN:
            y += 1;
            break;
    }
    
    moveSprite(1, x, y); // 1 is ID for snake's head.
    lastDirection = direction;
}
         
 void moveSprite(int spriteId, int x, int y) {
    move_sprite(spriteId, x * TILE_SIZE, y * TILE_SIZE);
 }




void increaseTailLength(int add){
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

void decreaseTailLength(int sub) {
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
