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
        float x;
        float y;
        float x_old;
        float y_old;
        uint8_t size;
        float direction;
        float direction_old;
        uint8_t sensor_left;
        uint8_t sensor_right;
        uint8_t lock;
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
        .x = 50,
        .y = 314,
        .size = 16,
        .direction = d2radian(225)
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
        *p++ = ' ';
        *p-- = '\0';
        do {         //Move back, inserting digits as u go
                *--p = digit[i%10];
                i = i/10;
        } while(i);
        return b;
}
void printnum(int line, int x){
        LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
        char buf[20];
        LCD_DisplayStringLine(25 * line, itoa(x, buf));
}

int gameInitflag = 1;

//Mode
uint8_t demoMode = 0;


void Draw_Wall(){
        for (uint64_t i = 0; i < 40; i++) {//x
                for (uint64_t j = 0; j < 48; j++) {//y
                        if (!(map[i] & (one << j))) {
                                //LCD_SetTextColor( LCD_COLOR_BLACK );
                                //LCD_DrawFullRect( i * 6, topBarsize + j * 6, 4, 4 );
                        }
                        else{
                                LCD_SetTextColor( LCD_COLOR_WHITE );
                                LCD_DrawFullRect( i * 6, topBarsize + j * 6, 6, 6 );
                                LCD_SetTextColor( LCD_COLOR_RED );
                                LCD_DrawFullRect( i * 6 + 1, topBarsize + j * 6 + 1, 4, 4 );
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

int sensor(uint16_t x, uint16_t y, float dir){
        int min = 102400, xd, yd, tmp;
        int tmp_x = -1, tmp_y = -1;


        printnum(0,x);
        printnum(1,y);
        //printnum(2,dir*10000);

        if(dir <= 1.5708) {
                xd = x - 18;
                yd = xd * tan(dir);
                tmp = xd * xd + yd * yd;
                if (min > tmp && xd >= 0 && yd >= 0) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;
                }


                yd = 246 - y;
                xd = yd * tan(1.5708 - dir);
                tmp = yd * yd + xd * xd;
                if (min > tmp && xd >= 0 && yd >= 0 && x - xd >= 84) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;
                }


                xd = x - 162;
                yd = xd * tan(dir);
                tmp = xd * xd + yd * yd;
                if (min > tmp && xd >= 0 && yd >= 0 && y + yd <= 174) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;
                }

                //LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);
                //LCD_DrawUniLine(x, y, x - tmp_x, y + tmp_y);
        }
        else if(dir <= 3.14159) {
                xd = 84 - x;
                yd = xd * tan(3.1415926 - dir);
                tmp = xd * xd + yd * yd;
                if (min > tmp && xd >= 0 && yd >= 0 && y + yd >= 246) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;
                }

                yd = 246 - y;
                xd = yd * tan(dir - 1.5708);
                tmp = yd * yd + xd * xd;
                if (min > tmp && xd >= 0 && yd >= 0 && x + xd >= 84) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;
                }

                xd = 234 - x;
                yd = xd * tan(3.1415926 - dir);
                tmp = xd * xd + yd * yd;
                if (min > tmp && xd >= 0 && yd >= 0) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;

                }
                //LCD_SetColors(LCD_COLOR_BLUE, LCD_COLOR_BLACK);
                //LCD_DrawUniLine(x, y, x + tmp_x, y + tmp_y);
        }
        else if(dir <= 4.71239) {
                xd = 84 - x;
                yd = xd * tan(dir - 3.1415926);
                tmp = xd * xd + yd * yd;
                if (min > tmp && xd >= 0 && yd >= 0 && y - yd >= 246) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;

                }

                yd = y - 174;
                xd = yd * tan(4.71239 - dir);
                tmp = yd * yd + xd * xd;
                if (min > tmp && xd >= 0 && yd >= 0 && x + xd <= 162) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;

                }

                xd = 234 - x;
                yd = xd * tan(dir - 3.1415926);
                tmp = xd * xd + yd * yd;
                if (min > tmp && xd >= 0 && yd >= 0) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;

                }
                //LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
                //LCD_DrawUniLine(x, y, x + tmp_x, y - tmp_y);
        }
        else{
                xd = x - 18;
                yd = xd * tan(6.28319 - dir);
                tmp = xd * xd + yd * yd;
                if (min > tmp && xd >= 0 && yd >= 0) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;

                }

                yd = y - 174;
                xd = yd * tan(dir - 4.71239);
                tmp = yd * yd + xd * xd;
                if (min > tmp && xd >= 0 && yd >= 0 && x - xd <= 162) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;

                }

                xd = x - 162;
                yd = xd * tan(6.28319 - dir);
                tmp = xd * xd + yd * yd;

                if (min > tmp && xd >= 0 && yd >= 0 && y - yd <= 174) {
                        min = tmp;
                        tmp_x = xd, tmp_y = yd;

                }
                //LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
                //LCD_DrawUniLine(x, y, x - tmp_x, y - tmp_y);

        }

        return min;
}
void Game_Init()
{

        LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);
        // LCD_SetTextColor(LCD_COLOR_WHITE);

        Draw_Wall();

}


void GAME_EventHandler1()
{
        if( STM_EVAL_PBGetState( BUTTON_USER ) ) {
                mycar.direction = (mycar.direction + 0.1 > 6.2831 ? mycar.direction - 6.1831 : (mycar.direction + 0.1));
                vTaskDelay(100);
        }
}

void GAME_EventHandler2()
{
        TP_STATE *tmp = IOE_TP_GetState();
        if( tmp->TouchDetected && tmp->X < 220 && tmp->Y < 300 && tmp->X > 20 && tmp->Y > 20) {
                // mycar.x += 1;
                mycar.x = tmp->X;
                mycar.y = tmp->Y;
                printnum(0, tmp->X);
                printnum(1, tmp->Y);
                vTaskDelay(20);


        }
}

void GAME_EventHandler3()
{
        // if( ballIsRun == 0 ){
        //  BallReset();
        // }
}

void GAME_Update()
{
        if(gameInitflag) {
                Game_Init();
                gameInitflag = 0;
        }
        char buf[10];
        int l,r;
        float theta;
        mycar.lock = 1;
        l = sensor(mycar.x, mycar.y, (mycar.direction + 0.785398 > 6.2831 ? mycar.direction + 0.785398 - 6.2831 : (mycar.direction + 0.785398)));
        r = sensor(mycar.x, mycar.y, (mycar.direction - 0.785398 < 0 ? mycar.direction - 0.785398 + 6.2831 : (mycar.direction - 0.785398)));

        printnum(2, l);
        printnum(3, r);

        mycar.x_old = mycar.x;
        mycar.y_old = mycar.y;

        //TODO
        theta = 0.5;

        mycar.direction_old = mycar.direction;
        mycar.direction = mycar.direction + asin(2 * sin(theta) / 36);

        mycar.x = mycar.x_old - (cos(theta + mycar.direction) + sin(theta) * sin(mycar.direction));
        mycar.y = mycar.y_old + (sin(theta + mycar.direction) - sin(theta) * cos(mycar.direction));
        mycar.lock = 0;


}

void GAME_Render()
{

        if(~mycar.lock) {

                LCD_SetTextColor( LCD_COLOR_BLACK );
                LCD_DrawCircle(mycar.x_old, mycar.y_old, mycar.size);
                LCD_DrawUniLine(mycar.x_old, mycar.y_old, mycar.x_old - 24 * cos(mycar.direction_old), mycar.y_old + 24 * sin(mycar.direction_old));

                LCD_SetTextColor( LCD_COLOR_RED );
                LCD_DrawCircle(mycar.x, mycar.y, mycar.size);
                LCD_DrawUniLine(mycar.x, mycar.y, mycar.x - 24 * cos(mycar.direction), mycar.y + 24 * sin(mycar.direction));
        }

        LCD_SetTextColor( LCD_COLOR_WHITE );
        LCD_DrawLine(  0, LCD_PIXEL_HEIGHT - buttonPadding, LCD_PIXEL_WIDTH, LCD_DIR_HORIZONTAL );

}
