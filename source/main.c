#include <string.h>
#include <stdio.h>
#include <switch.h>

// Test for Win

int testwin(int a, int b, int c, int points[]) {
    int win= 0;
    if(points[a] == points[b] && points[b] == points[c] && points[a] != 0) {
        points[a] += 3;
        points[b] += 3;
        points[c] += 3;
        win = 1;
    }
    return win;
}


// Square with Center coords

bool pointInSquareC(int px, int py, int x, int y, int s) {
    if(px > x-s/2 && x+s/2 > px && y-s/2 < py && y+s/2 > py) {
        return 1;
    }
    return 0;
}
u32 width, height;
u32 act_pos;
    u32* framebuf;
    int points[480] = {0};
    int first = 1;
    int game_end = 0;


void drawRect( u32 c, u32 rx,u32 ry, u32 rw, u32 rh, u32 b, u32 bc) {
    for (int y=ry; y<ry+rh; y++)//Access the buffer linearly.
            {
                for (int x=rx; x<rw+rx; x++)
                {
                u32 pos = y * width + x;
                // Background-Color:
                if(x<rx+b || x > rw+rx-b-1 || y < ry+b || y > ry+rh-b-1) {
                framebuf[pos] = bc;
                } else
                framebuf[pos] = (u32) c;
            }
        }
}

void createDigit(u32 rx,u32 ry, int a, int b, int c, int d, int e, int f, int g) {
    rx += 12;
    ry += 10;
    if(a == 1) {
        drawRect(0xFF000000, rx+12, ry+2, 21, 2, 0x00000000, 0x00000000);
    }
    if(g == 1) {
        drawRect(0xFF000000, rx+12, ry+22, 21, 2, 0x00000000, 0x00000000);
    }
    if(d == 1) {
        drawRect(0xFF000000, rx+12, ry+42, 21, 2, 0x00000000, 0x00000000);
    }
    if(f == 1) {
        drawRect(0xFF000000, rx+12, ry+2, 2, 21, 0x00000000, 0x00000000);
    }
    if(b == 1) {
        drawRect(0xFF000000, rx+32, ry+2, 2, 21, 0x00000000, 0x00000000);
    }
    if(e == 1) {
        drawRect(0xFF000000, rx+12, ry+22, 2, 21, 0x00000000, 0x00000000);
    }
    if(c == 1) {
        drawRect(0xFF000000, rx+32, ry+22, 2, 21, 0x00000000, 0x00000000);
    }
}

void singleDigit(int n, u32 rx,u32 ry) {
    if(n == 1) createDigit(rx, ry, 0, 1, 1, 0, 0, 0, 0);
    if(n == 2) createDigit(rx, ry, 1, 1, 0, 1, 1, 0, 1);
    if(n == 3) createDigit(rx, ry, 1, 1, 1, 1, 0, 0, 1);
    if(n == 4) createDigit(rx, ry, 0, 1, 1, 0, 0, 1, 1);
    if(n == 5) createDigit(rx, ry, 1, 0, 1, 1, 0, 1, 1);
    if(n == 6) createDigit(rx, ry, 1, 0, 1, 1, 1, 1, 1);
    if(n == 7) createDigit(rx, ry, 1, 1, 1, 0, 0, 0, 0);
    if(n == 8) createDigit(rx, ry, 1, 1, 1, 1, 1, 1, 1);
}

int getNum(u32 field) {
    return 0 + (field+1 < 480 && field%30 != 29 && (points[field+1] == 2 || points[field+1] == 4)) 
    + (field-1 >= 0 && field%30 != 0 && (points[field-1] == 2 || points[field-1] == 4)) 
    + (field+30 < 480 && (points[field+30] == 2 || points[field+30] == 4)) 
    + (field-30 >= 0 && (points[field-30] == 2 || points[field-30] == 4)) 
    + (field+31 < 480 && field%30 != 29 && (points[field+1+30] == 2 || points[field+1+30] == 4)) 
    + (field-1+30 < 480 && field%30 != 0 && (points[field-1+30] == 2 || points[field-1+30] == 4)) 
    + (field-30+1 >= 0 && field%30 != 29 && (points[field+1-30] == 2 || points[field+1-30] == 4)) 
    + (field-30-1 >= 0 && field%30 != 0 && (points[field-1-30] == 2 || points[field-1-30] == 4));
}



void openField(field) {
    if(field < 0 || field >= 480) return;
    if(getNum(field) < 1 && points[field] != 1) {
        points[field] = 1;
        openAround(field);
    } else {
                points[field] = 1;
    }
}

void openAround(field) {
    if(field%30 != 29) openField(field+1);
    if(field%30 != 0) openField(field-1);
    if(field%30 != 29) openField(field+1+30);
    if(field%30 != 0) openField(field-1+30);
    if(field%30 != 29) openField(field+1-30);
    if(field%30 != 0) openField(field-1-30);
    openField(field+30);
    openField(field-30);
}

int main(int argc, char **argv)
{

    u32  cnt=0;

    //Enable max-1080p support. Remove for 720p-only resolution.
    gfxInitResolutionDefault();

    gfxInitDefault();


    // Needed Vars
    int player = 1;
    int win;
    u32 error = 999;
    act_pos = 0;
    win = 0;

    int start = 0;


    while(appletMainLoop())
    {
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);



        // Check inputs and move act_pos field/cursor
        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu
        else if (kDown & KEY_DDOWN) {
            act_pos += 30;
        }
        else if (kDown & KEY_DRIGHT)  {
            act_pos += 1;
        }
        else if (kDown & KEY_DLEFT) {
            act_pos -= 1;
        }
        else if (kDown & KEY_DUP)  {
            act_pos -= 30;
        }
        else if (kDown & KEY_JOYCON_DOWN) { 
            if(points[act_pos] == 2) {
                points[act_pos] = 4;
            } else if(points[act_pos] == 0) {
                points[act_pos] = 3;
            } 
        }
        else if (kDown & KEY_JOYCON_RIGHT) { // Pressed A
            if(game_end) {
                game_end = 0;
                memset(points, 0, sizeof points);
                first = 1;
                error = 999;
            } else

            if(points[act_pos] != 1) {
                if(act_pos > 30*16-1 || act_pos < 0) act_pos = 0;
                if(first) {
                    points[act_pos] = 1;
                    // Setze bomben
                    int bombs = 0;
                    while(bombs < 100) {
                        int point = randomGet64()%(30*16);
                        if(point-1 == act_pos ||
                            point+1 == act_pos ||
                            point+2 == act_pos ||
                            point-2 == act_pos ||
                            point+60 == act_pos ||
                            point-60 == act_pos ||
                            point-1+30 == act_pos ||
                            point+1+30 == act_pos ||
                            point+30 == act_pos ||
                            point-30 == act_pos ||
                            point-1-30 == act_pos ||
                            point+1-30 == act_pos ||
                            point == act_pos ||
                            points[point] != 0
                        ) {
                            continue;
                        }
                        points[point] = 2;
                        bombs++;
                    }
                    first = 0;
                    if(getNum(act_pos) == 0) openAround(act_pos);

                } else {
                    if(points[act_pos] == 2) {
                        game_end = 1;
                        error = act_pos;
                    } else if(points[act_pos] == 0) {
                        points[act_pos] = 1;
                        if(getNum(act_pos) == 0) openAround(act_pos);
                    } else if(points[act_pos] == 4) {
                        points[act_pos] = 2;
                    } else {
                        points[act_pos] = 0;
                    }
                }
            }
        }

        if (kDown & KEY_MINUS) {
            main(argc, argv);
            return 0; 
        }
        if(act_pos > 30*16-1 || act_pos < 0) act_pos = 0;


        framebuf = (u32*) gfxGetFramebuffer((u32*)&width, (u32*)&height);
        touchPosition touch;

        u32 touch_count = hidTouchCount();
        if(touch_count > 0) {
            hidTouchRead(&touch, 0);
            act_pos = (touch.px*30)/1280+(touch.py*16)/720*30;
        }

for (int y=0; y<height; y++)//Access the buffer linearly.
        {
            for (int x=0; x<width; x++)
            {
                u32 pos = y * width + x;
                u32 color = 0x00FF00FF;

                // Background-Color:
                framebuf[pos] = color;

            }
        }
       /* drawRect( 0xFF00FFFF, 20,20,50,50,0);
        drawRect( 0x00F0FFF0, 360, 300,80,50,5);
        drawRect( 0xFFFFFFFF, 200, 300,20,50,2);
        drawRect( 0xFFFFFFFF, 200, 100,80,50,7);*/
        int fieldw = 30;
        int fieldh = 16;
        for(int y=0; y<fieldh; y++) {
            for(int x=0; x<fieldw; x++) {
                int i = fieldw*y+x;
                u32 c = 0xFFAAAAAA;
                u32 bc = 0xFF000000;
                u32 b = 1;
                if(points[i] == 1) c = 0xFFAAAAFF;
                //if(points[i] == 2) c = 0xFFFF88FF;
                if(points[i] == 3 || points[i] == 4) c = 0xFF88FFFF;
                if(error == i) c = 0xFFFF0000;
                if(i == act_pos) {
                    bc = 0xFFFF0000;
                    b = 5;
                }

                drawRect(c, width/fieldw*x, height/fieldh*y, width/fieldw, height/fieldh, b, bc);
                if(points[i] == 1) singleDigit(getNum(i), width/fieldw*x, height/fieldh*y);
            }
        }
  
/*
        for (y=0; y<height; y++)//Access the buffer linearly.
        {
            for (x=0; x<width; x++)
            {
                pos = y * width + x;
                color = 0x00000000;

                // Background-Color:
                framebuf[pos] = color;

                // Fill Fields
                for(int rx = 0; rx < 3; rx++) {
                    for(int ry = 0; ry < 3; ry++) {
                        int id = ry+rx*3;
                        if(pointInSquareC(x,y, width/2-s-d+(s+d)*rx, height/2-s-d+(s+d)*ry, s+b*2)) {
                            color = 0x00000000;
                            if(act_pos == id) color = 0xFFFFFFFF; else color = 0x00000000;
                            if(points[id] > 2) color = wincolor;
                            framebuf[pos] = color;//Set framebuf to different shades of grey.
                        }

                        if(pointInSquareC(x,y, width/2-s-d+(s+d)*rx, height/2-s-d+(s+d)*ry, s)) {
                            if(points[id]%3 == 0) color = 0x99999999; // Grey -> Not set
                            if(points[id]%3 == 1) color = 0x1111FFFF; // Player 1 - Color
                            if(points[id]%3 == 2) color = 0x1111FF11; // Player 2 - Color
                            framebuf[pos] = color;//Set framebuf to different shades of grey.
                        }
                    }
                }
            }
        }*/

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}
