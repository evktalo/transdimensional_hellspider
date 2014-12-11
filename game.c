
#include "config.h"

#include "allegro.h"

#include <math.h>

#include "globvars.h"

#include "stuff.h"

#include "palette.h"
#include "display.h"
#include "level.h"
#include "hs.h"
#include "input.h"
#include "pbullet.h"
#include "ebullet.h"
#include "cloud.h"
#include "menu.h"
#include "hs_build.h"

#include "sound.h"

void begin_game(void);

void new_game(void);
void run_player(int p);

void init_stars(int p);

extern volatile unsigned char ticked;
extern volatile int framecounter;
extern int slacktime;

unsigned char counter;
void init_player_new_level(void);
void init_player_spawn(int p);
void player_respawn(int p);
void run_stars(int p);
void set_star(int i);
void player_hit(int p, int force);
char run_pause(void);
extern BITMAP *interlude_screen;

#ifdef SHOW_GRAPHS
extern int graph_slack [200];
extern int graph_slack_pos;
extern int graph_fps [200];
extern int graph_fps_pos;
extern volatile int frames_per_second;
#endif

#define STAR_DIST (500<<10)

void new_game(void)
{

arena.counter = 0;


 int i;

 for (i = 0; i < U_TYPES; i ++)
 {
  player[0].upgrade [i] = 0;
 }

 player[0].ships = 2;

 init_player_new_level();

    player[0].x = 900;
    player[0].y = 900;
    player[0].angle = 0;
    player[0].turning = 0;
    player[0].turn_speed = 0;
    player[0].x_speed = 0;
    player[0].y_speed = 0;

    player[0].camera_x = 320;
    player[0].camera_y = 360;
    player[0].window_x = 640;
    player[0].window_y = 480;
    player[0].window_centre_x = 320;
    player[0].window_centre_y = 360;

    player[0].respawning = 0;
    player[0].drive = 0;
    player[0].flap [0] = 0;
    player[0].flap [1] = 0;

 init_level();
 init_stars(0);
 init_clouds();
 init_ebullets();
 init_pbullets();
 init_voices();
 init_beats();
 init_palette(hs[0].stage);

 build_tracks();

 arena.game_over = 0;
 arena.end_stage = 0;

}


char run_pause(void)
{
 int pc = 0;

  display_pause();

 do
 {
  pc ++;
  if (pc > 256)
   pc = 0;


   do
   {
       rest(1);
   } while(ticked == 0);
   ticked --;

  if (key [KEY_Y])
  {
//   clear_bitmap(screen);
//   vsync();
   return 1;
  }

  if (key [KEY_N])
   return 0;
 }
  while (TRUE);

 return 0;

}


void game_loop(void)
{

 clear_to_color(interlude_screen, COL_BACK);

//       upgrade_menu();

 int playing = 1;

 counter = 0;

 if (choose_menu() == 0)
  return;

 new_game(); // need a better place to put this

// upgrade_menu();

 do
 {

// get_input();
 //run_player();

 arena.counter ++;
 arena.counter &= 255;

  if (ticked == 0)
  {
   run_display(1, 1);
   framecounter++;
  }
   else
    run_display(0, 0);

  slacktime = 0;

  do
  {
   rest(1);
   slacktime ++;
  } while(ticked == 0);
  ticked --;

  counter ++;

 if (arena.game_over > 0)
 {
  arena.game_over --;
  if (arena.game_over <= 0)
   break;

 }
  else
  {
  if (arena.end_stage > 0)
  {
   arena.end_stage --;
   if (arena.end_stage <= 0)
   {
      if (hs[0].stage == 25)
      {
       congratulations();
       break;
      }
      get_interlude_background();
      if ((hs[0].stage-1) % 3 == 0)
       upgrade_menu();
      init_palette(hs[0].stage);
      init_player_new_level();
      build_tracks();
      respawn_hs(0);
      destroy_all_ebullets();
/*      init_clouds();
      init_ebullets();
      init_pbullets();*/
   }
  }
   else
   {
    if (arena.time_left <= 0)
    {
     player[0].ships = 0;
     player_hit(0, 1);
    }
     else
      arena.time_left --;
   }
  }

  run_player(0);
  run_hs();
  run_pbullets();
  run_ebullets();
  run_clouds();
  play_tracks();
  run_voices();

  if (key [KEY_ESC])
  {
      if (run_pause() == 1)
       playing = 0;
  }

#ifdef SHOW_GRAPHS
 graph_slack_pos ++;
 if (graph_slack_pos >= 200)
  graph_slack_pos = 0;
 graph_slack [graph_slack_pos] = slacktime;

 graph_fps_pos ++;
 if (graph_fps_pos >= 200)
  graph_fps_pos = 0;
 graph_fps [graph_fps_pos] = frames_per_second;
#endif



 } while(playing == 1);


}


void run_player(int p)
{

 if (player[p].alive == 0)
 {
  if (player[p].respawning > 0 && arena.game_over == 0)
  {
   player[p].respawning --;
   if (player[p].respawning <= 0)
    player_respawn(p);
  }
  run_stars(p);
  return;
 }

 player[p].turning = 0;

 if (player[p].recycle > 0)
  player[p].recycle --;

 if (player[p].drive > 0)
  player[p].drive --;

 if (player[p].flap [0] > 0)
  player[p].flap [0] --;
 if (player[p].flap [1] > 0)
  player[p].flap [1] --;

 if (player[p].shield_stock < player[p].max_shield)
 {
  player[p].next_shield += 4 + (player[p].upgrade [U_CHARGE] << 2);
  if (player[p].next_shield >= 12000)
  {
   player[p].next_shield = 0;
   player[p].shield_stock ++;
  }
 }


 if (player[p].shield_time > 0)
 {
  if (player[p].shield_time == 1)
   play_effectwfv(WAV_BLIP, 600, 70);
  player[p].shield_time --;
 }

 get_input(p);


 player[p].x += player[p].x_speed;
 player[p].y += player[p].y_speed;

      int dragged = 1000;
      if (player[p].upgrade [U_AGILITY] == 1)
       dragged = 1009;
      if (player[p].upgrade [U_AGILITY] == 2)
       dragged = 1016;

 player[p].x_speed *= dragged;
 player[p].x_speed >>= 10;
 player[p].y_speed *= dragged;
 player[p].y_speed >>= 10;

// player[p].range = (int) (hypot(hs[0].y - player[p].y, hs[0].x - player[p].x) - hs[0].range_minimum) >> 15;
 player[p].range = hypot(hs[0].y - player[p].y, hs[0].x - player[p].x) - (hs[0].range_minimum);
 if (player[p].range < 0)
  player[p].range = 0;
// player[p].range >>= 15;
 player[p].range /= 25000;
 player[p].angle_to_hs = radians_to_angle(atan2(hs[0].y - player[p].y, hs[0].x - player[p].x) - hs[0].angle) & ANGLE_MASK;
// player[p].angle_to_hs = (player[p].angle - radians_to_angle(hs[0].angle)) & ANGLE_MASK;

  run_stars(p);

  player[p].played_hit = 0;

}

void player_hit(int p, int force)
{

 if (player[p].shield_time > 0 && force == 0)
  return;

 int x = player[p].x;
 int y = player[p].y;
 int i, angle, dist, speed;

 int b;


 for (b = 0; b < NO_EBULLETS; b ++)
 {
     if (ebullet[b].type == EBULLET_NONE)
      continue;

             if (player[0].x > ebullet[b].x - ebullet[b].size - 50000
              && player[0].x < ebullet[b].x + ebullet[b].size + 50000
              && player[0].y > ebullet[b].y - ebullet[b].size - 50000
              && player[0].y < ebullet[b].y + ebullet[b].size + 50000)
              {
               if (hypot(player[0].x - ebullet[b].x, player[0].y - ebullet[b].y) <= 50000 + ebullet[b].size)
               {
                ebullet_explodes(b, 1);
               }
              }
 }

 // now let's do it again to get any new ebullets released by the previous go.
 for (b = 0; b < NO_EBULLETS; b ++)
 {
     if (ebullet[b].type == EBULLET_NONE)
      continue;

             if (player[0].x < ebullet[b].x - ebullet[b].size - 50000
              || player[0].x > ebullet[b].x + ebullet[b].size + 50000
              || player[0].y < ebullet[b].y - ebullet[b].size - 50000
              || player[0].y > ebullet[b].y + ebullet[b].size + 50000)
              {
               if (hypot(player[0].x - ebullet[b].x, player[0].y - ebullet[b].y) <= 50000 + ebullet[b].size)
               {
                ebullet_explodes(b, 1);
               }
              }
 }



 if (player[p].shield_stock > 0 && force == 0)
 {

  //quick_cloud(CLOUD_BALL_COL2, x, y, 0, 0, 70 + grand(20), 2, 0);
  quick_cloud(CLOUD_SMALL_SHOCK, x, y, 0, 0, 30, 2, 0);
  player[p].shield_stock --;
  player[p].shield_time = 75;
  play_effectwfv(WAV_SHIELD, 1000, 250);
  player[p].rocket_burst = 0;
  player[p].rocket_recycle = 0;
  player[p].charge = 0;
  return;
 }


 quick_cloud(CLOUD_BALL, x, y, 0, 0, 50 + grand(20), 0, 0);
 quick_cloud(CLOUD_BALL_COL2, x, y, 0, 0, 70 + grand(20), 0, 0);
 quick_cloud(CLOUD_HUGE_SHOCK, x, y, 0, 0, 50, 0, 0);
 play_effectwfv(WAV_KIL, 600, 200);

 angle = grand(ANGLE_1);

 for (i = 0; i < 40; i ++)
 {
  angle += ANGLE_16 + grand(ANGLE_32);
  dist = 5000;
  speed = 3000 + grand(3000);
  quick_cloud(CLOUD_2BALL, x + xpart(angle, dist), y + ypart(angle, dist), xpart(angle, speed), ypart(angle, speed), 30 + grand(20), 0, 990);
 }

 player[p].alive = 0;
 player[p].respawning = 100;
 player[p].turning = 0;
 player[p].x_speed = 0;
 player[p].y_speed = 0;
 player[p].charge = 0;
 player[p].recycle = 0;

 player[p].ships --;
 if (player[p].ships < 0)
  arena.game_over = 250;



}

void run_stars(int p)
{

 int i;

 int move_angle = ( - ANGLE_4 + radians_to_angle(atan2((player[p].y_speed), (player[p].x_speed))) - player[p].angle) & ANGLE_MASK;
 int move_speed = hypot(player[p].y_speed, player[p].x_speed);

 int x_speed = xpart(move_angle, move_speed);
 int y_speed = ypart(move_angle, move_speed);

// float star_angle;
 int star_dist;
 int angle2;

 float cos_angle = cos(angle_to_radians(player[p].turning));
 float sin_angle = sin(angle_to_radians(player[p].turning));
 float msin_angle = sin(angle_to_radians(player[p].turning)) * -1;
 int xd, yd;
 int window_x = player[p].window_centre_x << 10;
 int window_y = player[p].window_centre_y << 10;

 for (i = 0; i < NO_STARS; i ++)
 {
  star[i].old_x = star[i].x;
  star[i].old_y = star[i].y;

//  star_dist = hypot(star[i].y - (player[p].camera_y << 10), star[i].x - (player[p].camera_x << 10));
  star_dist = hypot(star[i].y - window_y, star[i].x - window_x);

  if (player[p].turning != 0)
  {
   xd = star[i].x - window_x;
   yd = star[i].y - window_y;
   star[i].x = (xd * cos_angle) + (yd * sin_angle) + window_x;
   star[i].y = (xd * msin_angle) + (yd * cos_angle) + window_y;
/*   star_angle = atan2(star[i].y - ((player[p].window_centre_y) << 10), star[i].x - (player[p].window_centre_x << 10));
   star_angle -= angle_to_radians(player[p].turning);
   star[i].x = (player[p].window_centre_x << 10) + cos(star_angle) * star_dist;
   star[i].y = ((player[p].window_centre_y) << 10) + sin(star_angle) * star_dist;*/
  }


//  star[i].x -= (x_speed >> 5) * 8;// * star[i].depth;
//  star[i].y -= (y_speed >> 5) * 8;// * star[i].depth;
  star[i].x -= (x_speed >> 2);// * star[i].depth;
  star[i].y -= (y_speed >> 2);// * star[i].depth;

//  if (star[i].x < (-200 << 10) || star[i].x > ((player[p].camera_x + 500) << 10)
//   || star[i].y < (-200 << 10) || star[i].y > ((player[p].camera_y + 500) << 10))
  if (star_dist > STAR_DIST)
   {
/*    side_placement = grand(1000) - 500;
    star[i].x = (player[p].window_centre_x << 10) + xpart(move_angle, 500000) + xpart(move_angle + ANGLE_4, side_placement << 10);
    star[i].y = (player[p].window_centre_y << 10) + ypart(move_angle, 500000) + ypart(move_angle + ANGLE_4, side_placement << 10);
    star[i].old_x = star[i].x;
    star[i].old_y = star[i].y;*/
    angle2 = grand(ANGLE_2) - ANGLE_2;
    star[i].x = (player[p].window_centre_x << 10) + xpart(angle2, STAR_DIST);
    star[i].y = ((player[p].window_centre_y) << 10) + ypart(angle2, STAR_DIST);
    star[i].old_x = star[i].x;
    star[i].old_y = star[i].y;


    //  star[i].depth = 4 + grand(28);
  //star[i].bright = 2 + star[i].depth / 2;
if (star[i].bright > 15)
 star[i].bright = 15;
 star[i].bright2 = star[i].bright / 3;

    set_star(i);

   }
 }
}



void init_player_new_level(void)
{

    player[0].alive = 1;
    player[0].respawning = 0;

    player[0].max_shield = 1 + player[0].upgrade [U_SHIELD];

    init_player_spawn(0);

    arena.time_left = 50 * (60 + hs[0].stage * 5);

}

void init_player_spawn(int p)
{

    player[p].turning = 0;
    player[p].turn_speed = 0;
    player[p].x_speed = 0;
    player[p].y_speed = 0;

    player[p].recycle = 0;
    player[p].rocket_recycle = 0;
    player[p].rocket_burst = 0;
    player[p].charge = 0;
    player[p].rocket_angle = 0;

    player[p].camera_x = 320;
    player[p].camera_y = 360;
    player[p].window_x = 640;
    player[p].window_y = 480;
    player[p].window_centre_x = 320;
    player[p].window_centre_y = 360;

    player[p].drive = 0;
    player[p].flap [0] = 0;
    player[p].flap [1] = 0;

    player[p].alive = 1;
    player[p].respawning = 0;

    player[p].shield_stock = player[0].max_shield;
    player[p].next_shield = 0;
    player[p].shield_time = 0;

/*    int i;

    for (i = 0; i < SBARS; i ++)
    {
     player[p].sbar_width [i] = 0;
     player[p].sbar_high [i] = 0;
     player[p].sbar_y [i] = 0;
    }*/

}


void player_respawn(int p)
{
    player[p].x = hs[0].x + fxpart(hs[0].angle, 400000);
    player[p].y = hs[0].y + fypart(hs[0].angle, 400000);
    player[p].angle = radians_to_angle(hs[0].angle) + ANGLE_2;
    init_stars(p);

    init_player_spawn(p);

    player[p].shield_time = 60;

}


void init_stars(int p)
{

 int i, angle, dist;

 for (i = 0; i < NO_STARS; i ++)
 {

  angle = grand(ANGLE_1);
  dist = (grand(STAR_DIST) + grand(STAR_DIST)) / 2;
  star[i].x = (player[p].window_centre_x << 10) + xpart(angle, dist);
  star[i].y = (player[p].window_centre_y << 10) + ypart(angle, dist);

//  star[i].x = grand((player[p].window_x + 600) * 1024) - (300 << 10);
//  star[i].y = grand((player[p].window_y + 600) * 1024) - (300 << 10);

  star[i].old_x = star[i].x;
  star[i].old_y = star[i].y;
  star[i].depth = 3;//4 + grand(28);
  star[i].bright = 15 - grand(10);//2 + star[i].depth / 2;


  set_star(i);

/*if (star[i].bright > 15)
 star[i].bright = 15;
    star[i].bright2 = ((star[i].bright-COL_STAR1) / 3) + COL_STAR1;*/

 }


}

void set_star(int i)
{
//    star[i].depth = 3+grand(6);//4 + grand(30);
//    star[i].bright = 15 - grand(13) + star[i].depth / 4;
    star[i].depth = 4 + grand(16);
    star[i].bright = ((star[i].depth - 4) / 2) + COL_STAR1;
//    star[i].bright2 = ((star[i].bright-COL_STAR1) / 2) + COL_STAR1;
    if (star[i].bright > COL_STAR7)
     star[i].bright = COL_STAR7;
    star[i].bright2 = star[i].bright - 2;
    if (star[i].bright2 < COL_STAR1)
     star[i].bright2 = COL_STAR1;

}


