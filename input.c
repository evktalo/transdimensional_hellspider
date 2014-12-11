

#include "config.h"

#include "allegro.h"

#include <math.h>

#include "globvars.h"

#include "stuff.h"

#include "palette.h"
#include "display.h"
#include "level.h"
#include "pbullet.h"
#include "cloud.h"
//#include "entity.h"

#include "sound.h"

#include <string.h>
extern RGB palet [256];

void player_fires(int p, int f);
void player_fires_secondary(int p, int charge);
void fire_rockets(int p);
void fire_missiles(int p);

int worm_angle_even [6] =
{
ANGLE_2 - ANGLE_16,
ANGLE_2 + ANGLE_16,
ANGLE_2 - ANGLE_8,
ANGLE_2 + ANGLE_8,
ANGLE_2 - ANGLE_4,
ANGLE_2 + ANGLE_4

};

int worm_angle_odd [5] =
{
ANGLE_2,
ANGLE_2 - ANGLE_8 + ANGLE_32,
ANGLE_2 + ANGLE_8 - ANGLE_32,
ANGLE_2 - ANGLE_4 + ANGLE_32,
ANGLE_2 + ANGLE_4 - ANGLE_32

};

void get_input(int p)
{

 int c;

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

  strcpy(sfile, "sshot");
  {
    char buf[20];
    sprintf(buf, "%d", scrs);
    strcat(sfile, buf);
  }
  strcat(sfile, ".bmp");
  save_bitmap(sfile, scrshot_bmp, palet);
  rectfill(screen, 0, 0, 640, 480, COL_WHITE);
//  clear_to_color(screen, COL_WHITE);
  scrs ++;
  sshot_counter = 15;
  destroy_bitmap(scrshot_bmp);
 }

//#define DEBUGGING

#ifdef DEBUGGING
 if (key [KEY_Q])
 {
    player[p].x_speed += xpart(player[p].angle, 1000);
    player[p].y_speed += ypart(player[p].angle, 1000);
 }

 if (key [KEY_W])
  player[p].shield_time = 150;

#endif


    if (player[p].respawning > 0)
    {
     player[p].respawning --;
     return;
    }

    int xc = 0, yc = 0;

//  int i;

   char key_left = 0;
   char key_right = 0;
   char key_up = 0;
   char key_down = 0;
   char key_fire1 = 0;
   char key_fire2 = 0;

 if (options.key_or_joy == 1)
 {
   poll_joystick();

   key_fire1 = joy[0].button[options.joy_button [0]].b;
   key_fire2 = joy[0].button[options.joy_button [1]].b;

   key_up = 0;
   if (joy[0].button[options.joy_button [2]].b)
    key_up = 1;

   key_down = 0;
   if (joy[0].button[options.joy_button [3]].b)
    key_down = 1;
   // up and down can also be set with the stick

   xc = joy[0].stick[options.joy_stick].axis[0].pos * options.joy_sensitivity;
   yc = joy[0].stick[options.joy_stick].axis[1].pos * options.joy_sensitivity;

//   int jangle = radians_to_angle(atan2(yc, xc));

//   jangle &= 1023;

   if (xc != 0 || yc != 0)
   {
     if (yc < -100)
      key_up = 1;
     if (yc > 100)
      key_down = 1;
     if (xc < -100)
      key_left = 1;
     if (xc > 100)
      key_right = 1;
   }

/*
// This is all still set up for direct movement, not inertia
   xc = joy[0].stick[options.joy_stick].axis[0].pos * options.joy_sensitivity;
   yc = joy[0].stick[options.joy_stick].axis[1].pos * options.joy_sensitivity;

   int jangle = radians_to_angle(atan2(yc, xc));
   jangle &= 1023;
   if (xc == 0 && yc == 0)
    jangle = ANGLE_4;
   int dist = hypot(yc, xc);

   if (dist > 6000)
    dist = 6000;

   if (joy[0].button[options.joy_button [2]].b)
    dist /= 2;

   xc = xpart(jangle, dist);
   yc = ypart(jangle, dist);

  if (xc < -6000)
   xc = -6000;
  if (yc < -6000)
   yc = -6000;
  if (xc > 6000)
   xc = 6000;
  if (yc > 6000)
   yc = 6000;


     player[p].x += xc;
     player[p].y += yc;
     if (player[p].x < X_MIN)
       player[p].x = X_MIN;
     if (player[p].y < Y_MIN)
       player[p].y = Y_MIN;
     if (player[p].x > X_MAX)
       player[p].x = X_MAX;
     if (player[p].y > Y_MAX)
       player[p].y = Y_MAX;

*/
 }
  else
  {

//   int moving = MOVE_NO;

   key_left = key [player[p].ckey [CKEY_LEFT]];
   key_right = key [player[p].ckey [CKEY_RIGHT]];
   key_up = key [player[p].ckey [CKEY_UP]];
   key_down = key [player[p].ckey [CKEY_DOWN]];
   key_fire1 = key [player[p].ckey [CKEY_FIRE1]];
   key_fire2 = key [player[p].ckey [CKEY_FIRE2]];
//   char key_fire3 = key [player[p].ckey [CKEY_FIRE3]];
  }
//#define PLAYER_DRAG 1000

#define TURNING_MAX 30
#define TURNING_UPGRADE 4
// was 24/4
#define DRIVE_BASE 200
#define DRIVE_UPGRADE 30

   if (key_left)
   {
    player[p].turn_speed -= 4 + player[p].upgrade [U_AGILITY];
    if (player[p].turn_speed < -TURNING_MAX - (player[p].upgrade [U_AGILITY] * TURNING_UPGRADE))
     player[p].turn_speed = -TURNING_MAX - (player[p].upgrade [U_AGILITY] * TURNING_UPGRADE);
    player[p].flap [0] += 2;
    if (player[p].flap [0] > 10)
     player[p].flap [0] = 10;

   }
    else
    {
     if (key_right)
     {
      player[p].turn_speed += 4 + player[p].upgrade [U_AGILITY];
      if (player[p].turn_speed > TURNING_MAX + (player[p].upgrade [U_AGILITY] * TURNING_UPGRADE))
       player[p].turn_speed = TURNING_MAX + (player[p].upgrade [U_AGILITY] * TURNING_UPGRADE);
      player[p].flap [1] += 2;
      if (player[p].flap [1] > 10)
       player[p].flap [1] = 10;
     }
    }

   if (!key_left && !key_right)
   {
    if (player[p].turn_speed < 0)
     player[p].turn_speed += 4;
    if (player[p].turn_speed > 0)
     player[p].turn_speed -= 4;
    if (player[p].turn_speed < 4 && player[p].turn_speed > -4)
     player[p].turn_speed = 0;

   }

   if (key_up)
   {
    player[p].x_speed += xpart(player[p].angle, DRIVE_BASE + (player[p].upgrade [U_AGILITY] * DRIVE_UPGRADE));
    player[p].y_speed += ypart(player[p].angle, DRIVE_BASE + (player[p].upgrade [U_AGILITY] * DRIVE_UPGRADE));
    player[p].drive += 2;
    if (player[p].drive > 10)
     player[p].drive = 10;
    if (counter % 8 == 0)
     play_effectwfv(WAV_DRIVE, 300, 20);

    if (counter & 1)
    {

//      c = simple_cloud(CLOUD_BALL_COL3, player[p].x - xpart(player[p].angle, 22<<10), player[p].y - ypart(player[p].angle, 22<<10), 0, 10 + grand(1 + player[p].drive * 2));

     if (c != -1)
     {
/*      cloud[c].x_speed = player[p].x_speed - xpart(player[p].angle + grand(ANGLE_16) - ANGLE_32, 3500);
      cloud[c].y_speed = player[p].y_speed - ypart(player[p].angle + grand(ANGLE_16) - ANGLE_32, 3500);
      cloud[c].drag = 1000;*/
     }
    }

   }
    else
     if (key_down)
     {
      int dragged = 1000;
      player[p].x_speed *= dragged;
      player[p].x_speed >>= 10;
      player[p].y_speed *= dragged;
      player[p].y_speed >>= 10;
      player[p].flap [0] += 2;
      if (player[p].flap [0] > 10)
       player[p].flap [0] = 10;
      player[p].flap [1] += 2;
      if (player[p].flap [1] > 10)
       player[p].flap [1] = 10;
     }

   if (key_fire1 && !key_fire2 && player[p].shield_time == 0)
    player_fires(p, 0);

   if (key_fire2 && !key_fire1 && player[p].rocket_burst == 0 && player[p].rocket_recycle == 0 && player[p].shield_time == 0)
   {
    if (player[0].charge < 10000 && player[0].charge > 9909)
     play_effectwfv(WAV_BLIP, 1000, 70);
    player[0].charge += 90;
    if (player[0].charge > 10000)
     player[0].charge = 10000;

   }
    else
    {
     if (player[0].charge > 0)
     {
      player_fires_secondary(p, player[0].charge);
     }
    }

  player[p].turning = player[p].turn_speed;
  player[p].angle += player[p].turn_speed;


//  } // end keyboard control

 if (player[p].weapon == WEAPON_ROCKET)
  fire_rockets(p);

 if (player[p].weapon == WEAPON_MISSILE)
  fire_missiles(p);

}

void player_fires(int p, int f)
{

 if (player[p].recycle > 0)
  return;

 int b, i;
//#define DEBUG_SHOT

#ifdef DEBUG_SHOT

 if (f == 1)
 {
  for (i = 0; i < 2; i ++)
  {

  b = create_pbullet(PBULLET_SHOT);

 if (b != -1)
 {
   pbullet[b].x = player[p].x + xpart(player[p].angle, 8000);
   pbullet[b].y = player[p].y + ypart(player[p].angle, 8000);
   if (i == 0)
   {
    pbullet[b].x += xpart(player[p].angle - ANGLE_4, 3000);
    pbullet[b].y += ypart(player[p].angle - ANGLE_4, 3000);
   }
    else
    {
     pbullet[b].x += xpart(player[p].angle + ANGLE_4, 3000);
     pbullet[b].y += ypart(player[p].angle + ANGLE_4, 3000);
    }
   pbullet[b].x_speed = player[p].x_speed + xpart(player[p].angle, 9000);
   pbullet[b].y_speed = player[p].y_speed + ypart(player[p].angle, 9000);
   pbullet[b].timeout = 30;
   pbullet[b].colour = 2;
   pbullet[b].angle = player[p].angle;
   pbullet[b].size = 5000;
   pbullet[b].damage = 200;
   pbullet[b].force = 100;


  }

  player[p].recycle = 10;
 }

  return;
 }

#endif


  for (i = 0; i < 2; i ++)
  {

  b = create_pbullet(PBULLET_SHOT);

 if (b != -1)
 {
   pbullet[b].x = player[p].x + xpart(player[p].angle, 8000);
   pbullet[b].y = player[p].y + ypart(player[p].angle, 8000);
   if (i == 0)
   {
    pbullet[b].x += xpart(player[p].angle - ANGLE_4, 4000);
    pbullet[b].y += ypart(player[p].angle - ANGLE_4, 4000);
   }
    else
    {
     pbullet[b].x += xpart(player[p].angle + ANGLE_4, 4000);
     pbullet[b].y += ypart(player[p].angle + ANGLE_4, 4000);
    }
   pbullet[b].x_speed = player[p].x_speed + xpart(player[p].angle, 7500 + player[p].upgrade [U_RANGE] * 1600);
   pbullet[b].y_speed = player[p].y_speed + ypart(player[p].angle, 7500 + player[p].upgrade [U_RANGE] * 1600);
   pbullet[b].timeout = 30;
   pbullet[b].colour = 0;
   if (player[p].upgrade [U_POWER] == 2)
    pbullet[b].colour = 2;
   pbullet[b].angle = player[p].angle;
   pbullet[b].size = 5000;
   pbullet[b].damage = 200 + player[p].upgrade [U_POWER] * 70;
   pbullet[b].force = 100;
   pbullet[b].status = 3;
   if (player[p].upgrade [U_POWER] == 1)
    pbullet[b].status = 4;

/*    c = simple_cloud(CLOUD_BALL_COL2, pbullet[b].x, pbullet[b].y, 0, 15);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
    }*/
/*    c = simple_cloud(CLOUD_BALL_COL2, pbullet[b].x, pbullet[b].y, 0, 20);
    if (c != -1)
    {
     cloud[c].x_speed = pbullet[b].x_speed;
     cloud[c].y_speed = pbullet[b].y_speed;
    }*/
  }
  play_effectwfv(WAV_FIRE, 1200 - (player[p].upgrade [U_POWER] * 150), 70);
  player[p].recycle = 10 - player[p].upgrade [U_POWER];
 }




}


void player_fires_secondary(int p, int charge)
{

 if (charge < 2000
  || player[p].shield_time > 0)
 {
  player[p].charge = 0;
  return;
 }

 int b, c, i, angle, dist, speed, number;

 float power_factor = (2 + player[p].upgrade [U_POWER]) / 2;
// float range_factor = (2 + player[p].upgrade [U_RANGE]) / 2;


 switch(player[p].weapon)
 {
  case WEAPON_BASIC:

  b = create_pbullet(PBULLET_BASIC);

 if (b != -1)
 {
   pbullet[b].x = player[p].x + xpart(player[p].angle, 8000);
   pbullet[b].y = player[p].y + ypart(player[p].angle, 8000);
   pbullet[b].x_speed = player[p].x_speed + xpart(player[p].angle, 7500 + player[p].upgrade [U_RANGE] * 1600);
   pbullet[b].y_speed = player[p].y_speed + ypart(player[p].angle, 7500 + player[p].upgrade [U_RANGE] * 1600);
   pbullet[b].timeout = 40;
   pbullet[b].colour = 2;
   pbullet[b].angle = player[p].angle;
//   pbullet[b].size = 5000 + (charge - 1500);
   pbullet[b].size = 3000 + (charge - 1500) / 2;
   pbullet[b].size *= 3 + player[p].upgrade [U_POWER];
   pbullet[b].size /= 3;
//   pbullet[b].draw_size = 5 + (charge - 1500) / 2000;
   pbullet[b].draw_size = 3 + (charge - 1500) / 4000;
   pbullet[b].draw_size *= 3 + player[p].upgrade [U_POWER];
   pbullet[b].draw_size /= 3;
   pbullet[b].damage = (charge - 1500) / 4;
   pbullet[b].damage *= power_factor;
   pbullet[b].force = (charge - 1500) / 8;
   pbullet[b].force *= power_factor;

    c = simple_cloud(CLOUD_BALL_COL2, pbullet[b].x, pbullet[b].y, 2, 25 + (charge - 1500) / 500);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
    }
    c = simple_cloud(CLOUD_BALL, pbullet[b].x, pbullet[b].y, 2, 20 + (charge - 1500) / 700);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
    }

    quick_cloud(CLOUD_SPARSE_FLARE, pbullet[b].x, pbullet[b].y,
     player[0].x_speed + xpart(pbullet[b].angle + ANGLE_3 + ANGLE_16, 7000),
     player[0].y_speed + ypart(pbullet[b].angle + ANGLE_3 + ANGLE_16, 7000), 20 + (charge - 1500) / 700, 2, 960);

    quick_cloud(CLOUD_SPARSE_FLARE, pbullet[b].x, pbullet[b].y,
     player[0].x_speed + xpart(pbullet[b].angle - ANGLE_3 - ANGLE_16, 7000),
     player[0].y_speed + ypart(pbullet[b].angle - ANGLE_3 - ANGLE_16, 7000), 20 + (charge - 1500) / 700, 2, 960);


  }
  play_effectwfv(WAV_BASIC, 2200 - (charge /15), 170);
  break;

  case WEAPON_SCATTER:

  number = ((player[p].charge - 1500) / 500) * power_factor;
  dist = (number << 3) + 10;
  for (i = 0; i < number; i ++)
  {

  b = create_pbullet(PBULLET_SCATTER);
  angle = player[p].angle + grand(dist) - grand(dist);
  speed = 3000 + grand(700) + grand(700) + grand(700);
  speed *= 2 + player[p].upgrade [U_RANGE];
  speed /= 2;

  if (b != -1)
  {
   pbullet[b].x = player[p].x + xpart(player[p].angle, 8000);
   pbullet[b].y = player[p].y + ypart(player[p].angle, 8000);
   pbullet[b].x_speed = player[p].x_speed + xpart(angle, speed);
   pbullet[b].y_speed = player[p].y_speed + ypart(angle, speed);
   pbullet[b].status = 20 + grand(10) + grand(10) + grand(10);
   pbullet[b].status2 = 10 + grand(10) + grand(10);
   pbullet[b].timeout = 5;
   pbullet[b].colour = 2;
   pbullet[b].angle = player[p].angle;
//   pbullet[b].size = 5000 + (charge - 1500);// / 2;
//   pbullet[b].draw_size = 5 + (charge - 1500) / 2000;
//   pbullet[b].damage = (charge - 1500) / 5;
//   pbullet[b].force = (charge - 1500) / 10;
// these properties are calculated in run_pbullets as they change as bullet shrinks


   }
  }

    c = simple_cloud(CLOUD_BALL, player[p].x + xpart(player[p].angle, 8000), player[p].y + ypart(player[p].angle, 8000), 2, 20 + (charge - 1500) / 700);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
    }

    quick_cloud(CLOUD_SPARSE_FLARE, player[p].x + xpart(player[p].angle, 8000), player[p].y + ypart(player[p].angle, 8000),
     player[0].x_speed + xpart(player[p].angle + ANGLE_2, 7000),
     player[0].y_speed + ypart(player[p].angle + ANGLE_2, 7000), 20 + (charge - 1500) / 700, 2, 960);

  play_effectwfv(WAV_SPAWNA, 1200 - (charge /30), 120);

  break;

  case WEAPON_MINE:

  b = create_pbullet(PBULLET_MINE);

 if (b != -1)
 {
   pbullet[b].x = player[p].x + xpart(player[p].angle, 8000);
   pbullet[b].y = player[p].y + ypart(player[p].angle, 8000);
   pbullet[b].x_speed = player[p].x_speed + xpart(player[p].angle, 300);
   pbullet[b].y_speed = player[p].y_speed + ypart(player[p].angle, 300);
   pbullet[b].timeout = 90 + 45 * player[p].upgrade [U_RANGE];
   pbullet[b].colour = 2;
   pbullet[b].angle = player[p].angle;
   pbullet[b].size = 5000 + (charge - 1500);// / 2;
   pbullet[b].draw_size = 5 + (charge - 1500) / 2000;
   pbullet[b].damage = (charge - 1500) * 0.7;
   pbullet[b].damage *= power_factor;
   pbullet[b].force = (charge - 1500) / 5;
/*   for (i = 0; i < MINE_BITS; i ++)
   {
    pbullet[b].bit_dist [i] = 10 + grand(5);
    pbullet[b].bit_angle [i] = grand(ANGLE_1);
    pbullet[b].bit_time [i] = grand(40);

    if (i & 1)
    {
     pbullet[b].bit_spin [i] = 25 + grand(25);
    }
     else
      pbullet[b].bit_spin [i] = -25 - grand(25);
   }*/

    c = simple_cloud(CLOUD_BALL_COL2, pbullet[b].x, pbullet[b].y, 2, 25 + (charge - 1500) / 500);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
    }
    c = simple_cloud(CLOUD_BALL, pbullet[b].x, pbullet[b].y, 2, 20 + (charge - 1500) / 700);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
    }
 angle = grand(ANGLE_1);

 for (i = 0; i < 20; i ++)
 {
  angle += ANGLE_8 + grand(ANGLE_16);
  dist = 4000;
  speed = 2000 + grand(2000) + (charge - 1500) / 5;
  quick_cloud(CLOUD_2BALL, pbullet[b].x + xpart(angle, dist), pbullet[b].y + ypart(angle, dist), pbullet[b].x_speed + xpart(angle, speed), pbullet[b].y_speed + ypart(angle, speed), 10 + grand(10) + (charge - 1500) / 1000, 2, 920);
 }

  }
  play_effectwfv(WAV_MINE, 1200 - (charge /30), 170);
  break;

  case WEAPON_WORM:
  number = (player[p].charge - 1500) / 1417;
  if (player[p].charge == 10000)
   number = 6;
  if (number == 0)
   number = 1;

  for (i = 0; i < number; i++)
  {

  b = create_pbullet(PBULLET_WORM);
  if (number & 1)
   angle = worm_angle_odd [i];
    else
     angle = worm_angle_even [i];

 if (b != -1)
 {
   pbullet[b].x = player[p].x + xpart(player[p].angle + angle, 8000);
   pbullet[b].y = player[p].y + ypart(player[p].angle + angle, 8000);
   pbullet[b].x_speed = player[p].x_speed + xpart(player[p].angle + angle, 9000);
   pbullet[b].y_speed = player[p].y_speed + ypart(player[p].angle + angle, 9000);
   pbullet[b].timeout = 170 + player[p].upgrade [U_RANGE] * 50;
   pbullet[b].colour = 0;
   pbullet[b].angle = player[p].angle + angle;
   pbullet[b].size = 5000;// + (charge - 1500);// / 2;
   pbullet[b].draw_size = 5;// + (charge - 1500) / 2000;
   pbullet[b].damage = 400 * power_factor;//(charge - 1500) / 5;
   pbullet[b].force = 300;//(charge - 1500) / 10;
   pbullet[b].status = 10;// + (part[p].size >> 2);
   pbullet[b].status2 = 52;//part[p].size;

    c = simple_cloud(CLOUD_BALL_COL2, pbullet[b].x, pbullet[b].y, 0, 35);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
     cloud[c].drag = 950;
    }
    c = simple_cloud(CLOUD_BALL, pbullet[b].x, pbullet[b].y, 0, 30);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
     cloud[c].drag = 950;
    }


  }
  }
  play_effectwfv(WAV_WORM2, 700 - (charge /30), 170);
  break;
  case WEAPON_ROCKET:
  player[p].rocket_burst = (player[p].charge - 1500) / 708;
  if (player[p].charge == 10000)
   player[p].rocket_burst = 12;
  if (player[p].rocket_burst == 0)
   player[p].rocket_burst = 1;
  player[p].rocket_recycle = 1;
  break;
  case WEAPON_MISSILE:
  player[p].rocket_burst = (player[p].charge - 1500) / 1417;
  if (player[p].charge == 10000)
   player[p].rocket_burst = 6;
  if (player[p].rocket_burst == 0)
   player[p].rocket_burst = 1;
  player[p].rocket_recycle = 1;
  break;

}

player[p].charge = 0;


}


void fire_rockets(int p)
{

 int b, c, angle;

 if (player[p].rocket_recycle > 0)
 {
  player[p].rocket_recycle --;
  if (player[p].rocket_recycle == 0)
  {
//  play_effectwfv(WAV_BANG5, 2000 + (player[p].upgrade [U_POWER] * 150), 100);

   if (player[p].rocket_angle == ANGLE_2 + ANGLE_8)
    player[p].rocket_angle = ANGLE_2 - ANGLE_8;
     else
      player[p].rocket_angle = ANGLE_2 + ANGLE_8;

   angle = player[p].rocket_angle;

   b = create_pbullet(PBULLET_ROCKET);

   if (b != -1)
   {
    pbullet[b].x = player[p].x + xpart(player[p].angle + angle, 8000);
    pbullet[b].y = player[p].y + ypart(player[p].angle + angle, 8000);
    pbullet[b].x_speed = player[p].x_speed + xpart(player[p].angle + angle, 4000 + player[p].upgrade [U_RANGE] * 2000);
    pbullet[b].y_speed = player[p].y_speed + ypart(player[p].angle + angle, 4000 + player[p].upgrade [U_RANGE] * 2000);
    pbullet[b].timeout = 200;
    pbullet[b].colour = 1;
    pbullet[b].angle = player[p].angle;
    if (player[p].rocket_angle == ANGLE_2 + ANGLE_8)
     pbullet[b].angle += ANGLE_64;
      else
       pbullet[b].angle -= ANGLE_64;
    pbullet[b].angle += grand(ANGLE_128) - grand(ANGLE_128);
    pbullet[b].size = 5000;// + (charge - 1500);// / 2;
    pbullet[b].draw_size = 5 + player[p].upgrade [U_POWER]*2;// + (charge - 1500) / 2000;
    pbullet[b].damage = 250;
    pbullet[b].damage *= 2 + player[p].upgrade [U_POWER];
    pbullet[b].damage /= 2;
    pbullet[b].force = 300;//(charge - 1500) / 10;
    pbullet[b].status = 6;// + (part[p].size >> 2);
    pbullet[b].status2 = 22;//part[p].size;

    c = simple_cloud(CLOUD_BALL_COL2, pbullet[b].x, pbullet[b].y, 1, 35);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
     cloud[c].drag = 950;
    }
    c = simple_cloud(CLOUD_BALL, pbullet[b].x, pbullet[b].y, 1, 30);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
     cloud[c].drag = 950;
    }


  }


   player[p].rocket_burst --;
   if (player[p].rocket_burst > 0)
    player[p].rocket_recycle = 8;
  }
 }



}



void fire_missiles(int p)
{

 int b, c, angle;

 if (player[p].rocket_recycle > 0)
 {
  player[p].rocket_recycle --;
  if (player[p].rocket_recycle == 0)
  {

   if (player[p].rocket_angle == ANGLE_2 + ANGLE_8 + ANGLE_32)
    player[p].rocket_angle = ANGLE_2 - ANGLE_8 - ANGLE_32;
     else
      player[p].rocket_angle = ANGLE_2 + ANGLE_8 + ANGLE_32;

   angle = player[p].rocket_angle;

   b = create_pbullet(PBULLET_MISSILE);

   if (b != -1)
   {
    pbullet[b].x = player[p].x + xpart(player[p].angle + angle, 8000);
    pbullet[b].y = player[p].y + ypart(player[p].angle + angle, 8000);
    pbullet[b].x_speed = player[p].x_speed + xpart(player[p].angle + angle, 2000);
    pbullet[b].y_speed = player[p].y_speed + ypart(player[p].angle + angle, 2000);
    pbullet[b].timeout = 400 + player[p].upgrade [U_RANGE] * 200;
    pbullet[b].colour = 1;
    pbullet[b].angle = player[p].angle;// + angle;
    pbullet[b].size = 5000;// + (charge - 1500);// / 2;
    pbullet[b].draw_size = 4 + (player[p].upgrade [U_POWER]<<1);// + (charge - 1500) / 2000;
    pbullet[b].damage = 500;//(charge - 1500) / 5;
    pbullet[b].damage *= 2 + player[p].upgrade [U_POWER];
    pbullet[b].damage /= 2;
    pbullet[b].force = 300;//(charge - 1500) / 10;
    pbullet[b].force *= 2 + player[p].upgrade [U_POWER];
    pbullet[b].force /= 2;
    pbullet[b].status = 6;// + (part[p].size >> 2);
    pbullet[b].status2 = 6;//part[p].size;

    c = simple_cloud(CLOUD_BALL_COL2, pbullet[b].x, pbullet[b].y, 1, 35);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
     cloud[c].drag = 950;
    }
    c = simple_cloud(CLOUD_BALL, pbullet[b].x, pbullet[b].y, 1, 30);
    if (c != -1)
    {
     cloud[c].x_speed = player[p].x_speed;
     cloud[c].y_speed = player[p].y_speed;
     cloud[c].drag = 950;
    }


  }


   player[p].rocket_burst --;
   if (player[p].rocket_burst > 0)
    player[p].rocket_recycle = 24;
  }
 }


}



void init_joystick(void)
{

   options.joystick = 0;

   if (install_joystick(JOY_TYPE_AUTODETECT) != 0)
      return;

   if (num_joysticks == 0)
    return;

//   if (options.joy_analogue)
//    analogmode = TRUE;
//     else


   options.joystick = 1;

}

