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
        uint16_t x_old;
        uint16_t y_old;
        uint8_t size;
        float direction;
        float direction_old;
        uint8_t sensor_left;
        uint8_t sensor_right;
} Car;

long long map[40]={
        0x0,0x0,0xFFFFFF000000,0x000001000000,0x000001000000,
        0x000001000000,0x000001000000,0x000001000000,0x000001000000,0x000001000000,
        0x000001000000,0x000001000000,0x000001000000, 0x000001000000,0xFFE001000000,
        0x2001000000,0x2001000000,0x2001000000,0x2001000000,0x2001000000,
        0x2001000000,0x2001000000,0x2001000000,0x2001000000,0x2001000000,
        0x2001000000, 0x2001FFFFFF, 0x2000000000,0x2000000000,0x2000000000,
        0x2000000000,0x2000000000,0x2000000000,0x2000000000,0x2000000000,
        0x2000000000,0x2000000000,0x2000000000,0x2000000000,0x3FFFFFFFFF
};
long long one = 0x1;

Car mycar = {
        .x = 51,
        .y = 320,
        .size = 16,
        .direction = d2radian(271)
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

//Mode
uint8_t demoMode = 0;


void Draw_Wall(){
        for (uint64_t i = 0; i < 40; i++) {//x
                for (uint64_t j = 0; j < 48; j++) {//y
                        if (!(map[i] & (one << j))) {
                                LCD_SetTextColor( LCD_COLOR_BLACK );
                                LCD_DrawFullRect( i * 6, topBarsize + j * 6, 4, 4 );
                        }
                        else{
                                LCD_SetTextColor( LCD_COLOR_WHITE );
                                LCD_DrawFullRect( i * 6, topBarsize + j * 6, 4, 4 );
                        }
                }
        }
}
void Draw_Car(Car *x){
        char *buf[10];
        LCD_DrawCircle(x->x, x->y, x->size);
        // LCD_DisplayString(0,itoa(x->x,buf));
        LCD_DrawUniLine(x->x, x->y, x->x - 24 * cos(x->direction), x->y + 24 * sin(x->direction));
}

int sensor(x, y, dir, max = ){

}
void
Game_Init()
{

        LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);
        // LCD_SetTextColor(LCD_COLOR_WHITE);
        char buf[10];
        LCD_DisplayStringLine(0, itoa(cos(mycar.direction) * 1000000, buf));

        Draw_Wall();
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
        mycar.x_old = mycar.x;
        mycar.y_old = mycar.y;
        mycar.direction_old = mycar.direction;
        // mycar.x -= 2 * 1;
        mycar.y -= 2 * 1;

        // mycar.x -= 2 * cos(d2radian(mycar.direction));
        // mycar.y += 2 * sin(d2radian(mycar.direction));

}

void
GAME_Render()
{

        LCD_SetTextColor( LCD_COLOR_BLACK );

        LCD_DrawCircle(mycar.x_old, mycar.y_old, mycar.size);
        LCD_DrawUniLine(mycar.x_old, mycar.y_old, mycar.x_old - 24 * cos(mycar.direction_old), mycar.y_old + 24 * sin(mycar.direction_old));




        LCD_SetTextColor( LCD_COLOR_WHITE );
        LCD_DrawLine(  0, LCD_PIXEL_HEIGHT - buttonPadding, LCD_PIXEL_WIDTH, LCD_DIR_HORIZONTAL );

        LCD_SetTextColor( LCD_COLOR_RED );
        LCD_DrawCircle(mycar.x, mycar.y, mycar.size);
        LCD_DrawUniLine(mycar.x, mycar.y, mycar.x - 24 * cos(mycar.direction), mycar.y + 24 * sin(mycar.direction));
        // LCD_DrawUniLine(mycar.x, mycar.y, mycar.x + 24, mycar.y + 24);

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
