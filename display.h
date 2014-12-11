
//#ifndef IN_DISPLAY_C

#define WSHIP_ROTATIONS 128
#define WSHIP_ROTATION_MASK 127
#define WSHIP_ROTATION_BITSHIFT 3

#define TURRET_ROTATIONS 128
#define TURRET_ROTATION_MASK 127
#define TURRET_ROTATION_BITSHIFT 3

#define CIRCLES 40
#define RLE_BCIRCLES 32
#define RLE_CCIRCLES 64
#define RLE_SCIRCLES 128


/*

rotations must be power of 2, and maximum 1024

mask is rotations - 1

bitshift is the number of bitshifts needed to step down from 1024 to the rotations value

should be possible to change these three values for each rotation type without changing anything else.

*/

struct RLE_STRUCT
{
 RLE_SPRITE *sprite;
 int x;
 int y;
};


//#endif

void run_display(int draw_everything, int star_motion);
void run_underlay(void);
int evolve_screen(char reset);
void display_pause(void);
void get_interlude_background(void);

enum
{
WSHIP_SPRITE_BASIC,
WSHIP_SPRITES
};

enum
{
TURRET_SPRITE_BASIC,
TURRET_SPRITES
};

enum
{
PLAYER_RLE_BASE_1,
PLAYER_RLE_BASE_2,
PLAYER_RLE_BASE_3,
PLAYER_RLE_SIDE_L1,
PLAYER_RLE_SIDE_L2,
PLAYER_RLE_SIDE_L3,
PLAYER_RLE_SIDE_R1,
PLAYER_RLE_SIDE_R2,
PLAYER_RLE_SIDE_R3,
PLAYER_RLE_FRONT_1,
PLAYER_RLE_FRONT_2,
PLAYER_RLE_FRONT_3,
PLAYER_RLE_FRONT_4,
PLAYER_RLE_BIG,
PLAYER_RLE_WEAPON1,
PLAYER_RLE_WEAPON2,
PLAYER_RLE_WEAPON3,
PLAYER_RLE_WEAPON4,
PLAYER_RLE_WEAPON5,
PLAYER_RLE_WEAPON6,
PLAYER_RLE_MINI,
PLAYER_RLE_MENU1,
PLAYER_RLE_MENU2,
PLAYER_RLE_MENU3,
PLAYER_RLE_MENU4,
PLAYER_RLE_MENU5,
PLAYER_RLE_MENU6,
PLAYER_RLES

};

#define SHIELD_RLES 10
