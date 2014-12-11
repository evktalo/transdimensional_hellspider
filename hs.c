
#include "config.h"

#include "allegro.h"

#include "globvars.h"

#include "stuff.h"
#include "ebullet.h"
#include "cloud.h"
#include "hs_build.h"
#include "sound.h"
#include <math.h>

void run_part(int h, int p);
int part_track_target(int p, int turning, int forbid);
void part_fires(int h, int p);
void part_explodes(int p, int core_destroyed);
void destroy_part(int p);
void calculate_moment_of_inertia(int h);
void calculate_centre_of_mass(int h);
void move_hs(int h);
void fix_angular_velocity(int h, int old_moment);
void parts_destroyed(int h);
void hs_impulse(int h, int force_angle, int impulse);
void drive_impulse(int p);
void apply_torque_to_hs(int h, int x, int y, int force, int angle);
void apply_frame_torque_to_hs(int h, int x, int y, int force, int angle, int p);
void hs_thinks_about_moving(int h);
void set_drive_directions(int h);
void hs_change_position(int h, int change);
void chain_explosion(int p, int first, int core_destroyed);
void hs_chooses_attacks(int h);
void find_attack(int f, int exclude);

extern int part_ranges [NO_PARTS] [5]; // in hs_build

#define CORE_WEIGHT 80

void run_hs(int h)
{
//int i;

 if (hs[h].alive == 0)
  return;

 int i, p;

 for (i = 0; i < NO_CORE_CIRCLES; i ++)
 {
   hs[0].core_circle_x [i] = hs[0].x + fxpart(hs[0].angle + hs[0].core_circle_angle [i], hs[0].core_circle_dist [i] << 10);
   hs[0].core_circle_y [i] = hs[0].y + fypart(hs[0].angle + hs[0].core_circle_angle [i], hs[0].core_circle_dist [i] << 10);
 }

 calculate_centre_of_mass(h);

/* if (key [KEY_U])
 {
  hs_change_position(h, -1);*/

 if (hs[h].firing_a_beam == 0)
//  || (arena.counter & 3) == 3)
 {
  if (hs[h].position > hs[h].target_position)
  {
   hs_change_position(h, -1);
  }
  if (hs[h].position < hs[h].target_position)
  {
   hs_change_position(h, 1);
  }
 }
/* }
  if (key [KEY_I])
   hs_change_position(h, 1);*/

 int old_moment = hs[h].moment;
 calculate_moment_of_inertia(h);
 fix_angular_velocity(h, old_moment);

 for (i = 0; i < NO_PARTS; i ++)
 {
//  if (part[i].type == PART_NONE)
//   continue;
  part[i].old_x2 = part[i].x;
  part[i].old_y2 = part[i].y;
 } // this one is needed before move_hs and run_part
 hs[h].old_x = hs[h].x;
 hs[h].old_y = hs[h].y;

 move_hs(h);
// remember - if hs changes its mass distribution, must call set_drive_directions

 hs[0].firing_a_beam = 0;

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].type != PART_NONE)
   run_part(h, p);
 }

 hs[0].counter ++;

 if (hs[0].hurt_flash > 0)
  hs[0].hurt_flash --;

  if (hs[0].core_hp < (hs[0].core_max_hp >> 1))
  {
   if (hs[0].counter % 64 == 0)
    hs[0].hurt_flash = 4;
   if (hs[0].core_hp < (hs[0].core_max_hp >> 2)
    && hs[0].counter % 64 == 8)
     hs[0].hurt_flash = 4;
   if (hs[0].core_hp < (hs[0].core_max_hp >> 3)
    && hs[0].counter % 64 == 16)
     hs[0].hurt_flash = 4;
  }


 for (i = 0; i < NO_PARTS; i ++)
 {
  if (part[i].type == PART_NONE)
   continue;
  part[i].move_x = part[i].x - part[i].old_x;
  part[i].move_y = part[i].y - part[i].old_y;
  part[i].old_x = part[i].x;
  part[i].old_y = part[i].y;
 }
// avoid moving parts or hs after this point. Can accelerate, but not move.

// let's work out what the hs is thinking

 if (hs[0].next_thought > 0)
  hs[0].next_thought --;
   else
   {
    hs_thinks_about_moving(0);
    if (hs[0].movement == MOVE_TURN_RIGHT
     && hs[0].can_turn_right == 0)
      hs[0].movement = MOVE_TURN_LEFT;
    if (hs[0].movement == MOVE_TURN_LEFT
     && hs[0].can_turn_left == 0)
      hs[0].movement = MOVE_TURN_RIGHT;

   }

  if (hs[0].firing_time <= 0)
  {
    hs_chooses_attacks(0);
  }
   else
    hs[0].firing_time --;


}



void hs_chooses_attacks(int h)
{

 if (hs[h].firing_a_beam)
  return;

 //hs[h].firing = grand(2);
 //hs[h].firing = 0;


// here

 find_attack(0, hs[0].firing [1]);

 if (hs[0].core_hp <= hs[0].core_max_hp >> 1)
  find_attack(1, hs[0].firing [0]);

 hs[h].firing_time = 200;
/*
 textprintf_ex(screen, font, 10, 100, -1, -1, "firing %i range %i p_r %i diff %i", best, player[0].range, part_ranges [hs[0].weapon [best]] [0], best_range_difference);

 do
 {
     rest(5);
 }
 while (key [KEY_1] == 0);
*/
 int p;

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].weapon == hs[h].firing [0]
   || part[p].weapon == hs[h].firing [1])
  {
   part[p].recycle = 20 + part[p].index * 6;
  }
 }

}


void find_attack(int f, int exclude)
{

int h = 0;
 int i, range_difference;
 int best = -1;
 int best_range_difference = 1000;//abs(best_range - player[0].range);

 for (i = 0; i < NO_WEAPONS; i ++)
 {
  if (hs[0].weapon [i] == PART_NONE
   || i == exclude)
   continue;
  if (part_ranges [hs[0].weapon [i]] [4] == DIR_FORWARD
   && player[0].angle_to_hs > ANGLE_FRONT
   && player[0].angle_to_hs < ANGLE_1 - (ANGLE_FRONT))
    continue;
  if (part_ranges [hs[0].weapon [i]] [4] == DIR_ALL
   && (player[0].angle_to_hs < ANGLE_FRONT
   || player[0].angle_to_hs > ANGLE_1 - (ANGLE_FRONT)))
    continue;
  range_difference = abs(part_ranges [hs[0].weapon [i]] [0] - player[0].range);
  if (range_difference < best_range_difference)
  {
//   best_range = part_ranges [hs[0].weapon [i]] [0];
   best_range_difference = range_difference;
   best = i;
  }

 }

 if (best == -1)
 {
  best_range_difference = 1000;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (hs[0].weapon [i] == PART_NONE
    || i == exclude)
    continue;
   range_difference = abs(part_ranges [hs[0].weapon [i]] [0] - player[0].range);
   if (range_difference < best_range_difference)
   {
    best_range_difference = range_difference;
    best = i;
   }

  }

 }

 hs[h].firing [f] = best;

}


void hs_thinks_about_moving(int h)
{
 int angle = radians_to_angle(atan2(hs[h].y - player[0].y, hs[h].x - player[0].x));
 int dist = hypot(hs[h].y - player[0].y, hs[h].x - player[0].x) - hs[h].range_minimum;
 if (dist < 1)
  dist = 1;
 int hs_angle_int = radians_to_angle(hs[h].angle);
 int angle_difference = (angle - hs_angle_int) & ANGLE_MASK;
/*
 if (dist < 300000)
  hs[h].target_position = 0;

 if (dist > 330000)
  hs[h].target_position = POSITIONS - 1;
*/

 hs[h].next_reposition --;

 if (hs[h].next_reposition <= 0)
 {
//  hs[h].target_position = dist / 1850;
  hs[h].target_position = dist / 2350;

  hs[h].target_position -= 40;

  if (hs[h].target_position < 0)
   hs[h].target_position = 0;
  if (hs[h].target_position > POSITIONS - 1)
   hs[h].target_position = POSITIONS - 1;

  hs[h].next_reposition = 20 + grand(20);
 }


 if (((hs_angle_int - angle) & ANGLE_MASK) < angle_difference)
  angle_difference = (hs_angle_int - angle) & ANGLE_MASK;

 int turning = delta_turn_towards_angle(hs_angle_int & ANGLE_MASK, angle & ANGLE_MASK, 1);

// if (angle_difference < ANGLE_8)
 {
  if (hs[h].movement == MOVE_TURN_LEFT && hs[h].spin < -0.03)
  {
     hs[h].next_thought = 10 + grand(10);
     hs[h].movement = MOVE_STAY;//FORWARDS;//MOVE_TURN_RIGHT;
     return;
  }

  if (hs[h].movement == MOVE_TURN_RIGHT && hs[h].spin > 0.03)
  {
     hs[h].next_thought = 10 + grand(10);
     hs[h].movement = MOVE_STAY;//FORWARDS;//MOVE_TURN_LEFT;
     return;
  }
 }

 if (angle_difference < ANGLE_8)
 {
  hs[h].movement = MOVE_FORWARDS;
  if (angle_difference > ANGLE_16)
  {
   if (turning == -1 && hs[h].spin > 0)
   {
     hs[h].next_thought = 10 + grand(10);
     hs[h].movement = MOVE_TURN_LEFT;
     return;
   }
   if (turning == 1 && hs[h].spin < 0)
   {
     hs[h].next_thought = 10 + grand(10);
     hs[h].movement = MOVE_TURN_RIGHT;
     return;
   }
  }
 }
  else
  {
   if (turning == 1)
   {
    hs[h].movement = MOVE_TURN_RIGHT;
    hs[h].next_thought = 10 + grand(10);
   }
     else
     {
      hs[h].movement = MOVE_TURN_LEFT;
      hs[h].next_thought = 10 + grand(10);
     }
  }

}

void hs_change_position(int h, int change)
{

 int old_x = hs[h].centre_x;
 int old_y = hs[h].centre_y;

  hs[h].position += change;
  if (hs[h].position < 0)
   hs[h].position = 0;
  if (hs[h].position > POSITIONS - 1)
   hs[h].position = POSITIONS - 1;

 int p;

 hs[h].range_minimum = 0;

// let's relocate the parts according to their new position vectors:
 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].type != PART_NONE)
  {
   part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);
   part[p].y = hs[h].y + fypart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);
   if (part[p].position_dist [hs[h].position] > hs[h].range_minimum)
    hs[h].range_minimum = part[p].position_dist [hs[h].position];
  }
 }

 hs[h].range_minimum >>= 1;

 calculate_centre_of_mass(h);

// if the hs is uneven, flexing an arm may cause it to rotate.
 if (change > 0)
 {
  for (p = 0; p < NO_PARTS; p ++)
  {
   if (part[p].type != PART_NONE
    && part[part[p].mirror_part].type == PART_NONE)
   {
    apply_frame_torque_to_hs(h, part[p].x, part[p].y, (part[p].size * part[p].position_move_dist [hs[h].position]) >> 3, radians_to_angle(hs[h].angle + part[p].position_move_angle [hs[h].position]) & ANGLE_MASK, p);
//   apply_frame_torque_to_hs(h, part[p].x, part[p].y, part[p].size * part[p].position_move_dist [hs[h].position], part[p].position_move_angle [hs[h].position]);

//   part[p].debug = part[p].position_move_dist [hs[h].position];
//   part[p].fdebug = part[p].position_move_angle [hs[h].position];
//   apply_frame_torque_to_hs(h, part[p].x, part[p].y, part[p].size * part[p].position_move_dist [hs[h].position], part[p].position_move_angle [hs[h].position]);
   }
  }
 }
  else
  {
   for (p = 0; p < NO_PARTS; p ++)
   {
    if (part[p].type != PART_NONE
     && part[part[p].mirror_part].type == PART_NONE)
    {
     apply_frame_torque_to_hs(h, part[p].x, part[p].y, ((part[p].size * part[p].position_move_dist [hs[h].position]) >> 3) * -1, radians_to_angle(hs[h].angle + part[p].position_move_angle [hs[h].position]) & ANGLE_MASK, p);
    }
   }
  }

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].type != PART_NONE)
  {
   part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);
   part[p].y = hs[h].y + fypart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);
  }
 }

 calculate_centre_of_mass(h);

// we don't want the position change to move the centre of mass.
//  So keep the centre of mass in the same position by moving the core:
 hs[h].x -= hs[h].centre_x - old_x;
 hs[h].y -= hs[h].centre_y - old_y;
// hs[h].centre_x = old_x;
// hs[h].centre_y = old_y;

 calculate_moment_of_inertia(h);
 set_drive_directions(h);

}

void run_part(int h, int p)
{

/* int ul = part[p].uplink;

 if (ul == -1)
 {
//   part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].up_angle, part[p].up_dist);
//   part[p].y = hs[h].y + fypart(hs[h].angle + part[p].up_angle, part[p].up_dist);
   part[p].cumulative_angle = hs[h].angle + part[p].position_up_angle [hs[h].position];
 }
  else
  {
//   part[p].x = part[ul].x + fxpart(hs[h].angle + part[p].up_angle, part[p].up_dist);
//   part[p].y = part[ul].y + fypart(hs[h].angle + part[p].up_angle, part[p].up_dist);
   part[p].cumulative_angle = part[ul].cumulative_angle + (hs[h].angle - part[p].position_up_angle [hs[h].position]);
  }*/

   part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);
   part[p].y = hs[h].y + fypart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);


  part[p].counter ++;

  if (part[p].hp < (part[p].max_hp >> 1))
  {
   if (part[p].counter % 64 == 0)
    part[p].hurt_flash = 4;
   if (part[p].hp < (part[p].max_hp >> 2)
    && part[p].counter % 64 == 8)
     part[p].hurt_flash = 4;
   if (part[p].hp < (part[p].max_hp >> 3)
    && part[p].counter % 64 == 16)
     part[p].hurt_flash = 4;
  }

  switch(part[p].tracks)
  {
      case TRACK_FOLLOW:
//       if ((part[p].counter >> 3) & 1)
       if ((part[p].counter >> 6) & 1)
        part[p].slew_dir = 0;
       part[p].angle = part_track_target(p, part[p].slew, part[p].slew_dir * -1);
       part[p].angle += radians_to_angle(hs[h].spin); // very inexact!
       break;
      case TRACK_FOLLOW2:
       part[p].angle += radians_to_angle(hs[h].spin); // very inexact!
       if (part[p].recycle > 10)
       {
        if ((part[p].counter >> 3) & 1)
         part[p].slew_dir = 0;
        part[p].angle = part_track_target(p, part[p].slew, part[p].slew_dir * -1);
       }
       break;
/*      case TRACK_FOLLOW2:
       part[p].angle += radians_to_angle(hs[h].spin); // very inexact!
       if (part[p].recycle <= 10)
        break;
       if ((part[p].counter >> 3) & 1)
        part[p].slew_dir = 0;
       part[p].angle = part_track_target(p, part[p].slew, part[p].slew_dir * -1);
       break;*/
      case TRACK_BEAM:
       if ((part[p].counter >> 3) & 7)
        part[p].slew_dir = 0;
       if (part[p].burst == 0)
        part[p].angle = part_track_target(p, part[p].slew, part[p].slew_dir * -1);
       part[p].angle += radians_to_angle(hs[h].spin); // very inexact!
       break;
      case TRACK_LOCKED:
//       part[p].angle = radians_to_angle(part[p].cumulative_angle + part[p].position_up_angle [hs[h].position] + part[p].locked_angle);
       part[p].angle = radians_to_angle(part[p].position_up_angle [hs[h].position] + hs[h].angle + part[p].locked_angle);// + part[p].locked_angle);
       break;
  }

  part[p].angle &= ANGLE_MASK;

  switch(part[p].type)
  {
/*   case PART_DRONE:
    part[p].recycle --;
    if (part[p].recycle <= 0)
    {
     int b = create_ebullet(EBULLET_DRONE);
     if (b != -1)
     {
      ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
      ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
      ebullet[b].x_speed = part[p].x_speed;
      ebullet[b].y_speed = part[p].y_speed;
      ebullet[b].timeout = 12000;
      ebullet[b].colour = 0;
      ebullet[b].angle = part[p].angle + ANGLE_2;
      ebullet[b].size = 5000 + part[p].size * 100;
      ebullet[b].draw_size = 3 + (part[p].size >> 3);
      ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
      ebullet[b].status = part[p].burst;
      ebullet[b].status2 = 3 + (part[p].size >> 2);
     }
     part[p].recycle = 0;
     part[p].burst ++;
     if (part[p].burst >= 70)
     {
      part[p].recycle = 70;
      part[p].burst = 0;
     }
     hs[0].firing_a_beam = 1;
    }
    break;   */
   case PART_BASIC:
   case PART_FORWARD:
   case PART_MISSILE:
   default:
   // use this one for all firing parts
    if (part[p].fired > 0)
     part[p].fired --;

    if (part[p].weapon == hs[h].firing [0]
     || part[p].weapon == hs[h].firing [1])
     part[p].recycle --;

    if (part[p].recycle <= 0)
    {
     part_fires(h, p);
    }
    break;
   case PART_ROTARY:
    if (part[p].fired > 0)
     part[p].fired --;
    if (part[p].weapon == hs[h].firing [0]
     || part[p].weapon == hs[h].firing [1])
     part[p].recycle --;
    if (part[p].recycle <= 0)
    {
     part_fires(h, p);
    }
    if (part[p].side == 0)
     part[p].spin_angle += 10;
      else
       part[p].spin_angle -= 10;
    part[p].spin_angle &= ANGLE_MASK;
    break;
   case PART_DRIVE:
   case PART_DRIVE2:
    part[p].drive -= part[p].size;
    if (part[p].drive < 0)
     part[p].drive = 0;
//    if (hs[h].firing_a_beam == 0)
    {
     if (hs[h].movement == MOVE_FORWARDS
      && part[p].drive_push == 1)
     {
      part[p].drive += part[p].size << 1;
      if (part[p].drive > part[p].max_drive)
       part[p].drive = part[p].max_drive;
     }
     if (hs[h].movement == MOVE_TURN_LEFT
      && part[p].drive_turn == -1)
     {
      part[p].drive += part[p].size << 1;
      if (part[p].drive > part[p].max_drive)
       part[p].drive = part[p].max_drive;
     }
     if (hs[h].movement == MOVE_TURN_RIGHT
      && part[p].drive_turn == 1)
     {
      part[p].drive += part[p].size << 1;
      if (part[p].drive > part[p].max_drive)
       part[p].drive = part[p].max_drive;
     }
    }
    if (part[p].drive > 0)
    {
     drive_impulse(p);
    }
    break;
   case PART_SHIELD:
   if (part[p].shield_target < part[p].shield)
   {
    part[p].shield -= 1500;
//    if (part[p].shield < 0)
//     part[p].shield = 0;
   }
    else
    {
     part[p].shield += part[p].size * 3;
     if (part[p].shield > part[p].size << 12)
      part[p].shield = part[p].size << 12;
     if (part[p].shield > 127 << 10)
      part[p].shield = 127 << 10;
     part[p].shield_target = part[p].shield;
    }
    if (part[p].side == 0)
     part[p].spin_angle += 10;
      else
       part[p].spin_angle -= 10;
    part[p].spin_angle &= ANGLE_MASK;
    if (part[p].shield_flash > 0)
     part[p].shield_flash --;
    break;
  }

 if (part[p].hurt_flash > 0)
  part[p].hurt_flash --;

}


void drive_impulse(int p)
{

 int speed = part[p].drive * 20;//10;//((int) hypot(pbullet[b].y_speed - hs[0].y_speed, pbullet[b].x_speed - hs[0].x_speed)) >> 12;
 int force_angle = part[p].angle;// - radians_to_angle(hs[0].angle);
 //radians_to_angle(atan2(pbullet[b].y_speed - hs[0].y_speed, pbullet[b].x_speed - hs[0].x_speed)) - ANGLE_2;

 apply_torque_to_hs(0, part[p].x, part[p].y, speed, force_angle);

 hs_impulse(0, force_angle + ANGLE_2, speed);

}

void set_drive_directions(int h)
{

 int p, lever_angle, torque;

 hs[h].can_turn_left = 0;
 hs[h].can_turn_right = 0;

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].type != PART_DRIVE
   && part[p].type != PART_DRIVE2)
   continue;

  lever_angle = radians_to_angle(atan2(part[p].y - hs[h].centre_y, part[p].x - hs[h].centre_x));
  torque = ypart(lever_angle - part[p].angle, 10000);

  part[p].drive_turn = 0;

  if (torque > 0)
  {
   part[p].drive_turn = 1;
   hs[h].can_turn_right = 1;
  }

  if (torque < 0)
  {
   part[p].drive_turn = -1;
   hs[h].can_turn_left = 1;
  }

  part[p].debug = torque;

  part[p].drive_push = 0;
//  if (part[p].angle < ANGLE_4 || part
  if (((part[p].angle - radians_to_angle(hs[0].angle) + ANGLE_4) & ANGLE_MASK) < ANGLE_2)
   part[p].drive_push = 1;

 }


}

void part_fires(int h, int p)
{

 int b;
 int size, firing, speed;


 switch(part[p].type)
 {
  case PART_BEAM:
   if (part[p].burst == 0 && hs[h].firing_time < 3)
    break; // don't fire if the hs is waiting to change weapons
    // (needed because the hs will not change weapons if a beam is being fired)
   b = create_ebullet(EBULLET_BEAM);
   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
//    ebullet[b].x_speed = //part[p].x + xpart(part[p].angle, 1000 + (part[p].burst << 12));
//    ebullet[b].y_speed = part[p].y + ypart(part[p].angle, 1000 + (part[p].burst << 12));
    ebullet[b].timeout = 2;
    ebullet[b].colour = 0;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
    ebullet[b].status = part[p].burst;
    ebullet[b].status2 = 3 + (part[p].size >> 2);
    ebullet[b].status3 = part[p].size >> 3; // this is the width of the beam
//    ebullet[b].damage = 500;
   }
   part[p].recycle = 0;
   if (part[p].burst == 0)
    play_effectwfvxy_xs_ys(WAV_BEAM1, 2500 - part[p].size*2, 180, ebullet[b].x, ebullet[b].y, part[p].move_x, part[p].move_y);
   part[p].burst ++;
   if (part[p].burst >= 70)
   {
    part[p].recycle = 70 - part[p].size;
    part[p].burst = 0;
   }
//   part[p].fired = 40 + grand(10);
   hs[0].firing_a_beam = 1;
   break;
  case PART_BEAM2:
   if (part[p].burst == 0 && hs[h].firing_time < 3)
    break; // don't fire if the hs is waiting to change weapons
    // (needed because the hs will not change weapons if a beam is being fired)
   b = create_ebullet(EBULLET_BEAM2);
   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].timeout = 2;
    ebullet[b].colour = 2;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
    ebullet[b].status = part[p].burst;
    ebullet[b].status2 = 4 + part[p].size/3;//(part[p].size >> 2);
    ebullet[b].status3 = part[p].size >> 3; // this is the width of the beam
   }
   part[p].recycle = 0;
   if (part[p].burst == 0)
    play_effectwfvxy_xs_ys(WAV_BEAM, 2000 - part[p].size*2, 180, ebullet[b].x, ebullet[b].y, part[p].move_x, part[p].move_y);
   part[p].burst ++;
   if (part[p].burst >= 70)
   {
    part[p].recycle = 70 - part[p].size;
    part[p].burst = 0;
   }
   hs[0].firing_a_beam = 1;
   break;
  case PART_ROTARY:
   if (part[p].side == 0)
   {
    part[p].which_fired ++;
    if (part[p].which_fired > 3)
     part[p].which_fired = 0;
   }
    else
    {
     part[p].which_fired --;
     if (part[p].which_fired < 0)
      part[p].which_fired = 3;
    }
   firing = part[p].which_fired;
   b = create_ebullet(EBULLET_SPIN);
   speed = 2300 + (part[p].size << 8);
   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].spin_angle + firing * ANGLE_4, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].spin_angle + firing * ANGLE_4, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, speed);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, speed);
    ebullet[b].timeout = 57;
    ebullet[b].colour = 0;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
    if (part[p].side == 0)
     ebullet[b].status = 64;
      else
       ebullet[b].status = -64;
   }
   part[p].burst --;
   if (part[p].burst <= 0)
   {
    part[p].burst = 2 + (part[p].size >> 2);
    part[p].recycle = 120 - (part[p].size << 1);
   }
    else
     part[p].recycle = 10;
   part[p].fired = 10;
   play_effectwfvxy(WAV_ROTARY, 1000 - part[p].size * 10, 150, part[p].x, part[p].y);
   break;
  case PART_MGUN:
   size = (part[p].size)<<9;
   b = create_ebullet(EBULLET_SHOT);
   speed = 2300 + (part[p].size * 180);
   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle + ANGLE_8, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle + ANGLE_8, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, speed);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, speed);
    ebullet[b].timeout = 47;
    ebullet[b].colour = 2;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
   }
   b = create_ebullet(EBULLET_SHOT);
   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle - ANGLE_8, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle - ANGLE_8, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, speed);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, speed);
    ebullet[b].timeout = 47;
    ebullet[b].colour = 2;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
   }
   part[p].burst --;
   if (part[p].burst <= 0)
   {
    part[p].burst = 2 + (part[p].size >> 2); // if changed must also change in hs_build init_part
    part[p].recycle = 120 - (part[p].size << 1);
   }
    else
     part[p].recycle = 10;
   part[p].fired = 40 + grand(10);
   play_effectwfvxy(WAV_MGUN, 1000 - part[p].size * 10, 50, part[p].x, part[p].y);
   break;
  case PART_BASIC:
   b = create_ebullet(EBULLET_SHOT);

   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, 3000 + (part[p].size << 8));
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, 3000 + (part[p].size << 8));
    ebullet[b].timeout = 57;
    ebullet[b].colour = 2;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
//    ebullet[b].damage = 500;
   }
   part[p].recycle = 50 - (part[p].size);
   part[p].fired = 40 + grand(10);
//   play_effectwfvxy(WAV_CLICKY, 1000 - part[p].size * 10, 150, part[p].x, part[p].y);
   play_effectwfvxy_xs_ys(WAV_DZAP, 2000 - part[p].size * 14, 150, ebullet[b].x, ebullet[b].y, ebullet[b].x_speed, ebullet[b].y_speed);
   break;
  case PART_3CANNON:
   speed = 1700 + (part[p].size << 6);
   b = create_ebullet(EBULLET_BLOB);

   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, speed);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, speed);
    ebullet[b].timeout = 150 + part[p].size;
    ebullet[b].colour = 1;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 6000 + part[p].size * 200;
    ebullet[b].draw_size = 4 + (part[p].size >> 2);
    ebullet[b].draw_size2 = 3 + (part[p].size >> 2);
//    ebullet[b].damage = 500;
   }

   b = create_ebullet(EBULLET_BLOB);

   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle + ANGLE_64, speed);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle + ANGLE_64, speed);
    ebullet[b].timeout = 130 + part[p].size;
    ebullet[b].colour = 1;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 6000 + part[p].size * 200;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
//    ebullet[b].damage = 500;
   }

   b = create_ebullet(EBULLET_BLOB);

   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle - ANGLE_64, speed);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle - ANGLE_64, speed);
    ebullet[b].timeout = 130 + part[p].size;
    ebullet[b].colour = 1;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 6000 + part[p].size * 200;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
//    ebullet[b].damage = 500;
   }

   part[p].recycle = 100 - part[p].size;
   part[p].fired = 40 + grand(10);
   play_effectwfvxy_xs_ys(WAV_3CANNON, 1000 - part[p].size*12, 180, ebullet[b].x, ebullet[b].y, part[p].move_x, part[p].move_y);
   break;
  case PART_FLAK:
   b = create_ebullet(EBULLET_FLAK);

   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, 2000 + (part[p].size << 7));
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, 2000 + (part[p].size << 7));
    ebullet[b].timeout = 30 + grand(40);
    ebullet[b].colour = 0;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 4 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 3 + (part[p].size >> 3);
    ebullet[b].status = part[p].size;
    if (part[p].side == 0)
     ebullet[b].status2 = ANGLE_128;
      else
       ebullet[b].status2 = -ANGLE_128;
    play_effectwfvxy_xs_ys(WAV_HIGHZAP, 1000 - part[p].size * 14, 150, ebullet[b].x, ebullet[b].y, ebullet[b].x_speed, ebullet[b].y_speed);
   }
   part[p].recycle = 140 - (part[p].size * 3);
   part[p].fired = 40 + grand(10);
   break;
  case PART_FORWARD:
   b = create_ebullet(EBULLET_SHOT2);

   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, 3300 + part[p].size * 170);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, 3300 + part[p].size * 170);
    ebullet[b].timeout = 67;
    ebullet[b].colour = 2;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 8000 + part[p].size * 200;
    ebullet[b].draw_size = 5 + (part[p].size >> 2);
    ebullet[b].draw_size2 = 3 + (part[p].size >> 2);
//    ebullet[b].damage = 500;
    play_effectwfvxy_xs_ys(WAV_DZAP, 600 - part[p].size * 7, 150, ebullet[b].x, ebullet[b].y, ebullet[b].x_speed, ebullet[b].y_speed);
   }
   part[p].recycle = 50 - part[p].size;
   part[p].fired = 25 + grand(10) + part[p].size;
   break;
  case PART_MISSILE:
   b = create_ebullet(EBULLET_MISSILE);

   if (b != -1)
   {
    ebullet[b].x = part[p].x + xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y + ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, 2000);
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, 2000);
    ebullet[b].timeout = 220;//297;
    ebullet[b].colour = 0;
    ebullet[b].angle = part[p].angle;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 3);
    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
    ebullet[b].status = 120 + (part[p].size << 1);
    quick_cloud(CLOUD_SPARSE_NARROW_FLARE, part[p].x - xpart(part[p].angle, part[p].size<<10), part[p].y - ypart(part[p].angle, part[p].size<<10),
     ebullet[b].x_speed * -1, ebullet[b].y_speed * -1, ebullet[b].draw_size << 1, ebullet[b].colour, 960);
    play_effectwfvxy_xs_ys(WAV_WHOOSH, 400 - part[p].size*2, 140, ebullet[b].x, ebullet[b].y, ebullet[b].x_speed + xpart(ebullet[b].angle, 2500), ebullet[b].y_speed + ypart(ebullet[b].angle, 2500));
//    ebullet[b].damage = 500;
   }
   part[p].recycle = 150 - (part[p].size * 3);
   part[p].fired = 25 + grand(10) + part[p].size;
   break;
  case PART_SEEKER:
   b = create_ebullet(EBULLET_SEEKER);
   if (b != -1)
   {
    ebullet[b].x = part[p].x - xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y - ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x - xpart(part[p].angle, 2000);// + (part[p].size << 6));
    ebullet[b].y_speed = part[p].move_y - ypart(part[p].angle, 2000);// + (part[p].size << 6));
    ebullet[b].timeout = 200 + (part[p].size << 2);
    ebullet[b].colour = 2;
    ebullet[b].angle = (part[p].angle + ANGLE_2) & ANGLE_MASK;
    ebullet[b].size = 4500 + part[p].size * 120;
    ebullet[b].draw_size = 3 + (part[p].size / 3);
//    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
    ebullet[b].status = 8 + (part[p].size >> 2);
    ebullet[b].status2 = part[p].size;

//    ebullet[b].damage = 500;
    play_effectwfvxy_xs_ys(WAV_WORMS, 1000 - part[p].size*2, 180, ebullet[b].x, ebullet[b].y, ebullet[b].x_speed + xpart(ebullet[b].angle, 2500), ebullet[b].y_speed + ypart(ebullet[b].angle, 2500));
   }
   part[p].recycle = 180 - (part[p].size * 3);
   part[p].fired = 40 + grand(10);
   break;
  case PART_EEL:
   b = create_ebullet(EBULLET_EEL);
   if (b != -1)
   {
    ebullet[b].x = part[p].x - xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y - ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x + xpart(part[p].angle, 2000);// + (part[p].size << 6));
    ebullet[b].y_speed = part[p].move_y + ypart(part[p].angle, 2000);// + (part[p].size << 6));
    ebullet[b].timeout = 157 + (part[p].size << 1);
    ebullet[b].colour = 2;
    ebullet[b].angle = (part[p].angle) & ANGLE_MASK;
    ebullet[b].size = 4600 + part[p].size * 130;
    ebullet[b].draw_size = 3 + (part[p].size / 3);
//    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
    ebullet[b].status = 8 + (part[p].size >> 2);
    ebullet[b].status2 = part[p].size;
    ebullet[b].status3 = 10 + grand(3);
    ebullet[b].status4 = 50 + grand(50);
    if (part[p].side == 1)
     ebullet[b].status4 *= -1;
   }
   part[p].recycle = 160 - (part[p].size * 3);
   part[p].fired = 40 + grand(10);
   play_effectwfvxy_xs_ys(WAV_WORMS, 600 - part[p].size*2, 180, ebullet[b].x, ebullet[b].y, ebullet[b].x_speed + xpart(ebullet[b].angle, 2500), ebullet[b].y_speed + ypart(ebullet[b].angle, 2500));
   break;
  case PART_ROCKET:
   b = create_ebullet(EBULLET_ROCKET);
   if (b != -1)
   {
    ebullet[b].x = part[p].x - xpart(part[p].angle, part[p].size<<10);
    ebullet[b].y = part[p].y - ypart(part[p].angle, part[p].size<<10);
    ebullet[b].x_speed = part[p].move_x - xpart(part[p].angle, 3000);// + (part[p].size << 6));
    ebullet[b].y_speed = part[p].move_y - ypart(part[p].angle, 3000);// + (part[p].size << 6));
    ebullet[b].timeout = 257 + (part[p].size << 1);
    ebullet[b].colour = 0;
    ebullet[b].angle = (part[p].angle + grand(ANGLE_32) - grand(ANGLE_32)) & ANGLE_MASK;
    ebullet[b].size = 5000 + part[p].size * 100;
    ebullet[b].draw_size = 3 + (part[p].size >> 1);
//    ebullet[b].draw_size2 = 2 + (part[p].size >> 3);
    ebullet[b].status = 400 + (part[p].size << 3);
    ebullet[b].status2 = 7 + (part[p].size << 4);
   }
   part[p].recycle = 140 - (part[p].size * 3);
   part[p].fired = 40 + grand(10);
   break;
 }



}


void hurt_part(int p, int dam)
{

 int new_flash;

 if (part[p].type == PART_SHIELD && part[p].shield > 0) // can be negative
 {
  new_flash = dam / 20;
  if (new_flash < 2)
   new_flash = 2;
  if (new_flash > 5)
   new_flash = 5;
  part[p].shield_flash = new_flash;
  part[p].shield_target -= dam << 5;
  return;
 }

 new_flash = dam / 100;

 if (new_flash < 2)
  new_flash = 2;

 if (new_flash > part[p].hurt_flash)
  part[p].hurt_flash = new_flash;

 part[p].hp -= dam;

 if (part[p].hp <= 0)
  part_explodes(p, 0);


}

void hurt_core(int dam)
{
 int new_flash = dam / 100;

 if (new_flash < 2)
  new_flash = 2;

 if (new_flash > hs[0].hurt_flash)
  hs[0].hurt_flash = new_flash;

 hs[0].core_hp -= dam;

 if (hs[0].core_hp <= 0)
 {
  int i, j, p;

  for (i = 0; i < 2; i ++)
  {
   for (j = 0; j < 4; j ++)
   {
    p = hs[0].arm_part [i] [j];
    if (p != -1)
     part_explodes(p, 1);
   }
  }

int xcol = 0;

#define GUARD_CLOUD_COL xcol

      int x = hs[0].x;
      int y = hs[0].y;
      int dist, speed, angle, angle2;
     quick_cloud(CLOUD_BALL, x, y, 0, 0, 50 + grand(20), GUARD_CLOUD_COL, 0);
     quick_cloud(CLOUD_BALL_COL2, x, y, 0, 0, 70 + grand(20), GUARD_CLOUD_COL, 0);
     quick_cloud(CLOUD_HUGE_SHOCK, x, y, 0, 0, 50, GUARD_CLOUD_COL, 0);

     quick_cloud(CLOUD_BALL, x + fxpart(hs[0].angle, (50 << 10)), y + fypart(hs[0].angle, (50 << 10)), 0, 0, 50 + grand(20), GUARD_CLOUD_COL, 0);
     quick_cloud(CLOUD_BALL, x - fxpart(hs[0].angle, (50 << 10)), y - fypart(hs[0].angle, (50 << 10)), 0, 0, 40 + grand(10), GUARD_CLOUD_COL, 0);
     quick_cloud(CLOUD_BALL, x + fxpart(hs[0].angle + PI_2 + PI_4, (50 << 10)), y + fypart(hs[0].angle + PI_2 + PI_4, (50 << 10)), 0, 0, 40 + grand(10), GUARD_CLOUD_COL, 0);
     quick_cloud(CLOUD_BALL, x + fxpart(hs[0].angle - PI_2 - PI_4, (50 << 10)), y + fypart(hs[0].angle - PI_2 - PI_4, (50 << 10)), 0, 0, 40 + grand(10), GUARD_CLOUD_COL, 0);

     angle = grand(ANGLE_1);
     for (i = 0; i < 3; i ++)
     {
      for (j = 0; j < 4; j ++)
      {
       dist = 10000 + grand(4000) + 9000 * j;
       speed = 100 + grand(100) + 100 * j;
       angle2 = angle + grand(ANGLE_16) - ANGLE_32;
       quick_cloud(CLOUD_BALL_COL2, x + xpart(angle2, dist), y + ypart(angle2, dist),
        xpart(angle2, speed), ypart(angle2, speed), 30 + grand(20), GUARD_CLOUD_COL, 1010);
       speed += 300;
       quick_cloud(CLOUD_BALL, x + xpart(angle2, dist), y + ypart(angle2, dist),
        xpart(angle2, speed), ypart(angle2, speed), 20 + grand(15), GUARD_CLOUD_COL, 1015);

      }
      dist = 10000 + grand(4000);
      speed = 5000 + grand(5000);
      angle2 = angle + grand(ANGLE_16) - ANGLE_32;
      quick_cloud(CLOUD_FLARE, x + xpart(angle2, dist), y + ypart(angle2, dist),
       xpart(angle2, speed), ypart(angle2, speed), 30 + grand(20), GUARD_CLOUD_COL, 910);
      angle += ANGLE_3 + grand(ANGLE_16);
     }

  play_effectwfvxy(WAV_BANG3, 700, 170, hs[0].x, hs[0].y);
  play_effectwfvxy(WAV_BANG4, 600, 170, hs[0].x, hs[0].y);


  hs[0].alive = 0;
  hs[0].stage ++;

  arena.end_stage = 200;

 }



}



void part_explodes(int p, int core_destroyed)
{

// int p2 = part[p].downlink;

 int i, j;
 int x;
 int y;
 int speed, angle, angle2, dist;

 part[p].chain = 0;
 part[p].wasted = 0;

 if (core_destroyed == 0)
 {
  hs[0].destroyed [hs[0].destroyed_index] = p;
  if (part[p].side == 1)
   hs[0].destroyed [hs[0].destroyed_index] = part[p].mirror_part;
  hs[0].destroyed_index ++;
  hs[0].destroyed [hs[0].destroyed_index] = -1;
// I think we can assume that destroyed_index will not be out of bounds.
  if (part[p].downlink [0] == -1 && part[p].downlink [1] == -1)
   play_effectwfvxy(WAV_BANG1, 1200 - part[p].size * 10, 250, part[p].x, part[p].y);
    else
     play_effectwfvxy(WAV_BANG3, 1200 - part[p].size * 10, 250, part[p].x, part[p].y);
 }

 chain_explosion(part[p].downlink [0], p, core_destroyed);
 chain_explosion(part[p].downlink [1], p, core_destroyed);

 x = part[p].x;
 y = part[p].y;

int xcol = 0;
//if (part[p].max_drive > 0)
// xcol = 2;
 switch(part[p].type)
 {
  case PART_DRIVE2:
  case PART_DRIVE: xcol = 2; break;
  case PART_SHIELD: xcol = 1; break;

 }


    quick_cloud(CLOUD_BALL, x, y, 0, 0, 50 + grand(20), GUARD_CLOUD_COL, 0);
    quick_cloud(CLOUD_BALL_COL2, x, y, 0, 0, 70 + grand(20), GUARD_CLOUD_COL, 0);

//    int x2 = part[p].x - fxpart(part[p].up_angle + hs[0].angle, (part[p].size + (part[p].size >> 1)) >> 10);
//    int y2 = part[p].y - fypart(part[p].up_angle + hs[0].angle, (part[p].size + (part[p].size >> 1)) >> 10);
    int x2 = part[p].x - fxpart(part[p].up_angle + hs[0].angle, (part[p].size + (part[p].size >> 1)));
    int y2 = part[p].y - fypart(part[p].up_angle + hs[0].angle, (part[p].size + (part[p].size >> 1)));
    quick_cloud(CLOUD_BALL_COL2, x2, y2, 0, 0, 50 + grand(20), GUARD_CLOUD_COL, 0);

    if (!core_destroyed)
    {
     float up_angle;
     if (part[p].uplink == -1)
      up_angle = part[p].up_angle + hs[0].angle;
       else
        up_angle = atan2(part[p].y - part[part[p].uplink].y, part[p].y - part[part[p].uplink].y);
     int force = part[p].size + (part[p].chain >> 1);
     hs_impulse(0, radians_to_angle(up_angle), force << 12);
     apply_torque_to_hs(0, part[p].x, part[p].y, force << 12, up_angle);

     quick_cloud(CLOUD_LARGE_SHOCK, x, y, 0, 0, 50, GUARD_CLOUD_COL, 0);
     angle = grand(ANGLE_1);
     for (i = 0; i < 3; i ++)
     {
      for (j = 0; j < 4; j ++)
      {
       dist = 10000 + grand(4000) + 9000 * j;
       speed = 100 + grand(100) + 100 * j;
       angle2 = angle + grand(ANGLE_16) - ANGLE_32;
       quick_cloud(CLOUD_BALL_COL2, x + xpart(angle2, dist), y + ypart(angle2, dist),
        xpart(angle2, speed), ypart(angle2, speed), force + grand(20), GUARD_CLOUD_COL, 1010);
       speed += 300;
       quick_cloud(CLOUD_BALL, x + xpart(angle2, dist), y + ypart(angle2, dist),
        xpart(angle2, speed), ypart(angle2, speed), force + grand(15), GUARD_CLOUD_COL, 1015);

      }
      dist = 10000 + grand(4000);
      speed = 5000 + grand(5000) + (force << 5);
      angle2 = angle + grand(ANGLE_16) - ANGLE_32;
      quick_cloud(CLOUD_FLARE, x + xpart(angle2, dist), y + ypart(angle2, dist),
       xpart(angle2, speed), ypart(angle2, speed), force + grand(20), GUARD_CLOUD_COL, 910);
      angle += ANGLE_3 + grand(ANGLE_16);
     }

    }


 if (part[p].uplink == -1)
 {
  hs[0].arm_part [part[p].side] [part[p].arm] = -1;
 }
  else
  {
   if (part[part[p].uplink].downlink [0] == p)
    part[part[p].uplink].downlink [0] = -1;
   if (part[part[p].uplink].downlink [1] == p)
    part[part[p].uplink].downlink [1] = -1;
  }

 part[p].killed = 1;

// with all of the necessary killed values set, can work out new velocity and spin:
 parts_destroyed(0);

 for (i = 0; i < NO_PARTS; i ++)
 {
  if (part[i].killed == 1)
   destroy_part(i);
 }

 calculate_centre_of_mass(0);
 calculate_moment_of_inertia(0);
 set_drive_directions(0); // must be after calculate_centre_of_mass

}

// this function is called recursively!
void chain_explosion(int p, int first, int core_destroyed)
{

 if (p == -1)
  return;

 int x;
 int y;
//#define GUARD_CLOUD_COL 0
int xcol = 0;
if (part[p].max_drive > 0)
 xcol = 2;


  x = part[p].x;
  y = part[p].y;
  quick_cloud(CLOUD_BALL, x, y, 0, 0, 50 + grand(20), GUARD_CLOUD_COL, 0);
  quick_cloud(CLOUD_BALL_COL2, x, y, 0, 0, 60 + grand(20), GUARD_CLOUD_COL, 0);

  x = part[p].x - fxpart(part[p].up_angle + hs[0].angle, (part[p].size + (part[p].size >> 1)) >> 10);
  y = part[p].y - fypart(part[p].up_angle + hs[0].angle, (part[p].size + (part[p].size >> 1)) >> 10);
  quick_cloud(CLOUD_BALL_COL2, x, y, 0, 0, 50 + grand(20), GUARD_CLOUD_COL, 0);

/*  for (i = 0; i < 5; i ++)
  {
   angle = grand(ANGLE_1);
   dist = 20000 + grand(20000);
   size = 10 + grand(30);
   quick_cloud(CLOUD_BALL, x + xpart(angle, dist), y + xpart(angle, dist), 0, 0, size, GUARD_CLOUD_COL, 0);
   quick_cloud(CLOUD_BALL_COL2, x + xpart(angle, dist), y + ypart(angle, dist), 0, 0, size + 5 + grand(10), GUARD_CLOUD_COL, 0);
  }*/

/*  if (c != -1)
  {
   cloud[c].angle = grand(ANGLE_1);
   cloud[c].status = 10;
  }*/

 part[p].killed = 1;
 if (core_destroyed == 0)
 {
  part[first].chain += part[p].size;
  part[first].wasted += part[p].max_hp - part[p].hp;
 }

 chain_explosion(part[p].downlink [0], first, core_destroyed);
 chain_explosion(part[p].downlink [1], first, core_destroyed);

}


void destroy_part(int p)
{

 if (part[p].type != PART_NONE && part[p].weapon != -1)
 {
  hs[0].weapon_number [part[p].weapon] --;

  if (hs[0].weapon_number [part[p].weapon] <= 0)
   hs[0].weapon [part[p].weapon] = PART_NONE;
 }

 part[p].type = PART_NONE;

}

int part_track_target(int p, int turning, int forbid)
{
 int retval = turn_towards_xy_forbid(part[p].x, part[p].y, player[0].x, player[0].y, part[p].angle, turning, forbid);

 if (retval < part[p].angle || (part[p].angle < ANGLE_8 && retval > (ANGLE_1 - ANGLE_8)))
  part[p].slew_dir = -1;
   else
    part[p].slew_dir = 1;

  return retval;
}

// also calculates hs' inertia
void calculate_centre_of_mass(int h)
{

 hs[h].centre_x = hs[h].x;
 hs[h].centre_y = hs[h].y;

 int x = 0, y = 0, i;

 int weight = CORE_WEIGHT; // need to make sure this matches up with MoI!!
 // note - also used in parts_destroyed

 for (i = 0; i < NO_PARTS; i ++)
 {
  if (part[i].type == PART_NONE)
   continue;
  x += (part[i].x - hs[h].x) * part[i].size;
  y += (part[i].y - hs[h].y) * part[i].size;
  weight += part[i].size;
 }

 hs[h].mass = weight; // yeah, I know

 hs[h].centre_x = hs[h].x + (x / weight);
 hs[h].centre_y = hs[h].y + (y / weight);

}


void calculate_moment_of_inertia(int h)
{
  int i;

  hs[h].moment = 5000; // core's moment
  int dist;
  int mass;

  for (i = 0; i < NO_PARTS; i ++)
  {
   if (part[i].type == PART_NONE)
    continue;
   dist = ((int) hypot(part[i].y - hs[h].centre_y, part[i].x - hs[h].centre_x)) >> 12;
   mass = part[i].size;
   hs[h].moment += mass * dist * dist;
  }


}

void apply_torque_to_hs(int h, int x, int y, int force, int angle)
{

 int dist = ((int) hypot(y - hs[h].centre_y, x - hs[h].centre_x)) >> 12;
 int lever_angle = radians_to_angle(atan2(y - hs[h].centre_y, x - hs[h].centre_x));
 int torque = ypart(lever_angle - angle, dist * force);

// float delta_spin = angle_to_radians(torque / hs[h].moment);

// hs[h].spin += angle_to_radians(torque / hs[h].moment); //delta_spin;
 hs[h].spin += angle_to_radians(torque) / hs[h].moment; //delta_spin;

}

void apply_frame_torque_to_hs(int h, int x, int y, int force, int angle, int p)
{
// like the previous torque function but only rotates the hs for 1 frame and doesn't affect its spin.
//  used for
 int dist = ((int) hypot(y - hs[h].centre_y, x - hs[h].centre_x)) >> 12;
 int lever_angle = radians_to_angle(atan2(y - hs[h].centre_y, x - hs[h].centre_x));
 int torque = ypart(lever_angle - angle, dist * force);

// float delta_spin = angle_to_radians(torque / hs[h].moment);

// hs[h].spin += angle_to_radians(torque / hs[h].moment); //delta_spin;
 hs[h].angle += angle_to_radians(torque) / hs[h].moment; //delta_spin;

//   part[p].debug = angle;
//   part[p].fdebug = part[p].position_move_angle [hs[h].position];

}

/*
void apply_torque_to_hs(int h, int x, int y, int force, int angle)
{

 int dist = ((int) hypot(y - hs[h].centre_y, x - hs[h].centre_x)) >> 16;//>> 12;
 int lever_angle = radians_to_angle(atan2(y - hs[h].centre_y, x - hs[h].centre_x));
 float torque = fypart(lever_angle - angle, dist * force);

 float delta_spin = (float) torque / hs[h].moment;

 hs[h].spin += delta_spin;

}
*/
void hs_impulse(int h, int force_angle, int impulse)
{

 int accel = impulse / hs[h].mass;

 hs[0].x_speed += xpart(force_angle, accel);
 hs[0].y_speed += ypart(force_angle, accel);

}


void move_hs(int h)
{
//return;
 float angle = atan2(hs[h].centre_y - hs[h].y, hs[h].centre_x - hs[h].x);
 int dist = hypot(hs[h].y - hs[h].centre_y, hs[h].x - hs[h].centre_x);

/* hs[h].x_speed = 0;
 hs[h].y_speed = 0;
// hs[h].spin = 0;

 if (key [KEY_O])
  hs[0].spin += PI / 120;
 if (key [KEY_P])
  hs[0].spin -= PI / 120;
 if (key [KEY_L])
  hs[0].spin = 0;
 if (key [KEY_K])
 {
  hs[0].x_speed = xpart(angle, 2000);
  hs[0].y_speed = ypart(angle, 2000);
 }

*/
 angle += hs[h].spin;

 hs[h].centre_x += hs[h].x_speed;
 hs[h].centre_y += hs[h].y_speed;

 hs[h].x_speed *= 1010;
 hs[h].x_speed >>= 10;
 hs[h].y_speed *= 1010;
 hs[h].y_speed >>= 10;
   hs[h].spin *= 0.97;

 if (hs[h].firing_a_beam)
 {
  hs[h].spin *= 0.93;
  hs[h].x_speed *= 1000;
  hs[h].x_speed >>= 10;
  hs[h].y_speed *= 1000;
  hs[h].y_speed >>= 10;
 }

// if (hs[h].spin > 0.02
  //|| hs[h].spin < -0.02)

 hs[h].x = hs[h].centre_x - fxpart(angle, dist);
 hs[h].y = hs[h].centre_y - fypart(angle, dist);

 hs[h].angle += hs[h].spin;

}

void fix_angular_velocity(int h, int old_moment)
{

 float angular_momentum = old_moment * hs[h].spin;
// need to make sure that angular momentum is conserved despite changes in moment of inertia.

 hs[h].spin = angular_momentum / hs[h].moment;


}


void parts_destroyed(int h)
{

 int i;

 int old_centre_x = hs[h].centre_x;
 int old_centre_y = hs[h].centre_y;

 hs[h].centre_x = hs[h].x; // this could be any arbitrary coordinate set.
 hs[h].centre_y = hs[h].y;

 int x = (hs[h].old_x - hs[h].x) * 20; // note weight used here!!!
 int y = (hs[h].old_y - hs[h].y) * 20;

 int weight = CORE_WEIGHT;
 // note - also used in calculate_centre_of_mass()
 // and below!

// let's work out the centre of mass of the surviving system of masses as it was
//  for the last frame:
 for (i = 0; i < NO_PARTS; i ++)
 {
  if (part[i].type == PART_NONE
   || part[i].killed == 1)
   continue;
  x += (part[i].old_x2 - hs[h].x) * part[i].size;
  y += (part[i].old_y2 - hs[h].y) * part[i].size;
  weight += part[i].size;
 }

 old_centre_x = hs[h].x + (x / weight);
 old_centre_y = hs[h].y + (y / weight);

// now let's work out the new centre of mass:

 x = 0;
 y = 0;
 weight = CORE_WEIGHT; // need to make sure this matches up with MoI!!
 // note - also used in calculate_centre_of_mass()
 // and above!

 for (i = 0; i < NO_PARTS; i ++)
 {
  if (part[i].type == PART_NONE
   || part[i].killed == 1)
   continue;
  x += (part[i].x - hs[h].x) * part[i].size;
  y += (part[i].y - hs[h].y) * part[i].size;
  weight += part[i].size;
 }

 hs[h].centre_x = hs[h].x + (x / weight);
 hs[h].centre_y = hs[h].y + (y / weight);

// now we know how much the centre of mass of the system of surviving masses moved in the last frame.
//  this is the new velocity of that system:
 hs[h].x_speed = hs[h].centre_x - old_centre_x;
 hs[h].y_speed = hs[h].centre_y - old_centre_y;

}



