#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define topBarsize 24
#define buttonPadding 6
#define carSize 3

#define d2radian(x) 0.0174533 * x

// #define xtranslate(x) x * 6
// #define ytranslate(y) (LCD_PIXEL_HEIGHT - y * 6)
// #define scaler(x) x * 6

typedef struct Car {
        uint16_t x;
        uint16_t y;
        uint8_t size;
        uint16_t direction;
        uint8_t sensor_left;
        uint8_t sensor_right;
} Car;

uint64_t map[40];

Car mycar = {
        .x = 120,
        .y = 320,
        .size = 16,
        .direction = 315
};

char* itoa(int i, char b[]){
        char const digit[] = "0123456789";
        char* p = b;
        if(i<0) {
                *p++ = '-';
                i *= -1;
        }
        int shifter = i;
        do {         //Move to where representation ends
                ++p;
                shifter = shifter/10;
        } while(shifter);
        *p = '\0';
        do {         //Move back, inserting digits as u go
                *--p = digit[i%10];
                i = i/10;
        } while(i);
        return b;
}

int gameInitflag = 1;
//
// //Player2
// int16_t player2X = LCD_PIXEL_WIDTH - 20;
// int16_t player2Y = LCD_PIXEL_HEIGHT - 20;
// uint16_t player2W = 60;
// uint16_t player2H = 10;
// uint8_t player2IsReversed = 0;
//
// //Ball
// uint16_t ballSize = 5;
// int16_t ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
// int16_t ballY = ( LCD_PIXEL_HEIGHT - 5 ) - 30;// / 2;
// int16_t ballVX = -3;
// int16_t ballVY = -3;
// uint8_t ballIsRun = 0;
// uint8_t ballStepBack = 0;
// uint16_t ballRadius = 3;//ballSize / 2;
//Bricks
uint16_t bricks[24];
uint8_t bricksinit = 1;

//Mode
uint8_t demoMode = 0;

// void
// BallReset()
// {
//  ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
//  ballY = ( LCD_PIXEL_HEIGHT - 5 ) -30;/// 2;
//
//  ballVX = -3;
//  ballVY = -3;
//
//  ballIsRun = 1;
// }
void Draw_Car(Car *x){
        char *buf[10];
        LCD_DrawCircle(x->x, x->y, x->size);
        // LCD_DisplayString(0,itoa(x->x,buf));
        LCD_DrawUniLine(x->x, x->y, x->x - 24 * cos(d2radian(x->direction)), x->y + 24 * sin(d2radian(x->direction)));
}
void
Game_Init()
{
        LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);
        LCD_SetTextColor(LCD_COLOR_WHITE);
        // LCD_DisplayStringLine(0, "Hello");
        Draw_Car(&mycar);
        // for (int16_t i = 0; i < 24;i++){
        //   bricks[i] = 0xffff;
        // }
        // LCD_SetTextColor( LCD_COLOR_WHITE );
        // for (uint16_t i = 0; i < 24;i++){
        //  for (uint16_t j = 0; j < 16;j++){
        //   LCD_DrawFullRect( i * 10, topBarsize + j * 10, 8, 8 );
        //  }
        // }
}


void
GAME_EventHandler1()
{
}

void
GAME_EventHandler2()
{
        if( IOE_TP_GetState()->TouchDetected ) {
                mycar.x = 120;
                mycar.y = 320;
                //  player2IsReversed = 1;

                // while( IOE_TP_GetState()->TouchDetected ) ;
                // player2IsReversed = 0;

        }
}

void
GAME_EventHandler3()
{
        // if( ballIsRun == 0 ){
        //  BallReset();
        // }
}

void
GAME_Update()
{
        if(gameInitflag) {
                Game_Init();
                gameInitflag = 0;
        }
        mycar.x -= 2 * cos(d2radian(mycar.direction));
        mycar.y += 2 * sin(d2radian(mycar.direction));

}

void
GAME_Render()
{
        LCD_SetTextColor( LCD_COLOR_RED );
        LCD_DrawLine(  0, LCD_PIXEL_HEIGHT - buttonPadding, LCD_PIXEL_WIDTH, LCD_DIR_HORIZONTAL );

				Draw_Car(&mycar);
// LCD_SetBackColor(LCD_COLOR_BLACK);


//  LCD_SetTextColor( LCD_COLOR_BLACK );
//  LCD_DrawCircle( ballX, ballY, ballRadius);
//  for (uint16_t i = 0; i < 24;i++){
//   for (uint16_t j = 0; j < 16;j++){
//    if (!(bricks[i] & (1 << j))){
//     LCD_SetTextColor( LCD_COLOR_BLACK );
//     LCD_DrawFullRect( i * 10, topBarsize + j * 10, 8, 8 );
//    }
//    else{
//     LCD_SetTextColor( LCD_COLOR_WHITE );
//     LCD_DrawFullRect( i * 10, topBarsize + j * 10, 8, 8 );
//    }
//   }
//  }
//  LCD_SetTextColor( LCD_COLOR_WHITE );
// //	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
//  LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
// //	LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );
//  LCD_DrawCircle( ballX, ballY, ballRadius);
//  LCD_DrawLine( 0, topBarsize - 1, LCD_PIXEL_WIDTH, LCD_DIR_HORIZONTAL );
}
