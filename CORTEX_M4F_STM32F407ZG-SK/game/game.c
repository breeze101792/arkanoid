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
static float buf_tmp;
// #define xtranslate(x) x * 6
// #define ytranslate(y) (LCD_PIXEL_HEIGHT - y * 6)
// #define scaler(x) x * 6
typedef struct Car {
        float x;
        float y;
        uint8_t size;
        float x_old;
        float y_old;
        float direction;
        float direction_old;
        float theta;
        uint32_t sensor_left;
        uint32_t sensor_middle;
        uint32_t sensor_right;
        uint8_t lock;
} Car;

float asin_List[81] = {-0.035718, -0.034969, -0.034210, -0.033440, -0.032661, -0.031871, -0.031071, -0.030262, -0.029444, -0.028617, -0.027781, -0.026937, -0.026085, -0.025224, -0.024356, -0.023481, -0.022598, -0.021709, -0.020813, -0.019911, -0.019002, -0.018088, -0.017168, -0.016244, -0.015314, -0.014379, -0.013441, -0.012498, -0.011551, -0.010601, -0.009647, -0.008691, -0.007732, -0.006771, -0.005807, -0.004842, -0.003875, -0.002908, -0.001939, -0.000970, 0.000000, 0.000970, 0.001939, 0.002908, 0.003875, 0.004842, 0.005807, 0.006771, 0.007732, 0.008691, 0.009647, 0.010601, 0.011551, 0.012498, 0.013441, 0.014379, 0.015314, 0.016244, 0.017168, 0.018088, 0.019002, 0.019911, 0.020813, 0.021709, 0.022598, 0.023481, 0.024356, 0.025224, 0.026085, 0.026937, 0.027781, 0.028617, 0.029444, 0.030262, 0.031071, 0.031871, 0.032661, 0.033440, 0.034210, 0.034969, 0.035718, 0.036456, 0.037182, 0.037898, 0.038602, 0.039294, 0.039974, 0.040642, 0.041298, 0.041941, 0.042571, 0.043188, 0.043792, 0.044383, 0.044961, 0.045524, 0.046074, 0.046610, 0.047131, 0.047638, 0.048131, 0.048609, 0.049072, 0.049521, 0.049954, 0.050372, 0.050774, 0.051161, 0.051533, 0.051889, 0.052229, 0.052553, 0.052861, 0.053153, 0.053429, 0.053688, 0.053931, 0.054158, 0.054368, 0.054562, 0.054739 };
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
        .x = 45,
        .y = 300,
        .size = 16,
        .direction = d2radian(270)
};
// Car mycar = {
//         .x = 194,
//         .y = 51,
//         .size = 16,
//         .direction = d2radian(90)
// };

float asinList (int x){
        return asin_List[x+40];
}
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
void printnumtwo(int line, int x, int y){
        LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
        char buf[20];
        int i;
        itoa(x, buf);
        for(i = 0; buf[i] != '\0'; ++i);
        buf[i++] = ',';
        itoa(y, buf +i);
        LCD_DisplayStringLine(25 * line, buf);
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
        int tmp_x = 0, tmp_y = 0;


        // printnum(0,x);
        // printnum(1,y);
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

                // LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);
                // LCD_DrawUniLine(x, y, x - tmp_x, y + tmp_y);
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
                // LCD_SetColors(LCD_COLOR_BLUE, LCD_COLOR_BLACK);
                // LCD_DrawUniLine(x, y, x + tmp_x, y + tmp_y);
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
                // LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
                // LCD_DrawUniLine(x, y, x + tmp_x, y - tmp_y);
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
                // LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
                // LCD_DrawUniLine(x, y, x - tmp_x, y - tmp_y);

        }
        // printnum(5, tmp_x);
        // printnum(6, tmp_y);
        // printnum(6, min);
        return min < 0 ? 102400 : min;

}

float fuzzyControl(float left, float middle, float right){
        // nomorlization
        // int max = left;
        // printf("%f, %f, %f\n",left, middle, right );
        const int bound = 5000;//25000;
        float lb = 0, ls = 0, mb = 0, ms = 0, rb = 0, rs = 0;
        float active_num;
        float c[80] = {};
        float tmp, tmp_b;


        // left
        if(left > bound) {
                lb = 1;
        }
        else if(left <= 0) {
                lb = 0;
        }
        else{
                lb = left / bound;
        }

        if(left > bound) {
                ls = 0;
        }
        else if(left <= 0) {
                ls = 1;
        }
        else{
                ls = -left / bound + 1;
        }
        // middle

        if(middle > bound) {
                mb = 1;
        }
        else if(middle <= 0) {
                mb = 0;
        }
        else{
                mb = middle / bound;
        }

        if(middle > bound) {
                ms = 0;
        }
        else if(middle <= 0) {
                ms = 1;
        }
        else{
                ms = -middle / bound + 1;
        }
        // right
        if(right > bound) {
                rb = 1;
        }
        else if(right <= 0) {
                rb = 0;
        }
        else{
                rb = right / bound;
        }

        if(right > bound) {
                rs = 0;
        }
        else if(right <= 0) {
                rs = 1;
        }
        else{
                rs = -right / bound + 1;
        }

        // rule 1
        // lb, ms, rs
        active_num = lb;
        // if(active_num > ms) active_num = ms;
        if(active_num > rs) active_num = rs;
        // printf("active: %f\n", active_num);

        for (int i = 0; i < 40; i++) {
                c[i] = (active_num < (40.0-i)/40.0 ? active_num : (40.0-i)/40.0);
                // printf("%f, ", (80.0-i)/80.0);
        }
        // for (int i = 0; i < 80; i++) {
        //         printf("%f, ", c[i]);
        //         if((i + 1)%10 == 0) printf("\n");
        // }
        // printf("\n");

        // rule 2
        // ls, ms, rb
        active_num = ls;
        // if(active_num > ms) active_num = ms;
        if(active_num > rb) active_num = rb;
        // printf("active: %f\n", active_num);

        for (int i = 79; i >= 40; i--) {
                c[i] = (active_num < (i-40)/40.0 ? active_num : (i-40)/40.0);
                // c[i] = (tmp > c[i] ? tmp : c[i]);
        }

        // for (int i = 0; i < 80; i++) {
        //         printf("%f, ", c[i]);
        //         if((i + 1)%10 == 0) printf("\n");
        // }
        // printf("\n");

        // rule 3
        // ls, mb, rs
        active_num = ls;
        if(active_num > mb) active_num = mb;
        if(active_num > rs) active_num = rs;

        for (int i = 0; i >= 80; i++) {
                if (i < 40) {
                        tmp = (active_num < (i)/40.0 ? active_num : (i)/40.0);
                        c[i] = (tmp > c[i] ? tmp : c[i]);
                }
                else{
                        tmp = (active_num < (80-i)/40.0 ? active_num : (80-i)/40.0);
                        c[i] = (tmp > c[i] ? tmp : c[i]);

                }
        }


        // defuzzy
        tmp = 0, tmp_b = 0;
        for (int i = 0; i < 80; i++) {
                tmp += c[i];
                tmp_b += c[i] * (i - 40);

                // printf("%f x %f, ", c[i], c[i] * (i - 40));
                // if((i + 1)%10 == 0) printf("\n");
        }
        // return 0;
        // printnum(2, left);
        // printnum(3, middle);
        // printnum(4, right);
        // vTaskDelay(10000);
        // printnum(5,tmp_b/tmp * 10000);
        return tmp_b/tmp;
}
// void cardrive(Car mycar){
//   mycar.theta = fuzzyControl(1,1,1);
// }

void Game_Init()
{

        LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);
        // LCD_SetTextColor(LCD_COLOR_WHITE);

        Draw_Wall();

}


void GAME_EventHandler1()
{
        if( STM_EVAL_PBGetState( BUTTON_USER ) ) {
                mycar.direction_old = mycar.direction;
                mycar.direction = (mycar.direction + 0.1 > 6.2831 ? mycar.direction - 6.1831 : (mycar.direction + 0.1));

                LCD_SetTextColor( LCD_COLOR_BLACK );
                LCD_DrawCircle(mycar.x_old, mycar.y_old, mycar.size);
                LCD_DrawUniLine(mycar.x_old, mycar.y_old, mycar.x_old - 24 * cos(mycar.direction_old), mycar.y_old + 24 * sin(mycar.direction_old));

                LCD_SetTextColor( LCD_COLOR_RED );
                LCD_DrawCircle(mycar.x, mycar.y, mycar.size);
                LCD_DrawUniLine(mycar.x, mycar.y, mycar.x - 24 * cos(mycar.direction), mycar.y + 24 * sin(mycar.direction));
                vTaskDelay(100);
        }
}

void GAME_EventHandler2()
{
        TP_STATE *tmp = IOE_TP_GetState();

        if( tmp->TouchDetected && tmp->X < 220 && tmp->Y < 300 && tmp->X > 20 && tmp->Y > 20) {
                if(tmp->TouchDetected && tmp->X < 170 && tmp->Y < 170) {
                        mycar.lock  = !mycar.lock;
                        vTaskDelay(100);
                }
                else{

                        // mycar.x += 1;
                        mycar.lock = 1;
                        mycar.x_old = mycar.x;
                        mycar.y_old = mycar.y;

                        mycar.x = tmp->X;
                        mycar.y = tmp->Y;
                        LCD_SetTextColor( LCD_COLOR_BLACK );
                        LCD_DrawCircle(mycar.x_old, mycar.y_old, mycar.size);
                        LCD_DrawUniLine(mycar.x_old, mycar.y_old, mycar.x_old - 24 * cos(mycar.direction_old), mycar.y_old + 24 * sin(mycar.direction_old));

                        LCD_SetTextColor( LCD_COLOR_RED );
                        LCD_DrawCircle(mycar.x, mycar.y, mycar.size);
                        LCD_DrawUniLine(mycar.x, mycar.y, mycar.x - 24 * cos(mycar.direction), mycar.y + 24 * sin(mycar.direction));
                        mycar.lock = 0;
                        // printnum(0, tmp->X);
                        // printnum(1, tmp->Y);
                        vTaskDelay(20);
                }

        }
}

void GAME_EventHandler3()
{
        // if( ballIsRun == 0 ){
        //  BallReset();
        // }
}

void volatile GAME_Update()
{
        if(gameInitflag) {
                Game_Init();
                gameInitflag = 0;
        }
        if(!mycar.lock) {
                mycar.lock = 1;
                mycar.sensor_left = (float)sensor(mycar.x, mycar.y, (mycar.direction + 0.785398 > 6.2831 ? mycar.direction + 0.785398 - 6.2831 : (mycar.direction + 0.785398)));
                mycar.sensor_right = sensor(mycar.x, mycar.y, (mycar.direction - 0.785398 < 0 ? mycar.direction - 0.785398 + 6.2831 : (mycar.direction - 0.785398)));
                mycar.sensor_middle = sensor(mycar.x, mycar.y, mycar.direction);

                printnumtwo(0, mycar.x, mycar.y);
                // printnumtwo(2, mycar.sensor_left, mycar.sensor_left);
                // int(mycar.sensor_left);
                printnum(2, mycar.sensor_left);
                printnum(3, mycar.sensor_middle);
                printnum(4, mycar.sensor_right);
                mycar.sensor_left;

                mycar.x_old = mycar.x;
                mycar.y_old = mycar.y;
                int tmp = fuzzyControl(mycar.sensor_left, mycar.sensor_middle, mycar.sensor_right);
                printnum(1, tmp);
                //TODO this solve compile bug.
                mycar.direction_old = mycar.direction;
                mycar.direction = mycar.direction + asinList(-tmp);
                // printnum(5, tmp);

                mycar.x = mycar.x_old - (cos(mycar.theta + mycar.direction) + sin(mycar.theta) * sin(mycar.direction));
                mycar.y = mycar.y_old + (sin(mycar.theta + mycar.direction) - sin(mycar.theta) * cos(mycar.direction));
                mycar.lock = 0;
        }


}

void GAME_Render()
{

        if(!mycar.lock) {

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
