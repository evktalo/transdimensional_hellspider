
#include "config.h"

#include "allegro.h"

#include <math.h>

#include "globvars.h"
#include "palette.h"
#include "hs.h"
#include "hs_build.h"
#include "display.h"

#include "stuff.h"

void build_hs(void);
int new_part_from_map(int type, int p);
void copy_part(int from, int to);
void mirror_hs(void);
void arrange_all_parts(int h);
void arrange_part(int h, int p);
int check_position_blocked(int h, int start_p, int size_plus);
int flex_arm(int h, int start_p, int arm, int side, float start_angle_inc, int pos, int size_plus);
void calculate_flex_angles(int h);
void mirror_hs_angles(void);
void set_positions(int h, int p, int pos);
void calculate_fixed_angles(int h);
void mirror_arm(int h, int mirror_p, int previous, int dl, int arm);
void set_angle_inc(int p, int last_p);
void flex_subroutine(int h, int p, int side, int size_plus);
void set_positions_subroutine(int h, int p, int pos);
void arrange_part_subroutine(int h, int p);
void mirror_hs_angles_subroutine(int p, int p2);
int check_position_blocked_subroutine(int h, int p, int size_plus);
int new_part(int type);
int new_map(int type);
void init_part(int p);
void map_arrange_part(int h, int m);
void map_arrange_part_subroutine(int h, int m);
void map_arrange_all_parts(int h);
int map_check_position_blocked_subroutine(int h, int m);
int map_check_position_blocked(int h, int start_m);
void map_arrange_arm(int h, int start_m);
void arrange_arm(int h, int start_p);
int grow_new_map(int h, int ul, int type, int dl_pos, int ulp);
void destroy_map(int m);
void hs_evolves(int h);
void set_basic_map_properties(int m);
int find_best_weapon(int p);
void flex_forward_subroutine(int h, int p, float flex_amount, int size_plus, int count);
void draw_hs_build_debug(int col, int number);
void draw_map_hs_build_debug(int col, int number);
void mapping_sub(int m);
void setup_weapon_list_part(void);
void setup_weapon_list_map(void);
int get_part_hp(int size);

// used in flex_arm etc
float angle_inc [100];
float global_angle_inc;

extern BITMAP *display;

int dline;

int part_ranges [NO_PARTS] [5] =
{
// best, minimum, maximum, track type
 {0, 0, 0, TRACK_LOCKED, DIR_NONE}, // PART_NONE
 {6, 6, 6, TRACK_FOLLOW, DIR_ALL}, // PART_BASIC,
  {10, 9, 10, TRACK_LOCKED, DIR_FORWARD}, // PART_FORWARD,
 {0, 0, 0, TRACK_LOCKED, DIR_NONE}, // PART_DRIVE,
  {17, 12, 1000, TRACK_LOCKED, DIR_FORWARD}, // PART_MISSILE,
 {2, 0, 4, TRACK_FOLLOW, DIR_ALL}, // PART_MGUN,
 {8, 4, 14, TRACK_FOLLOW, DIR_ALL}, // PART_3CANNON,
  {4, 0, 7, TRACK_LOCKED, DIR_FORWARD}, // PART_ROTARY,
 {7, 4, 11, TRACK_LOCKED, DIR_ALL}, // PART_SEEKER,
 {5, 3, 6, TRACK_BEAM, DIR_ALL}, // PART_BEAM,
  {6, 4, 8, TRACK_LOCKED, DIR_FORWARD}, // PART_BEAM2,
 {3, 2, 5, TRACK_FOLLOW, DIR_ALL}, // PART_FLAK,
 {0, 0, 0, TRACK_LOCKED, DIR_NONE}, // PART_SHIELD
 {9, 5, 14, TRACK_FOLLOW, DIR_ALL}, // PART_ROCKET,
  {12, 10, 15, TRACK_LOCKED, DIR_FORWARD}, // PART_EEL,
 {0, 0, 0, TRACK_LOCKED, DIR_NONE}, // PART_DRIVE2,

/*
 {0, 0, 0, TRACK_LOCKED}, // PART_NONE
 {8, 7, 9, TRACK_FOLLOW}, // PART_BASIC,
 {11, 10, 12, TRACK_LOCKED}, // PART_FORWARD,
 {0, 0, 0, TRACK_LOCKED}, // PART_DRIVE,
 {40, 20, 1000, TRACK_LOCKED}, // PART_MISSILE,
 {6, 0, 10, TRACK_FOLLOW2}, // PART_MGUN,
 {0, 0, 0, TRACK_FOLLOW}, // PART_3CANNON,
 {16, 10, 20, TRACK_LOCKED}, // PART_ROTARY,
 {14, 4, 18, TRACK_LOCKED}, // PART_SEEKER,
 {12, 6, 16, TRACK_BEAM}, // PART_BEAM,
 {8, 0, 18, TRACK_LOCKED}, // PART_BEAM2,
 {10, 6, 12, TRACK_FOLLOW}, // PART_FLAK,
 {0, 0, 0, TRACK_LOCKED} // PART_SHIELD
*/
// evolves bit assumes that missiles have the longest range.
};

void init_hs_at_stage_1(int h)
{

 hs[h].stage = 1;

 evolve_screen(1);

 hs[h].core_strength = 0;

 int i;

 for (i = 0; i < 4; i ++)
 {
   hs[0].arm_map [0] [i] = -1;
   hs[0].arm_map [1] [i] = -1;
 }

 for (i = 0; i < NO_PARTS; i ++)
 {
  map[i].type = PART_NONE;
 }


    hs[0].core_circle_size [0] = 15;
    hs[0].core_circle_angle [0] = 0;
    hs[0].core_circle_dist [0] = 0;
    hs[0].core_circle_dist2 [0] = 0;

    hs[0].core_circle_size [1] = 13;
    hs[0].core_circle_angle [1] = 0;
    hs[0].core_circle_dist [1] = 13;
    hs[0].core_circle_dist2 [1] = 15;

    hs[0].core_circle_size [2] = 11;
    hs[0].core_circle_angle [2] = -PI;
    hs[0].core_circle_dist [2] = 11;
    hs[0].core_circle_dist2 [2] = 13;

    hs[0].core_circle_size [3] = 11;
    hs[0].core_circle_angle [3] = -PI_4;
    hs[0].core_circle_dist [3] = 13;
    hs[0].core_circle_dist2 [3] = 17;

    hs[0].core_circle_size [4] = 11;
    hs[0].core_circle_angle [4] = PI_4;
    hs[0].core_circle_dist [4] = 13;
    hs[0].core_circle_dist2 [4] = 17;


// FORWARD arm
 int m;

 m = new_map(PART_FORWARD);

 if (m == -1)
  exit(10);

 map[m].side = 0;
 map[m].arm = 0;
 hs[h].arm_map [map[m].side] [map[m].arm] = m;

 map[m].uplink = -1; // indicates linked to core
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = PI/2 + PI_4 + PI_32;
 map[m].up_dist = 32000;
 map[m].max_hp = get_part_hp(map[m].size);
 map[m].hp = map[m].max_hp;
 map[m].size = 7;
 map[m].angle = radians_to_angle(map[m].up_angle);//map[m].up_angle - ANGLE_4 - ANGLE_32;//radians_to_angle(map[m].up_angle);
// map[m].locked_angle = angle_to_radians(map[m].angle);
 map[m].tracks = part_ranges [map[m].type] [3];
// map[m].tracks = TRACK_FOLLOW;
 map[m].slew = 0;//3;
 set_basic_map_properties(m);
/*
 int m2 = m;

 m = new_map(PART_SEEKER);

 map[m].side = 0;
 map[m].arm = 0;

 map[m].uplink = m2; // indicates linked to core
 map[m2].downlink [0] = m;
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = 0;
 map[m].up_dist = 32000;
 map[m].max_hp = 1000;
 map[m].hp = map[m].max_hp;
 map[m].size = 11;
 map[m].angle = radians_to_angle(map[m].up_angle);//map[m].up_angle - ANGLE_4 - ANGLE_32;//radians_to_angle(map[m].up_angle);
// map[m].locked_angle = angle_to_radians(map[m].angle);
 map[m].tracks = TRACK_LOCKED;
 //map[m].tracks = TRACK_FOLLOW2;
 map[m].slew = 10;

 m2 = m;

 m = new_map(PART_SEEKER);

 map[m].side = 0;
 map[m].arm = 0;

 map[m].uplink = m2; // indicates linked to core
 map[m2].downlink [0] = m;
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = 0;
 map[m].up_dist = 32000;
 map[m].max_hp = 1000;
 map[m].hp = map[m].max_hp;
 map[m].size = 11;
 map[m].angle = radians_to_angle(map[m].up_angle);//map[m].up_angle - ANGLE_4 - ANGLE_32;//radians_to_angle(map[m].up_angle);
// map[m].locked_angle = angle_to_radians(map[m].angle);
 map[m].tracks = TRACK_LOCKED;
 //map[m].tracks = TRACK_FOLLOW2;
 map[m].slew = 10;

 m2 = m;

 m = new_map(PART_SEEKER);

 map[m].side = 0;
 map[m].arm = 0;

 map[m].uplink = m2; // indicates linked to core
 map[m2].downlink [0] = m;
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = 0;
 map[m].up_dist = 32000;
 map[m].max_hp = 1000;
 map[m].hp = map[m].max_hp;
 map[m].size = 11;
 map[m].angle = radians_to_angle(map[m].up_angle);//map[m].up_angle - ANGLE_4 - ANGLE_32;//radians_to_angle(map[m].up_angle);
// map[m].locked_angle = angle_to_radians(map[m].angle);
 map[m].tracks = TRACK_LOCKED;
 //map[m].tracks = TRACK_FOLLOW2;
 map[m].slew = 10;

 m2 = m;

 m = new_map(PART_SEEKER);

 map[m].side = 0;
 map[m].arm = 0;

 map[m].uplink = m2; // indicates linked to core
 map[m2].downlink [0] = m;
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = 0;
 map[m].up_dist = 32000;
 map[m].max_hp = 1000;
 map[m].hp = map[m].max_hp;
 map[m].size = 11;
 map[m].angle = radians_to_angle(map[m].up_angle);//map[m].up_angle - ANGLE_4 - ANGLE_32;//radians_to_angle(map[m].up_angle);
// map[m].locked_angle = angle_to_radians(map[m].angle);
 map[m].tracks = TRACK_LOCKED;
 //map[m].tracks = TRACK_FOLLOW2;
 map[m].slew = 10;
*/

// BASIC arm
 m = new_map(PART_BASIC);

 map[m].side = 0;
 map[m].arm = 1;
 hs[h].arm_map [map[m].side] [map[m].arm] = m;

 map[m].uplink = -1; // indicates linked to core
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = PI/2;
 map[m].up_dist = 32000;
 map[m].max_hp = get_part_hp(map[m].size);
 map[m].hp = map[m].max_hp;
 map[m].size = 9;
 map[m].angle = 0;//radians_to_angle(map[m].up_angle);
// map[m].side = 0;
// map[m].arm = 0;
 map[m].tracks = part_ranges [map[m].type] [3];
// map[m].slew = 32;
 set_basic_map_properties(m);


// DRIVE arm
 m = new_map(PART_DRIVE);

 map[m].side = 0;
 map[m].arm = 2;
 hs[h].arm_map [map[m].side] [map[m].arm] = m;

 map[m].uplink = -1; // indicates linked to core
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = PI/2 - PI_4;
 map[m].up_dist = 32000;
 map[m].max_hp = get_part_hp(map[m].size);
 map[m].hp = map[m].max_hp;
 map[m].size = 7;
 map[m].angle = -ANGLE_4;//radians_to_angle(map[m].up_angle);
 map[m].locked_angle = angle_to_radians(map[m].angle);
 map[m].tracks = part_ranges [map[m].type] [3];
 map[m].drive = 0;
 map[m].max_drive = map[m].size * 15;
 set_basic_map_properties(m);

/*
 w = 0;

 hs[h].weapon [0] = PART_NONE;
 hs[h].weapon [1] = PART_NONE;
 hs[h].weapon [2] = PART_NONE;
 hs[h].weapon [3] = PART_NONE;
 hs[h].weapon [4] = PART_NONE;

 for (i = 0; i < NO_PARTS; i ++)
 {
  if (part[i].type
 hs[h].weapon [0] = map[0].type;
 hs[h].weapon [1] = map[1].type;
 hs[h].weapon [2] = PART_NONE;
 hs[h].weapon [3] = PART_NONE;
 hs[h].weapon [4] = PART_NONE;
 hs[h].firing = 0;
 hs[h].firing_time = 250;
*/
/*
 int m2 = m;

 m = new_map(PART_DRIVE);

 map[m].side = 0;
 map[m].arm = 2;

 map[m].uplink = m2; // indicates linked to core
 map[m2].downlink [0] = m;
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = 0;//PI/2 - PI_4;
 map[m].up_dist = 32000;
 map[m].max_hp = 1000;
 map[m].hp = map[m].max_hp;
 map[m].size = 7;
 map[m].angle = -ANGLE_4;//radians_to_angle(map[m].up_angle);
 map[m].locked_angle = angle_to_radians(map[m].angle);
 map[m].tracks = TRACK_LOCKED;
 map[m].drive = 0;
 map[m].max_drive = map[m].size * 10;

*/

 build_hs();

 do
 {
     rest(1);
 } while(evolve_screen(0) == 0);

}


void respawn_hs(int h)
{

 evolve_screen(1);

   hs[h].angle = 0;
   hs[h].x = 0;
   hs[h].y = 0;

   // evolves
   hs_evolves(h);

   build_hs();

    hs[0].position = 0;

//   draw_hs_build_debug(COL_LGREY, 9);

 do
 {
     rest(1);
 } while(evolve_screen(0) == 0);

}


void build_hs(void)
{

    dline = 0;

    hs[0].core_max_hp = 3000 + hs[0].core_strength * 100;
    hs[0].core_hp = hs[0].core_max_hp;
    hs[0].alive = 1;
    hs[0].counter = 0;

// hs[0].x =
    hs[0].x = 0;//player[0].x - xpart(player[0].angle, 300000);
    hs[0].y = 0;//player[0].y - ypart(player[0].angle, 300000);
    hs[0].x_speed = 0;
    hs[0].y_speed = 0;
    hs[0].angle = 0;
    hs[0].spin = 0;
//    hs[0].turn_speed = 0;
    hs[0].x_speed = 0;
    hs[0].y_speed = 0;
    hs[0].hurt_flash = 0;
    hs[0].moment = 100; // this will probably be calculated before it's needed
    hs[0].mass = 100; // this will probably be calculated before it's needed
    hs[0].movement = MOVE_FORWARDS;
    hs[0].next_thought = 5;
    hs[0].next_reposition = 5;
    hs[0].target_position = 0;
    hs[0].range_minimum = 0;

    hs[0].destroyed_index = 0;
    hs[0].destroyed [0] = -1;
    hs[0].firing_a_beam = 0;
    hs[0].firing [0] = 0;
    hs[0].firing [1] = -2;

 int i, j;

 for (i = 0; i < 4; i ++)
 {
   hs[0].arm_part [0] [i] = -1;
   hs[0].arm_part [1] [i] = -1;
 }

 int m, p;

 for (p = 0; p < NO_PARTS; p ++)
 {
  part[p].type = PART_NONE;
  part[p].chain = 0;
  part[p].downlink [0] = -1;
  part[p].downlink [1] = -1;
  part[p].uplink = -1;
 }

 for (m = 0; m < NO_PARTS; m ++)
 {
  if (map[m].type != PART_NONE)
  {
   p = new_part_from_map(map[m].type, m); // the "p =" isn't necessary
  }
 }

   draw_hs_build_debug(COL_LGREY, 1);

 for (i = 0; i < 4; i ++)
 {
   hs[0].arm_part [0] [i] = hs[0].arm_map [0] [i];
   hs[0].arm_part [1] [i] = hs[0].arm_map [1] [i];
 }

   draw_hs_build_debug(COL_LGREY, 2);


 mirror_hs();

   draw_hs_build_debug(COL_LGREY, 3);

 arrange_all_parts(0);

   draw_hs_build_debug(COL_LGREY, 4);

 evolve_screen(0);


 calculate_flex_angles(0);
   draw_hs_build_debug(COL_LGREY, 5);

 calculate_fixed_angles(0);
    draw_hs_build_debug(COL_LGREY, 6);

 mirror_hs_angles();

   draw_hs_build_debug(COL_LGREY, 7);

// need to just mirror angles here

 calculate_centre_of_mass(0);
 calculate_moment_of_inertia(0);
 set_drive_directions(0);

   draw_hs_build_debug(COL_LGREY, 8);

 setup_weapon_list_part();

 evolve_screen(0);

 for (j = 0; j < NO_PARTS; j ++)
 {
  if (part[j].type != PART_NONE)
   part[j].max_hp = get_part_hp(part[j].size);
  part[j].hp = part[j].max_hp;
 }


int h = 0;
    hs_change_position(0, 1);
    calculate_centre_of_mass(0);
    calculate_moment_of_inertia(h);

    for (p = 0; p < NO_PARTS; p ++)
    {
      if (part[p].type == PART_NONE)
       continue;
      part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);
      part[p].y = hs[h].y + fypart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);


  switch(part[p].tracks)
  {
      case TRACK_LOCKED:
       part[p].angle = radians_to_angle(part[p].position_up_angle [hs[h].position] + hs[h].angle + part[p].locked_angle);// + part[p].locked_angle);
       break;
  }

    }



    set_drive_directions(0);


/*
  textprintf_ex(screen, font, 10, 100 + glob * 15, -1, -1, "part %i: x %i y %i drive %i torque %i, part %i: x %i y %i drive %i torque %i", 2, part[2].x, part[2].y, part[2].drive_turn, part[2].debug, 5, part[5].x, part[5].y, part[5].drive_turn, part[2].debug);
  do
  {
   rest(5);
  } while (key [KEY_1] == 0);
  glob ++;*/


   int appear_angle = player[0].angle + grand(ANGLE_4) - grand(ANGLE_4);

    hs[0].x = player[0].x + xpart(appear_angle, 800000);
    hs[0].y = player[0].y + ypart(appear_angle, 800000);

}

int get_part_hp(int size)
{

 return (size * 250) + ((size*size*size) / 4) - 50;

}


void setup_weapon_list_map(void)
{

 int i, j, finished = 0;

  for (j = 0; j < NO_WEAPONS; j ++)
  {
   hs[0].weapon_number [j] = 0;
   hs[0].weapon [j] = PART_NONE;
  }

 for (i = 0; i < NO_PARTS; i ++)
 {
  if (map[i].type == PART_NONE
   || map[i].type == PART_DRIVE
   || map[i].type == PART_DRIVE2
   || map[i].type == PART_SHIELD)
   {
    map[i].weapon = -1;
    continue;
   }
  finished = 0;

// first let's see whether there's already a slot for this weapon type:
  for (j = 0; j < NO_WEAPONS; j ++)
  {
   if (hs[0].weapon [j] == map[i].type)
   {
    hs[0].weapon_number [j] ++;
    finished = 1;
    map[i].weapon = j;
    break;
   }
  }
  if (finished == 1)
   continue;

// no? Okay, create a new one.
  for (j = 0; j < NO_WEAPONS; j ++)
  {
   if (hs[0].weapon [j] == PART_NONE)
   {
    hs[0].weapon [j] = map[i].type;
    hs[0].weapon_number [j] = 1;
    map[i].weapon = j;
    break;
   }
  }
 }
}

void setup_weapon_list_part(void)
{

 int i, j, finished;

  for (j = 0; j < NO_WEAPONS; j ++)
  {
   hs[0].weapon_number [j] = 0;
   hs[0].weapon [j] = PART_NONE;
  }

 for (i = 0; i < NO_PARTS; i ++)
 {
  if (part[i].type == PART_NONE
   || part[i].type == PART_DRIVE
   || part[i].type == PART_DRIVE2
   || part[i].type == PART_SHIELD)
   {
    part[i].weapon = -1;
    continue;
   }
  finished = 0;

// first let's see whether there's already a slot for this weapon type:
  for (j = 0; j < NO_WEAPONS; j ++)
  {
   if (hs[0].weapon [j] == part[i].type)
   {
    hs[0].weapon_number [j] ++;
    finished = 1;
    part[i].weapon = j;
    break;
   }
  }
  if (finished == 1)
   continue;

// no? Okay, create a new one.
  for (j = 0; j < NO_WEAPONS; j ++)
  {
   if (hs[0].weapon [j] == PART_NONE)
   {
    hs[0].weapon [j] = part[i].type;
    hs[0].weapon_number [j] = 1;
    part[i].weapon = j;
    break;
   }
  }
 }
}


void calculate_fixed_angles(int h)
{

 hs[h].position = (POSITIONS - 1);
 arrange_all_parts(h);

 int p;

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].tracks != TRACK_LOCKED)
   continue;

//  part[p].angle = radians_to_angle(part[p].position_up_angle [hs[h].position] + hs[h].angle + part[p].locked_angle);
//  part[p].locked_angle = 2*PI - radians_to_angle(part[p].position_up_angle [hs[h].position] + hs[h].angle);
  part[p].locked_angle = hs[h].angle - part[p].position_up_angle [hs[h].position] + PI;

  if (part[p].type == PART_DRIVE
   || part[p].type == PART_DRIVE2)
   part[p].locked_angle -= PI;

  if (part[p].type == PART_SEEKER)
   part[p].locked_angle += PI_8;


 }



}



void calculate_flex_angles(int h)
{

 int p, j, count, p2;
 int i;

 float flex_amount;
 int size_plus = 0;

// first let's put the hs into closed position, with all parts as far forward as possible.
//  (rearmost 'arms' go back instead of forward - this gives increased manouevreability).

  for (i = 0; i < 3; i ++)
  {
   switch(i)
   {
    case 0: j = 2; break;
    case 1: j = 0; break;
    case 2: j = 1; break;
   }
   if (hs[h].arm_part [0] [j] == -1)
    continue;
//   p = part[hs[h].arm_part [0] [j]].downlink [0];
   p = hs[h].arm_part [0] [j];

   if (p == -1)
    continue;

   flex_amount = 0.01;
   if (j == 2)
    flex_amount = -0.01;

   switch(j)
   {
//    case 0: flex_amount = 0.03; break;
//    case 1: flex_amount = 0.022; break;
//    case 2: flex_amount = 0.016;
    case 0: flex_amount = 0.010; size_plus = 0; break;
    case 1: flex_amount = 0.009; size_plus = 3; break;
    case 2: flex_amount = -0.008; size_plus = 0; break;
   }

   //do
   {
    count = 0;
    do
    {
     set_positions(h, hs[h].arm_part [0] [j], count);
     flex_forward_subroutine(h, p, flex_amount, size_plus, count);
     evolve_screen(0);
//     draw_hs_build_debug(COL_LGREY);
     count ++;
    }
     while (count < 200);

   }
    //while (p2 != -1);
/*
   do
   {
    count = 0;

    do
    {


     set_positions(h, hs[h].arm_part [0] [j], count);
     count ++;

    } while (count < POSITIONS && flex_arm(h, p, j, 0, flex_amount, 0, size_plus) == 1);

    p = part[p].downlink [0];


   } while (p != -1);
*/
/*
   do
   {
    count = 0;

    do
    {


     set_positions(h, hs[h].arm_part [0] [j], count);
     count ++;

    } while (count < POSITIONS && flex_arm(h, p, j, 0, flex_amount, 0, size_plus) == 1);

    p = part[p].downlink [0];


   } while (p != -1);
*/



  }

// now let's move them back step by step, starting with the second from rearmost, and record each part's
//  vector position (from the core) for each step:


    hs[h].position = 0;

    do
    {

  for (i = 0; i < 3; i ++)
  {
   switch(i)
   {
    case 0: j = 2; break;
    case 1: j = 1; break;
    case 2: j = 0; break;
   }

   if (hs[h].arm_part [0] [j] == -1)
    continue;
   p = hs[h].arm_part [0] [j];//part[hs[h].arm_part [0] [j]].downlink;

   if (p == -1)
    continue;

   flex_amount = -0.01;
   if (j == 3)
    flex_amount = 0.01;

   switch(j)
   {
    case 0: flex_amount = -0.010; size_plus = 0; break;
    case 1: flex_amount = -0.009; size_plus = 3; break;
    case 2: flex_amount = 0.008; size_plus = 0; break;
   }

   for (p2 = 0; p2 < NO_PARTS; p2 ++)
   {
    part[p2].old_x = part[p2].x;
    part[p2].old_y = part[p2].y;
   }

   flex_arm(h, p, j, 0, flex_amount, hs[h].position, size_plus);
//     draw_hs_build_debug(COL_WHITE);

   set_positions(h, hs[h].arm_part [0] [j], hs[h].position);
   evolve_screen(0);

   for (p2 = 0; p2 < NO_PARTS; p2 ++)
   {
    if (part[p2].arm != j
     || part[p2].type == PART_NONE
     || part[p2].side != 0)
      continue;
    part[p2].position_move_angle [hs[h].position] = atan2(part[p2].y - part[p2].old_y, part[p2].x - part[p2].old_x);
    part[p2].position_move_dist [hs[h].position] = hypot(part[p2].y - part[p2].old_y, part[p2].x - part[p2].old_x);
//    part[p2].position_move_angle [hs[h].position] = atan2(part[p2].old_y - part[p2].y, part[p2].old_x - part[p2].x);
//    part[p2].position_move_dist [hs[h].position] = hypot(part[p2].old_y - part[p2].y, part[p2].old_x - part[p2].x);
   }

  }
      hs[h].position ++;

    } while (hs[h].position < POSITIONS);


}

void flex_forward_subroutine(int h, int p, float flex_amount, int size_plus, int count)
{

    if (p == -1)
     return;
//    flex_arm(h, p, part[p].arm, 0, flex_amount, count, size_plus);
    flex_arm(h, p, part[p].arm, 0, flex_amount, count, size_plus);
//    textprintf_ex(screen, font, 10, 10, -1, -1, "h %i p %i a %i f_a %f count %i", h, p, part[p].arm, flex_amount, count);
//    rest(1);

    flex_forward_subroutine(h, part[p].downlink [0], flex_amount, size_plus, count);
    flex_forward_subroutine(h, part[p].downlink [1], flex_amount, size_plus, count);

}


void set_positions(int h, int p, int pos)
{

 if (pos >= POSITIONS)
  return;

/* int ul;

 while (p != -1)
 {


  p = part[p].downlink [0];
 };*/

 set_positions_subroutine(h, p, pos);


}

void set_positions_subroutine(int h, int p, int pos)
{

  if (p == -1)
   return;

  part[p].position_angle [pos] = atan2(part[p].y - hs[h].y, part[p].x - hs[h].x);
  part[p].position_dist [pos] = hypot(part[p].y - hs[h].y, part[p].x - hs[h].x);
  int ul = part[p].uplink;
  if (ul == -1)
  {
   part[p].position_up_angle [pos] = part[p].position_angle [pos];
   part[p].position_up_dist [pos] = part[p].position_dist [pos];
  }
   else
   {
    part[p].position_up_angle [pos] = atan2(part[p].y - part[ul].y, part[p].x - part[ul].x);
    part[p].position_up_dist [pos] = hypot(part[p].y - part[ul].y, part[p].x - part[ul].x);
   }

 set_positions_subroutine(h, part[p].downlink [0], pos);
 set_positions_subroutine(h, part[p].downlink [1], pos);


}



int flex_arm(int h, int start_p, int arm, int side, float start_angle_inc, int pos, int size_plus)
{
//return 1;
 int p = start_p;

 global_angle_inc = start_angle_inc;

 set_angle_inc(start_p, -1);


 p = start_p;

 flex_subroutine(h, p, side, size_plus);

//   while (p != -1)
//   {


//here

//   };

   return 1;

}

void flex_subroutine(int h, int p, int side, int size_plus)
{
    if (p == -1)
     return;

    if (part[p].uplink != -1)
    {
     part[p].up_angle += angle_inc [p];
     //arrange_all_parts(h);
     arrange_arm(h, p);

//    float angle = atan2(part[p].y, part[p].x);

//    if ((side == 0 && part[p].y - (part[p].size << 10) < 10000)
//     || (side == 1 && part[p].y + (part[p].size << 10) > -10000)
     if (check_position_blocked(h, p, size_plus) != 0)
     {
      part[p].up_angle -= angle_inc [p];
//     return;
     }
    }

    flex_subroutine(h, part[p].downlink [0], side, size_plus);

    flex_subroutine(h, part[p].downlink [1], side, size_plus);

}

/*
int check_all_downlinks(int h, int p)
{

    if (p == -1)
     return 1;

    if ((side == 0 && part[p].y - (part[p].size << 10) < 10000)
     || (side == 1 && part[p].y + (part[p].size << 10) > -10000)
     || check_position_blocked(h, p) != 0)
    {
     part[p].up_angle -= angle_inc [p];
     return 0;
    }

    int ret = flex_subroutine(h, part[p].downlink [0], side);
    if (ret == 0)
    {
     part[p].up_angle -= angle_inc [p];

    }

    ret = flex_subroutine(h, part[p].downlink [1], side);
    if (ret == 0)
    {
     return 0;
    }

    return 1;

}

*/
/*
void flex_subroutine(int h, int p, int side)
{
    if (p == -1)
     return;

    part[p].up_angle += angle_inc [p];
    arrange_all_parts(h);

//    float angle = atan2(part[p].y, part[p].x);

    if ((side == 0 && part[p].y - (part[p].size << 10) < 10000)
     || (side == 1 && part[p].y + (part[p].size << 10) > -10000)
     || check_position_blocked(h, p) != 0)
//    if (check_position_blocked(h, p) != 0)
    {
     part[p].up_angle -= angle_inc [p];
 //    arrange_all_parts(h);
    }

    flex_subroutine(h, part[p].downlink [0], side);
    flex_subroutine(h, part[p].downlink [1], side);

}
*/

/*
int cancel_flex(int p, int start_p, int finish);
{

 if (p == -1)
  return -1;

 int i;

 i = p;

  part[i].up_angle -= angle_inc [p];
  i = part[i].downlink [0];
// } while (i != part[p].downlink [0]);

 cancel_flex(part[p].downlink [0], start_p);
 cancel_flex(part[p].downlink [1], start_p);

 start_p = part[p].downlink [0];
 p = part[p].downlink [0];

}
*/

// this function is called recursively!
void set_angle_inc(int p, int last_p)
{

  if (p == -1)
   return;

  if (last_p == -1)
  {
   angle_inc [p] = global_angle_inc;
  }
   else
   {
    angle_inc [p] = angle_inc [last_p] * 0.99;
   }

  set_angle_inc(part[p].downlink [0], p);
  set_angle_inc(part[p].downlink [1], p);

}

/*
int flex_arm(int h, int start_p, int arm, int side, float start_angle_inc, int pos)
{

 int p = start_p;
 int i;

float angle_inc [100];

 while (p != -1)
 {
  angle_inc [p] = start_angle_inc;
  start_angle_inc *= 0.6;
  p = part[p].downlink [0];

 };

 p = start_p;

   while (p != -1)
   {
    part[p].up_angle += angle_inc [p];//start_angle_inc;
    arrange_all_parts(h);


    if ((side == 0 && part[p].y - (part[p].size << 10) < 10000)
     || (side == 1 && part[p].y + (part[p].size << 10) > -10000)
     || check_position_blocked(h, p) != 0)
    {
     i = start_p;
     do
     {
      part[i].up_angle -= angle_inc [p];
      i = part[i].downlink [0];
     } while (i != part[p].downlink [0]);
     start_p = part[p].downlink [0];
     p = part[p].downlink [0];
     continue;
    }

    p = part[p].downlink [0];

   };

   return 1;

}

*/
/*
int check_position_blocked(int h, int start_p)
{


 int p = start_p;
 int i, old_p;

   old_p = -1;
   while (p != -1)
   {
    for (i = 0; i < NO_PARTS; i ++)
    {
     if (part[i].type == PART_NONE)
      continue;
     if (i == p)
      continue;
//     if (part[i].x - (part[i].size << 10) > part[p].x + (part[p].size << 10)
//      && part[i].x + (part[i].size << 10) < part[p].x - (part[p].size << 10)
//      && part[i].y - (part[i].size << 10) > part[p].y + (part[p].size << 10)
//      && part[i].y + (part[i].size << 10) < part[p].y - (part[p].size << 10))
      {
       if (hypot(part[i].y - part[p].y, part[i].x - part[p].x) < ((part[i].size + part[p].size + 8) << 10))
        return i + 1;
      }
    }

    old_p = p;
    p = part[old_p].downlink [0];

   };


 return 0;

}*/

int check_position_blocked(int h, int start_p, int size_plus)
{

 int p = start_p;

 int ret = check_position_blocked_subroutine(h, p, size_plus);

 return ret;

}

int check_position_blocked_subroutine(int h, int p, int size_plus)
{

  if (p == -1)
   return 0;

  int i;

// ASSUMES side == 0
    if (part[p].y - (part[p].size << 10) < 10000)
     return 1;


    for (i = 0; i < NO_PARTS; i ++)
    {
     if (part[i].type == PART_NONE)
      continue;
     if (i == p)
      continue;

       if (hypot(part[i].y - part[p].y, part[i].x - part[p].x) < ((part[i].size + part[p].size + 2 + size_plus) << 10))
        return i + 1;
    }

 int ret;

 ret = check_position_blocked_subroutine(h, part[p].downlink [0], size_plus);

 if (ret != 0)
  return ret;

 ret = check_position_blocked_subroutine(h, part[p].downlink [1], size_plus);
// don't need to test here as a zero return is okay
 return ret;


}


void arrange_arm(int h, int start_p)
{

 int i;

 int p = hs[h].arm_part [part[start_p].side] [part[start_p].arm];

 for (i = 0; i < NO_PARTS; i ++)
 {
  part[i].cumulative_angle = 0;
 }

 arrange_part_subroutine(h, p);

}


void arrange_all_parts(int h)
{

 int i, j, k, p, old_p;

// clear_bitmap(display);

 for (i = 0; i < NO_PARTS; i ++)
 {
  part[i].cumulative_angle = 0;
 }

 for (i = 0; i < 2; i ++)
 {
  for (k = 0; k < 3; k ++)
  {
   switch(k)
   {
    case 0: j = 2; break;
    case 1: j = 1; break;
    case 2: j = 0; break;
   }
   p = hs[h].arm_part [i] [j];
   old_p = -1;
//   while (p != -1)
//   {
    arrange_part_subroutine(h, p);
//    if (check_position_blocked(0,p) != 0)
//     circlefill(screen, (part[p].x + 100000) >> 10, (part[p].y + 100000) >> 10, part[p].size, COL_HS_INNER);
//    circle(screen, (part[p].x + 100000) >> 10, (part[p].y + 100000) >> 10, part[p].size, COL_WHITE);
//    textprintf_ex(screen, font, (part[p].x + 100000) >> 10, (part[p].y + 100000) >> 10, -1, -1, "%i", part[p].y >> 10);

    //  textprintf_ex(display, font, x3, y3, -1, -1, "%f", part[pt].locked_angle);

//    line(screen, part[p].x>>10, part[p].y>>10, part[check_position_blocked(0,p)-1].x>>10, part[check_position_blocked(0,p)-1].y>>10, COL_HS_OUTER);
//    old_p = p;
//    p = part[p].downlink [0];

//   };
  }
 }

//  blit(display, screen, 0, 0, 0, 0, 640, 480);


}

void arrange_part_subroutine(int h, int p)
{
  if (p == -1
   || part[p].type == PART_NONE)
   return;

  arrange_part(h, p);

/*    if (check_position_blocked(0,p) != 0)
     circlefill(display, (part[p].x + 300000) >> 10, (part[p].y + 300000) >> 10, part[p].size, COL_HS_INNER);
    circle(display, (part[p].x + 300000) >> 10, (part[p].y + 300000) >> 10, part[p].size, COL_WHITE);
    textprintf_ex(display, font, (part[p].x + 300000) >> 10, (part[p].y + 300000) >> 10, -1, -1, "%i, %i, %i", part[p].y >> 10, part[p].side, part[p].y - (part[p].size << 10) < 10000);
if (key [KEY_ESC])
 exit(1);*/

  arrange_part_subroutine(h, part[p].downlink [0]);
  arrange_part_subroutine(h, part[p].downlink [1]);

}

void arrange_part(int h, int p)
{

   if (p == -1
    || part[p].type == PART_NONE)
     return;

 int ul = part[p].uplink;

 if (ul == -1)
 {
   part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].up_angle, part[p].up_dist);
   part[p].y = hs[h].y + fypart(hs[h].angle + part[p].up_angle, part[p].up_dist);
   part[p].cumulative_angle = hs[h].angle + part[p].up_angle;
 }
  else
  {
//   part[p].cumulative_angle = part[ul].cumulative_angle + part[ul].up_angle + part[p].up_angle;
   part[p].cumulative_angle = part[ul].cumulative_angle + part[p].up_angle;
   part[p].x = part[ul].x + fxpart(part[p].cumulative_angle, part[p].up_dist);
   part[p].y = part[ul].y + fypart(part[p].cumulative_angle, part[p].up_dist);
//   part[p].x = part[ul].x + fxpart(hs[h].angle + part[p].up_angle, part[p].up_dist);
//   part[p].y = part[ul].y + fypart(hs[h].angle + part[p].up_angle, part[p].up_dist);
/*   part[p].x = part[ul].x + fxpart(part[p].up_angle + part[ul].cumulative_angle, part[p].up_dist);
   part[p].y = part[ul].y + fypart(part[p].up_angle + part[ul].cumulative_angle, part[p].up_dist);
//   part[p].x = part[ul].x + fxpart(hs[h].angle + part[p].up_angle, part[p].up_dist);
//   part[p].y = part[ul].y + fypart(hs[h].angle + part[p].up_angle, part[p].up_dist);
   part[p].cumulative_angle = part[ul].cumulative_angle - part[ul].up_angle + part[p].up_angle;*/
  }

//  part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);
//  part[p].y = hs[h].y + fypart(hs[h].angle + part[p].position_angle [hs[h].position], part[p].position_dist [hs[h].position]);

//  part[p].x = hs[h].x + fxpart(hs[h].angle + part[p].position_angle [199], part[p].position_dist [199]);
  //part[p].y = hs[h].y + fypart(hs[h].angle + part[p].position_angle [199], part[p].position_dist [199]);


// part[p].position_angle [0] = atan2(part[p].y - hs[h].y, part[p].x - hs[h].x);
// part[p].position_dist [0] = hypot(part[p].y - hs[h].y, part[p].x - hs[h].x);

  switch(part[p].tracks)
  {
      case TRACK_FOLLOW:
       break;
      case TRACK_LOCKED:
//       part[p].angle = radians_to_angle(part[p].cumulative_angle + part[p].up_angle + part[p].locked_angle);
       break;
  }


  part[p].angle &= ANGLE_MASK;

}



int new_map(int type)
{

  int m;

  for (m = 0; m < NO_PARTS; m ++)
  {
      if (map[m].type == PART_NONE)
       break;
      if (m == (NO_PARTS>>1) - 1)
       return -1;

  }

 map[m].type = type;
 map[m].uplink = -1;
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;
 map[m].recycle = 100 + (m%3) *20;
 map[m].fired = 0;
 map[m].hurt_flash = 0;
 map[m].killed = 0;
 map[m].cumulative_angle = 0;
 map[m].drive_push = 0;

 return m;

}


int new_part_from_map(int type, int p)
{
// p should match map m which it copies

/*  int p;

  for (p = 0; p < NO_PARTS; p ++)
  {
      if (part[p].type == PART_NONE)
       break;
      if (p == NO_PARTS - 1)
       return -1;

  }*/

 int m = p;

 part[p].type = map[m].type;

 init_part(p);

 part[p].side = map[m].side;
 part[p].arm = map[m].arm;

// if (hs[0].arm_map [map[m].side] [map[m].arm] == m)
// if (map[m].uplink == -1)
//  hs[0].arm_part [part[p].side] [part[p].arm] = p;

 part[p].uplink = map[m].uplink;
 part[p].downlink [0] = map[m].downlink [0];
 part[p].downlink [1] = map[m].downlink [1];

 part[p].up_angle = map[m].up_angle;
 part[p].up_dist = map[m].up_dist;
 part[p].max_hp = map[m].max_hp;
 part[p].hp = map[m].hp;
 part[p].counter = p * 8;
 part[p].size = map[m].size;
 part[p].angle = map[m].angle;//radians_to_angle(map[m].up_angle);
 part[p].locked_angle = map[m].locked_angle;
 part[p].tracks = map[m].tracks;
 part[p].drive = map[m].drive;
 part[p].max_drive = map[m].max_drive;
 part[p].slew = map[m].slew;
 part[p].index = m;
 map[m].index = p;
 part[p].drive_push = 0;


 return p;

}

void init_part(int p)
{

 part[p].uplink = -1;
 part[p].downlink [0] = -1;
 part[p].downlink [1] = -1;
 part[p].recycle = 100 + (p%3) *20;
 part[p].fired = 0;
 part[p].hurt_flash = 0;
 part[p].killed = 0;
 part[p].cumulative_angle = 0;
 part[p].drive_push = 0;
 part[p].chain = 0;
 part[p].wasted = 0;
 part[p].burst = 0;
 part[p].slew_time = 1;
 part[p].slew_dir = 0;
 switch(part[p].type)
 {
  case PART_MGUN:
  case PART_ROTARY:
   part[p].burst = 2 + (part[p].size >> 2);
   break;
 }
 part[p].which_fired = 0;
 part[p].spin_angle = 0;
 part[p].shield = 10000;
 part[p].shield_target = part[p].shield;
 part[p].shield_flash = 0;
 part[p].total_range = 0;
 part[p].total_direction = -1;
 part[p].range_hits = 0;
 part[p].average_range = 0;
 part[p].base_type = part[p].type;

}

int new_part(int type)
{

  int p;

  for (p = 0; p < NO_PARTS; p ++)
  {
      if (part[p].type == PART_NONE)
       break;
      if (p == NO_PARTS - 1)
       return -1;

  }

 part[p].type = type;
 init_part(p);

 return p;

}


void mirror_hs(void)
{


 int j;

  for (j = 0; j < 4; j ++)
  {
//   mirror_p = hs[0].arm_part [0] [j];

   if (hs[0].arm_part [0] [j] == -1)
    continue;

   mirror_arm(0, hs[0].arm_part [0] [j], -1, 0, j);

  }

/*
 int j, p, mirror_p, old_p;

  for (j = 0; j < 4; j ++)
  {
   mirror_p = hs[0].arm_part [0] [j];

   if (mirror_p == -1)
    continue;

   p = new_part(part[mirror_p].type);
   hs[0].arm_part [1] [j] = p;
   copy_part(mirror_p, p);
   part[p].up_angle = PI*2-part[mirror_p].up_angle;
   part[p].uplink = -1;
   part[p].side = 1;
   part[p].arm = j;
   part[p].locked_angle = part[mirror_p].locked_angle * -1;
   part[p].angle = part[mirror_p].angle;
   mirror_p = part[mirror_p].downlink [0];
   old_p = p;

   while (mirror_p != -1)
   {
    p = new_part(part[mirror_p].type);
    copy_part(mirror_p, p);
    part[p].up_angle = part[mirror_p].up_angle * -1;
    part[p].uplink = old_p;
    part[p].side = 1;
    part[p].arm = j;
    part[p].locked_angle = part[mirror_p].locked_angle * -1;
    part[p].angle = part[mirror_p].angle;
    part[old_p].downlink [0] = p;
    mirror_p = part[mirror_p].downlink [0];
    old_p = p;
   };


  }

*/
}

// this function is recursive
void mirror_arm(int h, int mirror_p, int previous, int dl, int arm)
{

 if (mirror_p == -1)
  return;

 int p = new_part(part[mirror_p].type);
 part[p].mirror_part = mirror_p;
 part[mirror_p].mirror_part = p;
 copy_part(mirror_p, p);
 if (previous == -1)
 {
  hs[0].arm_part [1] [arm] = p;
  part[p].up_angle = PI*2-part[mirror_p].up_angle;
 }
   else
    part[p].up_angle = part[mirror_p].up_angle * -1;
 part[p].uplink = previous;
 part[p].side = 1;
 part[p].arm = arm;
 part[p].locked_angle = part[mirror_p].locked_angle * -1;
 if (previous != -1)
  part[previous].downlink [dl] = p;

 mirror_arm(h, part[mirror_p].downlink [0], p, 0, arm);
 mirror_arm(h, part[mirror_p].downlink [1], p, 1, arm);

}


void copy_part(int from, int to)
{

 part[to].type = part[from].type;
 part[to].up_angle = part[from].up_angle;
 part[to].up_dist = part[from].up_dist;
 part[to].max_hp = part[from].max_hp;
 part[to].hp = part[from].hp;
 part[to].size = part[from].size;
 part[to].angle = part[from].angle;
 part[to].slew = part[from].slew;
 part[to].tracks = part[from].tracks;
 part[to].drive = part[from].drive;
 part[to].max_drive = part[from].max_drive;
 part[to].index = part[from].index;
 part[to].counter = part[from].counter;

}

// assumes hs is symmetrical!
void mirror_hs_angles(void)
{

 int j, p, p2;

  for (j = 0; j < 4; j ++)
  {
   p = hs[0].arm_part [0] [j];
   p2 = hs[0].arm_part [1] [j];

   mirror_hs_angles_subroutine(p, p2);

/*
   while (p != -1 && p2 != -1)
   {

    part[p2].locked_angle = part[p].locked_angle * -1;
    part[p2].angle = part[p].angle;
    part[p2].up_angle = (2*PI) - part[p].up_angle;
    for (i = 0; i < POSITIONS; i ++)
    {
     part[p2].position_angle [i] = (2*PI) - part[p].position_angle [i];
     part[p2].position_up_angle [i] = (2*PI) - part[p].position_up_angle [i];
     part[p2].position_dist [i] = part[p].position_dist [i];
     part[p2].position_up_dist [i] = part[p].position_up_dist [i];
//     part[p2].position = part[p].position;
    }
    p = part[p].downlink [0];
    p2 = part[p2].downlink [0];
//    old_p = p;
   };*/

  }

}


void mirror_hs_angles_subroutine(int p, int p2)
{

  if (p == -1 || p2 == -1)
   return;

   int i;

    part[p2].locked_angle = part[p].locked_angle * -1;
    part[p2].angle = part[p].angle;
    part[p2].up_angle = (2*PI) - part[p].up_angle;
    for (i = 0; i < POSITIONS; i ++)
    {
     part[p2].position_angle [i] = (2*PI) - part[p].position_angle [i];
     part[p2].position_up_angle [i] = (2*PI) - part[p].position_up_angle [i];
     part[p2].position_dist [i] = part[p].position_dist [i];
     part[p2].position_up_dist [i] = part[p].position_up_dist [i];
     part[p2].position_move_dist [i] = part[p].position_move_dist [i];
     part[p2].position_move_angle [i] = (2*PI) - part[p].position_move_angle [i];
//     part[p2].position = part[p].position;
    }

    mirror_hs_angles_subroutine(part[p].downlink [0], part[p2].downlink [0]);
    mirror_hs_angles_subroutine(part[p].downlink [1], part[p2].downlink [1]);

}

// call this at end of level.
void hs_evolves(int h)
{
 int power = 30;
 int change = 3;

 int i, attempt, p;
 int biggest_chain_size = 0;
 int biggest_chain = -1;
 int increase;
 int m;
 int blocked;
 int m2, dl;
 int new_type = 0;

 evolve_screen(0);

 setup_weapon_list_map();

// let's work out if the player is attacking from outside the max range of all the hs' weapons
 int hs_total_range = 0;
 int hs_range_hits = 0;
 int hs_average_range = 0;
 int hs_total_direction = 0;
 int hs_average_direction = 0;
 int in_range = 0;
 int range_difference;
 int any_hits = 0;

 int shield_parts = 0;
 int drive2_parts = 0;

 for (m = 0; m < NO_PARTS; m ++)
 {
  if (map[m].type == PART_SHIELD)
   shield_parts ++;
  if (map[m].type == PART_DRIVE2)
   drive2_parts ++;
 }

 for (p = 0; p < NO_PARTS; p ++)
 {
    if (part[p].chain > 0
     && part[p].range_hits != 0)
    {
     hs_total_range += part[p].total_range;
     hs_range_hits += part[p].range_hits;
     hs_total_direction += part[p].total_direction;
    }

    if (part[p].range_hits != 0)
     any_hits = 1;
 } // only count hits that destroy parts.

 if (any_hits == 0) // core destroyed without destroying any parts
  power = 60;

 if (hs_range_hits > 0)
 {
  hs_average_range = hs_total_range / hs_range_hits;
  hs_average_direction = hs_total_direction / hs_range_hits;
  for (i = 0; i < NO_WEAPONS; i ++)
  {
   if (hs[0].weapon [i] == PART_NONE)
    continue;
/*   if (part_ranges [hs[0].weapon [i]] [4] == DIR_FORWARD
    && hs_total_direction > ANGLE_FRONT
    && hs_total_direction < ANGLE_1 - (ANGLE_FRONT))
     continue;
   if (part_ranges [hs[0].weapon [i]] [4] == DIR_ALL
    && (hs_total_direction < ANGLE_FRONT
    || hs_total_direction > ANGLE_1 - (ANGLE_FRONT)))
     continue;*/ // don't worry about direction for now
   if (hs_average_range >= part_ranges [hs[0].weapon [i]] [1]
    && hs_average_range <= part_ranges [hs[0].weapon [i]] [2])
    {
     in_range = 1;
     break;
    }
  }
  if (in_range == 0) // hs doesn't have a weapon of appropriate range to deal with the way player is attacking
  {
   int best_range_difference = 1000;
   int best = -1;
   for (i = 0; i < PART_TYPES; i ++)
   {
    if (part_ranges [i] [0] == 0)
     continue; // must be a non-weapon part type
    if (hs_average_range < part_ranges [i] [1]
     || hs_average_range > part_ranges [i] [2])
      continue;
/*    if (part_ranges [i] [4] == DIR_FORWARD
     && part[p].average_direction > ANGLE_FRONT
     && part[p].average_direction < ANGLE_1 - (ANGLE_FRONT))
      continue;
    if (part_ranges [i] [4] == DIR_ALL
     && (part[p].average_direction < ANGLE_FRONT
     || part[p].average_direction > ANGLE_1 - (ANGLE_FRONT)))
      continue;*/
    range_difference = abs(part_ranges [i] [0] - hs_average_range);
    if (range_difference < best_range_difference)
    {
     best_range_difference = range_difference;
     best = i;
    }

   }

   if (best != -1) // we've found an appropriate weapon! let's replace the least appropriate weapon with it.
   {
    int worst_range_difference = -1;
    int worst = -1;
    for (i = 0; i < NO_WEAPONS; i ++)
    {
     if (hs[0].weapon [i] == PART_NONE)
      continue;
/*    if (part_ranges [i] [4] == DIR_FORWARD
     && part[p].average_direction > ANGLE_FRONT
     && part[p].average_direction < ANGLE_1 - (ANGLE_FRONT))
      continue;
    if (part_ranges [i] [4] == DIR_ALL
     && (part[p].average_direction < ANGLE_FRONT
     || part[p].average_direction > ANGLE_1 - (ANGLE_FRONT)))
      continue;*/
     range_difference = abs(part_ranges [hs[0].weapon [i]] [0] - hs_average_range);
     if (range_difference > worst_range_difference)
     {
      worst_range_difference = range_difference;
      worst = i;
     }
   }
   if (worst != -1) // should always be true
   {
    for (m = 0; m < NO_PARTS; m ++)
    {
     if (map[m].type == hs[0].weapon [worst])
      map[m].type = best;
     set_basic_map_properties(m);
    }
   }
   }

  }
 }

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].range_hits == 0)
  {
   part[p].average_range = -1;
   part[p].average_direction = ANGLE_2;
  }
    else
    {
     part[p].average_range = part[p].total_range / part[p].range_hits;
     part[p].average_direction = part[p].total_direction / part[p].range_hits;
    }
 }

 map_arrange_all_parts(h);

  for (attempt = 0; attempt < 20; attempt ++)
  {
   if (power <= 0)
    break;
   biggest_chain = -1;
   biggest_chain_size = 0;
   for (i = 0; i < NO_PARTS; i ++)
   {
//    if (hs[h].destroyed [i] == -1)
//     break;
//    p = hs[h].destroyed [i];
    p = i;
    if (part[p].chain > biggest_chain_size)
    {
     biggest_chain = p;
     biggest_chain_size = part[p].chain;
    }
   }
   if (biggest_chain_size == 0)
    break;
   p = biggest_chain;
   if (p == -1)
    break;
   increase = 1;
   if (part[p].chain > 7)
    increase = 2;
   if (part[p].chain > 14)
    increase = 3;
   if (part[p].chain > 19)
    increase = 4;
   if (part[p].chain > 23)
    increase = 5;
   if (part[p].size + increase >= 30)
    increase = 30 - part[p].size;
   if (increase > power)
    increase = power;
   m = part[p].index;

   if (change > 0 && part[p].wasted > (2000 + shield_parts * 200) && map[m].type != PART_DRIVE)
   {
    map[m].type = PART_SHIELD;
    set_basic_map_properties(m);
    change --;
   }
    else
     if (change > 0 && map[m].type != PART_DRIVE
     && (part[p].average_direction > ANGLE_2 - ANGLE_8 + (drive2_parts * 10))
     && (part[p].average_direction < ANGLE_2 + ANGLE_8 - (drive2_parts * 10)))
     {
      map[m].type = PART_DRIVE2;
      set_basic_map_properties(m);
      change --;
     }
      else
       if (change > 0 && map[m].type != PART_DRIVE &&
          (part[p].average_range < part_ranges [part[p].type] [1]
        || part[p].average_range > part_ranges [part[p].type] [2]))
    {
     map[m].type = find_best_weapon(p);
     setup_weapon_list_map();
     set_basic_map_properties(m);
     change --;
    }

   if (increase <= 0)
   {
/*    if (map[m].downlink [0] != -1)
     part[map[m].downlink [0]].chain += part[p].chain;
      else
      {
       if (map[m].downlink [1] != -1)
        part[map[m].downlink [1]].chain += part[p].chain;
         else
         {
          if (map[m].uplink != -1)
           part[map[m].uplink].chain += part[p].chain;
         }
      }*/
    part[p].chain = -1;
    continue;
   }
//   if (part[p].side == 1)
//    m = part[part[p].mirror_part].index;
   map[m].size += increase;
   map[m].up_dist += (increase << 10) * 2;
   if (map[m].downlink [0] != -1)
    map[map[m].downlink [0]].up_dist += (increase << 10);
   if (map[m].downlink [1] != -1)
    map[map[m].downlink [1]].up_dist += (increase << 10);
// should try smaller increases if big one fails...
   map_arrange_arm(h, m);
   blocked = map_check_position_blocked(h, m);

// textprintf_ex(screen, font, 100, 100 + dline * 15, -1, -1, "increase %i %i %i %i %i %i", m, p, part[p].index, part[p].mirror_part, part[p].chain, part[part[p].mirror_part].chain);
// textprintf_ex(screen, font, 100, 100 + dline * 15, -1, -1, "increase %i from %i to %i", m, map[m].size - increase, map[m].size);
// dline ++;

   if (blocked == 0)
   {
    power -= increase;
    set_basic_map_properties(m);
   }
    else
    {
     map[m].size -= increase;
     map[m].up_dist -= (increase << 10) * 2;
     if (map[m].downlink [0] != -1)
      map[map[m].downlink [0]].up_dist -= (increase << 10);
     if (map[m].downlink [1] != -1)
      map[map[m].downlink [1]].up_dist -= (increase << 10);
     map_arrange_arm(h, m);
/*     if (map[m].downlink [0] != -1)
      part[map[m].downlink [0]].chain += part[p].chain;
       else
       {
        if (map[m].downlink [1] != -1)
         part[map[m].downlink [1]].chain += part[p].chain;
          else
          {
           if (map[m].uplink != -1)
            part[map[m].uplink].chain += part[p].chain;
          }
       }*/

// textprintf_ex(screen, font, 100, 100 + dline * 15, -1, -1, "reduce %i %i %i %i", m, p, part[p].index, part[p].mirror_part);
// dline ++;
    }
    part[p].chain = -1; // can't be 0 or might grow new parts
  }


// should test for bounds of map array here...

 evolve_screen(0);

 map_arrange_all_parts(h);


 for (attempt = 0; attempt < 10; attempt ++)
 {
  if (power < 7)
   break;
  evolve_screen(0);
  for (i = 0; i < NO_PARTS; i ++)
  {
   if (hs[h].destroyed [i] == -1)
    break;
   p = hs[h].destroyed [i];
   m = part[p].index;
//   if (part[p].side == 1)
//    m = part[part[p].mirror_part].index;
   if (part[p].chain == 0 && map[m].downlink [0] == -1)
    dl = 0;
     else
      continue;
     new_type = map[m].type;
     m2 = grow_new_map(h, m, new_type, dl, map[m].index);
     setup_weapon_list_map();

     if (m2 != -1)
     {
     map[m].downlink [dl] = m2;
     map_arrange_arm(h, m);

// textprintf_ex(screen, font, 100, 100 + dline * 15, -1, -1, "new m %i m2 %i dl %i at (%i,%i)", m, m2, dl, map[m2].x, map[m2].y);
// dline ++;

//     map_arrange_all_parts(h);

     draw_map_hs_build_debug(COL_LGREY, 1);

     blocked = map_check_position_blocked(h, m);

     part[p].chain = -1;

     if (blocked != 0)
     {

/*      textprintf_ex(screen, font, 10, 10 + glob * 15, -1, -1, "1blocked p%i m%i by %i", p, m, blocked);
      glob ++;
      do {rest(5);} while (key [KEY_A] == 0);
      do {rest(5);} while (key [KEY_B] == 0);*/
      destroy_map(m2);
      map[m].downlink [dl] = -1;
      map_arrange_arm(h, m);
      draw_map_hs_build_debug(COL_LGREY, 10);
      setup_weapon_list_map();
      continue;
     }
     power -= 7;
     } // end of if m2 != -1

   }
 }


 map_arrange_all_parts(h);

 for (attempt = 0; attempt < 10; attempt ++)
 {
  if (power < 7)
   break;
  evolve_screen(0);
  for (i = 0; i < NO_PARTS; i ++)
  {
   if (hs[h].destroyed [i] == -1)
    break;
   p = hs[h].destroyed [i];
   m = part[p].index;

   if (part[p].chain == 0 && map[m].downlink [1] == -1)
    dl = 1;
     else
     {
      part[p].chain = -1;
      continue;
     }

     new_type = map[m].type;
     m2 = grow_new_map(h, m, new_type, dl, map[m].index);
     setup_weapon_list_map();
     if (m2 != -1)
     {
     map[m].downlink [dl] = m2;
     map_arrange_arm(h, m);

     draw_map_hs_build_debug(COL_LGREY, 2);

     blocked = map_check_position_blocked(h, m);
     part[p].chain = -1;
     if (blocked != 0)
     {
/*
      textprintf_ex(screen, font, 10, 40 + glob * 15, -1, -1, "2blocked p %i m %i m2 %i by %i", p, m, m2, blocked);
      glob ++;
      do {rest(5);} while (key [KEY_A] == 0);
      do {rest(5);} while (key [KEY_B] == 0);*/

      destroy_map(m2);
      map[m].downlink [dl] = -1;
      map_arrange_arm(h, m);
      draw_map_hs_build_debug(COL_LGREY, 20);
      setup_weapon_list_map();
      continue;
     }
     power -= 7;
     }// end of if m2 != -1

   }

  if (power <= 0)
   break;
 }

 if (power > 0)
 {
    hs[h].core_strength += power;
 }



// do
// {rest(5);} while (key [KEY_A] == 0);

}

int grow_new_map(int h, int ul, int type, int dl_pos, int ulp)
{

// now, we may want to add a new type, by changing the type variable
 if (type != PART_DRIVE)
 {
  if (part[ulp].average_range < part_ranges [type] [1]
   || part[ulp].average_range > part_ranges [type] [2])
// hm, not ideal... let's find a better one
   {
    type = find_best_weapon(ulp);

   }

 }


 int m = new_map(type);

 if (m == -1)
  return -1;

 map[m].side = 0;
 map[m].arm = map[ul].arm;
// hs[h].arm_map [map[m].side] [map[m].arm] = m;

 map[m].uplink = ul;
 map[m].downlink [0] = -1;
 map[m].downlink [1] = -1;

 map[m].up_angle = 0;
 if (dl_pos == 1)
  map[m].up_angle = - PI_4;
 map[m].up_dist = 15000 + (map[ul].size << 10) + (map[ul].size << 8);
 map[m].max_hp = get_part_hp(map[m].size);
 map[m].hp = map[m].max_hp;
 map[m].size = 7;
 map[m].angle = 0;//radians_to_angle(map[m].up_angle);
// map[m].side = 0;
// map[m].arm = 0;
 map[m].tracks = part_ranges [type] [3];

 set_basic_map_properties(m);

// map[m].slew = 32;


 return m;

}


int find_best_weapon(int p)
{

 int i, range_difference;
 int best = -1;
 int best_range_difference = 1000;
// textprintf_ex(screen, font, 10, 10 + 15 * glob, -1, -1, "looking for %i", p);
// glob ++;


 for (i = 0; i < NO_WEAPONS; i ++)
 {
  if (hs[0].weapon [i] == PART_NONE
   || hs[0].weapon [i] == -1)
   continue;
  if (part[p].average_range < part_ranges [hs[0].weapon [i]] [1]
   || part[p].average_range > part_ranges [hs[0].weapon [i]] [2])
    continue;
  if (part_ranges [hs[0].weapon [i]] [4] == DIR_FORWARD
   && part[p].average_direction > ANGLE_FRONT
   && part[p].average_direction < ANGLE_1 - (ANGLE_FRONT))
    continue;
  if (part_ranges [hs[0].weapon [i]] [4] == DIR_ALL
   && (part[p].average_direction < ANGLE_FRONT
   || part[p].average_direction > ANGLE_1 - (ANGLE_FRONT)))
    continue;
  range_difference = abs(part_ranges [hs[0].weapon [i]] [0] - part[p].average_range);
  if (range_difference < best_range_difference)
  {
   best_range_difference = range_difference;
   best = i;
  }

 }

 if (best != -1)
  return hs[0].weapon [best];
// we don't already have a useful weapon. so we might have to add a new type:

 if (hs[0].weapon [NO_WEAPONS - 1] != PART_NONE)
  return hs[0].weapon [NO_WEAPONS - 1]; // have to deal with lack of empty slots later...

 best = -1;
 best_range_difference = 1000;

 for (i = 0; i < PART_TYPES; i ++)
 {
  if (part_ranges [i] [0] == 0)
   continue; // must be a non-weapon part type
  if (part[p].average_range < part_ranges [i] [1]
   || part[p].average_range > part_ranges [i] [2])
    continue;
  if (part_ranges [i] [4] == DIR_FORWARD
   && part[p].average_direction > ANGLE_FRONT
   && part[p].average_direction < ANGLE_1 - (ANGLE_FRONT))
    continue;
  if (part_ranges [i] [4] == DIR_ALL
   && (part[p].average_direction < ANGLE_FRONT
   || part[p].average_direction > ANGLE_1 - (ANGLE_FRONT)))
    continue;
  range_difference = abs(part_ranges [i] [0] - part[p].average_range);
  if (range_difference < best_range_difference)
  {
   best_range_difference = range_difference;
   best = i;
  }

 }

 if (best != -1)
  return best;

// oh well, can't find anything (unlikely)
 return part[p].base_type;

}


void set_basic_map_properties(int m)
{

 if (m == -1)
  return;

 map[m].tracks = part_ranges [map[m].type] [3];
 map[m].slew = 10;

 switch(map[m].type)
 {
  case PART_DRIVE:
   map[m].locked_angle = angle_to_radians(map[m].angle);
   map[m].drive = 0;
   map[m].max_drive = map[m].size * 15;
   break;
  case PART_DRIVE2:
   map[m].locked_angle = angle_to_radians(map[m].angle);
   map[m].drive = 0;
   map[m].max_drive = map[m].size * 11;
   break;
  case PART_FORWARD:
  case PART_MISSILE:
  case PART_ROTARY:
  case PART_SEEKER:
  case PART_BEAM2:
  case PART_EEL:
   map[m].locked_angle = angle_to_radians(map[m].angle);
   break;
  case PART_BEAM:
//   map[m].slew = 5;
   break;
  case PART_MGUN:
   map[m].slew = 3;
   break;
  case PART_SHIELD:
   break;

 }

}


void map_arrange_arm(int h, int start_m)
{

 int i;

 int m = hs[h].arm_map [map[start_m].side] [map[start_m].arm];

 for (i = 0; i < NO_PARTS; i ++)
 {
  map[i].cumulative_angle = 0;
 }

 map_arrange_part_subroutine(h, m);

}


int map_check_position_blocked(int h, int start_m)
{

 int m = start_m;

 int ret = map_check_position_blocked_subroutine(h, m);

 return ret;

}

int map_check_position_blocked_subroutine(int h, int m)
{

  if (m == -1)
   return 0;

  int i;

// ASSUMES side == 0
    if (map[m].y - (map[m].size << 10) < 9000)
    {
/*        textprintf_ex(screen, font, 100, 100 + dline * 15, -1, -1, "blocked m %i (%i,%i)", m, map[m].x, map[m].y);
        dline ++;
        textprintf_ex(screen, font, 100, 100 + dline * 15, -1, -1, "size is %i or %i and distance from centre is %i", map[m].size, map[m].size << 10, map[m].y - (map[m].size << 10));
        dline ++;*/
     return 100;
    }


    for (i = 0; i < NO_PARTS; i ++)
    {
     if (map[i].type == PART_NONE)
      continue;
     if (i == m)
      continue;

       if (hypot(map[i].y - map[m].y, map[i].x - map[m].x) < ((map[i].size + map[m].size + 2) << 10))
       {
//        textprintf_ex(screen, font, 100, 100 + dline * 15, -1, -1, "m %i at %i,%i blocked by %i at %i, %i distance %i", m, map[m].x, map[m].y, i, map[i].x, map[i].y, (int) hypot(map[i].y - map[m].y, map[i].x - map[m].x));
//        dline ++;
        return i + 1;
       }
    }

 int ret;

 ret = map_check_position_blocked_subroutine(h, map[m].downlink [0]);

 if (ret != 0)
  return ret;

 ret = map_check_position_blocked_subroutine(h, map[m].downlink [1]);
// don't need to test here as a zero return is okay
 return ret;


}


void map_arrange_all_parts(int h)
{

 int i, j, k, m, old_m;

// clear_bitmap(display);

 for (i = 0; i < NO_PARTS; i ++)
 {
  map[i].cumulative_angle = 0;
 }

 for (i = 0; i < 2; i ++)
 {
  for (k = 0; k < 3; k ++)
  {
   switch(k)
   {
    case 0: j = 2; break;
    case 1: j = 1; break;
    case 2: j = 0; break;
   }
   m = hs[h].arm_map [i] [j];
   old_m = -1;
    map_arrange_part_subroutine(h, m);

  }
 }


/*  blit(display, screen, 0, 0, 0, 0, 640, 480);
  do
  {
      rest(1);
  }while (key [KEY_U] == 0);*/

}




void map_arrange_part_subroutine(int h, int m)
{
  if (m == -1)
   return;

  map_arrange_part(h, m);
//    if (check_position_blocked(0,m) != 0)
//     circlefill(display, (map[m].x + 100000) >> 10, (map[m].y + 100000) >> 10, map[m].size, COL_HS_INNER);
//    circle(display, (map[m].x + 100000) >> 10, (map[m].y + 100000) >> 10, map[m].size, COL_WHITE);

  map_arrange_part_subroutine(h, map[m].downlink [0]);
  map_arrange_part_subroutine(h, map[m].downlink [1]);

}

void map_arrange_part(int h, int m)
{

 int ul = map[m].uplink;

 if (ul == -1)
 {
   map[m].x = hs[h].x + fxpart(hs[h].angle + map[m].up_angle, map[m].up_dist);
   map[m].y = hs[h].y + fypart(hs[h].angle + map[m].up_angle, map[m].up_dist);
   map[m].cumulative_angle = hs[h].angle + map[m].up_angle;
 }
  else
  {
   map[m].cumulative_angle = map[ul].cumulative_angle + map[m].up_angle;
   map[m].x = map[ul].x + fxpart(map[m].cumulative_angle, map[m].up_dist);
   map[m].y = map[ul].y + fypart(map[m].cumulative_angle, map[m].up_dist);
  }

  switch(map[m].tracks)
  {
      case TRACK_FOLLOW:
       break;
      case TRACK_LOCKED:
       break;
  }


  map[m].angle &= ANGLE_MASK;

}

void destroy_map(int m)
{
 map[m].type = PART_NONE;

}

void draw_hs_build_debug(int col, int number)
{
return;
 clear_bitmap(display);
 textprintf_ex(display, font, 10, 10, -1, -1, "number %i", number);

#define SIZE_SHIFT 9

 int p, x, y;

 for (p = 0; p < NO_PARTS; p ++)
 {
  if (part[p].type == PART_NONE)
   continue;
  x = (part[p].x >> SIZE_SHIFT) + 320;
  y = (part[p].y >> SIZE_SHIFT) + 240;
  circle(display, x, y, part[p].size, col);
  if (part[p].uplink == -1)
   line(display, x, y, 320, 240, col);
    else
    {
     line(display, x, y, (part[part[p].uplink].x >> SIZE_SHIFT) + 320, (part[part[p].uplink].y >> SIZE_SHIFT) + 240, col + TRANS_BLUE2);
     textprintf_ex(display, font, (part[part[p].uplink].x >> SIZE_SHIFT) + 320, (part[part[p].uplink].y >> SIZE_SHIFT) + 255, -1, -1, "u%i", part[p].uplink);
     textprintf_ex(display, font, (part[p].x >> SIZE_SHIFT) + 320, (part[p].y >> SIZE_SHIFT) + 240, -1, -1, "p%i", p);
    }
  if (part[p].downlink [0] != -1)
  {
   line(display, x, y, (part[part[p].downlink [0]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [0]].y >> SIZE_SHIFT) + 240, col + TRANS_RED2);
   textprintf_ex(display, font, (part[part[p].downlink [0]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [0]].y >> SIZE_SHIFT) + 270, -1, -1, "1d%i", part[p].downlink [0]);
   textprintf_ex(display, font, (part[p].x >> SIZE_SHIFT) + 320, (part[p].y >> SIZE_SHIFT) + 240, -1, -1, "p%i", p);
  }
  if (part[p].downlink [1] != -1)
  {
   line(display, x, y, (part[part[p].downlink [1]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [1]].y >> SIZE_SHIFT) + 240, col + TRANS_GREEN2);
   textprintf_ex(display, font, (part[part[p].downlink [1]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [1]].y >> SIZE_SHIFT) + 270, -1, -1, "2d%i", part[p].downlink [1]);
   textprintf_ex(display, font, (part[p].x >> SIZE_SHIFT) + 320, (part[p].y >> SIZE_SHIFT) + 240, -1, -1, "p%i", p);
  }

 }
// rest(1);

if (hs[0].stage > 1)
  blit(display, screen, 0, 0, 0, 0, 640, 480);

do
{
 rest(10);
 if (key [KEY_ESC])
  exit(0);
} while (key [KEY_1 - 1 + number] == 0);

 textprintf_ex(display, font, 10, 25, -1, -1, "next");


}



void draw_map_hs_build_debug(int col, int number)
{
return;
 static int press = 1;
 if (press == 1)
  press = 2;
   else
    press = 1;
 clear_bitmap(display);
 textprintf_ex(display, font, 10, 10, -1, -1, "number %i press %i", number, press);
/*glob = 2;

 int i, j;
 for (i = 0; i < 2; i ++)
 {
  for (j = 0; j < 4; j ++)
  {
   textprintf_ex(display, font, 3, 10 + glob*15, -1, -1, "side %i arm %i downlink %i", i, j, hs[0].arm_map [i] [j]);
   glob ++;
   mapping_sub(hs[0].arm_map [i] [j]);
  }
 }
*/


#define SIZE_SHIFT 9

 int p, x, y, m;

 for (m = 0; m < NO_PARTS; m ++)
 {
  if (map[m].type == PART_NONE)
   continue;
  p = map[m].index;
  x = (map[m].x >> SIZE_SHIFT) + 320;
  y = (map[m].y >> SIZE_SHIFT) + 240;
  circle(display, x, y, map[m].size, col);
     textprintf_ex(display, font, (map[m].x >> SIZE_SHIFT) + 320, (map[m].y >> SIZE_SHIFT) + 240, -1, -1, "m%i", m);
  if (map[m].uplink == -1)
   line(display, x, y, 320, 240, col);
    else
    {
     line(display, x, y, (map[map[m].uplink].x >> SIZE_SHIFT) + 320, (map[map[m].uplink].y >> SIZE_SHIFT) + 240, col + TRANS_BLUE2);
     //textprintf_ex(display, font, (map[map[m].uplink].x >> SIZE_SHIFT) + 320, (map[map[m].uplink].y >> SIZE_SHIFT) + 255, -1, -1, "u%i", map[m].uplink);
    }
  if (map[m].downlink [0] != -1)
  {
   line(display, x, y, (map[map[m].downlink [0]].x >> SIZE_SHIFT) + 320, (map[map[m].downlink [0]].y >> SIZE_SHIFT) + 240, col + TRANS_RED2);
   //textprintf_ex(display, font, (map[map[m].downlink [0]].x >> SIZE_SHIFT) + 320, (map[map[m].downlink [0]].y >> SIZE_SHIFT) + 270, -1, -1, "1d%i", map[m].downlink [0]);
  }
  if (map[m].downlink [1] != -1)
  {
   line(display, x, y, (map[map[m].downlink [1]].x >> SIZE_SHIFT) + 320, (map[map[m].downlink [1]].y >> SIZE_SHIFT) + 240, col + TRANS_GREEN2);
   //textprintf_ex(display, font, (map[map[m].downlink [1]].x >> SIZE_SHIFT) + 320, (map[map[m].downlink [1]].y >> SIZE_SHIFT) + 270, -1, -1, "2d%i", map[m].downlink [1]);
  }
     textprintf_ex(display, font, (map[m].x >> SIZE_SHIFT) + 320, (map[m].y >> SIZE_SHIFT) + 240, -1, -1, "m%i", m);
     textprintf_ex(display, font, (map[m].x >> SIZE_SHIFT) + 320, (map[m].y >> SIZE_SHIFT) + 255, -1, -1, "u%i", map[m].uplink);
     textprintf_ex(display, font, (map[m].x >> SIZE_SHIFT) + 320, (map[m].y >> SIZE_SHIFT) + 270, -1, -1, "1d%i 2d%i", map[m].downlink [0], map[m].downlink [1]);

 }

/*
 int p, x, y, size, m;

 for (m = 0; m < NO_PARTS; m ++)
 {
  if (map[m].type == PART_NONE)
   continue;
  p = map[m].index;
  x = (part[p].x >> SIZE_SHIFT) + 320;
  y = (part[p].y >> SIZE_SHIFT) + 240;
  circle(display, x, y, map[m].size, col);
     textprintf_ex(display, font, (part[p].x >> SIZE_SHIFT) + 320, (part[p].y >> SIZE_SHIFT) + 240, -1, -1, "p%i", p);
  if (part[p].uplink == -1)
   line(display, x, y, 320, 240, col);
    else
    {
     line(display, x, y, (part[part[p].uplink].x >> SIZE_SHIFT) + 320, (part[part[p].uplink].y >> SIZE_SHIFT) + 240, col + TRANS_BLUE2);
     textprintf_ex(display, font, (part[part[p].uplink].x >> SIZE_SHIFT) + 320, (part[part[p].uplink].y >> SIZE_SHIFT) + 255, -1, -1, "u%i", part[p].uplink);
    }
  if (part[p].downlink [0] != -1)
  {
   line(display, x, y, (part[part[p].downlink [0]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [0]].y >> SIZE_SHIFT) + 240, col + TRANS_RED2);
   textprintf_ex(display, font, (part[part[p].downlink [0]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [0]].y >> SIZE_SHIFT) + 270, -1, -1, "1d%i", part[p].downlink [0]);
  }
  if (part[p].downlink [1] != -1)
  {
   line(display, x, y, (part[part[p].downlink [1]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [1]].y >> SIZE_SHIFT) + 240, col + TRANS_GREEN2);
   textprintf_ex(display, font, (part[part[p].downlink [1]].x >> SIZE_SHIFT) + 320, (part[part[p].downlink [1]].y >> SIZE_SHIFT) + 270, -1, -1, "2d%i", part[p].downlink [1]);
  }
     textprintf_ex(display, font, (part[p].x >> SIZE_SHIFT) + 320, (part[p].y >> SIZE_SHIFT) + 240, -1, -1, "p%i", p);

 }
// rest(1);
*/
if (hs[0].stage > 1)
  blit(display, screen, 0, 0, 0, 0, 640, 480);

do
{
 rest(10);
 if (key [KEY_ESC])
  exit(0);
} while (key [KEY_1 - 1 + number] == 0);

 textprintf_ex(display, font, 10, 25, -1, -1, "next");


if (hs[0].stage > 1)
{
  blit(display, screen, 0, 0, 0, 0, 640, 480);

do
{
 rest(10);
 if (key [KEY_ESC])
  exit(0);
} while (key [KEY_1 - 1 + press] == 0);
}

}

void mapping_sub(int m)
{
 if (m == -1
  || m >= NO_PARTS)
  return;

 textprintf_ex(display, font, 10, 10 + glob*15, -1, -1, "map %i type %i uplink %i downlink1 %i downlink2 %i", m, map[m].type, map[m].uplink, map[m].downlink [0], map[m].downlink [1]);
 glob ++;

 mapping_sub(map[m].downlink [0]);
 mapping_sub(map[m].downlink [1]);

}
