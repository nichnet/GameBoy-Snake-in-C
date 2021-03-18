#include <gb/gb.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <rand.h>
#include "cells.c"
#include "SelectorSprite.c"

#define GRID_WIDTH 20
#define GRID_HEIGHT 18
#define CELL_SIZE 8
#define SPRITE_BUFFER_OFFSET CELL_SIZE


void init();
void createSelector();
void setSelectorPosition(int, int);
void updateSelectorGraphic();
void start();
void resetProgram();
void tick();
void repaint();
void createEmptyArray();
char getRandomCellState();
int getGridIndex(int, int);
char getCellState(int, int);
void setCellState(int, int, char);
int countNeighbours(int, int);
void updateSwitches();
void checkInput();


void playSound();


int started;

unsigned char cellBg[GRID_WIDTH * GRID_HEIGHT];

struct Selector {
    int id;
    int x;
    int y;
    int canMove;//0 = false, 1 = true
} selector;




void playSound() {
    //test
    NR10_REG = 0x38U;
    NR11_REG = 0x70U;
    NR12_REG = 0xE0U;
    NR13_REG = 0x0AU;
    NR14_REG = 0xC6U;
    NR51_REG |= 0x11;
    delay(500);
}

void main(){
    started = 0;

    //start sound
    NR50_REG = 0xFF;
  NR51_REG = 0xFF;
  NR52_REG = 0x80;


    init();

    //updateSwitches
    createEmptyArray();
    repaint();

    while(1) {
        //main loop
        checkInput();
        tick();
        updateSwitches();			// Make sure the SHOW_SPRITES and SHOW_BKG switches are on each loop
        wait_vbl_done();
    }
}

void init() {
//    int i = 0;

    srand(time(NULL)); // randomize seed


    DISPLAY_ON;

    //load in the cell background tile data to memory.
    set_bkg_data(0, 2, CellSprite);

    //Load in the selector sprite data to memory.
    set_sprite_data(0, 2, SelectorSprite);

    createSelector();
}

void createSelector() {
    int x = GRID_WIDTH / 2;
    int y = GRID_HEIGHT / 2;
    int id = 1;
    int canMove = 1;

    //create instance of selector struct
     selector.x = x;
     selector.y = y;
     selector.id = id;
     selector.canMove = canMove;

    //set pos.
    setSelectorPosition(x, y);
}

void setSelectorPosition(int x, int y) {
    int index;
    //char state;

    int xx = x;
    int yy = y;


    //TODO this first block could be imrpoved with a bounds function.
    if(xx < 0) {
        xx = GRID_WIDTH - 1;
    } else if(xx > GRID_WIDTH) {
        xx = 0;
    }

    if(yy < 0) {
        yy = GRID_HEIGHT - 1;
    } else if(yy > GRID_HEIGHT) {
        yy = 0;
    }
    //---

    //update struct data
    selector.x = xx;
    selector.y = yy;

    //move the sprite
    //here we update the grid coords to pixel coords. apply the buffer offset because the sprite is drawn in the top left corner.
    move_sprite(0, xx * CELL_SIZE + SPRITE_BUFFER_OFFSET, yy * CELL_SIZE + (SPRITE_BUFFER_OFFSET * 2));

    updateSelectorGraphic();
}

//updates the selectos graphic (sprite index), to be more visible over a living or dead cell.
void updateSelectorGraphic() {
    int x;
    int y;
    int index;
    char state;

    x = selector.x;
    y = selector.y;

    //set selector sprite index to make it more visible if over a living or dead cell.
    index = getGridIndex(x, y);
    state = cellBg[index];//this will either be 0x01 or 0x00, so it works for the value of the sprite index too..


    if(state == 0x01) {
        //TODO: first param 0 should come from selector.id, but how to get this without putting it into an int ref?
        set_sprite_tile(0, 1);//sprite id, and sprite data index.
    } else {
        set_sprite_tile(0, 0);//sprite id, and sprite data index.
    }
}

void resetProgram() {
    int i;

    //reset started to false
    started = 0;

    //clear all cell tile data
    for(i = 0; i < GRID_WIDTH * GRID_HEIGHT; i = i + 1) {
        cellBg[i] = 0x00;
    }

    createEmptyArray();
    clrscr();
    repaint();
}

//sets the start flag for the cells to begin simulating.
void start() {
    if(started == 0) {
        started = 1;
    } else {
        started = 0;
    }
}

//ticks the cells each loop frame.
void tick() {
    int x;
    int y;
    int cnt;
    char currentState;
    char newState;



    if(started == 1){
        for(y = 0; y < GRID_HEIGHT; y = y + 1) {
            for(x = 0; x < GRID_WIDTH; x = x + 1) {
                //iterate each cell and count neighbours
                cnt = countNeighbours(x, y);
                currentState = getCellState(x, y);

                newState = currentState;
                //determine if cell is on the edge... and stay the same..
            /*    if(x == 0 || x == GRID_WIDTH - 1 || y == 0 || y == GRID_HEIGHT - 1) {
                    //stay teh same state.
                    //todo write this better..
                } else {
            */
                    if(currentState == 0x01) {//alive
                        if(cnt < 2 || cnt > 3) {
                            //dies from under population, or over population
                            newState = 0x00;//death
                        }
                    } else {//dead
                        if(cnt == 3) {
                            //birth on next tick
                            //todo make birth on next tick not this tick!
                            newState = 0x01;//birth
                        }


                        //all other cases, stay the same state.
                    }
        

                //alive = getRandomCellState();
                setCellState(x, y, newState);
            }
        }

        //finally update the states!
        repaint();
    }
}

//TODO RENAME
void createEmptyArray() {
    int x;
    int y;
    char alive;

    alive = 0x00; //Default state

    //create the empty array of cells here.
    for(y = 0; y < GRID_HEIGHT; y = y + 1) {
        for(x = 0; x < GRID_WIDTH; x = x + 1) {
    //        alive = getRandomCellState();//todo test for now as random state..
            setCellState(x, y, alive);
        }
    }
}

//returns a ranomd cell state. living or dead.
char getRandomCellState() {
    int x;

    x = rand() % 2;
    x = abs(x);

    if(x == 0) {
        return 0x00;
    }

    return 0x01;
}

//converts the x, y coords of the grid to a single index for 1D array
int getGridIndex(int x, int y) {
    int index;

    if(x < 0 || x > GRID_WIDTH ||
        y < 0 || y > GRID_HEIGHT) {
        index = -1;//essentially null.
    } else {
        index =  y * GRID_WIDTH + x;
    }

    return index;
}

char getCellState(int x, int y) {
    int index;
    char state;

    index = getGridIndex(x, y);

    if(index < -1) {
        state = -1; //-1 if null.
    } else {
        state = cellBg[index];
    }

    return state;
}

//sets the given x,y cell state. living or dead.
void setCellState(int x, int y, char state) {
    int index;

    index = getGridIndex(x, y);

    if(state == 0x00) {
        cellBg[index] = 0x00;
    } else {
        cellBg[index] = 0x01;
    }
}

//counts the living neighbours of a given cell.
int countNeighbours(int x, int y) {
    int neighbourX;
    int neighbourY;
    char state;
    int count;

    int minX;
    int maxX;
    int minY;
    int maxY;


    count = 0;

    minX = x - 1;
    minY = y - 1;
    maxX = y + 2;
    maxY = y + 2;

    for(neighbourX = minX; neighbourX < maxX; neighbourX++) {
        for(neighbourY = minY; neighbourY < maxY; neighbourY++) {
           if(neighbourX == x && neighbourY == y){
               //printf("SELF\n");
           } else {
                state = getCellState(neighbourX, neighbourY);
                //printf("%d, %d, %d\n", neighbourX, neighbourY, state);


                if(state == 0x01) {
                    count++;
                }
//                 count += state; //can simply add state value (0 or 1) to the count.

//                if(state > ) {
//        //        }
            }
        }
    }

    return count;
}

//repaints the background tile cells.
void repaint() {
    set_bkg_tiles(0,0,GRID_WIDTH, GRID_HEIGHT, cellBg);
    updateSelectorGraphic();//update the selector graphic after redrawing the background tiles.
}


int j;

/*
void test(int, int);

void test(int x, int y) {
    int cnt;
    int i;

    j = j + 1;

    if(j >= 5) {
        j = 0;

        cnt = countNeighbours(x, y);

        for(i = 0; i < cnt; i++) {
            playSound();
        }
    }
}*/



//checks for input in the main loop.
void checkInput() {
    int x;
    int y;
    char oldState;
    char newState;
    int moved;
    int input;

    moved = 0;

    x = selector.x;
    y = selector.y;

    input = joypad();






    switch(input){
        //this movement is based on grid coords NOT pixel coords; hence only incrementing or decrementing by 1 (cell).
        case J_LEFT:
            x = x - 1;
            moved = 1;
            break;
        case J_RIGHT:
            x = x + 1;
            moved = 1;
            break;
        case J_UP:
            y = y - 1;
            moved = 1;
            break;
        case J_DOWN:
            y = y + 1;
            moved = 1;
            break;
        case J_A:
            oldState = getCellState(x, y);

            if(oldState == 0x00) {
                newState = 0x01;
            } else {
                newState = 0x00;
            }

            setCellState(x, y, newState);//set cell living
            updateSelectorGraphic();
            repaint();
            waitpadup();//wait for the pad to be released so we dont spam the input.
            break;
        case J_B:
            start();
            waitpadup();//wait for the pad to be released so we dont spam the input.
            break;
            case J_START:
            start();
            waitpadup();//wait for the pad to be released so we dont spam the input.
            break;
        //TODO this doesnt work - multi input start and select
        //case J_START & J_SELECT:
        case J_SELECT:
            resetProgram();
            waitpadup();//wait for the pad to be released so we dont spam the input.
            break;
    }
    //TODO should probably just waitpadup regardless if joypad() > 0

    if(moved) {
        setSelectorPosition(x, y);
        waitpadup();//wait for the pad to be released so we dont spam the input.
    }

    if(input > 0) {
    }
}

//updates the switch register flags.
void updateSwitches() {
	HIDE_WIN;//hide window layer
	SHOW_SPRITES;
	SHOW_BKG;
}
