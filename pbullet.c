#include "config.h"

#include <math.h>

#include "allegro.h"

#include "globvars.h"
#include "cloud.h"
#include "hs.h"
#include "sound.h"

#include "stuff.h"



void run_pbullets(void);
void destroy_pbullet(int b);
void drag_pbullet(int b, int drag);
void pbullet_explodes(int b, int hit);
int detect_pbullet_collision(int b);
void pbullet_torque(int b, int p);
void run_pbullet_seeker(int b);
void run_pbullet_rocket(int b);
void run_pbullet_missile(int b);



/*
Initialises the player's bullets. Must be called at start of each game/level.
*/
void init_pbullets(void)
{

 int b;

 for (b = 0; b < NO_PBULLETS; b ++)
 {
  pbullet[b].type = PBULLET_NONE;
 }

}

/*
Creates a bullet of type type and returns the index in the pbullet struct array.
*/
int create_pbullet(int type)
{
  int b;

  for (b = 0; b < NO_PBULLETS; b ++)
  {
      if (pbullet[b].type == PBULLET_NONE)
       break;
      if (b == NO_PBULLETS - 1)
       return -1;

  }

 pbullet[b].type = type;
 pbullet[b].timeout = 0;
 pbullet[b].time = 0;
 pbullet[b].range = player[0].range;
 pbullet[b].direction = player[0].angle_to_hs;
// pbullet[b].new_pbullet = 1;
 return b;

}

/*
Call this from the game loop to make the pbullets work.
*/
void run_pbullets(void)
{

  int b, bh, c, angle, dist, i, speed;
  char no_more_bullet = 0;

  for (b = 0; b < NO_PBULLETS; b ++)
  {
    if (pbullet[b].type == PBULLET_NONE)
     continue;
    no_more_bullet = 0;
    pbullet[b].time ++;
    switch(pbullet[b].type)
    {
        default:
            pbullet[b].x += pbullet[b].x_speed;
            pbullet[b].y += pbullet[b].y_speed;
            break;
        case PBULLET_SCATTER:
            pbullet[b].x += pbullet[b].x_speed;
            pbullet[b].y += pbullet[b].y_speed;
            pbullet[b].size = 5000 + (pbullet[b].status << 7);// / 2;
//            pbullet[b].draw_size = (pbullet[b].timeout << 1);
            pbullet[b].damage = 40 + (pbullet[b].status * 5);
            pbullet[b].force = pbullet[b].damage * 2;
            pbullet[b].timeout ++;
            if (pbullet[b].time > pbullet[b].status2)
             pbullet[b].status --;
            if (pbullet[b].status <= 0)
            {
             destroy_pbullet(b);
             no_more_bullet = 1;
            }
            break;
        case PBULLET_WORM:
            run_pbullet_seeker(b);
            break;
        case PBULLET_ROCKET:
            run_pbullet_rocket(b);
            break;
        case PBULLET_MISSILE:
            run_pbullet_missile(b);
            break;
        case PBULLET_MINE:/*
            for (i = 0; i < MINE_BITS; i ++)
            {
             pbullet[b].bit_angle [i] += pbullet[b].bit_spin [i];
             pbullet[b].bit_angle [i] &= ANGLE_MASK;
             pbullet[b].bit_time [i] --;
             if (pbullet[b].bit_time [i] < 0)
             {
              pbullet[b].bit_time [i] = 40;
              pbullet[b].bit_angle [i] = grand(ANGLE_1);
              pbullet[b].bit_dist [i] = 10 + grand(5);
              if (i & 1)
              {
                pbullet[b].bit_spin [i] = 25 + grand(25);
              }
               else
                pbullet[b].bit_spin [i] = -25 - grand(25);
             }
            }
*/
/*            angle = pbullet[b].angle + ANGLE_4;
            speed = xpart(pbullet[b].timeout * 150, 2500);
            c = simple_cloud(CLOUD_2BALL, pbullet[b].x, pbullet[b].y,
                 pbullet[b].colour, 8 + grand(pbullet[b].draw_size << 2));
            if (c != -1)
            {
             cloud[c].x_speed = pbullet[b].x_speed + xpart(angle, speed);
             cloud[c].y_speed = pbullet[b].y_speed + ypart(angle, speed);
             cloud[c].drag = 950;
            }
            pbullet[b].x += pbullet[b].x_speed;
            pbullet[b].y += pbullet[b].y_speed;
*/

            angle = grand(ANGLE_1);
            dist = grand(pbullet[b].damage << 1);
            speed = 300 + grand(300);
            c = simple_cloud(CLOUD_SMALL, pbullet[b].x + xpart(angle, dist), pbullet[b].y + ypart(angle, dist),
                 pbullet[b].colour, 20 + grand(pbullet[b].draw_size << 2));
            if (c != -1)
            {
             cloud[c].x_speed = pbullet[b].x_speed + xpart(angle, speed);
             cloud[c].y_speed = pbullet[b].y_speed + ypart(angle, speed);
             cloud[c].drag = 960;
            }
            pbullet[b].x += pbullet[b].x_speed;
            pbullet[b].y += pbullet[b].y_speed;
            pbullet[b].angle += 60;

            break;
        case PBULLET_MINE_BLAST:
            for (i = 0; i < MINE_BITS; i ++)
            {
             pbullet[b].bit_angle [i] += pbullet[b].bit_spin [i];
             pbullet[b].bit_angle [i] &= ANGLE_MASK;
             pbullet[b].bit_time [i] --;
//             pbullet[b].bit_dist [i] --;
            }
            break;
        case PBULLET_BASIC:
            angle = grand(ANGLE_1);
            dist = grand(pbullet[b].damage << 3);
            c = simple_cloud(CLOUD_2BALL, pbullet[b].x + xpart(angle, dist), pbullet[b].y + ypart(angle, dist),
                 pbullet[b].colour, 5 + grand(pbullet[b].draw_size << 3));
            if (c != -1)
            {
             cloud[c].x_speed = pbullet[b].x_speed;
             cloud[c].y_speed = pbullet[b].y_speed;
             cloud[c].drag = 960;
            }
            c = simple_cloud(CLOUD_BALL, pbullet[b].x + xpart(angle, dist), pbullet[b].y + ypart(angle, dist),
                 pbullet[b].colour, 3 + grand(pbullet[b].draw_size << 2));
            if (c != -1)
            {
             cloud[c].x_speed = pbullet[b].x_speed;
             cloud[c].y_speed = pbullet[b].y_speed;
             cloud[c].drag = 960;
            }
            pbullet[b].x += pbullet[b].x_speed;
            pbullet[b].y += pbullet[b].y_speed;
            break;
    }


    if (no_more_bullet == 1)
     continue;

    // can't hit anything if damage is 0
    if (pbullet[b].damage > 0)
    {

     bh = detect_pbullet_collision(b);

     if (bh >= 0)
     {
      pbullet_torque(b, bh);
      part[bh].total_range += pbullet[b].range;
      part[bh].range_hits ++;
      part[bh].total_direction += pbullet[b].direction;
      if (player[0].played_hit == 0)
       play_effectwfvxy(WAV_HIT, 1000 - part[bh].size * 15, 50, pbullet[b].x, pbullet[b].y);
      player[0].played_hit = 1;
      hurt_part(bh, pbullet[b].damage);
      pbullet_explodes(b, 1);
      continue;
     }

     if (bh == -2) // hits core
     {
      pbullet_torque(b, bh);
      if (player[0].played_hit == 0)
       play_effectwfvxy(WAV_HIT, 600, 50, pbullet[b].x, pbullet[b].y);
      player[0].played_hit = 1;
      hurt_core(pbullet[b].damage);
      pbullet_explodes(b, 1);
      continue;
     }

    }

    pbullet[b].timeout --;

/*    if (pbullet[b].x <= 0 || pbullet[b].y <= 0
          || pbullet[b].x >= arena.max_x || pbullet[b].y >= arena.max_y)
      pbullet_explodes(b, HIT_WALL, 0);
        else*/
        {
         if (pbullet[b].timeout <= 0)
          pbullet_explodes(b, 0);
        }

  }

}


void drag_pbullet(int b, int drag)
{
 pbullet[b].x_speed *= drag;
 pbullet[b].x_speed >>= 10;
 pbullet[b].y_speed *= drag;
 pbullet[b].y_speed >>= 10;

}

void pbullet_torque(int b, int p)
{

// int speed = ((int) hypot(pbullet[b].y_speed - part[p].move_y, pbullet[b].x_speed - part[p].move_x)) >> 12;
// int force_angle = atan2(pbullet[b].y_speed - part[p].move_y, pbullet[b].x_speed - part[p].move_x);
 int speed = ((int) hypot(pbullet[b].y_speed - hs[0].y_speed, pbullet[b].x_speed - hs[0].x_speed)) >> 12;
 int force_angle = radians_to_angle(atan2(pbullet[b].y_speed - hs[0].y_speed, pbullet[b].x_speed - hs[0].x_speed)) - ANGLE_2;

 apply_torque_to_hs(0, pbullet[b].x, pbullet[b].y, pbullet[b].force * speed * 20, force_angle);

 hs_impulse(0, force_angle + ANGLE_2, pbullet[b].force * speed * 20);

}


int detect_pbullet_collision(int b)
{

// int x2 = pbullet[b].x - pbullet[b].x_speed;
// int y2 = pbullet[b].y - pbullet[b].y_speed;

 if (hs[0].alive == 0)
  return -1;

 if (pbullet[b].x < hs[0].x - 700000
  || pbullet[b].x > hs[0].x + 700000
  || pbullet[b].y < hs[0].y - 700000
  || pbullet[b].y > hs[0].y + 700000)
   return -1;

 int p, size;


 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].type != PART_SHIELD)
   continue;
  size = part[p].size << 10;
  if (part[p].shield > 0)
   size += part[p].shield;
  if (part[p].x - size > pbullet[b].x + pbullet[b].size + 10000
   || part[p].x + size < pbullet[b].x - pbullet[b].size - 10000
   || part[p].y - size > pbullet[b].y + pbullet[b].size + 10000
   || part[p].y + size < pbullet[b].y - pbullet[b].size - 10000)
    continue;

  if (hypot(part[p].x - pbullet[b].x, part[p].y - pbullet[b].y) <= size + pbullet[b].size)
  {
   return p;
  }

 }

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].type == PART_NONE
   || part[p].type == PART_SHIELD)
   continue;
  size = part[p].size << 10;
  if (part[p].x - size > pbullet[b].x + pbullet[b].size + 10000
   || part[p].x + size < pbullet[b].x - pbullet[b].size - 10000
   || part[p].y - size > pbullet[b].y + pbullet[b].size + 10000
   || part[p].y + size < pbullet[b].y - pbullet[b].size - 10000)
    continue;

  if (hypot(part[p].x - pbullet[b].x, part[p].y - pbullet[b].y) <= size + pbullet[b].size)
  {
   return p;
  }

 }

 for (p = 0; p < NO_CORE_CIRCLES; p ++)
 {
  if (hs[0].core_circle_size [p] == 0)
   continue;
  size = hs[0].core_circle_size [p] << 10;
  if (hs[0].core_circle_x [p] - size > pbullet[b].x + pbullet[b].size + 10000
   || hs[0].core_circle_x [p] + size < pbullet[b].x - pbullet[b].size - 10000
   || hs[0].core_circle_y [p] - size > pbullet[b].y + pbullet[b].size + 10000
   || hs[0].core_circle_y [p] + size < pbullet[b].y - pbullet[b].size - 10000)
    continue;

  if (hypot(hs[0].core_circle_x [p] - pbullet[b].x, hs[0].core_circle_y [p] - pbullet[b].y) <= size + pbullet[b].size)
  {
   return -2;
  }

 }



 return -1;

}




void pbullet_explodes(int b, int hit)
{

 static struct pbulletstruct *eb;

 eb = &pbullet[b];

 int x = eb->x;
 int y = eb->y;
 int angle, i, dist, speed, size;

 switch(eb->type)
 {
  case PBULLET_MINE:
   simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, 30 + (pbullet[b].damage >> 7));
   simple_cloud(CLOUD_BALL, x, y, eb->colour, 20 + (pbullet[b].damage >> 8));
   quick_cloud(CLOUD_LARGE_SHOCK, x, y, 0, 0, 50, eb->colour, 0);
   pbullet[b].timeout = 90;
   pbullet[b].x_speed = 0;
   pbullet[b].y_speed = 0;
   pbullet[b].type = PBULLET_MINE_BLAST;
   for (i = 0; i < MINE_BITS; i ++)
   {
    pbullet[b].bit_dist [i] = 1 + grand(20) + grand(20) + grand(20) + grand(pbullet[b].damage >> 7);
    pbullet[b].bit_angle [i] = grand(ANGLE_1);
    pbullet[b].bit_time [i] = 50 + grand(40);

    if (i & 1)
    {
     pbullet[b].bit_spin [i] = 45 + grand(45);
    }
     else
      pbullet[b].bit_spin [i] = -45 - grand(45);
   }

   int freq = 1000 - pbullet[b].damage /9;
   if (freq < 300)
    freq = 300;

   freq = 1000;

   play_effectwfvxy(WAV_PHASEY, freq, 150, pbullet[b].x, pbullet[b].y);

   pbullet[b].damage = 0;

   return; // note this return
  case PBULLET_SHOT:
   simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, 20);
   break;
  case PBULLET_SCATTER:
   if (hit)
    simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, 20 + (pbullet[b].damage >> 6));
   break;
  case PBULLET_WORM:
  case PBULLET_ROCKET:
  case PBULLET_MISSILE:
   if (hit == 1)
    simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, 45);
     else
      simple_cloud(CLOUD_BALL_COL2, x, y, eb->colour, 25);
   break;
  case PBULLET_BASIC:
   size = pbullet[b].damage;
   if (hit == 0)
    size /= 3;
   simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, 20 + (size >> 6));
   simple_cloud(CLOUD_BALL, x, y, eb->colour, 10 + (size >> 5));
   angle = grand(ANGLE_1);

   for (i = 0; i < 15; i ++)
   {
    angle += ANGLE_16 + grand(ANGLE_32);
    dist = 5000;
    speed = 2000 + grand(3000) + (size >> 2);
    quick_cloud(CLOUD_2BALL, x + xpart(angle, dist), y + ypart(angle, dist), xpart(angle, speed), ypart(angle, speed), 15 + grand(15) + (size >> 6), eb->colour, 940);
   }
   break;
 }

// pbullet might not get here if transformed
 destroy_pbullet(b);
}


void run_pbullet_seeker(int b)
{

    int old_x = pbullet[b].x;
    int old_y = pbullet[b].y;

//    pbullet[b].x_speed += xpart(pbullet[b].angle, 300 + (pbullet[b].status2 << 4));
//    pbullet[b].y_speed += ypart(pbullet[b].angle, 300 + (pbullet[b].status2 << 4));
    pbullet[b].x_speed += xpart(pbullet[b].angle, 500);
    pbullet[b].y_speed += ypart(pbullet[b].angle, 500);
    pbullet[b].x += pbullet[b].x_speed;
    pbullet[b].y += pbullet[b].y_speed;

//    drag_pbullet(b, 890);
//    drag_pbullet(b, 950);
    drag_pbullet(b, 950);

    int c = create_cloud(CLOUD_WORM_TRAIL);
    if (c != -1)
    {
     cloud[c].x = pbullet[b].x;
     cloud[c].y = pbullet[b].y;
     cloud[c].x2 = old_x;
     cloud[c].y2 = old_y;
     cloud[c].x_speed = 0;
     cloud[c].y_speed = 0;
     cloud[c].timeout = pbullet[b].status;// + pbullet[b].level;
     cloud[c].fangle = atan2(pbullet[b].y - old_y, pbullet[b].x - old_x);
     cloud[c].status = (int) hypot(pbullet[b].y - old_y, pbullet[b].x - old_x) >> 10;
    }

  {
//    if (target_x != -1 || target_y != -1)
     if (hs[0].alive)
      pbullet[b].angle = turn_towards_xy(pbullet[b].x, pbullet[b].y, hs[0].x, hs[0].y, pbullet[b].angle, 22 + (pbullet[b].status2 >> 0)); // 22
  }

    pbullet[b].timeout --;


    if (pbullet[b].timeout <= 0)
    {
     pbullet_explodes(b, 0);
     destroy_pbullet(b);
    }

}


void run_pbullet_rocket(int b)
{


            int angle = grand(ANGLE_1);
            int dist = grand(pbullet[b].damage << 3);
            int c = simple_cloud(CLOUD_2BALL, pbullet[b].x + xpart(angle, dist), pbullet[b].y + ypart(angle, dist),
                 pbullet[b].colour, 5 + grand(pbullet[b].draw_size << 2));
            if (c != -1)
            {
             cloud[c].x_speed = pbullet[b].x_speed;
             cloud[c].y_speed = pbullet[b].y_speed;
             cloud[c].drag = 990;
            }

    if (pbullet[b].time > 30)
    {
     pbullet[b].x_speed += xpart(pbullet[b].angle, 300 + (pbullet[b].status2 << 4));
     pbullet[b].y_speed += ypart(pbullet[b].angle, 300 + (pbullet[b].status2 << 4));
    }
    if (pbullet[b].time == 30)
      play_effectwfvxy_xs_ys(WAV_WHOOSH, 1000 + (player[0].upgrade [U_POWER] * 150), 100, pbullet[b].x, pbullet[b].y, pbullet[b].x_speed + xpart(pbullet[b].angle, 1300), pbullet[b].y_speed + ypart(pbullet[b].angle, 1300));

    pbullet[b].x += pbullet[b].x_speed;
    pbullet[b].y += pbullet[b].y_speed;

    drag_pbullet(b, 950);

  {
//    if (target_x != -1 || target_y != -1)
//     pbullet[b].angle = turn_towards_xy(pbullet[b].x, pbullet[b].y, hs[0].x, hs[0].y, pbullet[b].angle, 22 + (pbullet[b].status2 >> 0)); // 22
  }

    pbullet[b].timeout --;


    if (pbullet[b].timeout <= 0)
    {
     pbullet_explodes(b, 0);
     destroy_pbullet(b);
    }

}


void run_pbullet_missile(int b)
{

    int old_x = pbullet[b].x;
    int old_y = pbullet[b].y;





    if (pbullet[b].time > 30)
    {
     pbullet[b].x_speed += xpart(pbullet[b].angle, 300 + (pbullet[b].status2 << 5));
     pbullet[b].y_speed += ypart(pbullet[b].angle, 300 + (pbullet[b].status2 << 5));
     drag_pbullet(b, 970);
     if (hs[0].alive)
      pbullet[b].angle = turn_towards_xy(pbullet[b].x, pbullet[b].y, hs[0].x, hs[0].y, pbullet[b].angle, 8); // 22
    }
    if (pbullet[b].time == 30)
      play_effectwfvxy_xs_ys(WAV_WHOOSH, 1300 + (player[0].upgrade [U_POWER] * 150), 100, pbullet[b].x, pbullet[b].y, pbullet[b].x_speed + xpart(pbullet[b].angle, 1300), pbullet[b].y_speed + ypart(pbullet[b].angle, 1300));


    pbullet[b].x += pbullet[b].x_speed;
    pbullet[b].y += pbullet[b].y_speed;

    int c = create_cloud(CLOUD_ROCKET_TRAIL);
    if (c != -1)
    {
     cloud[c].x = pbullet[b].x;
     cloud[c].y = pbullet[b].y;
     cloud[c].x2 = old_x;
     cloud[c].y2 = old_y;
     cloud[c].x_speed = 0;
     cloud[c].y_speed = 0;
     cloud[c].timeout = pbullet[b].status;// + pbullet[b].level;
     cloud[c].fangle = atan2(pbullet[b].y - old_y, pbullet[b].x - old_x);
     cloud[c].status = (int) hypot(pbullet[b].y - old_y, pbullet[b].x - old_x) >> 10;
    }



  {
//    if (target_x != -1 || target_y != -1)
  }

    pbullet[b].timeout --;


    if (pbullet[b].timeout <= 0)
    {
     pbullet_explodes(b, 0);
     destroy_pbullet(b);
    }

}



void destroy_pbullet(int b)
{
  pbullet[b].type = PBULLET_NONE;
}


