
#define ALLEGRO_STATICLINK


#define NO_KEYS CKEY_END
#define X_MIN 500
#define Y_MIN 500
#define X_MAX 79500
#define Y_MAX 59500

#define GRAIN 100

//#define ANGLEFULL_HEX 0x400
#define ANGLE_MASK 4095
#define ANGLE_1 4096
#define ANGLE_2 2048
#define ANGLE_3 1365
// 3 is not exact
#define ANGLE_4 1024
#define ANGLE_5 819
// not exact
#define ANGLE_6 683
#define ANGLE_7 585
// 6 is not exact
#define ANGLE_8 512
#define ANGLE_10 410
#define ANGLE_12 341
#define ANGLE_16 256
//#define ANGLE_8_3 384
//#define ANGLE_16_3 192
#define ANGLE_32 128
#define ANGLE_64 64
#define ANGLE_128 32
#define ANGLE_256 16
#define ANGLE_TO_FIXED 4

#ifndef PI
#define PI 3.141592
#endif

#define PI_2 (PI/2)
#define PI_4 (PI/4)
#define PI_8 (PI/8)
#define PI_16 (PI/16)
#define PI_32 (PI/32)

#define NO_ENEMIES 500
#define NO_EBULLETS 150
#define NO_PBULLETS 30
#define NO_CLOUDS 600
#define NO_PARTS 64
#define NO_WEAPONS 5

#define NO_STARS 500

#define D_SIZE 1000

#define ANGLE_FRONT 340
// this is the arc (to the left and the right) that counts as the front of the hs for the purposes of
//  working out whether to use a forward weapon or an all-around weapon.

enum
{
CKEY_LEFT,
CKEY_RIGHT,
CKEY_UP,
CKEY_DOWN,
CKEY_FIRE1,
CKEY_FIRE2,
CKEY_FIRE3,
CKEY_END

};

enum
{
WEAPON_BASIC,
WEAPON_MINE,
WEAPON_ROCKET,
WEAPON_WORM,
WEAPON_MISSILE,
WEAPON_SCATTER,
WEAPONS

};


enum
{
U_POWER,
U_RANGE,
U_AGILITY,
U_SHIELD,
U_CHARGE,
U_CHANGE,
U_EXTEND,
U_TYPES

};



enum
{
DIR_NONE,
DIR_FORWARD,
DIR_ALL

};

#define SBARS 15

struct playerstruct
{

 int x, y;
 int x_speed, y_speed;
 int target_distance;
 int angle;
 int turning;
 int turn_speed;
 int recycle;
 int ckey [CKEY_END];
 int weapon;
 int charge;
 int rocket_recycle;
 int rocket_burst;
 int rocket_angle;
 int range; // current distance to hs, in weapon range units
 int angle_to_hs;

 int alive;
 int ships;
 int respawning;
 int drive;
 int flap [2];

 int camera_x;
 int camera_y;
 int window_x;
 int window_y;
 int window_centre_x;
 int window_centre_y;

 int shield_stock;
 int max_shield;
 int next_shield;
 int shield_time;

 int upgrade [U_TYPES];

 char played_hit; // played the hit hs sound this frame? Avoids playing it many times for multiple hits.

/* int sbar_width [SBARS];
 int sbar_high [SBARS];
 int sbar_col [SBARS];
 int next_sbar;
 int sbar_y [SBARS];*/

};

#define MINE_BITS 15

struct pbulletstruct
{
 int x, y;
 int x_speed, y_speed;
 int type;
 int angle;
 int colour;
 int status;
 int status2;
 int timeout;
 int damage;
 int force;
 int size;
 int draw_size;
 int time;
 int bit_angle [MINE_BITS];
 int bit_dist [MINE_BITS];
 int bit_time [MINE_BITS];
 char bit_spin [MINE_BITS];

 int range; // player[p].range when this bullet was fired. set in create_pbullet.
 int direction; // player[p].angle_to_hs when this bullet was fired.
};

enum
{
PBULLET_NONE,
PBULLET_SHOT,
PBULLET_BASIC,
PBULLET_ROCKET,
PBULLET_WORM,
PBULLET_MINE,
PBULLET_SCATTER,
PBULLET_MISSILE,
PBULLET_BOLT,
PBULLET_MINE_BLAST

};

struct ebulletstruct
{
 int x, y;
 int x_speed, y_speed;
 int type;
 int angle;
 float fangle;
 int status;
 int status2;
 int status3;
 int status4;
 int colour;
 int timeout;
 int size;
 int draw_size;
 int draw_size2;
 int time;
};


struct arenastruct
{
 int max_x;
 int max_y;
 unsigned char counter; // assumes uc is 0-255
 int time_left;
 int game_over;

 int nutrient_level;
 int no_parasites; // not kept 100% accurate but useful anyway

 int end_stage;
};

struct starstruct
{
 int x;
 int y;
 int old_x;
 int old_y;
 unsigned char bright;
 unsigned char bright2;
 unsigned char depth;
};

#define NO_CORE_CIRCLES 5

enum
{
MOVE_STAY,
MOVE_FORWARDS,
MOVE_TURN_LEFT,
MOVE_TURN_RIGHT,
MOVE_TURN_LEFT_DRIVE, // also uses any axial drives
MOVE_TURN_RIGHT_DRIVE

};



struct hsstruct
{
 int alive;
 int stage;
 int x, y; // centre of core
 int old_x, old_y;
 int centre_x, centre_y; // centre of mass
 int x_speed, y_speed; // velocity of centre of mass
 float angle; // angle that core is facing
 float spin; // angular velocity
 int moment;
 int mass;
 int hurt_flash;
 unsigned char counter;
 int movement;
 int next_thought;
 int next_reposition;
 int angle_to_player;
 char can_turn_left;
 char can_turn_right;
 int position;
 int target_position;

 int arm_map [2] [4];
 int arm_part [2] [4];

 int core_circle_size [NO_CORE_CIRCLES];
 float core_circle_angle [NO_CORE_CIRCLES];
 int core_circle_dist [NO_CORE_CIRCLES];
 int core_circle_dist2 [NO_CORE_CIRCLES];
 int core_circle_x [NO_CORE_CIRCLES]; // used for collision detection
 int core_circle_y [NO_CORE_CIRCLES]; // used for collision detection

 int destroyed [NO_PARTS];
 int destroyed_index;
 int range_minimum;

 int core_hp;
 int core_max_hp;
 int core_strength;

 int weapon [NO_WEAPONS];
 int weapon_number [NO_WEAPONS];
 int firing [2];
 int firing_time;
 char firing_a_beam;


};

enum
{
PART_NONE,
PART_BASIC,
PART_FORWARD,
PART_DRIVE,
PART_MISSILE,
PART_MGUN,
PART_3CANNON,
PART_ROTARY,
PART_SEEKER,
PART_BEAM,
PART_BEAM2,
PART_FLAK,
PART_SHIELD,
PART_ROCKET,
PART_EEL,
PART_DRIVE2,
// when adding a part, must add it to part_ranges in hs_build.

// when adding a non-weapon part type, must add it to the list in build_hs which excludes it from the
//  weapon list.
PART_TYPES
};

enum
{
TRACK_LOCKED, // locked in a particular angle
TRACK_FOLLOW,
TRACK_FOLLOW2, // used for PART_MGUN, which stops following while firing
TRACK_BEAM // used for PART_BEAM, which will not change direction while firing
};

#define POSITIONS 200

struct partstruct
{
 int type;
 int base_type; // stores type in case it's destroyed
 int x, y;
 int old_x, old_y;
 int move_x, move_y;
 int old_x2, old_y2;
 int uplink;
 int downlink [2];
 int side; // which side on hs
 int arm; // which arm on hs
 int tracks; // whether it tracks player
 float locked_angle;
 int slew;
/*
REMEMBER
 - when adding a variable, must also add it to mirror_hs in hs_build!!
*/
 float up_angle;
 int up_dist;
 float cumulative_angle;
 float in_up_angle;
 float out_up_angle;

 float position_angle [POSITIONS];
 float position_up_angle [POSITIONS];
 int position_up_dist [POSITIONS];
 int position_dist [POSITIONS];
 float position_move_angle [POSITIONS];
 int position_move_dist [POSITIONS];
// int position;

 int hp;
 int max_hp;
 int size;
 int hurt_flash;

 int angle;
 unsigned char counter;
 char slew_dir;
 int slew_time;

 int recycle;
 int fired;
 int which_fired;
 int spin_angle; // used by PART_ROTARY and anything else that spins
 int drive;
 int max_drive;
 int drive_turn; // if it's a drive, which direction does it cause hs to turn?
 // need to update each time angle of drive or hs' centre of mass changes
 int drive_push; // does it push forwards or backwards?

 char killed; // set to 1 if about to explode

 int index; // in map, refers to part, and vice versa
 int mirror_part; // in part, refers to the part (not map) on other side

 int chain; // value of chain set off by part's destruction (in units of part.size)
 int wasted; // damage done to downlink parts
 int weapon; // index of its weapon type in the weapon array
 int burst;

 int debug;
 float fdebug;

 int shield;
 int shield_target; // shield shrinks to this size
 int shield_flash;

 int total_range; // total range values of all pbullets that hit it
 int range_hits; // number of pbullets that hit it. Used to find an average range.
 int average_range; // used in build_hs to work out which new weapons to add.
 int total_direction;
 int average_direction;

};



enum
{
EBULLET_NONE,
EBULLET_SHOT,
EBULLET_SHOT2,
EBULLET_MISSILE,
EBULLET_MISSILE_BLAST,
EBULLET_SPIN,
EBULLET_SEEKER,
EBULLET_MINE,
EBULLET_FLAK,
EBULLET_FLAK2,
EBULLET_BEAM,
EBULLET_BEAM2,
EBULLET_ROCKET,
EBULLET_EEL,
EBULLET_BLOB

};


struct cloudstruct
{
 int type;
 int x;
 int y;
 int x2, y2;
 int x_speed;
 int y_speed;
 int timeout;
 int time;
 int status;
 int status2;
 int colour;
 int drag;
 int angle;
 float fangle;

};

enum
{
CLOUD_NONE,
CLOUD_BALL,
CLOUD_SMALL,
CLOUD_BALL_COL2,
CLOUD_BALL_COL3,
CLOUD_BANG,
CLOUD_DRAG_BALL,
CLOUD_SMALL_SHOCK,
CLOUD_LARGE_SHOCK,
CLOUD_HUGE_SHOCK,
CLOUD_FLARE,
CLOUD_SPARSE_FLARE,
CLOUD_SPARSE_NARROW_FLARE,
CLOUD_BURST,
CLOUD_2BALL,
CLOUD_SEEKER_TRAIL,
CLOUD_WORM_TRAIL,
CLOUD_ROCKET_TRAIL

};


struct optionstruct
{
 int sound_init; // if 0, sound isn't initialised at all. Changed in config file only.
 int sound_mode; // mono, stereo, reversed, off
 int run_vsync; // on or off
 int windowed;
 int sfx_volume; // sound fx volume; if 0 sound effects not played
 int ambience_volume; // if 0 ambience not played
 int joystick;
 int highscore [3];
 int joy_button [4];
 int joy_sensitivity;
 int init_joystick;
 int joy_analogue;
 int key_or_joy;
 int joy_stick;



};

#define SHOW_GRAPHS


