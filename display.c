
#include "config.h"

#include "allegro.h"

#include <math.h>

#include "globvars.h"
#include "palette.h"

#define IN_DISPLAY_C
#include "display.h"
#undef IN_DISPLAY_C

#include "game.h"
#include "stuff.h"

#define TRANS_MODE drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
#define END_TRANS drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

#define SCAN_SIZE 50

#define TRANS_TABLE color_map = &trans_table;
#define ALPHA_TABLE color_map = &alpha_table;




BITMAP *display;

extern COLOR_MAP trans_table;
extern COLOR_MAP alpha_table;
// in palette.c

//#define SCREENSHOT

#ifdef SCREENSHOT
extern RGB palet [256];

#include "string.h"
#endif

//RLE_SPRITE *RLE_bubble [RLE_BUBBLES];

extern volatile int frames_per_second;
extern int slacktime;
extern int long_slacktime_store;

void draw_player(void);
void draw_enemies(void);
void draw_an_enemy(int e);

void draw_ebullets(int p);
void draw_an_ebullet(int b);

void draw_pbullets(int p);

void pline(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour);

void draw_stars(char motion);
void poly4(BITMAP *target, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int col);
void print_number(int x, int y, int n);
void draw_clouds(int p);
void draw_a_cloud(int c);
void draw_HUD(void);
void draw_trail_line(BITMAP *bmp, int x, int y, int size);
void draw_worm_trail_line(BITMAP *bmp, int x, int y, int size);
void draw_rocket_trail_line(BITMAP *bmp, int x, int y, int size);
void draw_shield_circle(BITMAP *bmp, int x, int y, int size);
void draw_pshield_circle(BITMAP *bmp, int x, int y, int strength);
void detect_player_collision(int p);
char check_pixel(int x, int y);
void run_display(int draw_everything, int star_motion);

void draw_hs(int p);

int points [8];

int scanner [SCAN_SIZE] [SCAN_SIZE];

RLE_SPRITE *circle_in [CIRCLES];
RLE_SPRITE *circle_out [CIRCLES];
RLE_SPRITE *circle_hurt [CIRCLES];
RLE_SPRITE *circle_white [CIRCLES];
RLE_SPRITE *circle_grey [CIRCLES];

//RLE_SPRITE *RLE_bcircle [3] [RLE_BCIRCLES];
//RLE_SPRITE *RLE_wcircle [3] [RLE_BCIRCLES];
RLE_SPRITE *RLE_ccircle_basic [3] [RLE_CCIRCLES];
RLE_SPRITE *RLE_ccircle_3cols [3] [RLE_CCIRCLES];
RLE_SPRITE *RLE_ccircle_2cols [3] [RLE_CCIRCLES];
RLE_SPRITE *RLE_small_shock [3] [30];
RLE_SPRITE *RLE_large_shock [3] [50];
RLE_SPRITE *RLE_huge_shock [3] [50];
RLE_SPRITE *RLE_player [PLAYER_RLES];
RLE_SPRITE *RLE_shield [SHIELD_RLES];
RLE_SPRITE *RLE_scircle [4] [RLE_SCIRCLES];


void draw_circle_in(int x, int y, int size, char hurt);
void draw_circle_out(int x, int y, int size);
void draw_circle_white(int x, int y, int size);
void draw_circle_grey(int x, int y, int size);
void ccircle(int x, int y, int size, int colour);
void ccircle3(int x, int y, int size, int colour);
void ccircle2(int x, int y, int size, int colour);

extern volatile unsigned char ticked;

#ifdef SHOW_GRAPHS
int graph_slack [200];
int graph_slack_pos;
int graph_fps [200];
int graph_fps_pos;
#endif

extern FONT* gfont;

BITMAP *interlude_screen;

/*
Call when you want the display updated.
Uses buffering: everything is drawn to the display bitmap, then the display bitmap is blitted to the screen
 in one go.
*/
void run_display(int draw_everything, int star_motion)
{
  if (!draw_everything)
  {
   if (!player[0].alive)
    return;
   draw_player();
   draw_ebullets(0);
   detect_player_collision(0);
   return;
  }

  clear_to_color(display, COL_BACK);
  draw_stars(star_motion);
  draw_hs(0);
  if (player[0].alive)
   draw_player();
  draw_ebullets(0);
  if (player[0].alive)
   detect_player_collision(0);
  draw_pbullets(0);
  draw_clouds(0);
  draw_HUD();

#ifdef DEBUG_DISPLAY
  textprintf_ex(display, font, 10, 10, -1, -1, "%i fps", frames_per_second);
  textprintf_ex(display, font, 10, 25, -1, -1, "%i slack", slacktime);
#endif

//  textprintf_ex(display, font, 10, 40, -1, -1, "%i, %i", player[0].x >> 10, player[0].y >> 10);
//  textprintf_ex(display, font, 10, 55, -1, -1, "%i, %i", player[0].x_speed, player[0].y_speed);
  //textprintf_ex(display, font, 10, 40, -1, -1, "%f, %i, %i, %i", hs[0].spin, hs[0].moment, hs[0].movement, hs[0].position);
//  textprintf_ex(display, font, 10, 55, -1, -1, "%i, %i : %i, %i: %i, %i", hs[0].x, hs[0].y, hs[0].centre_x, hs[0].centre_y, hs[0].x_speed, hs[0].y_speed);
/*  textprintf_ex(display, font, 10, 100, -1, -1, "%i %i %i", 0, hs[0].weapon [0], hs[0].weapon_number [0]);
  textprintf_ex(display, font, 10, 115, -1, -1, "%i %i %i", 1, hs[0].weapon [1], hs[0].weapon_number [1]);
  textprintf_ex(display, font, 10, 130, -1, -1, "%i %i %i", 2, hs[0].weapon [2], hs[0].weapon_number [2]);
  textprintf_ex(display, font, 10, 145, -1, -1, "%i %i %i", 3, hs[0].weapon [3], hs[0].weapon_number [3]);
  textprintf_ex(display, font, 10, 160, -1, -1, "%i %i %i", 4, hs[0].weapon [4], hs[0].weapon_number [4]);*/


#ifdef SHOW_GRAPHS
/*
 int i, pos;

 pos = graph_fps_pos + 1;

 for (i = 0; i < 200; i ++)
 {
  if (pos >= 200)
   pos = 0;
  if (graph_fps [pos] < 50)
   vline(display, 5 + i, 100, 100 - (graph_fps [pos]>>1), TRANS_RED2 + COL_DGREY);
    else
     vline(display, 5 + i, 100, 100 - (graph_fps [pos]>>1), TRANS_YELLOW4 + COL_DGREY);
  pos ++;
 }

 pos = graph_slack_pos + 1;

 for (i = 0; i < 200; i ++)
 {
  if (pos >= 200)
   pos = 0;
  vline(display, 5 + i, 120, 120 - (graph_slack [pos]<<1), TRANS_BLUE1 + COL_DGREY);
  pos ++;
 }
*/

#endif

/*  textprintf_ex(display, font, 10, 20, COL_WHITE, -1, "Press Escape to quit");

  textprintf_ex(display, font, 10, 30, COL_WHITE, -1, "parasites %i", arena.no_parasites);
  textprintf_ex(display, font, 10, 40, COL_WHITE, -1, "nutrient %i", arena.nutrient_level);
*/
/*

  int i;
  for (i = 0; i < 255; i ++)
  {
   rectfill(display, i * 3, 550, i * 3 + 3, 590, i);
  }*/

// vsync slows things down a lot but we'll leave it in for now.
//  vsync();


  blit(display, screen, 0, 0, 0, 0, 640, 480);
//  blit(display, screen, 0, 0, 0, 0, 320, 200);

#ifdef SCREENSHOT

static int scrs = 0;
static int sshot_counter = 0;


 char sfile [20];
 char istr [20];

 if (sshot_counter > 0)
  sshot_counter --;

 if (key [KEY_F1] && sshot_counter <= 0)
 {
  BITMAP *scrshot_bmp;
   scrshot_bmp = create_bitmap(640, 480);
   blit(screen, scrshot_bmp, 0,0,0,0,640,480);

  strcpy(sfile, "scr");
  strcat(sfile, itoa(scrs, istr, 10));
  strcat(sfile, ".bmp");
  save_bitmap(sfile, scrshot_bmp, palet);
  clear_to_color(screen, COL_WHITE);
  scrs ++;
  sshot_counter = 15;
  destroy_bitmap(scrshot_bmp);
 }

#endif

}

void get_interlude_background(void)
{

 run_display(1, 0);

 blit(display, interlude_screen, 0,0,0,0,640,480);

}

int evolve_screen(char reset)
{
 static int angle = 0;
 static int evolve_wait = 0;

// if (hs[0].stage <= 1)
//  return 1;

/*  textprintf_centre_ex(screen, font, 320, 10, -1, -1, "stage %i reset %i angle %i ticked %i", hs[0].stage, reset, angle, ticked);
   rest(10);*/

 if (reset == 1)
 {
//  clear_to_color(display, COL_BACK);
  blit(interlude_screen, display, 0,0,0,0,640,480);

  char gchar [2];
  gchar [0] = 'a' + hs[0].stage - 1;
  gchar [1] = 0;

  textprintf_centre_ex(display, font, 320, 150, -1, -1, "Hellspider");
  textprintf_centre_ex(display, gfont, 320, 171, -1, -1, gchar);
  textprintf_centre_ex(display, font, 320, 189, -1, -1, "Approaching");
  evolve_wait = 0;
 }
  else
  {
   if (ticked == 0)
    return 0;

   blit(interlude_screen, display, 320-40,360-40,320-40,360-40,80,80);

//   rectfill(display, 320 - 40, 360 - 40, 320 + 40, 360 + 40, COL_BACK);
  }

  angle += 10;

  int i;

/*//  TRANS_MODE
  circlefill(display, 320, 290, 22, TRANS_RED1 + COL_BACK);
  circlefill(display, 320, 290, 21, TRANS_RED2 + COL_BACK);
  circlefill(display, 320, 290, 20, TRANS_RED1 + COL_BACK);
//  END_TRANS*/

//  circlefill(display, 320, 290, 19, COL_BACK);

  for (i = 0; i < 6; i ++)
  {
    ccircle3(320 + xpart(angle + i * ANGLE_6, 31), 360 + ypart(angle + i * ANGLE_6, 31), 3, 0);
    ccircle2(320 + xpart(angle + i * ANGLE_6 + 50, 31), 360 + ypart(angle + i * ANGLE_6 + 50, 31), 2, 0);
    ccircle2(320 + xpart(angle + i * ANGLE_6 - 50, 31), 360 + ypart(angle + i * ANGLE_6 - 50, 31), 2, 0);
  }

  if (reset == 1)
   blit(display, screen, 0, 0, 0, 0, 640, 480);
    else
     blit(display, screen, 320 - 40, 360 - 40, 320 - 40, 360 - 40, 80, 80);

   ticked = 0;
   evolve_wait ++;

//   if(key [KEY_ESC])
//    exit(10);


   if (evolve_wait < 50)
    return 0;

 return 1;

}


void draw_player(void)
{

 int sprite = PLAYER_RLE_BASE_1;
 int y = -1;

 if (player[0].drive > 5)
 {
  sprite = PLAYER_RLE_BASE_2;
  y = 0;
 }

 if (player[0].drive > 9)
 {
  sprite = PLAYER_RLE_BASE_3;
  y = 1;
 }

 draw_rle_sprite(display, RLE_player [sprite], 320 - 12, 360 - 10);

 int sprite2 = PLAYER_RLE_SIDE_L1;
 if (player[0].flap [0] > 5)
  sprite2 = PLAYER_RLE_SIDE_L2;
 if (player[0].flap [0] > 9)
  sprite2 = PLAYER_RLE_SIDE_L3;
 draw_rle_sprite(display, RLE_player [sprite2], 320 - 15, 360 + y);

 sprite2 = PLAYER_RLE_SIDE_R1;
 if (player[0].flap [1] > 5)
  sprite2 = PLAYER_RLE_SIDE_R2;
 if (player[0].flap [1] > 9)
  sprite2 = PLAYER_RLE_SIDE_R3;
 draw_rle_sprite(display, RLE_player [sprite2], 320 + 9, 360 + y);

 int size;
 int drive = player[0].drive;

 int col = 0;

 if (player[0].upgrade [U_AGILITY] == 2)
  col = 2;

 if (drive > 10)
  drive = 10;

 if (drive > 0)
 {
  size = (drive>>1) + grand(4);

  if (player[0].upgrade [U_AGILITY] == 1)
   ccircle3(320, 360 + 8, size, col);
    else
     ccircle2(320, 360 + 8, size, col);

  size -= 2 + grand(3);

  if (size > 0)
   ccircle(320, 360 + 6, size, col);

  if (drive > 5)
  {
   if (player[0].upgrade [U_AGILITY] == 1)
    ccircle3(320, 360 + 10 + grand(3), size, col);
     else
      ccircle2(320, 360 + 10 + grand(3), size, col);
   if (drive > 8 && size > 2)
   {
    if (player[0].upgrade [U_AGILITY] == 1)
     ccircle3(320, 360 + 14 + grand(3), size - 2, col);
      else
       ccircle2(320, 360 + 14 + grand(3), size - 2, col);
   }

  }

 }

// draw_rle_sprite(display, RLE_player [PLAYER_RLE_BASE], 320 - 12, 360 - 4);

 int anim;

 if (player[0].recycle == 10)
  anim = 1;
 if (player[0].recycle == 9)
  anim = 2;
 if (player[0].recycle == 8)
  anim = 3;
 if (player[0].recycle < 8)
  anim = player[0].recycle >> 1;
 if (anim > 3) anim = 3;

 draw_rle_sprite(display, RLE_player [PLAYER_RLE_FRONT_1 + anim], 320 - 5, 360 - 11);

 if (player[0].recycle > 0)
 {
  int ccol = 0;
  if (player[0].upgrade [U_POWER] == 2)
   ccol = 2;
  ccircle(320 - 3, 360 - 10 + anim, player[0].recycle >> 1, ccol);
  ccircle(320 + 3, 360 - 10 + anim, player[0].recycle >> 1, ccol);

  ccircle2(320 - 3, 360 - 10 + anim, player[0].recycle >> 0, ccol);
  ccircle2(320 + 3, 360 - 10 + anim, player[0].recycle >> 0, ccol);
 }

 rectfill(display, 320 - 2, 360 - 4, 320 + 2, 360 + 1, COL_DGREY); // prepares player sprite for collision detection later
 hline(display, 320 - 1, 360 - 5, 320 + 1, COL_DGREY);
 hline(display, 320 - 1, 360 + 2, 320 + 1, COL_DGREY);

 if (player[0].shield_time > 0)
 {
  textprintf_centre_ex(display, font, 320, 420, -1, -1, "shield activated");
  textprintf_centre_ex(display, font, 320, 440, -1, -1, "weapons off-line");
  int num = player[0].shield_time * 2;
TRANS_MODE
            do_circle(display, 320, 360, 16, num, draw_pshield_circle);

  if (num > 146)
   circlefill(display, 320, 360, 16, TRANS_BLUE4);
    else
  if (num > 142)
   circlefill(display, 320, 360, 16, TRANS_BLUE3);
    else
  if (num > 138)
   circlefill(display, 320, 360, 16, TRANS_BLUE2);
    else
  if (num > 134)
   circlefill(display, 320, 360, 16, TRANS_BLUE1);
END_TRANS
// draw_trans_rle_sprite(display, RLE_shield [0], 320 - 18, 360 - 13);

/*  int i;
  int angle, dist;
  TRANS_MODE
  for (i = 0; i < num; i ++)
  {
    angle = grand(ANGLE_1);
    dist = grand(7) + grand(7) + grand(7) + grand(7) + grand(7) + grand(7);
    if (dist > 21)
     dist = 42 - dist;

    if (i < 16)
     putpixel(display, 320 + xpart(angle, dist), 360 + ypart(angle, dist), TRANS_BLUE1);
      else
       ccircle3(320 + xpart(angle, dist), 360 + ypart(angle, dist), 1 + grand(2), 2);

  }
 END_TRANS*/

 }

// draw_rle_sprite(display, RLE_player [PLAYER_RLE_1], 20, 20);

 //circle(display, 320, 360, 7, COL_WHITE);

/* int x = player.x / GRAIN;
 int y = player.y / GRAIN;

 rectfill(display, x - 5, y - 4, x + 5, y + 4, COL_YELLOW);
 rect(display, x - 6, y - 5, x + 6, y + 5, COL_OUTLINE);*/

// rectfill(display, 390, 292, 410, 308, 3);
// rect(display, x - 6, y - 5, x + 6, y + 5, COL_OUTLINE);
/*
  draw_rle_sprite(display, test_thing [arena.counter].sprite, 100 - test_thing [arena.counter].x, 100 - test_thing [arena.counter].y);

 circle(display, 100 + xpart(arena.counter * 4, 22), 100 + ypart(arena.counter * 4, 22), 2, 3);
 circle(display, 100 + xpart(arena.counter * 4 - ANGLE_2, 22), 100 + ypart(arena.counter * 4 - ANGLE_2, 22), 2, 3);
*/

/* rectfill(display, 600, 400, 800, 600, 0);
 putpixel(display, 700 + (player[0].x >> 12), 500 + (player[0].y >> 12), 15);

 putpixel(display, 700 + (player[0].x >> 12) + xpart(player[0].angle, 5), 500 + (player[0].y >> 12) + ypart(player[0].angle, 5), 10);
*/
//print_number(400, 400, (int) hypot(player[0].y - hs[0].y, player[0].x - hs[0].x) >> 15);
//print_number(400, 400, part[NO_PARTS - 1].x);
/*   textprintf_centre_ex(display, font, 400, 380, -1, -1, "%i", part[50].x);
   textprintf_centre_ex(display, font, 400, 400, -1, -1, "%i", part[50].y);
   textprintf_centre_ex(display, font, 400, 420, -1, -1, "%i", part[50].old_x);
   textprintf_centre_ex(display, font, 400, 440, -1, -1, "%i", part[50].old_y);
   textprintf_centre_ex(display, font, 400, 460, -1, -1, "%i", part[50].hp);
*/
#ifdef DEBUG_DISPLAY
print_number(400, 400, player[0].range);
print_number(400, 415, hs[0].firing [0]);
print_number(420, 415, hs[0].firing [1]);
print_number(400, 430, player[0].angle_to_hs);
#endif
}


void detect_player_collision(int p)
{
//    return;

  int x = 320;
  int y = 360;
/*
  if (check_pixel(x - 1, y + 2)
   || check_pixel(x, y + 2)
   || check_pixel(x + 1, y + 2)

   || check_pixel(x - 1, y + 1)
//   || check_pixel(x, y + 1)
   || check_pixel(x + 1, y + 1)

   || check_pixel(x - 1, y)
//   || check_pixel(x, y)
   || check_pixel(x + 1, y)

   || check_pixel(x - 1, y - 1)
//   || check_pixel(x, y + 1)
   || check_pixel(x + 1, y - 1)

   || check_pixel(x - 1, y - 2)
//   || check_pixel(x, y + 2)
   || check_pixel(x + 1, y - 2)

   || check_pixel(x - 1, y - 3)
   || check_pixel(x, y - 3)
   || check_pixel(x + 1, y - 3))
   {
    player_hit(p);
   }

*/


  if (check_pixel(x - 1, y + 2)
   || check_pixel(x, y + 2)
   || check_pixel(x + 1, y + 2)

   || check_pixel(x - 2, y + 1)
   || check_pixel(x + 2, y + 1)

   || check_pixel(x - 2, y + 0)
   || check_pixel(x + 2, y + 0)

   || check_pixel(x - 2, y - 1)
   || check_pixel(x + 2, y - 1)

   || check_pixel(x - 2, y - 2)
   || check_pixel(x + 2, y - 2)

   || check_pixel(x - 2, y - 3)
   || check_pixel(x + 2, y - 3)

   || check_pixel(x - 2, y - 4)
   || check_pixel(x + 2, y - 4)

   || check_pixel(x - 1, y - 5)
   || check_pixel(x, y - 5)
   || check_pixel(x + 1, y - 5)

   || check_pixel(x, y - 4)
   || check_pixel(x, y - 3)
   || check_pixel(x, y - 2)
   || check_pixel(x, y - 1)
   || check_pixel(x, y - 0)
   || check_pixel(x, y + 1)
   )
   {
    player_hit(p, 0);
   }

//  rectfill(display, 320 - 2, 360 - 3, 320 + 2, 360 + 2, COL_WHITE);
 rectfill(display, 320 - 2, 360 - 4, 320 + 2, 360 + 1, COL_WHITE);
 hline(display, 320 - 1, 360 - 5, 320 + 1, COL_WHITE);
 hline(display, 320 - 1, 360 + 2, 320 + 1, COL_WHITE);

  return;

}

char check_pixel(int x, int y)
{
  int pix = getpixel(display, x, y);

//  putpixel(display, x, y, TRANS_BLUE2 + grand(10));

  if (pix == COL_WHITE || pix >= TRANS_YELLOW4)
    return 1;

 return 0;

}





#define SCANNER_X 590
#define SCANNER_Y 430
#define SCANNER_SIZE 30

void draw_HUD(void)
{

// scanner time
//circlefill(display, SCANNER_X, SCANNER_Y, SCANNER_SIZE, 0);
// circle(display, SCANNER_X, SCANNER_Y, SCANNER_SIZE, COL_DGREY + TRANS_BLUE1);
 circlefill(display, SCANNER_X, SCANNER_Y, SCANNER_SIZE + 3, COL_DGREY + TRANS_BLUE1);
 circle(display, SCANNER_X, SCANNER_Y, SCANNER_SIZE + 3, COL_STAR3 + TRANS_BLUE1);
 circlefill(display, SCANNER_X, SCANNER_Y, SCANNER_SIZE + 1, COL_STAR3 + TRANS_BLUE1);
 circlefill(display, SCANNER_X, SCANNER_Y, SCANNER_SIZE, 0);
 putpixel(display, SCANNER_X, SCANNER_Y, COL_WHITE);

 int angle = radians_to_angle(atan2(player[0].y - hs[0].y, player[0].x - hs[0].x)) - player[0].angle + ANGLE_4;
 int dist = (int) hypot(player[0].y - hs[0].y, player[0].x - hs[0].x) >> 16;

 if (hs[0].core_hp > 0)
 {
 if (dist >= SCANNER_SIZE)
 {
//  circle(display, SCANNER_X + xpart(angle, SCANNER_SIZE - 1), SCANNER_Y + ypart(angle, SCANNER_SIZE - 1), 1, COL_DGREY + TRANS_YELLOW4);
  putpixel(display, SCANNER_X + xpart(angle, SCANNER_SIZE - 1), SCANNER_Y + ypart(angle, SCANNER_SIZE - 1), COL_DGREY + TRANS_YELLOW4);
 }
  else
  {
   putpixel(display, SCANNER_X + xpart(angle, dist) + xpart(hs[0].angle + angle, 2), SCANNER_Y + ypart(angle, dist) + ypart(hs[0].angle + angle, 2), COL_DGREY + TRANS_RED2);
   putpixel(display, SCANNER_X + xpart(angle, dist), SCANNER_Y + ypart(angle, dist), COL_DGREY + TRANS_YELLOW4);
  }
 }

 int b, i;

 for (b = 0; b < NO_PBULLETS; b ++)
 {
  if (pbullet[b].type == PBULLET_NONE
   || pbullet[b].type == PBULLET_SHOT
   || pbullet[b].type == PBULLET_MINE_BLAST)
    continue;

 angle = radians_to_angle(atan2(player[0].y - pbullet[b].y, player[0].x - pbullet[b].x)) - player[0].angle + ANGLE_4;
 dist = (int) hypot(player[0].y - pbullet[b].y, player[0].x - pbullet[b].x) >> 16;

 if (dist >= SCANNER_SIZE)
 {
  putpixel(display, SCANNER_X + xpart(angle, SCANNER_SIZE - 1), SCANNER_Y + ypart(angle, SCANNER_SIZE - 1), COL_DGREY + TRANS_BLUE1);
 }
  else
  {
   putpixel(display, SCANNER_X + xpart(angle, dist), SCANNER_Y + ypart(angle, dist), COL_DGREY + TRANS_BLUE3);
  }


 }

// shield time

#define SHIELD_X 10
#define SHIELD_Y 449
#define SHIELD_WIDTH 7

   for (i = 0; i < player[0].max_shield; i ++)
   {
    rect(display, SHIELD_X - 1, SHIELD_Y - i * 28 + 1,
                     SHIELD_X + SHIELD_WIDTH + 1, SHIELD_Y - i * 28 - 24 - 1,
                     TRANS_BLUE2);
   }

   for (i = 0; i < player[0].shield_stock; i ++)
   {
    rectfill(display, SHIELD_X , SHIELD_Y - i * 28,
                     SHIELD_X + SHIELD_WIDTH, SHIELD_Y - i * 28 - 24,
                     TRANS_BLUE3);
   }

   if (player[0].next_shield > 0)
   {
        i = player[0].shield_stock;

       int shield_bar = (player[0].next_shield * 24) / 12000;

    rectfill(display, SHIELD_X , SHIELD_Y - i * 28,
                     SHIELD_X + SHIELD_WIDTH, SHIELD_Y - i * 28 - shield_bar,
                     TRANS_BLUE2);


   }

// charge time

#define CHARGE_X 25
#define CHARGE_Y 450
#define CHARGE_WIDTH 10

 rect(display, CHARGE_X - 1, CHARGE_Y - 102 - 1, CHARGE_X + CHARGE_WIDTH + 1, CHARGE_Y + 1, COL_OUTLINE);
 rect(display, CHARGE_X, CHARGE_Y - 102, CHARGE_X + CHARGE_WIDTH, CHARGE_Y, COL_LGREY);
 rect(display, CHARGE_X + 1, CHARGE_Y - 102 + 1, CHARGE_X + CHARGE_WIDTH - 1, CHARGE_Y - 1, COL_OUTLINE);
 hline(display, CHARGE_X + 1, CHARGE_Y - 20, CHARGE_X + CHARGE_WIDTH - 1, COL_DGREY);

 if (player[0].charge > 0)
 {

 int bar = player[0].charge / 100;
 int col = TRANS_RED2;

 if (player[0].charge > 2000)
  col = TRANS_YELLOW4;

 TRANS_MODE
 rectfill(display, CHARGE_X + 2, CHARGE_Y - bar, CHARGE_X + CHARGE_WIDTH - 2, CHARGE_Y - 2, col);
 END_TRANS
 }

 if (player[0].rocket_burst > 0)
 {
  TRANS_MODE
  if (player[0].weapon == WEAPON_MISSILE)
  {
   for (i = 0; i < player[0].rocket_burst; i ++)
   {
    rectfill(display, CHARGE_X + CHARGE_WIDTH + 5, CHARGE_Y - i * 8,
                     CHARGE_X + CHARGE_WIDTH + 15, CHARGE_Y - i * 8 - 5,
                     TRANS_YELLOW4);
    rect(display, CHARGE_X + CHARGE_WIDTH + 5 - 1, CHARGE_Y - i * 8 + 1,
                     CHARGE_X + CHARGE_WIDTH + 15 + 1, CHARGE_Y - i * 8 - 5 - 1,
                     TRANS_GREEN2);
   }
  }
   else
   {
    for (i = 0; i < player[0].rocket_burst; i ++)
    {
     rectfill(display, CHARGE_X + CHARGE_WIDTH + 5, CHARGE_Y - i * 6,
                     CHARGE_X + CHARGE_WIDTH + 15, CHARGE_Y - i * 6 - 4,
                     TRANS_GREEN3);
    }
   }
  END_TRANS
 }


#define BAR_X 610
#define BAR_Y 365
#define END_WIDTH 2

  hline(display, BAR_X - END_WIDTH, BAR_Y, BAR_X + END_WIDTH, COL_LGREY);
  rect(display, BAR_X - END_WIDTH - 1, BAR_Y - 1, BAR_X + END_WIDTH + 1, BAR_Y + 1, COL_OUTLINE);

  int bar_height = hs[0].core_max_hp >> 8;

  hline(display, BAR_X - END_WIDTH, BAR_Y - bar_height - 4, BAR_X + END_WIDTH, COL_LGREY);
  rect(display, BAR_X - END_WIDTH - 1, BAR_Y - 1 - bar_height - 4, BAR_X + END_WIDTH + 1, BAR_Y + 1 - bar_height - 4, COL_OUTLINE);

  hline(display, BAR_X - END_WIDTH, BAR_Y - (bar_height>>1) - 2, BAR_X + END_WIDTH, COL_DGREY);
  rect(display, BAR_X - END_WIDTH - 1, BAR_Y - 1 - (bar_height>>1) - 2, BAR_X + END_WIDTH + 1, BAR_Y + 1 - (bar_height>>1) - 2, COL_OUTLINE);

  bar_height = hs[0].core_hp >> 8;

 if (hs[0].core_hp > 0)
 {
  int col2 = TRANS_BLUE2;
  if (hs[0].hurt_flash > 0)
   col2 = TRANS_BLUE3;
  if (hs[0].hurt_flash > 1)
   col2 = TRANS_BLUE4;
  if (hs[0].core_hp < hs[0].core_max_hp>>1)
  {
   col2 = TRANS_RED2;
   if (hs[0].hurt_flash > 0)
    col2 = TRANS_RED3;
   if (hs[0].hurt_flash > 1)
    col2 = TRANS_YELLOW4;
  }

//  TRANS_MODE
  vline(display, BAR_X, BAR_Y - 2 - bar_height, BAR_Y - 2, col2);
  vline(display, BAR_X - 1, BAR_Y - 2 - bar_height, BAR_Y - 2, col2);
  vline(display, BAR_X + 1, BAR_Y - 2 - bar_height, BAR_Y - 2, col2);
//  END_TRANS
 }

  char gchar [2];

  if (arena.end_stage > 0
   && arena.game_over == 0)
  {
   textprintf_centre_ex(display, font, 320, 100, -1, -1, "Hellspider");
  gchar [0] = 'a' + hs[0].stage - 2;
  gchar [1] = 0;
   textprintf_centre_ex(display, gfont, 320, 121, -1, -1, gchar);
   textprintf_centre_ex(display, font, 320, 140, -1, -1, "Destroyed");
   textprintf_centre_ex(display, gfont, BAR_X, BAR_Y + 5, -1, -1, gchar);
  }
   else
   {
    gchar [0] = 'a' + hs[0].stage - 1;
    gchar [1] = 0;
    textprintf_centre_ex(display, gfont, BAR_X, BAR_Y + 5, -1, -1, gchar);
   }

  if (arena.game_over > 0)
  {
   textprintf_centre_ex(display, font, 320, 100, -1, -1, "Game");
   textprintf_centre_ex(display, font, 320, 120, -1, -1, "Over");
   if (arena.time_left <= 0)
    textprintf_centre_ex(display, font, 320, 160, -1, -1, "Out of time.");
  }

   int minutes_left = arena.time_left / 3000;
   int seconds_left = (arena.time_left / 50) - (minutes_left * 60);
/*   int centiseconds_left = (arena.time_left) - (minutes_left * 2000) - seconds_left * 33.333;
   centiseconds_left *= 3;
   if (seconds_left < 0)
    seconds_left = 0;
   if (minutes_left < 0)
    minutes_left = 0;
   if (centiseconds_left < 0)
    centiseconds_left = 0;*/

 if (seconds_left < 10)
 {
  textprintf_right_ex(display, font, 630, 10, -1, -1, "%i:0%i", minutes_left, seconds_left);
  textprintf_right_ex(display, font, 630, 10, -1, -1, "%i:0%i", minutes_left, seconds_left);
 }
  else
  {
   textprintf_right_ex(display, font, 630, 10, -1, -1, "%i:%i", minutes_left, seconds_left);
   textprintf_right_ex(display, font, 630, 10, -1, -1, "%i:%i", minutes_left, seconds_left);
  }

 if (arena.end_stage == 0 && arena.game_over == 0
  && minutes_left == 0)
  {
   if (seconds_left > 26 && seconds_left < 31)
    textprintf_centre_ex(display, font, 320, 100, -1, -1, "30 seconds left");
   if (seconds_left > 6 && seconds_left < 11)
    textprintf_centre_ex(display, font, 320, 100, -1, -1, "10 seconds left");
  }

/*  if (centiseconds_left < 10)
  {
   textprintf_right_ex(bmp, font, time_x + 55, time_y + 21, -1, -1, "0%i", centiseconds_left);
  }
    else
    {
     textprintf_right_ex(bmp, font, time_x + 55, time_y + 21, -1, -1, "%i", centiseconds_left);
    }*/

 if (player[0].ships > 0)
 {
  for (i = 0; i < player[0].ships; i ++)
  {
   draw_rle_sprite(display, RLE_player [PLAYER_RLE_MINI], 618 - i * 14, 25);
  }
 }

}







void draw_stars(char motion)
{
 int i;

 for (i = 0; i < NO_STARS; i ++)
 {

/*  if (star[i].bright > 4)
  {
   putpixel(display, (star[i].x >> 10) - 1, (star[i].y >> 10), star[i].bright2-2);
   putpixel(display, (star[i].x >> 10) + 1, (star[i].y >> 10), star[i].bright2-2);
   putpixel(display, (star[i].x >> 10), (star[i].y >> 10) - 1, star[i].bright2-2);
   putpixel(display, (star[i].x >> 10), (star[i].y >> 10) + 1, star[i].bright2-2);
  }*/
  if (motion)
   pline(display, star[i].x >> 10, star[i].y >> 10, star[i].old_x >> 10, star[i].old_y >> 10, star[i].bright2);
  putpixel(display, star[i].x >> 10, star[i].y >> 10, star[i].bright);

//  putpixel(display, (star[i].x >> 11) + 320, (star[i].y >> 11) + 240, star[i].bright);

 }

}


void draw_hs(int p)
{

    if (hs[0].alive == 0)
     return;

// this will need to be much looser, to account for large hspiders. Can run a tighter one for the core and each part.
   if (hs[0].x < player[p].x - (900 << 10) || hs[0].x > player[p].x + (900 << 10)
    || hs[0].y < player[p].y - (900 << 10) || hs[0].y > player[p].y + (700 << 10))
     return;

   float angle = atan2(hs[0].y - player[p].y, hs[0].x - player[p].x) - (PI/2) - angle_to_radians(player[p].angle);
//   float angle = angle_to_radians(player[p].angle_to_hs) - (PI/2) - angle_to_radians(player[p].angle);
   int dist = hypot(hs[0].y - player[p].y, hs[0].x - player[p].x);
   int x = player[p].camera_x + fxpart(angle, dist >> 10);
   int y = player[p].camera_y + fypart(angle, dist >> 10);

   float angle_draw = hs[0].angle - angle_to_radians(player[p].angle + ANGLE_4);

   int i, pt;

   int x2, y2, old_x, old_y, x3, y3, size, size2;

   char hurt = 0;

   float part_angle;
   float part_up_angle;
   int part_angle_int;


   for (pt = 0; pt < NO_PARTS; pt ++)
   {
     if (part[pt].type == PART_NONE)
      continue;
     old_x = x;
     old_y = y;
     part_angle = angle_draw;

      part_angle = angle_draw + part[pt].position_angle [hs[0].position];
      part_up_angle = angle_draw + part[pt].position_up_angle [hs[0].position];

      if (part[pt].hurt_flash)
       hurt = 1;
        else
         hurt = 0;

      x2 = x + fxpart(part_angle, part[pt].position_dist [hs[0].position] >> 10);
      y2 = y + fypart(part_angle, part[pt].position_dist [hs[0].position] >> 10);

#define HCOL COL_HS_INNER_TRANS

      if (x2 >  (400 << 10)*-1 && x2 < (400 << 10)
       && y2 > (400 << 10)*-1 && y2 < (300 << 10))
       {
        switch(part[pt].type)
        {

          case PART_SHIELD:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           draw_circle_out(x3, y3, part[pt].size);
           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].spin_angle - player[p].angle - ANGLE_4;
           for (i = 0; i < 2; i ++)
           {
            size = part[pt].size >> 1;
            x3 = x2 + xpart(part_angle_int + i * ANGLE_2, part[pt].size - 3);
            y3 = y2 + ypart(part_angle_int + i * ANGLE_2, part[pt].size - 3);
            circlefill(display, x3, y3, 2, HCOL);
            circle(display, x3, y3, 2, COL_HS_INNER_LINE);
            // these need to be made trans so they don't overwrite other trans
            if (part[pt].which_fired == i && part[pt].fired > 0)
             {
             size = part[pt].fired;
             if (size > 0)
             {
              ccircle2(x3, y3, size, 0);
              size >>= 1;
              ccircle(x3, y3, size, 0);
             }
            }
           }
           if (part[pt].shield > 0) // can be negative
           {
            size = part[pt].size + (part[pt].shield >> 10);
            if (size >= RLE_SCIRCLES)
            size = RLE_SCIRCLES - 1;
            //draw_trans_rle_sprite(display, RLE_scircle [grand(4)] [size], x2 - size - 1, y2 - size - 1);
            size2 = part[pt].shield_flash >> 1;
            if (size2 > 2)
             size2 = 2;
             // note: need trans_mode to be set for draw_shield_circle
            do_circle(display, x2, y2, size, size2, draw_shield_circle);
            if (part[pt].shield_flash > 0)
            {
             switch(part[pt].shield_flash >> 1)
             {
              default: circlefill(display, x2, y2, size, TRANS_YELLOW4); break;
              case 0: circlefill(display, x2, y2, size, TRANS_GREEN1); break;
              case 1: circlefill(display, x2, y2, size, TRANS_GREEN2); break;
              case 2: circlefill(display, x2, y2, size, TRANS_GREEN3); break;
             }
            }
           }
           END_TRANS
           break;
          case PART_ROTARY:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
//           x3 = x2 - fxpart(part_up_angle, (part[pt].up_dist >> 12) + (part[pt].up_dist >> 12));
//           y3 = y2 - fypart(part_up_angle, (part[pt].up_dist >> 12) + (part[pt].up_dist >> 12));
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           draw_circle_out(x3, y3, part[pt].size);
           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].spin_angle - player[p].angle - ANGLE_4;
           for (i = 0; i < 4; i ++)
           {
            size = part[pt].size >> 1;
            x3 = x2 + xpart(part_angle_int + i * ANGLE_4, part[pt].size - 3);
            y3 = y2 + ypart(part_angle_int + i * ANGLE_4, part[pt].size - 3);
            circlefill(display, x3, y3, 2, HCOL);
            circle(display, x3, y3, 2, COL_HS_INNER_LINE);
            // these need to be made trans so they don't overwrite other trans
            if (part[pt].which_fired == i && part[pt].fired > 0)
             {
             size = part[pt].fired;
             if (size > 0)
             {
              ccircle2(x3, y3, size, 0);
              size >>= 1;
              ccircle(x3, y3, size, 0);
             }
            }
           }
           END_TRANS
           break;
          case PART_MGUN:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           draw_circle_out(x3, y3, part[pt].size);
           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           size = part[pt].size >> 1;
//           x3 = x2 + xpart(part_angle_int, part[pt].size - 3) + xpart(part_angle_int + ANGLE_4, size);
//           y3 = y2 + ypart(part_angle_int, part[pt].size - 3) + ypart(part_angle_int + ANGLE_4, size);
           x3 = x2 + xpart(part_angle_int + ANGLE_8, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int + ANGLE_8, part[pt].size - 3);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
           TRANS_MODE
           size = part[pt].size >> 1;
           x3 = x2 + xpart(part_angle_int - ANGLE_8, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int - ANGLE_8, part[pt].size - 3);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
           break;
          case PART_BASIC:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
//           part_angle_int = part[pt].angle + radians_to_angle(angle_draw);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
//           pline(display, x3, y3, x2 + xpart(part_angle_int, 235), y2 + ypart(part_angle_int, 235), COL_HS_INNER_LINE);

           break;
          case PART_ROCKET:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
//           part_angle_int = part[pt].angle + radians_to_angle(angle_draw);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 - xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 4, HCOL);
           circle(display, x3, y3, 4, COL_HS_INNER_LINE);

           x3 = x2 - xpart(part_angle_int, part[pt].size - 1);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 1);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 0);
             size >>= 1;
             ccircle(x3, y3, size, 0);
            }
           }
//           pline(display, x3, y3, x2 + xpart(part_angle_int, 235), y2 + ypart(part_angle_int, 235), COL_HS_INNER_LINE);

           break;
          case PART_FORWARD:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
//           part_angle_int = part[pt].angle + radians_to_angle(angle_draw);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 1;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
//           pline(display, x3, y3, x2 + xpart(part_angle_int, 235), y2 + ypart(part_angle_int, 235), COL_HS_INNER_LINE);

           break;
          case PART_MISSILE:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);
//           circlefill(display, x3, y3, COL_WHITE, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 4, HCOL);
           circle(display, x3, y3, 4, COL_HS_INNER_LINE);
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 0);
             size >>= 1;
             ccircle(x3, y3, size, 0);
            }
           }
           x3 = x2 - xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 1;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 0);
             size >>= 1;
             ccircle(x3, y3, size, 0);
            }
           }
//           textprintf_ex(display, font, x + 120, y, -1, -1, "%i, %i", part[pt].recycle, hs[0].firing_time);
           break;
          case PART_3CANNON:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);
//           circlefill(display, x3, y3, COL_WHITE, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int + ANGLE_4, part[pt].size - 4);
           y3 = y2 + ypart(part_angle_int + ANGLE_4, part[pt].size - 4);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           x3 = x2 + xpart(part_angle_int - ANGLE_4, part[pt].size - 4);
           y3 = y2 + ypart(part_angle_int - ANGLE_4, part[pt].size - 4);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);

           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 4, COL_HS_INNER_TRANS);
           circle(display, x3, y3, 4, COL_HS_INNER_LINE);
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 1);
             size >>= 1;
             ccircle(x3, y3, size, 1);
            }
           }
           END_TRANS
           break;
          case PART_FLAK:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);
//           circlefill(display, x3, y3, COL_WHITE, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 4, HCOL);
           circle(display, x3, y3, 4, COL_HS_INNER_LINE);
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 0);
             size >>= 1;
             ccircle(x3, y3, size, 0);
            }
           }
           x3 = x2 - xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 1;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 0);
             size >>= 1;
             ccircle(x3, y3, size, 0);
            }
           }
//           textprintf_ex(display, font, x + 120, y, -1, -1, "%i, %i", part[pt].recycle, hs[0].firing_time);
           break;
          case PART_SEEKER:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           draw_circle_out(x3, y3, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 2, HCOL);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 1;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
           x3 = x2 - xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 4, HCOL);
           circle(display, x3, y3, 4, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
//           textprintf_ex(display, font, x + 120, y, -1, -1, "%i, %i", part[pt].recycle, hs[0].firing_time);
           break;
          case PART_EEL:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           draw_circle_out(x3, y3, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 5, HCOL);
           circle(display, x3, y3, 5, COL_HS_INNER_LINE);
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 1;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
           x3 = x2 - xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 3, HCOL);
           circle(display, x3, y3, 3, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
//           textprintf_ex(display, font, x + 120, y, -1, -1, "%i, %i", part[pt].recycle, hs[0].firing_time);
           break;
          case PART_BEAM:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);
//           circlefill(display, x3, y3, COL_WHITE, part[pt].size);
           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 3, HCOL);
           circle(display, x3, y3, 3, COL_HS_INNER_LINE);
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 0);
             size >>= 1;
             ccircle(x3, y3, size, 0);
            }
           }
           x3 = x2 - xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 3, HCOL);
           circle(display, x3, y3, 3, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
//           textprintf_ex(display, font, x + 120, y, -1, -1, "%i, %i", part[pt].recycle, hs[0].firing_time);
           break;
          case PART_BEAM2:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);
//           circlefill(display, x3, y3, COL_WHITE, part[pt].size);
           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 3, HCOL);
           circle(display, x3, y3, 3, COL_HS_INNER_LINE);
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
           x3 = x2 - xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 - ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 3, HCOL);
           circle(display, x3, y3, 3, COL_HS_INNER_LINE);
           END_TRANS
           // these need to be made trans so they don't overwrite other trans
//           textprintf_ex(display, font, x + 120, y, -1, -1, "%i, %i", part[pt].recycle, hs[0].firing_time);
           break;
          case PART_DRIVE:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 5, HCOL);
           circle(display, x3, y3, 5, COL_HS_INNER_LINE);
           END_TRANS
           if (part[pt].drive > 0)
           {
            size = part[pt].drive >> 4;
            if (size > 0)
            {
             size += 2 + grand(3);
             x3 += xpart(part_angle_int, 7 + size);
             y3 += ypart(part_angle_int, 7 + size);
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size + grand(3), 2);
             x3 += xpart(part_angle_int, 5 + size);
             y3 += ypart(part_angle_int, 5 + size);
             ccircle2(x3, y3, size + grand(3), 2);
             size -= 3;
             x3 += xpart(part_angle_int, 5 + size);
             y3 += ypart(part_angle_int, 5 + size);
             ccircle2(x3, y3, size + grand(3), 2);
            }
           }
//           textprintf_ex(display, font, x3, y3, -1, -1, "%i", part[pt].size);
//           textprintf_ex(display, font, x3, y3, -1, -1, "%i, %i, %i", (part[pt].angle - radians_to_angle(hs[0].angle)) & ANGLE_MASK, radians_to_angle(atan2(part[p].y - hs[0].centre_y, part[p].x - hs[0].centre_x)), ypart((radians_to_angle(atan2(part[p].y - hs[0].centre_y, part[p].x - hs[0].centre_x)) - part[p].angle) & ANGLE_MASK, ANGLE_1));
//textprintf_ex(display, font, x3, y3, -1, -1, "%i", ((part[pt].angle - radians_to_angle(hs[0].angle) + ANGLE_4) & ANGLE_MASK));


//  lever_angle = radians_to_angle(atan2(part[p].y - hs[h].centre_y, part[p].x - hs[h].centre_x));
  //torque = ypart(lever_angle - part[p].angle, 10000);

//         print_number(x2 - 15, y2, part[pt].drive_turn);

           break;
          case PART_DRIVE2:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].size + (part[pt].size >> 1));
           y3 = y2 - fypart(part_up_angle, part[pt].size + (part[pt].size >> 1));
//           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);

           TRANS_MODE
           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 5, HCOL);
           circle(display, x3, y3, 5, COL_HS_INNER_LINE);
           END_TRANS
           if (part[pt].drive > 0)
           {
            size = part[pt].drive >> 4;
            if (size > 0)
            {
             size += 2 + grand(3);
             x3 += xpart(part_angle_int, 7 + size);
             y3 += ypart(part_angle_int, 7 + size);
             ccircle2(x3, y3, size, 0);
             size >>= 1;
             ccircle(x3, y3, size + grand(3), 0);
             x3 += xpart(part_angle_int, 5 + size);
             y3 += ypart(part_angle_int, 5 + size);
             ccircle2(x3, y3, size + grand(3), 0);
             size -= 3;
             x3 += xpart(part_angle_int, 5 + size);
             y3 += ypart(part_angle_int, 5 + size);
             ccircle2(x3, y3, size + grand(3), 0);
            }
           }
           break;
        }
// textprintf_ex(display, font, x2, y2, -1, -1, "%i %i %i", pt, part[pt].index, part[pt].mirror_part);
// textprintf_ex(display, font, x2, y2, -1, -1, "%f, %i", part[pt].fdebug, part[pt].debug);
// textprintf_ex(display, font, x2, y2, -1, -1, "%i", part[pt].debug);
// textprintf_ex(display, font, x2, y2, -1, -1, "%i %i", pt, part[pt].drive_turn);
// pline(display, x2, y2, x2 + xpart(part[pt].debug, 30), y2 + ypart(part[pt].debug, 30), COL_WHITE);
//if (part[pt].type == PART_BEAM2)
// textprintf_ex(display, font, x2, y2, -1, -1, "%i %i %i %f %f", pt, part[pt].tracks, part[pt].angle, part[pt].up_angle, part[pt].locked_angle);
//if (part[pt].range_hits > 0)
//  textprintf_ex(display, font, x2, y2, -1, -1, "%i", (int) part[pt].total_direction / part[pt].range_hits);
//if (hs[0].firing == part[pt].weapon)
  //textprintf_ex(display, font, x2, y2, -1, -1, "%i",  part[pt].recycle);

       }
       old_x = x2;
      old_y = y2;


   }
/*
   for (i = 0; i < 2; i ++)
   {
    for (j = 0; j < 4; j ++)
    {
     pt = hs[0].arm_part [i] [j];
     old_x = x;
     old_y = y;
     part_angle = angle_draw;
     while (pt != -1)
     {
      part_angle = angle_draw + part[pt].position_angle [hs[0].position];
      part_up_angle = angle_draw + part[pt].position_up_angle [hs[0].position];

      if (part[pt].hurt_flash)
       hurt = 1;
        else
         hurt = 0;

      x2 = x + fxpart(part_angle, part[pt].position_dist [hs[0].position] >> 10);
      y2 = y + fypart(part_angle, part[pt].position_dist [hs[0].position] >> 10);

      if (x2 >  (400 << 10)*-1 && x2 < (400 << 10)
       && y2 > (400 << 10)*-1 && y2 < (300 << 10))
       {
        switch(part[pt].type)
        {
          case PART_BASIC:
          case PART_FORWARD:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].up_dist >> 11);
           y3 = y2 - fypart(part_up_angle, part[pt].up_dist >> 11);
           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);

           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
//           part_angle_int = part[pt].angle + radians_to_angle(angle_draw);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 2, COL_HS_INNER);
           circle(display, x3, y3, 2, COL_HS_INNER_LINE);
           // these need to be made trans so they don't overwrite other trans
           if (part[pt].fired > 0)
           {
            size = part[pt].fired >> 2;
            if (size > 0)
            {
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size, 2);
            }
           }
//           pline(display, x3, y3, x2 + xpart(part_angle_int, 235), y2 + ypart(part_angle_int, 235), COL_HS_INNER_LINE);

           break;
          case PART_DRIVE:
           draw_circle_in(x2, y2, part[pt].size, hurt);
           draw_circle_out(x2, y2, part[pt].size + 4);
           x3 = x2 - fxpart(part_up_angle, part[pt].up_dist >> 11);
           y3 = y2 - fypart(part_up_angle, part[pt].up_dist >> 11);
           draw_circle_in(x3, y3, part[pt].size - 4, 0);
           draw_circle_out(x3, y3, part[pt].size);

           circle(display, x2, y2, part[pt].size - 3, COL_HS_INNER_LINE);
           part_angle_int = part[pt].angle - player[p].angle - ANGLE_4;
           x3 = x2 + xpart(part_angle_int, part[pt].size - 3);
           y3 = y2 + ypart(part_angle_int, part[pt].size - 3);
           circlefill(display, x3, y3, 5, COL_HS_INNER);
           circle(display, x3, y3, 5, COL_HS_INNER_LINE);
           if (part[pt].drive > 0)
           {
            size = part[pt].drive >> 4;
            if (size > 0)
            {
             size += 2 + grand(3);
             x3 += xpart(part_angle_int, 7 + size);
             y3 += ypart(part_angle_int, 7 + size);
             ccircle2(x3, y3, size, 2);
             size >>= 1;
             ccircle(x3, y3, size + grand(3), 2);
             x3 += xpart(part_angle_int, 5 + size);
             y3 += ypart(part_angle_int, 5 + size);
             ccircle2(x3, y3, size + grand(3), 2);
             size -= 3;
             x3 += xpart(part_angle_int, 5 + size);
             y3 += ypart(part_angle_int, 5 + size);
             ccircle2(x3, y3, size + grand(3), 2);
            }
           }


//         print_number(x2 - 15, y2, part[pt].drive_turn);
//  textprintf_ex(display, font, x3, y3, -1, -1, "%f", part[pt].locked_angle);

           break;
        }

       }
       old_x = x2;
      old_y = y2;
      pt = part[pt].downlink;

     };
    }
   }
*/
TRANS_MODE
/*
    draw_circle_in(x, y, 11);
    draw_circle_out(x, y, 15);

    draw_circle_in(x + fxpart(angle_draw, 7), y + fypart(angle_draw, 7), 9);
    draw_circle_out(x + fxpart(angle_draw, 8), y + fypart(angle_draw, 8), 13);
*/

    if (hs[0].hurt_flash)
     hurt = 1;
      else
       hurt = 0;


    for (i = 0; i < NO_CORE_CIRCLES; i ++)
    {
     if (hs[0].core_circle_size [i] == 0)
      continue;

     x2 = x + fxpart(angle_draw + hs[0].core_circle_angle [i], hs[0].core_circle_dist [i]);
     y2 = y + fypart(angle_draw + hs[0].core_circle_angle [i], hs[0].core_circle_dist [i]);
     draw_circle_in(x2, y2, hs[0].core_circle_size [i], hurt);

     x2 = x + fxpart(angle_draw + hs[0].core_circle_angle [i], hs[0].core_circle_dist2 [i]);
     y2 = y + fypart(angle_draw + hs[0].core_circle_angle [i], hs[0].core_circle_dist2 [i]);
     draw_circle_out(x2, y2, hs[0].core_circle_size [i] + 4);

    }

#define HCOL2 COL_HS_INNER_TRANS

    x2 = x + fxpart(angle_draw + PI_2 + PI_8, 7);
    y2 = y + fypart(angle_draw + PI_2 + PI_8, 7);
    circlefill(display, x2, y2, 3, HCOL2);
    circle(display, x2, y2, 3, COL_HS_INNER_LINE);

    x2 = x + fxpart(angle_draw - PI_2 - PI_8, 7);
    y2 = y + fypart(angle_draw - PI_2 - PI_8, 7);
    circlefill(display, x2, y2, 3, HCOL2);
    circle(display, x2, y2, 3, COL_HS_INNER_LINE);

    x2 = x + fxpart(angle_draw + PI_2 - PI_4, 11);
    y2 = y + fypart(angle_draw + PI_2 - PI_4, 11);
    circlefill(display, x2, y2, 3, HCOL2);
    circle(display, x2, y2, 3, COL_HS_INNER_LINE);

    x2 = x + fxpart(angle_draw - PI_2 + PI_4, 11);
    y2 = y + fypart(angle_draw - PI_2 + PI_4, 11);
    circlefill(display, x2, y2, 3, HCOL2);
    circle(display, x2, y2, 3, COL_HS_INNER_LINE);

    x2 = x + fxpart(angle_draw + PI_16, 15);
    y2 = y + fypart(angle_draw + PI_16, 15);
    circlefill(display, x2, y2, 2, HCOL2);
    circle(display, x2, y2, 2, COL_HS_INNER_LINE);

    x2 = x + fxpart(angle_draw - PI_16, 15);
    y2 = y + fypart(angle_draw - PI_16, 15);
    circlefill(display, x2, y2, 2, HCOL2);
    circle(display, x2, y2, 2, COL_HS_INNER_LINE);

    x2 = x + fxpart(angle_draw + PI + PI_16 + PI_32, 13);
    y2 = y + fypart(angle_draw + PI + PI_16 + PI_32, 13);
    circlefill(display, x2, y2, 2, HCOL2);
    circle(display, x2, y2, 2, COL_HS_INNER_LINE);

    x2 = x + fxpart(angle_draw - PI - PI_16 - PI_32, 13);
    y2 = y + fypart(angle_draw - PI - PI_16 - PI_32, 13);
    circlefill(display, x2, y2, 2, HCOL2);
    circle(display, x2, y2, 2, COL_HS_INNER_LINE);

//    draw_circle_in(x + fxpart(angle_draw + (PI/2), 7), y + fypart(angle_draw + (PI/2), 7), 9);
//    draw_circle_out(x + fxpart(angle_draw + (PI/2), 8), y + fypart(angle_draw + (PI/2), 8), 13);


/*    circlefill(display, x, y, 14, COL_HS_OUTER);
    circlefill(display, x, y, 15, COL_HS_OUTER_EDGE);

    circlefill(display, x + fxpart(angle_draw, 8), y + fypart(angle_draw, 8), 12, COL_HS_OUTER);
    circlefill(display, x + fxpart(angle_draw, 8), y + fypart(angle_draw, 8), 13, COL_HS_OUTER_EDGE);

    circlefill(display, x, y, 10, COL_HS_INNER);
    circlefill(display, x, y, 11, COL_HS_INNER_EDGE);

    circlefill(display, x + fxpart(angle_draw, 7), y + fypart(angle_draw, 7), 8, COL_HS_INNER);
    circlefill(display, x + fxpart(angle_draw, 7), y + fypart(angle_draw, 7), 9, COL_HS_INNER_EDGE);
*/
END_TRANS

//#define SHOW_CENTRE_OF_MASS

#ifdef SHOW_CENTRE_OF_MASS

   angle = atan2(hs[0].centre_y - player[p].y, hs[0].centre_x - player[p].x) - (PI/2) - angle_to_radians(player[p].angle);
   dist = hypot(hs[0].centre_y - player[p].y, hs[0].centre_x - player[p].x);
   x = player[p].camera_x + cos(angle) * (dist >> 10);
   y = player[p].camera_y + sin(angle) * (dist >> 10);

   hline(display, x - 5, y, x + 5, COL_WHITE);
   vline(display, x, y - 5, y + 5, COL_WHITE);

#endif

}



void draw_pbullets(int p)
{

int b, size, i;

for (b = 0; b < NO_PBULLETS; b ++)
{

   if (pbullet[b].type == PBULLET_NONE)
    continue;

   if (pbullet[b].x < player[p].x - (600 << 10) || pbullet[b].x > player[p].x + (600 << 10)
    || pbullet[b].y < player[p].y - (600 << 10) || pbullet[b].y > player[p].y + (500 << 10))
     continue;

   float angle = atan2(pbullet[b].y - player[p].y, pbullet[b].x - player[p].x) - (PI/2) - angle_to_radians(player[p].angle);
   int dist = hypot(pbullet[b].y - player[p].y, pbullet[b].x - player[p].x);
   int x = player[p].camera_x + cos(angle) * (dist >> 10);
   int y = player[p].camera_y + sin(angle) * (dist >> 10);

//   float angle_draw = pbullet[b].angle - angle_to_radians(player[p].angle + ANGLE_4);
   float angle_draw =  angle_to_radians(pbullet[b].angle - player[p].angle - ANGLE_4);

 switch(pbullet[b].type)
 {
    case PBULLET_WORM:
    case PBULLET_ROCKET:
    case PBULLET_MISSILE:
     ccircle(x, y, pbullet[b].draw_size + grand(2), pbullet[b].colour);
     ccircle2(x, y, pbullet[b].draw_size + 2 + grand(2), pbullet[b].colour);
     break;
    case PBULLET_SCATTER:
     ccircle(x, y, (pbullet[b].status>>2) + grand(2), pbullet[b].colour);
     break;

    case PBULLET_SHOT:
//     x += grand(2) - grand(2);
//     y += grand(2) - grand(2);
     ccircle(x, y, pbullet[b].status + grand(2), pbullet[b].colour);

//     if (pbullet[b].time < 20)
//      ccircle2(x, y, 11 - (pbullet[b].time >> 1), pbullet[b].colour);
     if (pbullet[b].time < 16)
     {
      if (player[0].upgrade [U_POWER] > 1)
       ccircle2(x, y, 9 - (pbullet[b].time >> 1), 2);
        else
         ccircle2(x, y, 9 - (pbullet[b].time >> 1), 0);
     }

     size = pbullet[b].status - 1 + grand(2);
     x -= fxpart(angle_draw, size+1);
     y -= fypart(angle_draw, size+1);
     ccircle2(x, y, size, pbullet[b].colour);

/*     size = 2 + grand(2);
     x -= xpart(ebullet[b].angle, size);
     y -= ypart(ebullet[b].angle, size);
     ccircle2(bmp, x, y, size, ebullet[b].colour);*/
     break;
    case PBULLET_BASIC:
    break;
     ccircle(x, y, pbullet[b].draw_size + grand(3), pbullet[b].colour);
     ccircle2(x, y, pbullet[b].draw_size + 4 + grand(3), pbullet[b].colour);
//     if (pbullet[b].time < 16)
//      ccircle2(x, y, 16 - (pbullet[b].time), pbullet[b].colour);

/*     size = 2 + grand(2);
     x -= fxpart(angle_draw, size+1);
     y -= fypart(angle_draw, size+1);
     ccircle2(x, y, size, pbullet[b].colour);*/
     break;
    case PBULLET_MINE:
     ccircle(x, y, pbullet[b].draw_size + grand(3), pbullet[b].colour);
     ccircle2(x, y, pbullet[b].draw_size + 4 + grand(3), pbullet[b].colour);
     for (i = 0; i < 6; i ++)
     {
      angle = pbullet[b].angle - player[p].angle - ANGLE_4 + (i*ANGLE_6);
      ccircle(x + xpart(angle, pbullet[b].draw_size-1),
              y + ypart(angle, pbullet[b].draw_size-1),
              4, pbullet[b].colour);
      angle = (pbullet[b].angle*-1) - player[p].angle - ANGLE_4 + (i*ANGLE_6);
      ccircle(x + xpart(angle, pbullet[b].draw_size-1),
              y + ypart(angle, pbullet[b].draw_size-1),
              4, pbullet[b].colour);
     }
/*     angle = pbullet[b].angle - player[p].angle - ANGLE_4;
     for (i = 0; i < MINE_BITS; i ++)
     {
      size = 1;
      if (pbullet[b].bit_time [i] > 6 && pbullet[b].bit_time [i] < 34)
       size = 2;
      if (pbullet[b].bit_time [i] > 12 && pbullet[b].bit_time [i] < 28)
       size = 3 + grand(2);
      if (pbullet[b].bit_time [i] > 18 && pbullet[b].bit_time [i] < 22)
       size = 3 + grand(3);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);

      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_3, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_3, pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);


      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_3, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_3, pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
     }*/
     break;
    case PBULLET_MINE_BLAST:
     angle = pbullet[b].angle - player[p].angle - ANGLE_4;
     for (i = 0; i < MINE_BITS; i ++)
     {
      if (pbullet[b].bit_time [i] <= 0
       || pbullet[b].bit_time [i] > 50)
       continue;
      size = pbullet[b].bit_time [i];
      if (size > 25)
       size = 50 - size;
      size >>= 2;
      if (size > 8)
       size = 8;
/*      ccircle(x + xpart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);
      ccircle(x - xpart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              y - ypart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);*/
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i], pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);

      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_3, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_3, pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] + ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] + ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);


      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_3, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_3, pbullet[b].bit_dist [i]),
              size, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_3 + ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
      ccircle(x + xpart(angle + pbullet[b].bit_angle [i] - ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              y + ypart(angle + pbullet[b].bit_angle [i] - ANGLE_3 - ANGLE_64, pbullet[b].bit_dist [i]),
              size-1, pbullet[b].colour);
     }
     break;

 }
}
}



void draw_ebullets(int p)
{

int b, size, x2, y2, x, y, x3, y3, dist, size2, i, visible, side, beam_power;
float angle, angle_draw, angle2;

for (b = 0; b < NO_EBULLETS; b ++)
{

   if (ebullet[b].type == EBULLET_NONE)
    continue;

   if ((ebullet[b].x < player[p].x - (600 << 10) || ebullet[b].x > player[p].x + (600 << 10)
    || ebullet[b].y < player[p].y - (600 << 10) || ebullet[b].y > player[p].y + (600 << 10))
    && ebullet[b].type != EBULLET_BEAM
    && ebullet[b].type != EBULLET_BEAM2)
     continue;

   angle = atan2(ebullet[b].y - player[p].y, ebullet[b].x - player[p].x) - (PI/2) - angle_to_radians(player[p].angle);
   dist = hypot(ebullet[b].y - player[p].y, ebullet[b].x - player[p].x);
   x = player[p].camera_x + cos(angle) * (dist >> 10);
   y = player[p].camera_y + sin(angle) * (dist >> 10);

//   float angle_draw = ebullet[b].angle - angle_to_radians(player[p].angle + ANGLE_4);
   angle_draw =  angle_to_radians(ebullet[b].angle - player[p].angle - ANGLE_4);

 switch(ebullet[b].type)
 {
    case EBULLET_BEAM:
     i = 0;
     x2 = x;
     y2 = y;
     fxpart(angle_draw, ebullet[b].status2);
     fypart(angle_draw, ebullet[b].status2);
     visible = 0;

     beam_power = ebullet[b].status >> 1;
     if (ebullet[b].status > 50)
      beam_power = (70 - ebullet[b].status) >> 1;
     if (beam_power > 10 + ebullet[b].status3)
      beam_power = 10 + ebullet[b].status3;


     if (x2 > -20 && x2 < 660 && y2 > -20 && y2 < 500)
     {
      ccircle(x2, y2, (beam_power + grand(3)), ebullet[b].colour);
     }

     do
     {
       if (x2 > -50 && x2 < 690 && y2 > -50 && y2 < 530)
       {
        if (visible == 0)
        {
         x3 = x2;
         y3 = y2;
         visible = 1; // has just become visible.
        }
       }
        else
        {
         if (visible == 1)
          break; // must have been on screen for a while then left.
        }
       x2 = x + fxpart(angle_draw, ebullet[b].status2 * i);
       y2 = y + fypart(angle_draw, ebullet[b].status2 * i);
       i ++;
     } while (i < ebullet[b].status);

     if (i > ebullet[b].status - 1)
     {
      ccircle(x2, y2, (beam_power + grand(3)), ebullet[b].colour);
     }

     if (visible)
     {
//      TRANS_MODE

      side = beam_power;
      TRANS_MODE
      poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_RED1);
      END_TRANS

      beam_power = (ebullet[b].status - 10) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (65 - ebullet[b].status) >> 1;
      if (beam_power > 7 + ebullet[b].status3)
       beam_power = 7 + ebullet[b].status3;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_RED2);
       END_TRANS
      }

      beam_power = (ebullet[b].status - 20) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (60 - ebullet[b].status) >> 1;
      if (beam_power > 5 + ebullet[b].status3)
       beam_power = 5 + ebullet[b].status3;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_RED3);
       END_TRANS
      }

      beam_power = (ebullet[b].status - 40) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (56 - ebullet[b].status) >> 1;
      if (beam_power > 3 + ebullet[b].status3)
       beam_power = 3 + ebullet[b].status3;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_YELLOW4);
       END_TRANS

      }

//      line(display, x2, y2, x3, y3, COL_WHITE);
//      pline(display, x2, y2, x3, y3, TRANS_RED3);
//      END_TRANS
     }
     break;
    case EBULLET_BEAM2:
     i = 0;
     x2 = x;
     y2 = y;
     fxpart(angle_draw, ebullet[b].status2);
     fypart(angle_draw, ebullet[b].status2);
     visible = 0;

     beam_power = ebullet[b].status >> 1;
     if (ebullet[b].status > 50)
      beam_power = (70 - ebullet[b].status);
     if (beam_power > 11 + ebullet[b].status3)
      beam_power = 11 + ebullet[b].status3;


     if (x2 > -20 && x2 < 660 && y2 > -20 && y2 < 500)
     {
      ccircle(x2, y2, (beam_power + grand(3)), ebullet[b].colour);
     }

     do
     {
       if (x2 > -50 && x2 < 690 && y2 > -50 && y2 < 530)
       {
        if (visible == 0)
        {
         x3 = x2;
         y3 = y2;
         visible = 1; // has just become visible.
        }
       }
        else
        {
         if (visible == 1)
          break; // must have been on screen for a while then left.
        }
       x2 = x + fxpart(angle_draw, ebullet[b].status2 * i);
       y2 = y + fypart(angle_draw, ebullet[b].status2 * i);
       i ++;
     } while (i < ebullet[b].status);

     if (i > ebullet[b].status - 1)
     {
      ccircle(x2, y2, (beam_power + grand(3)), ebullet[b].colour);
     }

     if (visible)
     {
//      TRANS_MODE

      side = beam_power;
      TRANS_MODE
      poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE1);
      END_TRANS

      beam_power = (ebullet[b].status - 10) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (65 - ebullet[b].status) >> 1;
      if (beam_power > 9 + ebullet[b].status3)
       beam_power = 9 + ebullet[b].status3;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE2);
       END_TRANS
      }

      beam_power = (ebullet[b].status - 20) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (60 - ebullet[b].status) >> 1;
      if (beam_power > 7 + ebullet[b].status3)
       beam_power = 7 + ebullet[b].status3;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE3);
       END_TRANS
      }

      beam_power = (ebullet[b].status - 40) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (56 - ebullet[b].status) >> 1;
      if (beam_power > 5 + ebullet[b].status3)
       beam_power = 5 + ebullet[b].status3;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE4);
       END_TRANS

      }

//      line(display, x2, y2, x3, y3, COL_WHITE);
//      pline(display, x2, y2, x3, y3, TRANS_RED3);
//      END_TRANS
     }
     break;
/*    case EBULLET_BEAM2:
     i = 0;
     x2 = x;
     y2 = y;
     fxpart(angle_draw, ebullet[b].status2);
     fypart(angle_draw, ebullet[b].status2);
     visible = 0;

     beam_power = ebullet[b].status >> 1;
     if (beam_power > 20)
      beam_power = 20;
     if (ebullet[b].status > 50)
      beam_power = (70 - ebullet[b].status);


     if (x2 > -20 && x2 < 660 && y2 > -20 && y2 < 500)
     {
      ccircle(x2, y2, (beam_power + grand(3)), ebullet[b].colour);
     }

     do
     {
       if (x2 > -50 && x2 < 690 && y2 > -50 && y2 < 530)
       {
        if (visible == 0)
        {
         x3 = x2;
         y3 = y2;
         visible = 1; // has just become visible.
        }
       }
        else
        {
         if (visible == 1)
          break; // must have been on screen for a while then left.
        }
       x2 = x + fxpart(angle_draw, ebullet[b].status2 * i);
       y2 = y + fypart(angle_draw, ebullet[b].status2 * i);
       i ++;
     } while (i < ebullet[b].status);

     if (i > ebullet[b].status - 1)
     {
      ccircle(x2, y2, (beam_power + grand(3)), ebullet[b].colour);
     }

     if (visible)
     {
//      TRANS_MODE

      side = beam_power;
      TRANS_MODE
      poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE1);
      END_TRANS

      beam_power = (ebullet[b].status - 10) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (65 - ebullet[b].status) >> 1;
      if (beam_power > 13)
       beam_power = 13;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE2);
       END_TRANS
      }

      beam_power = (ebullet[b].status - 20) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (60 - ebullet[b].status) >> 1;
      if (beam_power > 8)
       beam_power = 8;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE3);
       END_TRANS
      }

      beam_power = (ebullet[b].status - 40) >> 1;
      if (ebullet[b].status > 50)
       beam_power = (56 - ebullet[b].status) >> 1;
      if (beam_power > 5)
       beam_power = 5;

      if (beam_power > 0)
      {
       side = beam_power;
       TRANS_MODE
       poly4(display, x2 + fxpart(angle_draw+PI_2, side), y2 + fypart(angle_draw+PI_2, side),
                    x2 + fxpart(angle_draw-PI_2, side), y2 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw-PI_2, side), y3 + fypart(angle_draw-PI_2, side),
                    x3 + fxpart(angle_draw+PI_2, side), y3 + fypart(angle_draw+PI_2, side),
                    TRANS_BLUE4);
       END_TRANS

      }

//      line(display, x2, y2, x3, y3, COL_WHITE);
//      pline(display, x2, y2, x3, y3, TRANS_RED3);
//      END_TRANS
     }
     break;*/
    case EBULLET_ROCKET:
     ccircle(x, y, ebullet[b].draw_size + grand(3), ebullet[b].colour);
     break;
    case EBULLET_SEEKER:
     ccircle(x, y, ebullet[b].draw_size + grand(3), ebullet[b].colour);

     if (ebullet[b].time < 20)
      ccircle2(x, y, 11 - (ebullet[b].time >> 1), ebullet[b].colour);

//     size = ebullet[b].draw_size + grand(2) + 1;
/*     x -= fxpart(angle_draw, size);
     y -= fypart(angle_draw, size);
     size = ebullet[b].draw_size2 + grand(2);
     ccircle2(x, y, size, ebullet[b].colour);*/
     break;
    case EBULLET_EEL:
     ccircle(x, y, ebullet[b].draw_size + grand(2), ebullet[b].colour);
     ccircle2(x, y, ebullet[b].draw_size + 2 + grand(2), ebullet[b].colour);
     ccircle2(x + fxpart(angle_draw + PI_2 + PI_4, ebullet[b].draw_size << 1), y + fypart(angle_draw + PI_2 + PI_4, ebullet[b].draw_size << 1), (ebullet[b].draw_size >> 1) + grand(2), ebullet[b].colour);
     ccircle2(x + fxpart(angle_draw - PI_2 - PI_4, ebullet[b].draw_size << 1), y + fypart(angle_draw - PI_2 - PI_4, ebullet[b].draw_size << 1), (ebullet[b].draw_size >> 1) + grand(2), ebullet[b].colour);
     break;
    case EBULLET_FLAK:
     ccircle(x, y, ebullet[b].draw_size, ebullet[b].colour);
     if (ebullet[b].time < 20)
      ccircle2(x, y, 11 - (ebullet[b].time >> 1), ebullet[b].colour);
//     size = ebullet[b].draw_size + grand(2) + 1;
     angle = angle_draw;//ebullet[b].angle;
     size = (ebullet[b].timeout >> 1) + ebullet[b].draw_size - 2;
     if (size > ebullet[b].draw_size + 6)
      size = ebullet[b].draw_size + 6;
     for (i = 0; i < 3; i ++)
     {
         angle += (PI*2)/3;
         x2 = x + fxpart(angle, size);
         y2 = y + fypart(angle, size);
         ccircle(x2, y2, 3, ebullet[b].colour);
     }
     break;
    case EBULLET_FLAK2:
     ccircle(x, y, (ebullet[b].timeout>>2), ebullet[b].colour);
//     if (ebullet[b].time < 20)
//      ccircle2(x, y, 11 - (ebullet[b].time >> 1), ebullet[b].colour);
//     size = ebullet[b].draw_size + grand(2) + 1;
     break;
    case EBULLET_BLOB:
     if (ebullet[b].time < 32)
      ccircle2(x, y, 16 - (ebullet[b].time >> 1), ebullet[b].colour);
     ccircle(x, y, ebullet[b].draw_size, ebullet[b].colour);
     break;

    case EBULLET_SHOT:
//     x += grand(2) - grand(2);
//     y += grand(2) - grand(2);
     ccircle(x, y, ebullet[b].draw_size + grand(2), ebullet[b].colour);

     if (ebullet[b].time < 20)
      ccircle2(x, y, 11 - (ebullet[b].time >> 1), ebullet[b].colour);

     size = ebullet[b].draw_size + grand(2) + 1;
     x -= fxpart(angle_draw, size);
     y -= fypart(angle_draw, size);
     size = ebullet[b].draw_size2 + grand(2);
     ccircle2(x, y, size, ebullet[b].colour);

/*     size = 2 + grand(2);
     x -= xpart(ebullet[b].angle, size);
     y -= ypart(ebullet[b].angle, size);
     ccircle2(bmp, x, y, size, ebullet[b].colour);*/
     break;
    case EBULLET_SPIN:
     ccircle(x, y, ebullet[b].draw_size + grand(2), ebullet[b].colour);

     if (ebullet[b].time < 20)
      ccircle2(x, y, 11 - (ebullet[b].time >> 1), ebullet[b].colour);

     size = ebullet[b].draw_size - grand(2);
     x2 = x + fxpart(angle_draw, size);
     y2 = y + fypart(angle_draw, size);
     size = ebullet[b].draw_size2 + grand(2);
     ccircle(x2, y2, size, ebullet[b].colour);
     size = ebullet[b].draw_size - grand(2);
     x2 = x - fxpart(angle_draw, size);
     y2 = y - fypart(angle_draw, size);
     size = ebullet[b].draw_size2 + grand(2);
     ccircle(x2, y2, size, ebullet[b].colour);
     break;
    case EBULLET_SHOT2:
     ccircle(x, y, ebullet[b].draw_size + grand(2), ebullet[b].colour);

//     if (ebullet[b].time < 20)
     ccircle2(x, y, ebullet[b].draw_size + 4 + grand(4), ebullet[b].colour);

     size = ebullet[b].draw_size + grand(2) + 1;
     x -= fxpart(angle_draw, size);
     y -= fypart(angle_draw, size);
     size = ebullet[b].draw_size + grand(2);
     ccircle2(x, y, size, ebullet[b].colour);
     size = ebullet[b].draw_size + grand(2) + 1;
     x -= fxpart(angle_draw, size);
     y -= fypart(angle_draw, size);
     size = ebullet[b].draw_size2 - grand(2);
     ccircle2(x, y, size, ebullet[b].colour);
     ccircle3(x, y, size >> 1, ebullet[b].colour);

     break;
    case EBULLET_MISSILE_BLAST:
     size = ebullet[b].status;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     switch(ebullet[b].timeout >> 2)
     {
      case 0:
      TRANS_MODE
      switch(ebullet[b].colour)
      {
       default:
       case 0: circlefill(display, x, y, size, TRANS_RED1); break;
       case 1: circlefill(display, x, y, size, TRANS_GREEN1); break;
       case 2: circlefill(display, x, y, size, TRANS_BLUE1); break;
      }
      END_TRANS
      break;
      case 1: ccircle2(x, y, size, ebullet[b].colour); break;
      case 2: ccircle3(x, y, size, ebullet[b].colour); break;
      default:
      case 3: ccircle(x, y, size, ebullet[b].colour); break;
     }

      break;
    case EBULLET_MISSILE:
     draw_circle_white(x, y, ebullet[b].draw_size);
     size = ebullet[b].draw_size;
     x2 = x - fxpart(angle_draw, size);
     y2 = y - fypart(angle_draw, size);
     draw_circle_white(x2, y2, ebullet[b].draw_size - 2);
     size = ebullet[b].draw_size + 1;
     angle2 = 0;
     if (ebullet[b].time < 30)
      angle2 = 0.76 - (float) ebullet[b].time / 30;
     x2 = x - fxpart(angle_draw - PI_2 + PI_4 + angle2, size);
     y2 = y - fypart(angle_draw - PI_2 + PI_4 + angle2, size);
     draw_circle_grey(x2, y2, ebullet[b].draw_size - 1);
     x2 = x - fxpart(angle_draw + PI_2 - PI_4 - angle2, size);
     y2 = y - fypart(angle_draw + PI_2 - PI_4 - angle2, size);
     draw_circle_grey(x2, y2, ebullet[b].draw_size - 1);
     size2 = size + grand(4) - 1;
     if (size2 > ebullet[b].time>>2)
      size2 = ebullet[b].time>>2;
     if (ebullet[b].time < 20)
      break;
     x2 = x - fxpart(angle_draw, size + 7);
     y2 = y - fypart(angle_draw, size + 7);
     ccircle2(x2, y2, size2, ebullet[b].colour);
     x2 = x - fxpart(angle_draw, size + 4);
     y2 = y - fypart(angle_draw, size + 4);
     ccircle3(x2, y2, size2, ebullet[b].colour);
     break;
 }
}

// debug - show parts:
#ifdef SHOW_PARTS

for (b = 0; b < NO_PARTS; b ++)
{

   if (part[b].type == PART_NONE)
    continue;

   if (part[b].x < player[p].x - (400 << 10) || part[b].x > player[p].x + (400 << 10)
    || part[b].y < player[p].y - (400 << 10) || part[b].y > player[p].y + (300 << 10))
     return;

   float angle = atan2(part[b].y - player[p].y, part[b].x - player[p].x) - (PI/2) - angle_to_radians(player[p].angle);
   int dist = hypot(part[b].y - player[p].y, part[b].x - player[p].x);
   int x = player[p].camera_x + cos(angle) * (dist >> 10);
   int y = player[p].camera_y + sin(angle) * (dist >> 10);

//   float angle_draw = part[b].angle - angle_to_radians(player[p].angle + ANGLE_4);
   float angle_draw =  angle_to_radians(part[b].angle - player[p].angle - ANGLE_4);

     ccircle(x, y, 3 + grand(2), 1);
     print_number(x, y, b);

}
#endif

}




void draw_clouds(int p)
{

int c, size, i, x2, y2, angle2, size2, dist, x, y;
float angle_draw, angle;

for (c = 0; c < NO_CLOUDS; c ++)
{

   if (cloud[c].type == CLOUD_NONE)
    continue;

   if (cloud[c].x < player[p].x - (600 << 10) || cloud[c].x > player[p].x + (600 << 10)
    || cloud[c].y < player[p].y - (600 << 10) || cloud[c].y > player[p].y + (500 << 10))
     continue;

   angle = atan2(cloud[c].y - player[p].y, cloud[c].x - player[p].x) - (PI/2) - angle_to_radians(player[p].angle);
   dist = hypot(cloud[c].y - player[p].y, cloud[c].x - player[p].x);
   x = player[p].camera_x + cos(angle) * (dist >> 10);
   y = player[p].camera_y + sin(angle) * (dist >> 10);

//   float angle_draw =  angle_to_radians(pbullet[b].angle - player[p].angle - ANGLE_4);

 switch(cloud[c].type)
 {
/*    case CLOUD_DELAY_SPARKLE:
     size = cloud[c].timeout >> 1;
     size += grand(4);
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle(x, y, size, cloud[c].colour);
     break;
    case CLOUD_DELAY_SPARKLE_COL2:
     size = cloud[c].timeout >> 1;
     size += grand(4);
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle2(x, y, size, cloud[c].colour);
     break;
*/
    case CLOUD_SEEKER_TRAIL:
     angle_draw = cloud[c].fangle - angle_to_radians(player[p].angle) - PI_2;
     x2 = x - fxpart(angle_draw, cloud[c].status);
     y2 = y - fypart(angle_draw, cloud[c].status);
     size = (cloud[c].timeout >> 2) + 1 + coin();
     do_line(display, x, y, x2, y2, size, draw_trail_line);
     break;
    case CLOUD_WORM_TRAIL:
     angle_draw = cloud[c].fangle - angle_to_radians(player[p].angle) - PI_2;
     x2 = x - fxpart(angle_draw, cloud[c].status);
     y2 = y - fypart(angle_draw, cloud[c].status);
     size = (cloud[c].timeout >> 2) + 1 + coin();
     do_line(display, x, y, x2, y2, size, draw_worm_trail_line);
     break;
    case CLOUD_ROCKET_TRAIL:
     angle_draw = cloud[c].fangle - angle_to_radians(player[p].angle) - PI_2;
     x2 = x - fxpart(angle_draw, cloud[c].status);
     y2 = y - fypart(angle_draw, cloud[c].status);
     size = (cloud[c].timeout >> 2) + 1 + coin();
//     if (size > 2)
//      size = 2;
     do_line(display, x, y, x2, y2, size, draw_rocket_trail_line);
     break;
    case CLOUD_BURST:
     size = cloud[c].status;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     switch(cloud[c].timeout >> 2)
     {
      case 0:
      TRANS_MODE
      switch(cloud[c].colour)
      {
       default:
       case 0: circlefill(display, x, y, size, TRANS_RED1); break;
       case 1: circlefill(display, x, y, size, TRANS_GREEN1); break;
       case 2: circlefill(display, x, y, size, TRANS_BLUE1); break;
      }
      END_TRANS
      break;
      case 1: ccircle2(x, y, size, cloud[c].colour); break;
      case 2: ccircle3(x, y, size, cloud[c].colour); break;
      default:
      case 3: ccircle(x, y, size, cloud[c].colour); break;
     }
     break;
    case CLOUD_BALL:
     size = cloud[c].timeout >> 1;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle(x, y, size, cloud[c].colour);
     break;
    case CLOUD_SMALL:
     size = cloud[c].timeout >> 3;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle(x, y, size, cloud[c].colour);
     break;
    case CLOUD_2BALL:
     size = cloud[c].timeout >> 1;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle2(x, y, size, cloud[c].colour);
     size = size - 2;//cloud[c].timeout >> 1;
//     if (size >= RLE_CCIRCLES)
//      size = RLE_CCIRCLES - 1;
     if (size > 0)
      ccircle3(x, y, size, cloud[c].colour);
     break;
    case CLOUD_BANG:
//    break;
     size = cloud[c].timeout >> 1;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle2(x, y, size, cloud[c].colour);
     size = cloud[c].timeout / 5;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle(x, y, size, cloud[c].colour);
     angle2 = cloud[c].angle - player[p].angle - ANGLE_4;

     size = cloud[c].timeout / 3;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     size2 = cloud[c].timeout / 7;
     if (size2 >= RLE_CCIRCLES)
      size2 = RLE_CCIRCLES - 1;

//     size -= 3;
//     size2 -= 1;
     if (size < 0)
      break;
     for (i = 0; i < 7; i ++)
     {
       angle2 += ANGLE_7;
       x2 = x + xpart(angle2, cloud[c].status);
       y2 = y + ypart(angle2, cloud[c].status);
       ccircle2(x2, y2, size + grand(3), cloud[c].colour);
       ccircle(x2, y2, size2 + grand(3), cloud[c].colour);
     }
/*     size = cloud[c].timeout >> 2;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     size2 = cloud[c].timeout / 8;
     if (size2 >= RLE_CCIRCLES)
      size2 = RLE_CCIRCLES - 1;

     size -= 3;
     size2 -= 1;
     if (size < 0)
      break;
     for (i = 0; i < 3; i ++)
     {
       angle += ANGLE_3;
       x2 = x + xpart(angle, cloud[c].status>>2);
       y2 = y + ypart(angle, cloud[c].status>>2);
       ccircle2(x2, y2, size + grand(3), cloud[c].colour);
       ccircle(x2, y2, size2 + grand(3), cloud[c].colour);
     }*/
     break;
    case CLOUD_BALL_COL3:
     size = cloud[c].timeout >> 1;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle3(x, y, size, cloud[c].colour);
     break;
    case CLOUD_BALL_COL2:
     size = cloud[c].timeout >> 1;
     if (size >= RLE_CCIRCLES)
      size = RLE_CCIRCLES - 1;
     ccircle2(x, y, size, cloud[c].colour);
     break;
    case CLOUD_SMALL_SHOCK:
     size = 29 - cloud[c].timeout;
     if (size >= 30)
      size = 29;
     if (size < 0)
      size = 0;
     draw_trans_rle_sprite(display, RLE_small_shock [cloud[c].colour] [size], x - size - 16, y - size - 16);
     break;
    case CLOUD_LARGE_SHOCK:
//     circle(display, x, y, 5, COL_WHITE);
     size = 49 - cloud[c].timeout;
     if (size >= 50)
      size = 49;
     if (size < 0)
      size = 0;
     draw_trans_rle_sprite(display, RLE_large_shock [cloud[c].colour] [size], x - size - 26, y - size - 26);
     break;

    case CLOUD_HUGE_SHOCK:
     size = 49 - cloud[c].timeout;
     if (size >= 50)
      size = 49;
     if (size < 0)
      size = 0;
     draw_trans_rle_sprite(display, RLE_huge_shock [cloud[c].colour] [size], x - size - 51, y - size - 51);
     break;

 }

}
}

void draw_trail_line(BITMAP *bmp, int x, int y, int size)
{
    static char drawing;
    drawing ^= 1;
    if (drawing & 1)
     return;

    ccircle3(x, y, size + coin(), 2);
}

void draw_worm_trail_line(BITMAP *bmp, int x, int y, int size)
{
    static char drawing;
    drawing ^= 1;
    if (drawing & 1)
     return;

    ccircle3(x, y, size + coin(), 0);
}

void draw_rocket_trail_line(BITMAP *bmp, int x, int y, int size)
{
    static char drawing;
    drawing ^= 1;
    if (drawing & 1)
     return;

    ccircle3(x, y, size + coin(), 1);
}


void draw_shield_circle(BITMAP *bmp, int x, int y, int size)
{
/*    static char drawing;
    drawing ^= 1;
    if (drawing & 1)
     return;*/
    if (coin())
     return;

    static char size2;
    size2 = size + grand(3);
    if (size2 < 1)
    {
     putpixel(display, x, y, TRANS_GREEN1);
    }
      else
      {
       if (size2 < 2)
        putpixel(display, x, y, TRANS_GREEN2);
         else
//          ccircle3(x, y, size + coin(), 1);
          putpixel(display, x, y, TRANS_GREEN3);
      }

//       ccircle3(x, y, size + coin(), 1);
}


void draw_pshield_circle(BITMAP *bmp, int x, int y, int strength)
{
/*    static char drawing;
    drawing ^= 1;
    if (drawing & 1)
     return;*/
//    if (grand(strength >> 5) == 0)
//     return;

    strength += 20;

    static char size2;
    size2 = (grand(strength) >> 5) + coin();
    if (size2 < 2)
    {
     putpixel(display, x, y, TRANS_BLUE1);
    }
      else
      {
        if (size2 < 4)
         putpixel(display, x, y, TRANS_BLUE2);
          else
           putpixel(display, x, y, TRANS_BLUE3);
      }
//       ccircle3(x, y, size2 - 1, 2);
}

/*

Use this instead of Allegro's line function - it has better bounds checking.
Sometimes for some reason I get an extremely long line which slows things down.

*/
void pline(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour)
{
 if (x1 < -500 || x1 > 900
     || x2 < -500 || x2 > 900
     || y2 < -500 || y2 > 900
     || y2 < -500 || y2 > 900)
      return;

 line(bmp, x1, y1, x2, y2, colour);

}

void poly4(BITMAP *target, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int col)
{

  points [0] = x1;
  points [1] = y1;
  points [2] = x2;
  points [3] = y2;
  points [4] = x3;
  points [5] = y3;
  points [6] = x4;
  points [7] = y4;

  polygon(target, 4, points, col);

}



void print_number(int x, int y, int n)
{
  textprintf_ex(display, font, x, y, -1, -1, "%i", n);
}


void draw_circle_in(int x, int y, int size, char hurt)
{

   if (size > CIRCLES - 1)
    size = CIRCLES - 1;
   if (size < 0)
    size = 0;

   if (hurt)
    draw_trans_rle_sprite(display, circle_hurt [size], x - (size) - 1, y - (size) - 1);
     else
      draw_trans_rle_sprite(display, circle_in [size], x - (size) - 1, y - (size) - 1);


}

void draw_circle_out(int x, int y, int size)
{

   if (size > RLE_CCIRCLES - 1)
    size = RLE_CCIRCLES - 1;
   if (size < 0)
    size = 0;
   draw_trans_rle_sprite(display, circle_out [size], x - (size) - 1, y - (size) - 1);

}

void draw_circle_white(int x, int y, int size)
{

   if (size > RLE_CCIRCLES - 1)
    size = RLE_CCIRCLES - 1;
   if (size < 0)
    size = 0;
   draw_trans_rle_sprite(display, circle_white [size], x - (size) - 1, y - (size) - 1);

}

void draw_circle_grey(int x, int y, int size)
{

   if (size > RLE_CCIRCLES - 1)
    size = RLE_CCIRCLES - 1;
   if (size < 0)
    size = 0;
   draw_trans_rle_sprite(display, circle_grey [size], x - (size) - 1, y - (size) - 1);

}


void ccircle(int x, int y, int size, int colour)
{
if (size < 0)
 return;
if (size >= RLE_CCIRCLES)
 size = RLE_CCIRCLES - 1;
   draw_trans_rle_sprite(display, RLE_ccircle_basic [colour] [size], x - size - 1, y - size - 1);

}

void ccircle3(int x, int y, int size, int colour)
{
if (size < 0)
 return;
if (size >= RLE_CCIRCLES)
 size = RLE_CCIRCLES - 1;

   draw_trans_rle_sprite(display, RLE_ccircle_3cols [colour] [size], x - size - 1, y - size - 1);

}

void ccircle2(int x, int y, int size, int colour)
{
if (size < 0)
 return;
if (size >= RLE_CCIRCLES)
 size = RLE_CCIRCLES - 1;

   draw_trans_rle_sprite(display, RLE_ccircle_2cols [colour] [size], x - size - 1, y - size - 1);

}


void display_pause(void)
{
  vsync();

  int x1 = 200;
  int y1 = 100;
  int y2 = 300;

  TRANS_MODE
  rectfill(display, x1, y1, 640 - x1, y2, TRANS_BLUE1);
  rect(display, x1 - 1, y1 - 1, 641 - x1, y2 + 1, TRANS_BLUE2);
  rect(display, x1 - 2, y1 - 2, 642 - x1, y2 + 2, TRANS_BLUE3);
  rect(display, x1 - 3, y1 - 3, 643 - x1, y2 + 3, TRANS_BLUE1);
  END_TRANS

/*  rect(display, 200, 100, 440, 200, TRANS_GREEN5 - 1);

  rect(display, 190, 90, 450, 210, TRANS_RED3 - 1);
  rect(display, 191, 91, 449, 209, TRANS_RED5 - 1);
  rect(display, 189, 89, 451, 211, TRANS_RED5 - 1);*/

  textprintf_centre_ex(display, font, 320, y1 + 40, -1, -1, "Quit?");


  textprintf_centre_ex(display, font, 320, y1 + 100, -1, -1, "Press 'Y' to quit");
  textprintf_centre_ex(display, font, 320, y1 + 140, -1, -1, "or 'N' to continue");


  blit(display, screen, 0, 0, 0, 0, 640, 480);

}

