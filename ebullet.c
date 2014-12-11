
#include "config.h"

#include <math.h>

#include "allegro.h"

#include "globvars.h"
#include "cloud.h"
#include "pbullet.h"
#include "sound.h"

#include "stuff.h"



void run_ebullets(void);
void destroy_ebullet(int b);
void drag_ebullet(int b, int drag);
void ebullet_explodes(int b, int hit, int effect);
int detect_ebullet_collision(int b);
void accel_ebullet(int b, int accel);
void run_ebullet_seeker(int b);
void run_ebullet_eel(int b);
void run_ebullet_rocket(int b);



/*
Initialises the player's bullets. Must be called at start of each game/level.
*/
void init_ebullets(void)
{

 int b;

 for (b = 0; b < NO_EBULLETS; b ++)
 {
  ebullet[b].type = EBULLET_NONE;
 }

}

/*
Creates a bullet of type type and returns the index in the pbullet struct array.
*/
int create_ebullet(int type)
{
  int b;

  for (b = 0; b < NO_EBULLETS; b ++)
  {
      if (ebullet[b].type == EBULLET_NONE)
       break;
      if (b == NO_EBULLETS - 1)
       return -1;

  }

 ebullet[b].type = type;
 ebullet[b].timeout = 0;
 ebullet[b].time = 0;
 ebullet[b].x_speed = 0;
 ebullet[b].y_speed = 0;
// pbullet[b].new_pbullet = 1;
 return b;

}

/*
Call this from the game loop to make the pbullets work.
*/
void run_ebullets(void)
{

  int b;
  char no_more_bullet = 0;
  int i;

  for (b = 0; b < NO_EBULLETS; b ++)
  {
    if (ebullet[b].type == EBULLET_NONE)
     continue;
    no_more_bullet = 0;
    ebullet[b].time ++;
    switch(ebullet[b].type)
    {

        default:
            ebullet[b].x += ebullet[b].x_speed;
            ebullet[b].y += ebullet[b].y_speed;
            break;
        case EBULLET_MISSILE_BLAST:
         ebullet[b].status -= ebullet[b].status2;
         if (ebullet[b].status < 0)
         {
          destroy_ebullet(b);
          no_more_bullet = 1;
         }
        break;
        case EBULLET_BEAM:
            break;
        case EBULLET_SEEKER:
            run_ebullet_seeker(b);
            break;
        case EBULLET_EEL:
            run_ebullet_eel(b);
            break;
        case EBULLET_ROCKET:
            run_ebullet_rocket(b);
            break;
        case EBULLET_FLAK2:
            drag_ebullet(b, 950);
            ebullet[b].x += ebullet[b].x_speed;
            ebullet[b].y += ebullet[b].y_speed;
            break;
        case EBULLET_SPIN:
            ebullet[b].x += ebullet[b].x_speed;
            ebullet[b].y += ebullet[b].y_speed;
            ebullet[b].angle += ebullet[b].status;
            ebullet[b].angle &= ANGLE_MASK;
            break;
        case EBULLET_FLAK:
            ebullet[b].x += ebullet[b].x_speed;
            ebullet[b].y += ebullet[b].y_speed;
            ebullet[b].angle += ebullet[b].status2;
            break;
        case EBULLET_MISSILE:
            drag_ebullet(b, 1015);
            if (ebullet[b].time > 30)
            {
             accel_ebullet(b, ebullet[b].status);
             if ((ebullet[b].time) & 3)
             {
//              simple_cloud(CLOUD_2BALL, ebullet[b].x + grand(4000) - grand(4000) - xpart(ebullet[b].angle, 8000), ebullet[b].y + grand(4000) - grand(4000) - ypart(ebullet[b].angle, 8000),
//               ebullet[b].colour, 5 + grand(ebullet[b].draw_size << 2));
              simple_cloud(CLOUD_2BALL, ebullet[b].x + grand(3000) - grand(3000) - xpart(ebullet[b].angle, 8000), ebullet[b].y + grand(3000) - grand(3000) - ypart(ebullet[b].angle, 8000),
               ebullet[b].colour, 5 + grand(ebullet[b].draw_size << 2));
             }
            }
            ebullet[b].x += ebullet[b].x_speed;
            ebullet[b].y += ebullet[b].y_speed;
            for (i = 0; i < NO_PBULLETS; i ++)
            {
             if (pbullet[i].type == PBULLET_NONE)
              continue;
             if (pbullet[i].x < ebullet[b].x - pbullet[i].size - ebullet[b].size - 2000
              || pbullet[i].x > ebullet[b].x + pbullet[i].size + ebullet[b].size + 2000
              || pbullet[i].y < ebullet[b].y - pbullet[i].size - ebullet[b].size - 2000
              || pbullet[i].y > ebullet[b].y + pbullet[i].size + ebullet[b].size + 2000)
               continue;
             if (hypot(pbullet[i].y - ebullet[b].y, pbullet[i].x - ebullet[b].x) < pbullet[i].size + ebullet[b].size + 2000)
             {
              ebullet_explodes(b, 1, 1);
              pbullet_explodes(i, 1);
              no_more_bullet = 1;
             }
            }
             if (player[0].alive
              && player[0].x > ebullet[b].x - ebullet[b].size - 25000
              && player[0].x < ebullet[b].x + ebullet[b].size + 25000
              && player[0].y > ebullet[b].y - ebullet[b].size - 25000
              && player[0].y < ebullet[b].y + ebullet[b].size + 25000)
              {
               if (hypot(player[0].y - ebullet[b].y, player[0].x - ebullet[b].x) <= 25000)
               {
                ebullet_explodes(b, 1, 1);
                no_more_bullet = 1;
               }
              }
            break;
    }

    if (no_more_bullet == 1)
     continue;

/*    hits = detect_ebullet_collision(b);

    if (hits != -1)
    {
     ebullet_explodes(b, 1);
     continue;
    }*/

    ebullet[b].timeout --;

    if (ebullet[b].timeout <= 0)
     ebullet_explodes(b, 0, 1);

  }

}

void destroy_all_ebullets(void)
{
 int b;

 for (b = 0; b < NO_EBULLETS; b ++)
 {
  ebullet_explodes(b, 0, 0);
 }

}


void drag_ebullet(int b, int drag)
{
 ebullet[b].x_speed *= drag;
 ebullet[b].x_speed >>= 10;
 ebullet[b].y_speed *= drag;
 ebullet[b].y_speed >>= 10;

}

void accel_ebullet(int b, int accel)
{
 ebullet[b].x_speed += xpart(ebullet[b].angle, accel);
 ebullet[b].y_speed += ypart(ebullet[b].angle, accel);

}



int detect_ebullet_collision(int b)
{


  if (player[0].x - 10000 > ebullet[b].x + ebullet[b].size
   || player[0].x + 10000 < ebullet[b].x - ebullet[b].size
   || player[0].y - 10000 > ebullet[b].y + ebullet[b].size
   || player[0].y + 10000 < ebullet[b].y - ebullet[b].size)
    return -1;

  if (hypot(player[0].x - ebullet[b].x, player[0].y - ebullet[b].y) <= 20000 + pbullet[b].size)
  {
//   pbullet[b].x = x2;
//   pbullet[b].y = y2;
   ebullet_explodes(b, 1, 1);
   return b;
  }



 return -1;

}



void ebullet_explodes(int b, int hit, int effect)
{

 static struct ebulletstruct *eb;

 eb = &ebullet[b];

 int x = eb->x;
 int y = eb->y;
 int c, i, angle, size, b2, angle2, speed;

 switch(eb->type)
 {
// don't use default - eg beams and flak bits
  case EBULLET_BLOB:
  case EBULLET_SHOT:
  case EBULLET_SPIN:
  case EBULLET_SEEKER:
  case EBULLET_EEL:
  case EBULLET_ROCKET:
   simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, ebullet[b].draw_size << 3);
   break;
  case EBULLET_FLAK:
   simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, ebullet[b].draw_size << 3);
   if (effect == 0)
    break;
   angle2 = grand(ANGLE_1);
   play_effectwfvxy(WAV_BURST, 1000 - ebullet[b].status * 15, 100, ebullet[b].x, ebullet[b].y);
   for (i = 0; i < 20; i ++)
   {
    b2 = create_ebullet(EBULLET_FLAK2);
    angle = angle2;
    angle2 += ANGLE_16 + grand(ANGLE_16);
    speed = 1000 + grand(2000) + (ebullet[b].status << 7);

    if (b2 != -1)
    {
     ebullet[b2].x = eb->x;// + xpart(angle, speed);
     ebullet[b2].y = eb->y;// + ypart(angle, speed);
     ebullet[b2].x_speed = (eb->x_speed>>1) + xpart(angle, speed);
     ebullet[b2].y_speed = (eb->y_speed>>1) + ypart(angle, speed);
     ebullet[b2].timeout = 40 + grand(5) + (ebullet[b].status >> 1);
     ebullet[b2].colour = 2;
     ebullet[b2].angle = 0;//part[p].angle;
//     ebullet[b2].size = 5000 + part[p].size * 100;
//     ebullet[b2].draw_size = 4 + (part[p].size >> 3);
//     ebullet[b2].draw_size2 = 3 + (part[p].size >> 3);
//    ebullet[b].damage = 500;
    }

   }
   break;
  case EBULLET_SHOT2:
//   simple_cloud(CLOUD_BALL_COL3, x, y, eb->colour, 20);
   c = simple_cloud(CLOUD_BANG, x, y, eb->colour, ebullet[b].draw_size << 3);
   if (c != -1)
    cloud[c].status = ebullet[b].draw_size;
   break;
  case EBULLET_MISSILE:
   size = 25 + (ebullet[b].draw_size << 1);
   b2 = create_ebullet(EBULLET_MISSILE_BLAST);
   if (b2 != -1)
   {
    ebullet[b2].status2 = 1;
    ebullet[b2].status = size * ebullet[b2].status2; // timeout * status2
    ebullet[b2].x = ebullet[b].x;
    ebullet[b2].y = ebullet[b].y;
    ebullet[b2].timeout = 18;
    ebullet[b2].colour = 0;
   }
//   simple_cloud(CLOUD_BALL, x, y, eb->colour, ebullet[b].draw_size << 3);
//   simple_cloud(CLOUD_BALL_COL2, x, y, eb->colour, ebullet[b].draw_size << 2);
//   quick_cloud(CLOUD_SMALL_SHOCK, x, y, 0, 0, 30, ebullet[b].colour, 0);
      quick_cloud(CLOUD_SPARSE_FLARE, x, y,
       ebullet[b].x_speed, ebullet[b].y_speed, ebullet[b].draw_size << 2, ebullet[b].colour, 960);
   if (effect == 0)
    break;
      play_effectwfvxy(WAV_BURST, 1000 - ebullet[b].draw_size * 30, 100, ebullet[b].x, ebullet[b].y);
       break;
/*   size = 28;
   c = simple_cloud(CLOUD_BURST, x, y, eb->colour, size);
   if (c != -1)
   {
    cloud[c].status2 = 1;
    cloud[c].status = size * cloud[c].status2; // timeout * status2
   }
   for (i = 0; i < 7; i ++)
   {
    angle = grand(ANGLE_1);
    dist = 10000 + grand(15000);
    size = 30 + grand(15);
    c = simple_cloud(CLOUD_BURST, x + xpart(angle, dist), y + ypart(angle, dist), eb->colour, size);
    if (c != -1)
    {
     cloud[c].status2 = 1;
     cloud[c].status = size * cloud[c].status2; // timeout * status2
    }

   }
   break;*/
 }

 destroy_ebullet(b);
}


void destroy_ebullet(int b)
{
  ebullet[b].type = EBULLET_NONE;
}


void run_ebullet_seeker(int b)
{

    int old_x = ebullet[b].x;
    int old_y = ebullet[b].y;

    ebullet[b].x_speed += xpart(ebullet[b].angle, 300 + (ebullet[b].status2 << 4));
    ebullet[b].y_speed += ypart(ebullet[b].angle, 300 + (ebullet[b].status2 << 4));
    ebullet[b].x += ebullet[b].x_speed;
    ebullet[b].y += ebullet[b].y_speed;

    drag_ebullet(b, 940);

    int c = create_cloud(CLOUD_SEEKER_TRAIL);
    if (c != -1)
    {
     cloud[c].x = ebullet[b].x;
     cloud[c].y = ebullet[b].y;
     cloud[c].x2 = old_x;
     cloud[c].y2 = old_y;
     cloud[c].x_speed = 0;
     cloud[c].y_speed = 0;
//     cloud[c].angle = ebullet[b].angle;
     cloud[c].timeout = ebullet[b].status;// + pbullet[b].level;
     cloud[c].fangle = atan2(ebullet[b].y - old_y, ebullet[b].x - old_x);
     cloud[c].status = (int) hypot(ebullet[b].y - old_y, ebullet[b].x - old_x) >> 10;
    }

  {
//    if (target_x != -1 || target_y != -1)
     ebullet[b].angle = turn_towards_xy(ebullet[b].x, ebullet[b].y, player[0].x, player[0].y, ebullet[b].angle, 15 + (ebullet[b].status2 >> 2)); // 22
  }

    ebullet[b].timeout --;


    if (ebullet[b].timeout <= 0)
    {
     ebullet_explodes(b, 0, 1);
     destroy_ebullet(b);
    }

}

void run_ebullet_eel(int b)
{

    int old_x = ebullet[b].x;
    int old_y = ebullet[b].y;

    ebullet[b].x_speed += xpart(ebullet[b].angle, 300 + (ebullet[b].status2 << 5));
    ebullet[b].y_speed += ypart(ebullet[b].angle, 300 + (ebullet[b].status2 << 5));
    ebullet[b].x += ebullet[b].x_speed;
    ebullet[b].y += ebullet[b].y_speed;

    drag_ebullet(b, 930);

   if (ebullet[b].time > 2)
   {
    int c = create_cloud(CLOUD_SEEKER_TRAIL);
    if (c != -1)
    {
     cloud[c].x = ebullet[b].x;
     cloud[c].y = ebullet[b].y;
     cloud[c].x2 = old_x;
     cloud[c].y2 = old_y;
     cloud[c].x_speed = 0;
     cloud[c].y_speed = 0;
//     cloud[c].angle = ebullet[b].angle;
     cloud[c].timeout = ebullet[b].status;// + pbullet[b].level;
     cloud[c].fangle = atan2(ebullet[b].y - old_y, ebullet[b].x - old_x);
     cloud[c].status = (int) hypot(ebullet[b].y - old_y, ebullet[b].x - old_x) >> 10;
    }
   }

  {
//    if (target_x != -1 || target_y != -1)
     ebullet[b].angle += ebullet[b].status4;
     ebullet[b].status3 --;
     if (ebullet[b].status3 <= 0)
     {
      ebullet[b].status3 = 20 + grand(6);
      ebullet[b].status4 *= -1;
     }
  }

    ebullet[b].timeout --;


    if (ebullet[b].timeout <= 0)
    {
     ebullet_explodes(b, 0, 1);
     destroy_ebullet(b);
    }

}



void run_ebullet_rocket(int b)
{


            int angle = grand(ANGLE_1);
            int dist = grand(ebullet[b].status << 3);
            int c = simple_cloud(CLOUD_2BALL, ebullet[b].x + xpart(angle, dist), ebullet[b].y + ypart(angle, dist),
                 ebullet[b].colour, 5 + grand(ebullet[b].draw_size << 2));
            if (c != -1)
            {
             cloud[c].x_speed = ebullet[b].x_speed;
             cloud[c].y_speed = ebullet[b].y_speed;
             cloud[c].drag = 990;
            }

    if (ebullet[b].time > 25)
    {
     ebullet[b].x_speed += xpart(ebullet[b].angle, 300 + (ebullet[b].status2));
     ebullet[b].y_speed += ypart(ebullet[b].angle, 300 + (ebullet[b].status2));
    }

    if (ebullet[b].time == 25)
      play_effectwfvxy_xs_ys(WAV_WHOOSH, 500 + ebullet[b].draw_size * 100, 100, ebullet[b].x, ebullet[b].y, ebullet[b].x_speed + xpart(ebullet[b].angle, 1500), ebullet[b].y_speed + ypart(ebullet[b].angle, 1500));

    ebullet[b].x += ebullet[b].x_speed;
    ebullet[b].y += ebullet[b].y_speed;

    drag_ebullet(b, 970);

  {
//    if (target_x != -1 || target_y != -1)
//     pbullet[b].angle = turn_towards_xy(pbullet[b].x, pbullet[b].y, hs[0].x, hs[0].y, pbullet[b].angle, 22 + (pbullet[b].status2 >> 0)); // 22
  }

    ebullet[b].timeout --;


    if (ebullet[b].timeout <= 0)
    {
     ebullet_explodes(b, 0, 1);
     destroy_ebullet(b);
    }

}


