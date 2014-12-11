
#include "config.h"

#include "allegro.h"

#include <math.h>

#include "globvars.h"
#include "palette.h"
#include "stuff.h"
#include "display.h"



extern BITMAP *display;
extern RGB palet [256];

BITMAP *new_bitmap(int x, int y, const char errtxt []);
BITMAP *load_up_bitmap(const char fname []);
RLE_SPRITE *extract_rle_sprite(BITMAP *source, int x_source, int y_source, int x, int y);
void new_rle_struct(BITMAP *source, const char errtxt [], struct RLE_STRUCT *str, int bl);

void make_sprites(void);
BITMAP *extract_bitmap(BITMAP *source, int x_source, int y_source, int x, int y);
BITMAP *reduce_sprite(BITMAP *bmp, char do_alpha);
void prepare_circles(void);
void make_player_sprites(void);
RLE_SPRITE *extract_flip_rle_sprite(BITMAP *source, int x_source, int y_source, int x, int y, int flip_type);

extern struct RLE_STRUCT wship_sprite [WSHIP_SPRITES] [WSHIP_ROTATIONS];

extern struct RLE_STRUCT turret_sprite [TURRET_SPRITES] [TURRET_ROTATIONS];

void announce(const char anntext []);

extern RLE_SPRITE *circle_in [CIRCLES];
extern RLE_SPRITE *circle_out [CIRCLES];
extern RLE_SPRITE *circle_hurt [CIRCLES];
extern RLE_SPRITE *circle_white [CIRCLES];
extern RLE_SPRITE *circle_grey [CIRCLES];

extern RLE_SPRITE *RLE_bcircle [3] [RLE_BCIRCLES];
extern RLE_SPRITE *RLE_ccircle_basic [3] [RLE_CCIRCLES];
extern RLE_SPRITE *RLE_ccircle_3cols [3] [RLE_CCIRCLES];
extern RLE_SPRITE *RLE_ccircle_2cols [3] [RLE_CCIRCLES];
extern RLE_SPRITE *RLE_small_shock [3] [30];
extern RLE_SPRITE *RLE_large_shock [3] [50];
extern RLE_SPRITE *RLE_huge_shock [3] [50];
extern RLE_SPRITE *RLE_player [PLAYER_RLES];
extern RLE_SPRITE *RLE_scircle [4] [RLE_SCIRCLES];
extern RLE_SPRITE *RLE_shield [SHIELD_RLES];

#ifdef SHOW_GRAPHS
extern int graph_slack [200];
extern int graph_slack_pos;
extern int graph_fps [200];
extern int graph_fps_pos;
#endif

enum
{
FLIP_H,
FLIP_V,
FLIP_VH
};

FONT* gfont;
extern BITMAP *interlude_screen;

/*

Basic display init functions.

*/

/*
Call during start-up.
*/
void prepare_display(void)
{

 init_palette(-1);


 display = new_bitmap(800, 600, "Display");
 clear_to_color(display, COL_OUTLINE);

 DATAFILE *datf = load_datafile("gfx//data.dat");
 if (datf == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Error: Couldn't find data.dat! \n");
  allegro_message("\n\r");
  exit(1);
 }

 font = (FONT *)datf[0].dat;
 gfont = (FONT *)datf[1].dat;


make_sprites();
make_player_sprites();
prepare_circles();




//#define FIX_FONT

#ifdef FIX_FONT

int i,j;
 BITMAP *font_bitmap = load_up_bitmap("font_in.bmp");

 int back_col = getpixel(font_bitmap, 0, 0);
 int back_col2 = getpixel(font_bitmap, 3, 0);
 int out_col = getpixel(font_bitmap, 17, 4);
 int white_col = getpixel(font_bitmap, 18, 4);
 int other_col = getpixel(font_bitmap, 22, 44);
// int lgrey_col = getpixel(font_bitmap, 8, 19);

 for (i = 0; i < font_bitmap->w; i ++)
 {
  for (j = 0; j < font_bitmap->h; j ++)
  {
   if (getpixel(font_bitmap, i, j) == back_col
       || getpixel(font_bitmap, i, j) == back_col2)
   {
       putpixel(font_bitmap, i, j, 255);
       continue;
   }
   if (getpixel(font_bitmap, i, j) == out_col)
   {
       putpixel(font_bitmap, i, j, WRITE_OUT);//COL_OUTLINE);
       continue;
   }
   if (getpixel(font_bitmap, i, j) == white_col)
   {
       putpixel(font_bitmap, i, j, WRITE_IN);//COL_WHITE);
       continue;
   }
   if (getpixel(font_bitmap, i, j) == other_col)
   {
       putpixel(font_bitmap, i, j, 245);//other colour
       continue;
   }

   }
  }


 save_bitmap("font_out.bmp", font_bitmap, palet);

#endif

#ifdef SHOW_GRAPHS

 int k;

 for (k = 0; k < 200; k ++)
 {
 graph_slack [k] = 0;
 graph_fps [k] = 0;
 }

 graph_slack_pos = 0;
 graph_fps_pos = 0;

#endif

interlude_screen = new_bitmap(640, 480, "interlude");

}

void make_player_sprites(void)
{

 BITMAP *file_bitmap = load_up_bitmap("gfx//player.bmp");

 int i, j;


#define FIX_PLAYER 1

#ifdef FIX_PLAYER

int p;
int control [10];
control [0] = getpixel(file_bitmap, 0, 0);
control [1] = getpixel(file_bitmap, 1, 0);
control [2] = getpixel(file_bitmap, 2, 0);
control [3] = getpixel(file_bitmap, 3, 0);

control [4] = getpixel(file_bitmap, 9, 0);

control [5] = getpixel(file_bitmap, 4, 0);
control [6] = getpixel(file_bitmap, 5, 0);
control [7] = getpixel(file_bitmap, 6, 0);
control [8] = getpixel(file_bitmap, 7, 0);
control [9] = getpixel(file_bitmap, 8, 0);

for (i = 0; i < file_bitmap->w; i ++)
{
 for (j = 0; j < file_bitmap->h; j ++)
 {
  p = getpixel(file_bitmap, i, j);
  if (p == control [0])
   putpixel(file_bitmap, i, j, COL_WHITE);
  if (p == control [1])
   putpixel(file_bitmap, i, j, COL_LGREY);
  if (p == control [2])
   putpixel(file_bitmap, i, j, COL_DGREY);
  if (p == control [3])
   putpixel(file_bitmap, i, j, COL_OUTLINE);
  if (p == control [4])
   putpixel(file_bitmap, i, j, 0);
  if (p == control [5])
   putpixel(file_bitmap, i, j, TRANS_BLUE1);
  if (p == control [6])
   putpixel(file_bitmap, i, j, TRANS_BLUE2);
  if (p == control [7])
   putpixel(file_bitmap, i, j, TRANS_BLUE3);
  if (p == control [8])
   putpixel(file_bitmap, i, j, TRANS_BLUE4);
  if (p == control [9])
   putpixel(file_bitmap, i, j, COL_WHITE + TRANS_BLUE4);
 }
}

#endif

 RLE_player [PLAYER_RLE_BASE_1] = extract_rle_sprite(file_bitmap, 6, 11, 25, 16);
 RLE_player [PLAYER_RLE_BASE_2] = extract_rle_sprite(file_bitmap, 32, 11, 25, 17);
 RLE_player [PLAYER_RLE_BASE_3] = extract_rle_sprite(file_bitmap, 58, 11, 25, 18);

 RLE_player [PLAYER_RLE_SIDE_L1] = extract_rle_sprite(file_bitmap, 3, 33, 7, 10);
 RLE_player [PLAYER_RLE_SIDE_L2] = extract_rle_sprite(file_bitmap, 3, 43, 7, 10);
 RLE_player [PLAYER_RLE_SIDE_L3] = extract_rle_sprite(file_bitmap, 3, 54, 7, 10);

 RLE_player [PLAYER_RLE_SIDE_R1] = extract_flip_rle_sprite(file_bitmap, 3, 33, 7, 10, FLIP_H);
 RLE_player [PLAYER_RLE_SIDE_R2] = extract_flip_rle_sprite(file_bitmap, 3, 43, 7, 10, FLIP_H);
 RLE_player [PLAYER_RLE_SIDE_R3] = extract_flip_rle_sprite(file_bitmap, 3, 54, 7, 10, FLIP_H);

 RLE_player [PLAYER_RLE_FRONT_1] = extract_rle_sprite(file_bitmap, 30, 33, 11, 9);
 RLE_player [PLAYER_RLE_FRONT_2] = extract_rle_sprite(file_bitmap, 30, 44, 11, 9);
 RLE_player [PLAYER_RLE_FRONT_3] = extract_rle_sprite(file_bitmap, 30, 55, 11, 9);
 RLE_player [PLAYER_RLE_FRONT_4] = extract_rle_sprite(file_bitmap, 30, 66, 11, 9);

 RLE_player [PLAYER_RLE_BIG] = extract_rle_sprite(file_bitmap, 89, 7, 100, 66);

 RLE_player [PLAYER_RLE_WEAPON1] = extract_rle_sprite(file_bitmap, 1, 85, 29, 29);
 RLE_player [PLAYER_RLE_WEAPON2] = extract_rle_sprite(file_bitmap, 31, 85, 29, 29);
 RLE_player [PLAYER_RLE_WEAPON3] = extract_rle_sprite(file_bitmap, 61, 85, 29, 29);
 RLE_player [PLAYER_RLE_WEAPON4] = extract_rle_sprite(file_bitmap, 91, 85, 29, 29);
 RLE_player [PLAYER_RLE_WEAPON5] = extract_rle_sprite(file_bitmap, 121, 85, 29, 29);
 RLE_player [PLAYER_RLE_WEAPON6] = extract_rle_sprite(file_bitmap, 151, 85, 29, 29);

 RLE_player [PLAYER_RLE_MINI] = extract_rle_sprite(file_bitmap, 55, 40, 12, 8);

 RLE_player [PLAYER_RLE_MENU1] = extract_rle_sprite(file_bitmap, 190, 1, 100, 66);
 RLE_player [PLAYER_RLE_MENU2] = extract_rle_sprite(file_bitmap, 290, 1, 100, 67);
 RLE_player [PLAYER_RLE_MENU3] = extract_rle_sprite(file_bitmap, 390, 1, 100, 68);
 RLE_player [PLAYER_RLE_MENU4] = extract_rle_sprite(file_bitmap, 490, 1, 100, 69);
 RLE_player [PLAYER_RLE_MENU5] = extract_rle_sprite(file_bitmap, 183, 78, 100, 70);
 RLE_player [PLAYER_RLE_MENU6] = extract_rle_sprite(file_bitmap, 283, 78, 100, 71);


/*
 RLE_player [PLAYER_RLE_BASE_1] = extract_rle_sprite(file_bitmap, 5, 10, 25, 18);
 RLE_player [PLAYER_RLE_BASE_2] = extract_rle_sprite(file_bitmap, 31, 10, 25, 19);
 RLE_player [PLAYER_RLE_BASE_3] = extract_rle_sprite(file_bitmap, 57, 10, 25, 20);

 RLE_player [PLAYER_RLE_SIDE_L1] = extract_rle_sprite(file_bitmap, 3, 33, 7, 10);
 RLE_player [PLAYER_RLE_SIDE_L2] = extract_rle_sprite(file_bitmap, 3, 43, 7, 10);
 RLE_player [PLAYER_RLE_SIDE_L3] = extract_rle_sprite(file_bitmap, 3, 54, 7, 10);

 RLE_player [PLAYER_RLE_SIDE_R1] = extract_flip_rle_sprite(file_bitmap, 3, 33, 7, 10, FLIP_H);
 RLE_player [PLAYER_RLE_SIDE_R2] = extract_flip_rle_sprite(file_bitmap, 3, 43, 7, 10, FLIP_H);
 RLE_player [PLAYER_RLE_SIDE_R3] = extract_flip_rle_sprite(file_bitmap, 3, 54, 7, 10, FLIP_H);

 RLE_player [PLAYER_RLE_FRONT_1] = extract_rle_sprite(file_bitmap, 30, 33, 9, 9);
 RLE_player [PLAYER_RLE_FRONT_2] = extract_rle_sprite(file_bitmap, 40, 33, 9, 9);
 RLE_player [PLAYER_RLE_FRONT_3] = extract_rle_sprite(file_bitmap, 50, 33, 9, 9);
 RLE_player [PLAYER_RLE_FRONT_4] = extract_rle_sprite(file_bitmap, 60, 33, 9, 9);
*/
// RLE_shield [0] = extract_rle_sprite(file_bitmap, 83, 1, 37, 28);
// RLE_shield [1] = extract_rle_sprite(file_bitmap, 121, 1, 37, 28);

 destroy_bitmap(file_bitmap);


}



void make_sprites(void)
{

 int i;

 BITMAP *temp_bitmap;

 for (i = 0; i < CIRCLES; i ++)
 {
  temp_bitmap = new_bitmap((i + 1) * 2, (i + 1) * 2, "prepare circles");
  clear_bitmap(temp_bitmap);
  circlefill(temp_bitmap, i + 1, i + 1, i, COL_HS_INNER);
  circle(temp_bitmap, i + 1, i + 1, i, COL_HS_INNER_EDGE);
  circle_in [i] = get_rle_sprite(temp_bitmap);

  circlefill(temp_bitmap, i + 1, i + 1, i, COL_HS_OUTER);
  circle(temp_bitmap, i + 1, i + 1, i, COL_HS_OUTER_EDGE);
  circle_out [i] = get_rle_sprite(temp_bitmap);

  circlefill(temp_bitmap, i + 1, i + 1, i, COL_HS_HURT);
  circle(temp_bitmap, i + 1, i + 1, i, COL_HS_INNER_EDGE);
  circle_hurt [i] = get_rle_sprite(temp_bitmap);

  circlefill(temp_bitmap, i + 1, i + 1, i, COL_WHITE);
  circle(temp_bitmap, i + 1, i + 1, i, COL_DGREY);
  circle_white [i] = get_rle_sprite(temp_bitmap);

  circlefill(temp_bitmap, i + 1, i + 1, i, COL_LGREY);
  circle(temp_bitmap, i + 1, i + 1, i, COL_OUTLINE);
  circle_grey [i] = get_rle_sprite(temp_bitmap);

  destroy_bitmap(temp_bitmap);
 }

}


void prepare_circles(void)
{


 int beam_colours [3] [4] =
 {
  {TRANS_RED1, TRANS_RED2, TRANS_RED3, TRANS_YELLOW4},
  {TRANS_GREEN1, TRANS_GREEN2, TRANS_GREEN3, TRANS_YELLOW4},
  {TRANS_BLUE1, TRANS_BLUE2, TRANS_BLUE3, TRANS_BLUE4}
 };
// int cx, cy, ci, cs, ca, cd, cj, cn;

 int i, j;

 BITMAP *temp_bitmap;

 int rad;

 for (i = 0; i < RLE_CCIRCLES; i ++)
 {
  temp_bitmap = new_bitmap((i + 1) * 2, (i + 1) * 2, "prepare ccircles");
  clear_bitmap(temp_bitmap);
  for (j = 0; j < 3; j ++)
  {

   rad = i;
   circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [j] [0]);
   rad --;//*= 0.90;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [j] [1]);

   RLE_ccircle_2cols [j] [i] = get_rle_sprite(temp_bitmap);

   rad --;//*= 0.90;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [j] [2]);

   RLE_ccircle_3cols [j] [i] = get_rle_sprite(temp_bitmap);

   rad --;//*= 0.90;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [j] [3]);

   RLE_ccircle_basic [j] [i] = get_rle_sprite(temp_bitmap);
  }

  destroy_bitmap(temp_bitmap);
 }


 for (i = 0; i < RLE_SCIRCLES; i ++)
 {
  temp_bitmap = new_bitmap((i + 1) * 2, (i + 1) * 2, "prepare scircles");
  clear_bitmap(temp_bitmap);
  for (j = 0; j < 4; j ++)
  {

   rad = i;
   circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [1] [0]);
   if (j > 0)
   {
   rad --;//*= 0.90;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [1] [1]);
   if (j > 1)
   {
   rad --;//*= 0.90;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [1] [2]);
   if (j > 2)
   {
   rad --;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [1] [3]);
   }
   rad --;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [1] [2]);
   }
   rad --;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [1] [1]);
   }
   rad --;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, beam_colours [1] [0]);
   rad --;
   if (rad > 0)
    circlefill(temp_bitmap, i + 1, i + 1, rad, 0);

   RLE_scircle [j] [i] = get_rle_sprite(temp_bitmap);
  }

   destroy_bitmap(temp_bitmap);
 }

 int rad2;

 for (i = 0; i < 30; i ++)
 {
  rad = i + 15; // see below
  temp_bitmap = new_bitmap((rad + 1) * 2, (rad + 1) * 2, "prepare small_shock");
  clear_bitmap(temp_bitmap);
  for (j = 0; j < 3; j ++)
  {
   rad = i + 15; // see above
   rad2 = rad;

   circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [0]);
   if (i < 22)
   {
    rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [2]);
   }
/*   if (i < 22 && 0)
   {
    rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [4]);
   }*/
   if (i < 22)
   {
    rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [3]);
   }
   if (i < 24)
   {
    rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [2]);
   }
   if (i < 26)
   {
    rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [1]);
   }
   if (i < 28)
   {
    rad2 -= 2;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [0]);
   }

   rad2 -= 2;

   if (rad2 > -1)
   circlefill(temp_bitmap, rad + 1, rad + 1, rad2, 0);

   RLE_small_shock [j] [i] = get_rle_sprite(temp_bitmap);
  }

  destroy_bitmap(temp_bitmap);
 }


 for (i = 0; i < 50; i ++)
 {
  rad = i + 25; // see below
  temp_bitmap = new_bitmap((rad + 1) * 2, (rad + 1) * 2, "prepare large_shock");
  clear_bitmap(temp_bitmap);
  for (j = 0; j < 3; j ++)
  {
   rad = i + 25; // see above
   rad2 = rad;

   circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [0]);
   if (i < 42)
   {
    rad2 --;
    if (i < 32)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [2]);
   }
/*   if (i < 42 && 0)
   {
    rad2 --;
    if (i < 34)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [4]);
   }*/
   if (i < 42)
   {
    rad2 --;
    if (i < 36)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [3]);
   }
   if (i < 44)
   {
    rad2 --;
    if (i < 38)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [2]);
   }
   if (i < 46)
   {
    rad2 --;
    if (i < 40)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [1]);
   }
   if (i < 48)
   {
    rad2 -= 2;
    if (i < 42)
     rad2 -= 2;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [0]);
   }

   rad2 -= 2;
    if (i < 44)
     rad2 -= 2;

   if (rad2 > -1)
   circlefill(temp_bitmap, rad + 1, rad + 1, rad2, 0);

   RLE_large_shock [j] [i] = get_rle_sprite(temp_bitmap);
  }

  destroy_bitmap(temp_bitmap);
 }


 for (i = 0; i < 50; i ++)
 {
  rad = i + 50; // see below
  temp_bitmap = new_bitmap((rad + 1) * 2, (rad + 1) * 2, "prepare huge_shock");
  clear_bitmap(temp_bitmap);
  for (j = 0; j < 3; j ++)
  {
   rad = i + 50; // see above
   rad2 = rad;

   circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [0]);
   if (i < 42)
   {
    rad2 --;
    if (i < 32)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [2]);
   }
/*   if (i < 42 && 0)
   {
    rad2 --;
    if (i < 34)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [4]);
   }*/
   if (i < 42)
   {
    rad2 --;
    if (i < 36)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [3]);
   }
   if (i < 44)
   {
    rad2 --;
    if (i < 38)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [2]);
   }
   if (i < 46)
   {
    rad2 --;
    if (i < 40)
     rad2 --;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [1]);
   }
   if (i < 48)
   {
    rad2 -= 2;
    if (i < 42)
     rad2 -= 2;
    circlefill(temp_bitmap, rad + 1, rad + 1, rad2, beam_colours [j] [0]);
   }

   rad2 -= 2;
    if (i < 44)
     rad2 -= 2;

   if (rad2 > -1)
   circlefill(temp_bitmap, rad + 1, rad + 1, rad2, 0);

   RLE_huge_shock [j] [i] = get_rle_sprite(temp_bitmap);
  }

  destroy_bitmap(temp_bitmap);
 }

}



/*
Safe wrapper for the Allegro create_bitmap function.
*/
BITMAP *new_bitmap(int x, int y, const char errtxt [])
{

 BITMAP *bmp = create_bitmap(x, y);
 if (bmp == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Bitmap creation failed!  \n%s\n\n\r%s", allegro_error, errtxt);
  exit(1);
 }

 return bmp;

}



BITMAP *load_up_bitmap(const char fname [])
{
 RGB temp_palette [256];

 BITMAP *temp_bitmap = load_bitmap(fname, temp_palette);
 if (temp_bitmap == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Failed to load in bitmap! (File not found?)  \n%s", fname);
  exit(1);
 }

 return temp_bitmap;

}


RLE_SPRITE *extract_rle_sprite(BITMAP *source, int x_source, int y_source, int x, int y)
{

 BITMAP *tmp = new_bitmap(x, y, "extract_rle_sprite");

 blit(source, tmp, x_source, y_source, 0, 0, x, y);

 RLE_SPRITE *retval = get_rle_sprite(tmp);

 if (retval == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Rle_sprite extraction failed!  \n%s\n\n\r%s", allegro_error, "extract_rle_sprite");
  exit(1);
 }

 destroy_bitmap(tmp);

 return retval;

}

BITMAP *extract_bitmap(BITMAP *source, int x_source, int y_source, int x, int y)
{

 BITMAP *tmp = new_bitmap(x, y, "extract_bitmap");

 if (tmp== NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Bitmap extraction failed!  \n%s\n\n\r%s", allegro_error, "extract_bitmap");
  exit(1);
 }

 blit(source, tmp, x_source, y_source, 0, 0, x, y);

 return tmp;

}





void new_rle_struct(BITMAP *source, const char errtxt [], struct RLE_STRUCT *str, int bl)
{


 int x1 = -1, y1 = -1, x2 = -1, y2 = -1;

 int i, j;

 for (i = 0; i < source->w; i ++)
 {
  for (j = 0; j < source->h; j ++)
  {
    if (getpixel(source, i, j) != 0)
    {
     x1 = i;
     break;
    }
  }
  if (x1 != -1)
   break;
 }

  for (j = 0; j < source->h; j ++)
  {
   for (i = 0; i < source->w; i ++)
   {
    if (getpixel(source, i, j) != 0)
    {
     y1 = j;
     break;
    }
  }
  if (y1 != -1)
   break;
 }

 for (i = source->w - 1; i >= 0; i --)
 {
  for (j = 0; j < source->h; j ++)
  {
    if (getpixel(source, i, j) != 0)
    {
     x2 = i;
     break;
    }
  }
  if (x2 != -1)
   break;
 }


  for (j = source->h - 1; j >= 0; j --)
  {
   for (i = 0; i < source->w; i ++)
   {
    if (getpixel(source, i, j) != 0)
    {
     y2 = j;
     break;
    }
  }
  if (y2 != -1)
   break;
 }



 BITMAP *tmp = new_bitmap(x2 - x1 + 1, y2 - y1 + 1, "new_rle_struct temp bitmap");

 blit(source, tmp, x1, y1, 0, 0, x2 - x1 + 1, y2 - y1 + 1);

// circle(tmp, tmp->w / 2, tmp->h / 2, 2, 1);

 //rect(tmp, 0, 0, tmp->w-1, tmp->h-1, 15);

 if (bl == -1)
 {
   str->sprite = get_rle_sprite(tmp);
//   str->x = tmp->w / 2;//source->w / 2 - x1;
//   str->y = tmp->h / 2;//source->h / 2 - y1;
   str->x = tmp->w / 2;//source->w / 2 - x1;
   str->y = tmp->h / 2;//source->h / 2 - y1;
 }
  else
  {
   if (bl == -2)
   {
     str->sprite = get_rle_sprite(tmp);
//   str->x = tmp->w / 2;//source->w / 2 - x1;
//   str->y = tmp->h / 2;//source->h / 2 - y1;
     str->x = source->w / 2 - x1;
     str->y = source->h / 2 - y1;
   }
    else
    {
     str [bl].sprite = get_rle_sprite(tmp);
//     str [bl].x = tmp->w / 2; //source->w / 2 - x1;
//     str [bl].y = tmp->h / 2; //source->h / 2 - y1;
     str [bl].x = source->w / 2 - x1;
     str [bl].y = source->h / 2 - y1;
    }
  }

 destroy_bitmap(tmp);
}

void announce(const char anntext [])
{

 static int row = 0;

 textprintf_ex(screen, font, 10, 20 + row * 12, COL_WHITE, -1, anntext);

 row ++;

}

RLE_SPRITE *extract_flip_rle_sprite(BITMAP *source, int x_source, int y_source, int x, int y, int flip_type)
{

 BITMAP *tmp = new_bitmap(x, y, "extract_hflip_rle_sprite");
 BITMAP *tmp2 = new_bitmap(x, y, "extract_hflip_rle_sprite");

 clear_bitmap(tmp2);
 blit(source, tmp, x_source, y_source, 0, 0, x, y);

 switch(flip_type)
 {
  case FLIP_H: draw_sprite_h_flip(tmp2, tmp, 0, 0); break;
  case FLIP_V: draw_sprite_v_flip(tmp2, tmp, 0, 0); break;
  case FLIP_VH: draw_sprite_vh_flip(tmp2, tmp, 0, 0); break;
 }


 RLE_SPRITE *retval = get_rle_sprite(tmp2);

 if (retval == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Rle_sprite extraction failed!  \n%s\n\n\r%s", allegro_error, "extract_rle_sprite");
  exit(1);
 }

 destroy_bitmap(tmp);
 destroy_bitmap(tmp2);

 return retval;

}

