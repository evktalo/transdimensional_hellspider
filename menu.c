#include <string.h>

#include "config.h"

#include "allegro.h"

#include "globvars.h"

#include "palette.h"
#include "game.h"
#include "input.h"

#include "display.h"
#include "sound.h"

#include "stuff.h"

#define TRANS_MODE drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
#define END_TRANS_MODE drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
#define END_TRANS drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

#define MENU_CIRCLES 6

#define FREQ_SELECT 300
#define FREQ_SELECT1 600

#define VOL_SELECT 100
#define VOL_SELECT1 100

 extern RGB palet [256];

 RGB splash_palette [256];

 BITMAP *butter_bitmap;

void key_box(const char ktext [], int command);
int acceptable_char(int scode);
void define_keys(void);
int ship_select(void);
void print_standard_names(int x, int y, int w);
void set_options(void);

void aabox(int x1, int y1, int x2, int y2);

void congratulations(void);
void congrat_wait(int waiting);

extern volatile unsigned char ticked;

extern BITMAP *display;

int menu_select;
int key_wait;
int thing;

int menu_counter;
int counter2;

int new_menu_circle;

int menu_circle_number [MENU_CIRCLES];
int menu_circle_size [MENU_CIRCLES];
int menu_circle_max_size [MENU_CIRCLES];
int menu_circle_rad [MENU_CIRCLES];
int menu_circle_rot [MENU_CIRCLES];
int menu_circle_grow_rate [MENU_CIRCLES];
int menu_circle_angle [MENU_CIRCLES];

int minicircle_size [MENU_CIRCLES];
int minicircle_max_size [MENU_CIRCLES];
int minicircle_distance [MENU_CIRCLES];

void init_menu_background(void);
void run_menu_background(void);
void display_menu_background(void);

void run_menu_circles(void);
void init_menu_circles(void);
void draw_menu_circle(int i, int j);
void goodbye_menu_circles(void);

void draw_upgrade_screen(void);
void draw_upgrade_hello(int hello);
void upgrade_box(int x1, int y1, int x2, int y2, int col_out, int col_in);
void urect(int x1, int y1, int x2, int y2, int col);

void draw_choose_screen(void);
void draw_choose_hello(int hello);
int choose_menu(void);

extern RLE_SPRITE *RLE_player [PLAYER_RLES];

enum
{
MENU_START,
//MENU_DIFFICULTY,
MENU_JOY_OR_KEYS,
MENU_KEYS,
MENU_OPTIONS,
MENU_EXIT
};

extern FONT* gfont;


enum
{
OPT_SFX_VOLUME,
OPT_MUSIC_VOLUME,
OPT_MODE,
OPT_VSYNC,
OPT_JOY1,
OPT_JOY2,
OPT_JOY3,
OPT_JOY4,
OPT_EXIT
};

int menu_command(int cmd);
int any_joystick_input(void);

enum
{
MC_UP,
MC_LEFT,
MC_DOWN,
MC_RIGHT,
MC_SELECT
};

void set_options(void);


void reset_menu_palette(void);
void run_menu_background(void);

int select_a_ship(void);

extern BITMAP *interlude_screen;


void reset_menu_palette(void)
{

 vsync();
// clear_bitmap(screen);
 clear_bitmap(display);
 init_palette(0);

// arena.target_palette = 0;
// set_base_palette();

//   set_base_palette();
//   int newpal = build_new_palette(arena.change_palette, 0, 0, 0);
//   clear_bitmap(screen);
//   set_palette(palet [newpal]);

}



void startup_menu(void)
{


 reset_menu_palette();


 menu_select = 0;
 key_wait = 30;

// int counter;

 menu_counter = 0;
 counter2 = 0;

 int y1 = 170;
 int y2 = 190 + menu_select * 30;
 int y3 = 218 + menu_select * 30;
 int y4 = 218 + menu_select * 30;

 int anykey = 0;

 int i;

//#define TEST_MUSIC


#ifdef TEST_MUSIC

 arena.level = 3;

// init_beat_new_level();

#endif


 init_menu_background();

 while (TRUE)
 {


#ifdef TEST_MUSIC
run_beat();

#endif

 grand(100);

 clear_to_color(display, COL_BACK);


 run_menu_background();
 display_menu_background();


 menu_counter += 4;
 if (menu_counter >= 40)
  menu_counter = 0;

 counter2 ++;
 if (counter2 >= 256)
  counter2 = 0;

 textprintf_centre_ex(display, font, 320, 100, -1, -1, "Transdimensional Hellspider");
// textprintf_centre_ex(display, font, 320, 140, -1, -1, "another demo for shmup-dev.com");
// draw_rle_sprite(display, white_RLE, 150, 150);

 int my = 200;

 y1 = my - 23;
 y2 = my - 10 + menu_select * 30;
 y3 = my + 19 + menu_select * 30;
 y4 = my + 150;


// rectfill(display, 370, y1, 600, y2, COL_COL1);
 TRANS_MODE
 rectfill(display, 370, y2 + 9, 640, y3 - 2, TRANS_BLUE1);
 vline(display, 369, y2 + 9, y3 - 2, TRANS_BLUE2);
 hline(display, 370, y2 + 8, 640, TRANS_BLUE2);
 hline(display, 370, y3 - 1, 640, TRANS_BLUE2);
// rect(display, 369, y2 + 8, 641, y3 - 1, TRANS_BLUE3);

// rect(display, 368, y2 + 7, 641, y3 - 0, TRANS_BLUE3);
 END_TRANS
// rectfill(display, 370, y3, 600, y4, TRANS_DGREEN);


 textprintf_ex(display, font, 400, my, -1, -1, "start game");
// textprintf_ex(display [2], font, 400, 260, -1, -1, "STAGE - %i", arena.starting_level);
 textprintf_ex(display, font, 400, my + 60, -1, -1, "set keys");
 textprintf_ex(display, font, 400, my + 90, -1, -1, "options");
 textprintf_ex(display, font, 400, my + 120, -1, -1, "exit");

// textprintf_ex(display, font, 40, 10, -1, -1, "%i", joy[0].stick[options.joy_stick].axis[0].pos);
// textprintf_ex(display, font, 40, 30, -1, -1, "%i", joy[0].stick[options.joy_stick].axis[1].pos);

/*
 if (options.joystick)
  textprintf_ex(display, font, 400, my + 120, -1, -1, "CALIBRATE JOYSTICK");
   else
   {
    textprintf_ex(display, font, 400, my + 120, -1, -1, "NO JOYSTICK");
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    rectfill(display, 399, my + 110, 530, my + 135, CONVERT_WHITE_TO_GREY);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   }
*/
 if (options.joystick)
 {
  if (options.key_or_joy == 1)
  textprintf_ex(display, font, 400, my + 30, -1, -1, "controls - joystick");
   else
    textprintf_ex(display, font, 400, my + 30, -1, -1, "controls - keyboard");
 }
  else
  {
   textprintf_ex(display, font, 400, my + 30, -1, -1, "no joystick");
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   rectfill(display, 399, my + 29, 530, my + 45, CONVERT_WHITE_TO_GREY);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

  }


// textprintf_ex(display, font, 5, 5, -1, -1, "%i", any_joystick_input());



 my = 330;

// y2 = my + 7 + arena.just_got_highscore * 30;
// y3 = my + 29 + arena.just_got_highscore * 30;




 textprintf_right_ex(display, font, 630, 442, -1, -1, "by linley henzell");
 textprintf_right_ex(display, font, 630, 462, -1, -1, "features analogue synth by jph wacheski");

// should print last score here...

 anykey = 0;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i])
   {
    anykey = 1;
   }
  }

  if (anykey == 0 && (options.joystick == 0 || any_joystick_input() == 0))
   key_wait = 0;



 if (key_wait == 0)
 {
  if (menu_command(MC_UP))
  {
   menu_select --;
   if (menu_select < 0)
    menu_select = MENU_EXIT;
   if (menu_select == MENU_JOY_OR_KEYS && options.joystick == 0)
    menu_select = MENU_JOY_OR_KEYS - 1;
   key_wait = 7;
   play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);

  }
  if (menu_command(MC_DOWN))
  {
   menu_select ++;
   if (menu_select > MENU_EXIT)
    menu_select = 0;
   if (menu_select == MENU_JOY_OR_KEYS && options.joystick == 0)
    menu_select = MENU_KEYS;
   key_wait = 7;
   play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
  }
  if (menu_command(MC_LEFT))
  {
//   if (menu_select == 2)
//    arena.starting_level = 1;
   if (menu_select == MENU_JOY_OR_KEYS)
   {
    if (options.key_or_joy == 0)
     options.key_or_joy = 1;
      else
       options.key_or_joy = 0;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   key_wait = 7;
  }
  if (menu_command(MC_RIGHT))
  {
//   if (menu_select == 2)
//    arena.starting_level = 2;
   if (menu_select == MENU_JOY_OR_KEYS)
   {
    if (options.key_or_joy == 0)
     options.key_or_joy = 1;
      else
       options.key_or_joy = 0;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   key_wait = 7;
  }

//  if (key [KEY_ESC])
//   exit(0);
  if (menu_command(MC_SELECT))
  {
   if (menu_select == MENU_EXIT)
   {
    play_basicwfv(WAV_SELECT1, FREQ_SELECT1, VOL_SELECT1);
    exit(0);
   }

   if (menu_select == MENU_KEYS)
   {
    key_wait = 10;
    define_keys();
    key_wait = 10;
    play_basicwfv(WAV_SELECT1, FREQ_SELECT1, VOL_SELECT1);
   }

   if (menu_select == MENU_OPTIONS)
   {
    key_wait = 10;
    play_basicwfv(WAV_SELECT1, FREQ_SELECT1, VOL_SELECT1);
    set_options();
    key_wait = 10;
   }

/*   if (menu_select == MENU_CALIBRATE)
   {
    jstick_calibrate();
    key_wait = 20;
    ticked = 0;
   }*/

   if (menu_select == MENU_START)
   {
//    arena.level = 1;//arena.starting_level;
    ticked = 0;
    key_wait = 30;
    play_basicwfv(WAV_SELECT1, FREQ_SELECT1, VOL_SELECT1);
//    if (ship_select() == 1)
    {
      vsync();
      game_loop();
//      if (arena.level == 4)
//       congratulations();
      init_menu_background();
      key_wait = 10; // was 1
      ticked = 0;
     }
//    }
//     else key_wait = 20;
   }
    else
     key_wait = 20;
  }
 }
  else
   key_wait --;




    do
    {
        rest(1);
//        thing ++;
    } while (ticked == 0);
    ticked = 0;


 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);



 };


}

#define MENU_SENSE 100

int menu_command(int cmd)
{

 if (options.joystick)
  poll_joystick();

 switch(cmd)
 {
  case MC_UP:
   if (key [KEY_UP] || key [KEY_8_PAD])
    return 1;
   if (options.joystick && joy[0].stick[options.joy_stick].axis[1].pos < -MENU_SENSE)
    return 1;
   return 0;
  case MC_DOWN:
   if (key [KEY_DOWN] || key [KEY_2_PAD])
    return 1;
   if (options.joystick && joy[0].stick[options.joy_stick].axis[1].pos > MENU_SENSE)
    return 1;
   return 0;
  case MC_LEFT:
   if (key [KEY_LEFT] || key [KEY_4_PAD])
    return 1;
   if (options.joystick && joy[0].stick[options.joy_stick].axis[0].pos < -MENU_SENSE)
    return 1;
   return 0;


  case MC_RIGHT:
   if (key [KEY_RIGHT] || key [KEY_6_PAD])
    return 1;
   if (options.joystick && joy[0].stick[options.joy_stick].axis[0].pos > MENU_SENSE)
    return 1;
   return 0;
  case MC_SELECT:
   if (key [KEY_SPACE] || key [KEY_ENTER] || key [KEY_ENTER_PAD] || key [KEY_Z] || key [player[0].ckey [CKEY_FIRE1]])
    return 1;
   if (options.joystick && joy[0].button[options.joy_button [0]].b)
    return 1;
   return 0;

 }

 return 0;

}

int any_joystick_input(void)
{
 if (options.joystick)
  poll_joystick();

  if (joy[0].stick[options.joy_stick].axis[0].pos < - MENU_SENSE
   || joy[0].stick[options.joy_stick].axis[0].pos > MENU_SENSE
   || joy[0].stick[options.joy_stick].axis[1].pos < -MENU_SENSE
   || joy[0].stick[options.joy_stick].axis[1].pos > MENU_SENSE)
    return 1;

   if (joy[0].button[options.joy_button [0]].b)
    return 1;

 return 0;
}


void define_keys(void)
{

 key_box("press key for up", CKEY_UP);
 key_box("press key for left", CKEY_LEFT);
 key_box("press key for right", CKEY_RIGHT);
 key_box("press key for down", CKEY_DOWN);
 key_box("press key for fire 1", CKEY_FIRE1);
 key_box("press key for fire 2", CKEY_FIRE2);
// key_box("PRESS KEY FOR FIRE 3", CKEY_FIRE3);
// key_box("PRESS KEY FOR SLOW", CKEY_SLOW);

 set_config_int("Misc", "key_up", player[0].ckey [CKEY_UP]);
 set_config_int("Misc", "key_left", player[0].ckey [CKEY_LEFT]);
 set_config_int("Misc", "key_right", player[0].ckey [CKEY_RIGHT]);
 set_config_int("Misc", "key_down", player[0].ckey [CKEY_DOWN]);
 set_config_int("Misc", "key_fire1", player[0].ckey [CKEY_FIRE1]);
 set_config_int("Misc", "key_fire2", player[0].ckey [CKEY_FIRE2]);
// set_config_int("Misc", "key_fire3", player[0].ckey [CKEY_FIRE3]);
// set_config_int("Misc", "key_slow", player[0].ckey [CKEY_SLOW]);

}


void key_box(const char ktext [], int command)
{

 rectfill(display, 200, 240, 400, 290, COL_OUTLINE);
 rect(display, 201, 241, 399, 289, COL_LGREY);

 textprintf_centre_ex(display, font, 300, 257, -1, -1, ktext);

 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);

 int inputted = KEY_ESC;

 int i;
 int anykey = 0;

 char holding = 1;

 do
 {

  do
    {
        thing ++;
    } while (ticked == 0);
    ticked = 0;
  key_wait --;


 anykey = 0;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i])
   {
    anykey = 1;
   }
  }

  if (anykey == 0)
   holding = 0;


//  if (key_wait > 0)
//   continue;


  if (holding == 0)
  {

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i])
   {
    inputted = i;
    holding = 1;
   }
  }
 }
//   while(acceptable_char(inputted) == 0);
  if (acceptable_char(inputted) != 0)
   break;

 } while (TRUE);


 player[0].ckey [command] = inputted;

 key_wait = 10;

}

#define OPTION_X 100
#define OPTION_Y 150
#define CENTRE_X 150

void set_options(void)
{

 int i, anykey = 0;

 int option_select = 0;

 while (TRUE)
 {

 rectfill(display, OPTION_X, OPTION_Y, OPTION_X + 200, OPTION_Y + 250, COL_OUTLINE);
 rect(display, OPTION_X, OPTION_Y, OPTION_X + 200, OPTION_Y + 250, COL_LGREY);
 //rect(display, 201, 241, 399, 509, COL_LGREY);



 rectfill(display, OPTION_X + 1, OPTION_Y + 15, OPTION_X + 200 - 1, OPTION_Y + 42, TRANS_BLUE2);
 textprintf_centre_ex(display, font, CENTRE_X, OPTION_Y + 20, -1, -1, "options");

 rectfill(display, OPTION_X + 1, OPTION_Y + 60 + option_select * 20, OPTION_X + 200 - 1, OPTION_Y + 80 + option_select * 20, TRANS_BLUE1);
// hline(display, 202, 298 + option_select * 20, 398, TRANS_GREEN2);
// hline(display, 202, 320 + option_select * 20, 398, TRANS_GREEN2);

 textprintf_ex(display, font, CENTRE_X, OPTION_Y + 60, -1, -1, "effects volume - %i", options.sfx_volume);
 textprintf_ex(display, font, CENTRE_X, OPTION_Y + 80, -1, -1, "music volume - %i", options.ambience_volume);

 if (options.windowed == 0)
  textprintf_ex(display, font, CENTRE_X, OPTION_Y + 100, -1, -1, "mode - fullscreen");
   else
    textprintf_ex(display, font, CENTRE_X, OPTION_Y + 100, -1, -1, "mode - windowed");

 if (options.run_vsync == 0)
  textprintf_ex(display, font, CENTRE_X, OPTION_Y + 120, -1, -1, "vsync - off");
   else
    textprintf_ex(display, font, CENTRE_X, OPTION_Y + 120, -1, -1, "vsync - on");

 if (options.joystick)
 {
  textprintf_ex(display, font, CENTRE_X, OPTION_Y + 140, -1, -1, "fire 1 - button %i", options.joy_button [0] + 1);
  textprintf_ex(display, font, CENTRE_X, OPTION_Y + 160, -1, -1, "fire 2 - button %i", options.joy_button [1] + 1);
  textprintf_ex(display, font, CENTRE_X, OPTION_Y + 180, -1, -1, "engine - button %i", options.joy_button [2] + 1);
  textprintf_ex(display, font, CENTRE_X, OPTION_Y + 200, -1, -1, "brake - button %i", options.joy_button [3] + 1);
//  textprintf_ex(display, font, 220, 420, -1, -1, "fire 3 - joystick %i", options.joy_button [3] + 1);
//  textprintf_ex(display, font, 220, 440, -1, -1, "slow move - joystick %i", options.joy_button [2] + 1);

 }
  else
  {
   textprintf_ex(display, font, CENTRE_X, OPTION_Y + 140, -1, -1, "no joystick");
   TRANS_MODE
   rectfill(display, CENTRE_X - 1, OPTION_Y + 140 - 1, CENTRE_X + 100, OPTION_Y + 140 + 15, CONVERT_WHITE_TO_GREY);
   END_TRANS

  }

  textprintf_ex(display, font, CENTRE_X, OPTION_Y + 220, -1, -1, "exit");



 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);

 anykey = 0;


  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i])
   {
    anykey = 1;
   }
  }

  if (anykey == 0 && (options.joystick == 0 || any_joystick_input() == 0))
   key_wait = 0;



 if (key_wait <= 0)
 {
  if (menu_command(MC_UP))
  {
   option_select --;
   if (option_select < 0)
    option_select = OPT_EXIT;
   if (option_select == OPT_JOY4 && options.joystick == 0)
    option_select = OPT_VSYNC;
   key_wait = 7;
   play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
  }
  if (menu_command(MC_DOWN))
  {
   option_select ++;
   if (option_select > OPT_EXIT)
    option_select = 0;
   if (option_select == OPT_JOY1 && options.joystick == 0)
    option_select = OPT_EXIT;
   key_wait = 7;
   play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
  }

  if (menu_command(MC_LEFT))
  {
   if (option_select == OPT_SFX_VOLUME)
   {
    options.sfx_volume -= 10;
    if (options.sfx_volume < 0)
     options.sfx_volume = 0;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_MUSIC_VOLUME)
   {
    options.ambience_volume -= 10;
    if (options.ambience_volume < 0)
     options.ambience_volume = 0;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }

   if (option_select == OPT_JOY1)
   {
    if (options.joy_button [0] == 0)
     options.joy_button [0] = joy[0].num_buttons - 1;
      else
       options.joy_button [0] --;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_JOY2)
   {
    if (options.joy_button [1] == 0)
     options.joy_button [1] = joy[0].num_buttons - 1;
      else
       options.joy_button [1] --;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_JOY3)
   {
    if (options.joy_button [2] == 0)
     options.joy_button [2] = joy[0].num_buttons - 1;
      else
       options.joy_button [2] --;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_JOY4)
   {
    if (options.joy_button [3] == 0)
     options.joy_button [3] = joy[0].num_buttons - 1;
      else
       options.joy_button [3] --;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
/*   if (option_select == OPT_JOYSLOW)
   {
    if (options.joy_button [2] == 0)
     options.joy_button [2] = joy[0].num_buttons - 1;
      else
       options.joy_button [2] --;
   }
   if (option_select == OPT_JOY3)
   {
    if (options.joy_button [3] == 0)
     options.joy_button [3] = joy[0].num_buttons - 1;
      else
       options.joy_button [3] --;
   }*/
   key_wait = 7;
  }

  if (menu_command(MC_RIGHT))
  {
   if (option_select == OPT_SFX_VOLUME)
   {
    options.sfx_volume += 10;
    if (options.sfx_volume > 100)
     options.sfx_volume = 100;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_MUSIC_VOLUME)
   {
    options.ambience_volume += 10;
    if (options.ambience_volume > 100)
     options.ambience_volume = 100;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }

   if (option_select == OPT_JOY1)
   {
    if (options.joy_button [0] >= joy[0].num_buttons - 1)
     options.joy_button [0] = 0;
      else
       options.joy_button [0] ++;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_JOY2)
   {
    if (options.joy_button [1] >= joy[0].num_buttons - 1)
     options.joy_button [1] = 0;
      else
       options.joy_button [1] ++;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_JOY3)
   {
    if (options.joy_button [2] >= joy[0].num_buttons - 1)
     options.joy_button [2] = 0;
      else
       options.joy_button [2] ++;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_JOY4)
   {
    if (options.joy_button [3] >= joy[0].num_buttons - 1)
     options.joy_button [3] = 0;
      else
       options.joy_button [3] ++;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
/*   if (option_select == OPT_JOY3)
   {
    if (options.joy_button [3] >= joy[0].num_buttons - 1)
     options.joy_button [3] = 0;
      else
       options.joy_button [3] ++;
   }
   if (option_select == OPT_JOYSLOW)
   {
    if (options.joy_button [2] >= joy[0].num_buttons - 1)
     options.joy_button [2] = 0;
      else
       options.joy_button [2] ++;
   }*/
   key_wait = 7;
  }

  if (key [KEY_ESC])
   break;

  if (menu_command(MC_SELECT))
  {
   if (option_select == OPT_EXIT)
   {
    play_basicwfv(WAV_SELECT1, FREQ_SELECT1, VOL_SELECT1);
    break;
   }
  }

  if (menu_command(MC_SELECT) || menu_command(MC_LEFT) || menu_command(MC_RIGHT))
  {
   if (option_select == OPT_MODE)
   {
    options.windowed ^= 1;
    key_wait = 7;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
   if (option_select == OPT_VSYNC)
   {
    options.run_vsync ^= 1;
    key_wait = 7;
    play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
   }
  }


/*   if (menu_select == MENU_KEYS)
   {
    key_wait = 10;
    define_keys();
    key_wait = 10;
   }

   if (menu_select == MENU_OPTIONS)
   {
    key_wait = 10;
    set_options();
    key_wait = 10;
   }*/



 }
//  else
   key_wait --;




    do
    {
        thing ++;
    } while (ticked == 0);
    ticked = 0;

 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);



 };

 set_config_int("Misc", "sfx_volume", options.sfx_volume);
 set_config_int("Misc", "ambience_volume", options.ambience_volume);
 if (options.joystick)
 {
  set_config_int("Misc", "joy_button_1", options.joy_button [0]);
  set_config_int("Misc", "joy_button_2", options.joy_button [1]);
//  set_config_int("Misc", "joy_button_slow", options.joy_button [2]);
//  set_config_int("Misc", "joy_button_3", options.joy_button [3]);
 }
 set_config_int("Misc", "vsync", options.run_vsync);
 set_config_int("Misc", "Windowed", options.windowed);


}




int acceptable_char(int scode)
{

 switch(scode)
 {
  case KEY_ESC:
   key_wait = 7;
   return 0;
  case KEY_ENTER:
  case KEY_ENTER_PAD:
   key_wait = 7;
   return 0;
 }
 return 1;

}



void aabox(int x1, int y1, int x2, int y2)
{

 TRANS_MODE
 rectfill(display, x1, y1, x2, y2, TRANS_BLUE1);
 rect(display, x1, y1, x2, y2, TRANS_BLUE3);
 rect(display, x1 + 1, y1 - 1, x2 + 1, y2 - 1, TRANS_BLUE2);
 rect(display, x1 - 1, y1 + 1, x2 - 1, y2 + 1, TRANS_BLUE2);
 rect(display, x1 - 2, y1 + 2, x2 - 2, y2 + 2, TRANS_BLUE1);
 END_TRANS

}


void congratulations(void)
{

// clear_bitmap(screen);
 vsync();
 clear_bitmap(screen);
 clear_bitmap(display);

// arena.target_palette = BASECOL_CFLOWER;
// set_base_palette();

//   int newpal = build_new_palette(arena.change_palette, 0, 0, 0);
   vsync();
//   set_palette(palet [newpal]);

 int y;
 int y_int = 35;

 int c_count = 0;

// init_cflowers();

 do
 {

//   run_cflowers();

     y = 90;

 textprintf_centre_ex(display, font, 320, y, -1, -1, "V I C T O R Y ! !");

 y += y_int * 2;

 if (c_count >= 100)
  textprintf_centre_ex(display, font, 320, y, -1, -1, "YOU  HAVE  DEFEATED  ALL  OF  THE  HELLSPIDERS");
 y += y_int;

 if (c_count >= 200)
  textprintf_centre_ex(display, font, 320, y, -1, -1, "AND  SAVED  OUR  WORLD  FROM  DESTRUCTION.");
 y += y_int;

 if (c_count >= 350)
  textprintf_centre_ex(display, font, 320, y, -1, -1, "NICE  WORK.");

 y += y_int * 2;
/* if (c_count >= 450)
 {
  switch(arena.difficulty)
  {
     case 0: textprintf_centre_ex(display, font, 320, y, -1, -1, "BUT  CAN  YOU  DEFEAT  A  MORE  CHALLENGING  LEVEL?"); break;
     case 1: textprintf_centre_ex(display, font, 320, y, -1, -1, "YOUR  ACHIEVEMENT  IS  IMPRESSIVE!"); break;
     case 2: textprintf_centre_ex(display, font, 320, y, -1, -1, "SURELY THIS HAS BEEN PUNISHMENT ENOUGH."); break;
  }
 }*/

 if (c_count >= 550)
 {
  y += y_int * 2;
  textprintf_centre_ex(display, font, 320, y, -1, -1, "press space to continue");

  TRANS_MODE
//  rectfill(display, 100, y, 500, y + 15, CONVERT_WHITE_TO_GREY);
  END_TRANS_MODE

  if (key [KEY_SPACE])
   break;
 }

 blit(display, screen, 0, 0, 0, 0, 640, 480);

 do
 {
  rest(5);
 } while (ticked == 0);

 ticked --;
 c_count ++;
 if (c_count > 30000)
  c_count = 30000;

 } while(TRUE);

 clear_bitmap(screen);
 rest(200);




 clear_bitmap(screen);
 vsync();
 clear_bitmap(display);

// arena.target_palette = BASECOL_L1_SPRING;
// set_base_palette();

//   newpal = build_new_palette(arena.change_palette, 0, 0, 0);
   vsync();
//   set_palette(palet [newpal]);



}

#define UBOX_X 100
#define UBOX_Y 130
#define UBOX_H 35
#define UBOX_W 200

int upgrade_select;
int upgrade_displace;
int just_upgraded;
#define DISPLACE_MAX 40

void upgrade_menu(void)
{

// clear_to_color(display, COL_BACK);

 blit(interlude_screen, display, 0,0,0,0,640,480);

 upgrade_select = 0;
 int i, anykey;
 key_wait = 0;
 upgrade_displace = 0;//DISPLACE_MAX;
 just_upgraded = 0;

 int still_pressing_fire = 1;

 menu_counter = 0;


 while (player[0].upgrade [upgrade_select] >= 2)
 {
   upgrade_select ++;
 };


 for (i = 0; i < 300; i += 10)
 {
    draw_upgrade_hello(i);
    do
    {
        rest(1);
    } while (ticked == 0);
    ticked = 0;

  }

 while (TRUE)
 {

   if (!menu_command(MC_SELECT))
    still_pressing_fire = 0; // because you might be holding fire from the level still.

     draw_upgrade_screen();


 menu_counter ++;
 if (menu_counter >= 32)
  menu_counter = 0;

  anykey = 0;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i])
   {
    anykey = 1;
   }
  }

  if (anykey == 0 && (options.joystick == 0 || any_joystick_input() == 0))
   key_wait = 0;

  if (key_wait == 0
   && just_upgraded == 0)
  {

    if (menu_command(MC_UP))
    {
     do
     {
        upgrade_select --;
        if (upgrade_select < 0)
         upgrade_select = U_TYPES - 1;
     } while (player[0].upgrade [upgrade_select] >= 2);
      key_wait = 7;
      upgrade_displace = 0;
      play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
    }
    if (menu_command(MC_DOWN))
    {
     do
     {
        upgrade_select ++;
        if (upgrade_select > U_TYPES - 1)
         upgrade_select = 0;
     } while (player[0].upgrade [upgrade_select] >= 2);
      key_wait = 7;
      upgrade_displace = 0;
      play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
    }
    if (still_pressing_fire == 0 && menu_command(MC_SELECT))
    {
     if (upgrade_select == U_CHANGE)
     {
      key_wait = 7;
      play_basicwfv(WAV_SELECT1, FREQ_SELECT1, VOL_SELECT1);
      if (choose_menu())
      {
       return;
      }
     }
      else
       if (upgrade_select == U_EXTEND)
       {
//        key_wait = 7;
        just_upgraded = 40;
        player[0].ships ++;
        play_effectwfv(WAV_FLYS, 800, 250);
       }
        else
        {
         player[0].upgrade [upgrade_select] ++;
         just_upgraded = 40;
         play_effectwfv(WAV_FLYS, 1000, 250);
        }
    }

   }
    else
     key_wait --;

    upgrade_displace += 3;
    if (upgrade_displace > DISPLACE_MAX)
     upgrade_displace = DISPLACE_MAX;

    if (just_upgraded > 0)
    {
     just_upgraded --;
     if (just_upgraded == 0)
     {
      return;
     }
    }

    do
    {
        rest(1);
    } while (ticked == 0);
    ticked = 0;

 };


}

void draw_upgrade_hello(int hello)
{

// clear_to_color(display, COL_BACK);
 blit(interlude_screen, display, 0,0,0,0,640,480);

 int i, x, y;
 int cols [U_TYPES] [3] =
 {
  {TRANS_RED4, TRANS_RED3, TRANS_RED2},
  {TRANS_RED4, TRANS_RED3, TRANS_RED2},

  {TRANS_GREEN4, TRANS_GREEN3, TRANS_GREEN2},
//  {TRANS_GREEN4, TRANS_GREEN3, TRANS_GREEN2},
//  {TRANS_GREEN4, TRANS_GREEN3, TRANS_GREEN2},

  {TRANS_BLUE4, TRANS_BLUE3, TRANS_BLUE2},
  {TRANS_BLUE4, TRANS_BLUE3, TRANS_BLUE2},

  {TRANS_RED4, TRANS_RED3, TRANS_RED2},
  {TRANS_BLUE4, TRANS_BLUE3, TRANS_BLUE2}

 };
/*
 int cols [U_TYPES] [3] =
 {
  {TRANS_RED3, TRANS_RED2, TRANS_RED1},
  {TRANS_RED3, TRANS_RED2, TRANS_RED1},

  {TRANS_GREEN3, TRANS_GREEN2, TRANS_GREEN1},
  {TRANS_GREEN3, TRANS_GREEN2, TRANS_GREEN1},
  {TRANS_GREEN3, TRANS_GREEN2, TRANS_GREEN1},

  {TRANS_BLUE3, TRANS_BLUE2, TRANS_BLUE1},
  {TRANS_BLUE3, TRANS_BLUE2, TRANS_BLUE1},

  {TRANS_RED3, TRANS_RED2, TRANS_RED1}

 };
*/
// TRANS_MODE
 for (i = 0; i < U_TYPES; i ++)
 {
   x = UBOX_X - 300 + hello;
   y = UBOX_Y + i * (UBOX_H + 5);


     rectfill(display, x, y, x + UBOX_W, y + UBOX_H, cols [i] [2]);
//     rect(display, x - 1, y - 1, x + UBOX_W + 1, y + UBOX_H + 1, cols [i] [1]);

 }
// END_TRANS

 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);


}

void draw_upgrade_screen(void)
{

// clear_to_color(display, COL_BACK);
 blit(interlude_screen, display, 0,0,0,0,640,480);

 textprintf_centre_ex(display, font, 320, 70, -1, -1, "You may increase");
 textprintf_centre_ex(display, font, 320, 85, -1, -1, "the power of your vehicle.");

 int i, x, y, j;
 char dstring [30];

 int cols [U_TYPES] [3] =
 {

  {TRANS_RED4, TRANS_RED3, TRANS_RED2},
  {TRANS_RED4, TRANS_RED3, TRANS_RED2},

  {TRANS_GREEN4, TRANS_GREEN3, TRANS_GREEN2},
//  {TRANS_GREEN4, TRANS_GREEN3, TRANS_GREEN2},
//  {TRANS_GREEN4, TRANS_GREEN3, TRANS_GREEN2},

  {TRANS_BLUE4, TRANS_BLUE3, TRANS_BLUE2},
  {TRANS_BLUE4, TRANS_BLUE3, TRANS_BLUE2},

  {TRANS_RED4, TRANS_RED3, TRANS_RED2},
  {TRANS_BLUE4, TRANS_BLUE3, TRANS_BLUE2}

 };
  /*  {TRANS_YELLOW4, TRANS_RED3},
  {TRANS_YELLOW4, TRANS_RED3},

  {TRANS_GREEN3, TRANS_GREEN2},
  {TRANS_GREEN3, TRANS_GREEN2},
  {TRANS_GREEN3, TRANS_GREEN2},

  {TRANS_BLUE3, TRANS_BLUE2},
  {TRANS_BLUE3, TRANS_BLUE2},

  {TRANS_RED2, TRANS_RED1}*/


#define SHIP_X 459
#define SHIP_Y 236

 draw_rle_sprite(display, RLE_player [PLAYER_RLE_BIG], SHIP_X - 49, SHIP_Y - 33);

 TRANS_MODE
 for (i = 0; i < U_TYPES; i ++)
 {
   x = UBOX_X;
   y = UBOX_Y + i * (UBOX_H + 5);

   if (i == upgrade_select)
   {
    if (just_upgraded > 0)
    {
      if (just_upgraded & 4)
      {
         rectfill(display, x - upgrade_displace, y, x + UBOX_W, y + UBOX_H, cols [i] [1]);
         urect(x - 1 - upgrade_displace, y - 1, x + UBOX_W + 1, y + UBOX_H + 1, cols [i] [0]);
      }
       else
       {
         rectfill(display, x - upgrade_displace, y, x + UBOX_W, y + UBOX_H, cols [i] [0]);
         urect(x - 1 - upgrade_displace, y - 1, x + UBOX_W + 1, y + UBOX_H + 1, cols [i] [0]);
       }
    }
     else
     {
      if (key_wait > 5)
      {
       rectfill(display, x - upgrade_displace, y, x + UBOX_W, y + UBOX_H, cols [i] [0]);
       urect(x - 1 - upgrade_displace, y - 1, x + UBOX_W + 1, y + UBOX_H + 1, cols [i] [0]);
      }
        else
        {
         rectfill(display, x - upgrade_displace, y, x + UBOX_W, y + UBOX_H, cols [i] [1]);
         urect(x - 1 - upgrade_displace, y - 1, x + UBOX_W + 1, y + UBOX_H + 1, cols [i] [0]);
        }
     }
   }
    else
    {
     rectfill(display, x, y, x + UBOX_W, y + UBOX_H, cols [i] [2]);
     urect(x - 1, y - 1, x + UBOX_W + 1, y + UBOX_H + 1, cols [i] [1]);
    }

   if (i != U_CHANGE && i != U_EXTEND)// && player[0].upgrade [i] > 0)
   {
    for (j = 0; j < 2; j ++)
    {
     if (player[0].upgrade [i] > j)
      rectfill(display, x + UBOX_W + 10 + (j*25), y, x + UBOX_W + 30 + (j*25), y + UBOX_H, cols [i] [2]);
     urect(x + UBOX_W + 10 + (j*25) - 1, y - 1, x + UBOX_W + 30 + (j*25) + 1, y + UBOX_H + 1, cols [i] [1]);
    }
   }

   /*   if (player[0].upgrade [i] > 0)
   {
    for (j = 0; j < player[0].upgrade [i]; j ++)
    {
     rectfill(display, x + UBOX_W + 10 + (j*25), y, x + UBOX_W + 30 + (j*25), y + UBOX_H, cols [i] [2]);
     urect(x + UBOX_W + 10 + (j*25) - 1, y - 1, x + UBOX_W + 30 + (j*25) + 1, y + UBOX_H + 1, cols [i] [1]);
    }
   }*/

   switch(i)
   {
    case U_POWER: strcpy(dstring, "Weapon power ++"); break;
    case U_RANGE: strcpy(dstring, "Weapon range ++"); break;
    case U_AGILITY: strcpy(dstring, "Agility ++"); break;
//    case U_GLIDE: strcpy(dstring, "Glide ++"); break;
//    case U_TURN: strcpy(dstring, "Rotation ++"); break;
    case U_SHIELD: strcpy(dstring, "Shield capacity ++"); break;
    case U_CHARGE: strcpy(dstring, "Shield recharge ++"); break;
    case U_CHANGE: strcpy(dstring, "Change weapon"); break;
    case U_EXTEND: strcpy(dstring, "Extra life"); break;
    default: strcpy(dstring, "Error!"); break;

   }

 textprintf_ex(display, font, x + 10, y + 7, -1, -1, dstring);

 }
 END_TRANS

 switch(upgrade_select)
 {
  case U_POWER:
   upgrade_box(SHIP_X - 30, SHIP_Y - 24, SHIP_X - 7, SHIP_Y + 3, TRANS_RED2, TRANS_RED1);
   upgrade_box(SHIP_X + 30, SHIP_Y - 24, SHIP_X + 7, SHIP_Y + 3, TRANS_RED2, TRANS_RED1);
   break;
  case U_CHANGE:
   upgrade_box(SHIP_X - 30, SHIP_Y - 33, SHIP_X - 9, SHIP_Y + 3, TRANS_RED2, TRANS_RED1);
   upgrade_box(SHIP_X + 30, SHIP_Y - 33, SHIP_X + 9, SHIP_Y + 3, TRANS_RED2, TRANS_RED1);
   break;
  case U_EXTEND:
   upgrade_box(SHIP_X - 54, SHIP_Y - 33, SHIP_X + 54, SHIP_Y + 33, TRANS_BLUE2, TRANS_BLUE1);
   break;
  case U_RANGE:
   upgrade_box(SHIP_X - 29, SHIP_Y - 33, SHIP_X - 9, SHIP_Y - 4, TRANS_RED2, TRANS_RED1);
   upgrade_box(SHIP_X + 29, SHIP_Y - 33, SHIP_X + 9, SHIP_Y - 4, TRANS_RED2, TRANS_RED1);
   break;
  case U_AGILITY:
   upgrade_box(SHIP_X - 22, SHIP_Y + 1, SHIP_X + 22, SHIP_Y + 24, TRANS_GREEN2, TRANS_GREEN1);
   upgrade_box(SHIP_X - 54, SHIP_Y + 6, SHIP_X - 34, SHIP_Y + 34, TRANS_GREEN2, TRANS_GREEN1);
   upgrade_box(SHIP_X + 54, SHIP_Y + 6, SHIP_X + 34, SHIP_Y + 34, TRANS_GREEN2, TRANS_GREEN1);
   break;
/*  case U_GLIDE:
   upgrade_box(SHIP_X - 52, SHIP_Y - 8, SHIP_X - 27, SHIP_Y + 34, TRANS_GREEN2, TRANS_GREEN1);
   upgrade_box(SHIP_X + 52, SHIP_Y - 8, SHIP_X + 27, SHIP_Y + 34, TRANS_GREEN2, TRANS_GREEN1);
   break;
  case U_TURN:
   upgrade_box(SHIP_X - 54, SHIP_Y + 6, SHIP_X - 34, SHIP_Y + 34, TRANS_GREEN2, TRANS_GREEN1);
   upgrade_box(SHIP_X + 54, SHIP_Y + 6, SHIP_X + 34, SHIP_Y + 34, TRANS_GREEN2, TRANS_GREEN1);
   break;*/
  case U_SHIELD:
  case U_CHARGE:
   upgrade_box(SHIP_X - 11, SHIP_Y - 16, SHIP_X + 11, SHIP_Y + 23, TRANS_BLUE2, TRANS_BLUE1);
   break;

 }

 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);


}

void urect(int x1, int y1, int x2, int y2, int col)
{

 int temp;

 if (x1 > x2)
 {
  temp = x2;
  x2 = x1;
  x1 = temp;
 }
 if (y1 > y2)
 {
  temp = y2;
  y2 = y1;
  y1 = temp;
 }
// rect(display, x1, y1, x2, y2, col_out);
 hline(display, x1 + 1, y1, x2 - 1, col);
 hline(display, x1 + 1, y2, x2 - 1, col);
 vline(display, x1, y1 + 1, y2 - 1, col);
 vline(display, x2, y1 + 1, y2 - 1, col);
}

void upgrade_box(int x1, int y1, int x2, int y2, int col_out, int col_in)
{


 TRANS_MODE

 int temp;

 if (x1 > x2)
 {
  temp = x2;
  x2 = x1;
  x1 = temp;
 }
 if (y1 > y2)
 {
  temp = y2;
  y2 = y1;
  y1 = temp;
 }
// rect(display, x1, y1, x2, y2, col_out);
 hline(display, x1 + 1, y1, x2 - 1, col_out);
 hline(display, x1 + 1, y2, x2 - 1, col_out);
 vline(display, x1, y1 + 1, y2 - 1, col_out);
 vline(display, x2, y1 + 1, y2 - 1, col_out);

 int i;
 int max = abs(y2 - y1);

 for (i = 1; i < max; i ++)
 {
  if ((i - (menu_counter / 4)) % 3 != 0)
   hline(display, x1, i + y1, x2, col_in);
 }

// rectfill(display, x1, y1, x2, y2, col_in);

 END_TRANS


}





















int choose_select;
int just_chose;

int choose_menu(void)
{

// clear_to_color(display, COL_BACK);
 choose_select = 0;
 int i, anykey;
 key_wait = 0;
 just_chose = 0;

 int still_pressing_fire = 1;

 menu_counter = 0;



 for (i = 0; i < 200; i += 20)
 {
    draw_choose_hello(i);
    do
    {
        rest(1);
    } while (ticked == 0);
    ticked = 0;

  }

 while (TRUE)
 {

   if (!menu_command(MC_SELECT))
    still_pressing_fire = 0; // because you might be holding fire from the level still.

     draw_choose_screen();


 menu_counter ++;
 if (menu_counter >= 32)
  menu_counter = 0;

  anykey = 0;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i])
   {
    anykey = 1;
   }
  }

  if (anykey == 0 && (options.joystick == 0 || any_joystick_input() == 0))
   key_wait = 0;

  if (key_wait == 0
   && just_chose == 0)
  {

    if (menu_command(MC_UP))
    {
      choose_select --;
      if (choose_select < 0)
       choose_select = WEAPONS - 1;
      key_wait = 7;
      play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
//      upgrade_displace = 0;
    }
    if (menu_command(MC_DOWN))
    {
      choose_select ++;
      if (choose_select > WEAPONS - 1)
       choose_select = 0;
      key_wait = 7;
      play_basicwfv(WAV_SELECT0, FREQ_SELECT, VOL_SELECT);
//      upgrade_displace = 0;
    }
    if (still_pressing_fire == 0 && menu_command(MC_SELECT))
    {
     player[0].weapon = choose_select;
     just_chose = 40;
     play_basicwfv(WAV_SELECT1, FREQ_SELECT1, VOL_SELECT1);
    }

   }
    else
     key_wait --;

    if (just_chose > 0)
    {
     just_chose --;
     if (just_chose == 0)
      return 1;
    }

    if (key [KEY_ESC])
     return 0;

    do
    {
        rest(1);
    } while (ticked == 0);
    ticked = 0;

 };

 return 0;

}

void draw_choose_hello(int hello)
{

// clear_to_color(display, COL_BACK);

 urect(170, 240 - hello, 470, 240 + hello, TRANS_BLUE3);
 rectfill(display, 171, 241 - hello, 469, 239 + hello, TRANS_BLUE1);

 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);


}

#define CBOX_X 220
#define CBOX_Y 140
#define CBOX_H 28
#define CBOX_W 29

void draw_choose_screen(void)
{

// rectfill(display, 170, 240 - 200, 470, 240 + 200, COL_DGREY);
 urect(170, 240 - 200, 470, 240 + 200, TRANS_BLUE3);
 rectfill(display, 171, 241 - 200, 469, 239 + 200, TRANS_BLUE1);


 urect(190, 50, 450, 95, TRANS_BLUE3);
 rectfill(display, 191, 51, 449, 94, TRANS_BLUE2);
 textprintf_centre_ex(display, font, 320, 55, -1, -1, "Choose a weapon");
 textprintf_centre_ex(display, font, 320, 70, -1, -1, "with which to face the Hellspider.");


 int i, x, y;
 char dstring [30];
 char xstring [50];
 char xstring2 [50];

 for (i = 0; i < WEAPONS; i ++)
 {
   x = CBOX_X;
   y = CBOX_Y + i * (CBOX_H + 15) - 30;

 TRANS_MODE

   if (i == choose_select)
   {
     rectfill(display, x, y, x + CBOX_W, y + CBOX_H, TRANS_BLUE2);
     urect(x - 1, y - 1, x + CBOX_W + 1, y + CBOX_H + 1, TRANS_BLUE3);
     if (just_chose > 0
      && just_chose & 4)
     {
      rectfill(display, x, y, x + CBOX_W, y + CBOX_H, TRANS_BLUE3);

     }

   }
    else
    {
     rectfill(display, x, y, x + CBOX_W, y + CBOX_H, TRANS_BLUE1);
     urect(x - 1, y - 1, x + CBOX_W + 1, y + CBOX_H + 1, TRANS_BLUE2);
    }

 END_TRANS

 draw_rle_sprite(display, RLE_player [PLAYER_RLE_WEAPON1 + i], x+1, y);

   switch(i)
   {

    case WEAPON_BASIC: strcpy(dstring, "Furious Eye");
    strcpy(xstring, "A straightforward weapon.");
    strcpy(xstring2, "");
    break;
    case WEAPON_MINE: strcpy(dstring, "Drifting Eye");
    strcpy(xstring, "An intense focus of destruction.");
    strcpy(xstring2, "Dive-bombing tactics recommended.");
    break;
    case WEAPON_ROCKET: strcpy(dstring, "Burning Claws");
    strcpy(xstring, "Powerful but difficult to aim.");
    strcpy(xstring2, "");
    break;
    case WEAPON_WORM: strcpy(dstring, "Worms of Anguish");
    strcpy(xstring, "Seek the heart");
    strcpy(xstring2, "of the Hellspider.");
    break;
    case WEAPON_MISSILE: strcpy(dstring, "Worms of Despair");
    strcpy(xstring, "Strike at your enemy");
    strcpy(xstring2, "from a great distance.");
    break;
    case WEAPON_SCATTER: strcpy(dstring, "Chaos Mouth");
    strcpy(xstring, "For attacking at point-blank range.");
    strcpy(xstring2, "");
    break;
    default: strcpy(dstring, "Error!"); break;

   }

 textprintf_ex(display, font, x + 40, y + 7, -1, -1, dstring);
 if (i == choose_select)
 {
  urect(180, 390, 460, 430, TRANS_BLUE3);
  rectfill(display, 181, 391, 459, 429, TRANS_BLUE2);
  textprintf_centre_ex(display, font, 320, 395, -1, -1, xstring);
  textprintf_centre_ex(display, font, 320, 410, -1, -1, xstring2);
 }

 }

 vsync();
 blit(display, screen, 0, 0, 0, 0, 640, 480);


}


int mship_x;
int mship_y;
int mship_x_speed;
int mship_y_speed;
int mship_frame;
int mship_drive;
int mship_phase;
int next_move;

#define MSTARS 300

int mstar_x [MSTARS];
int mstar_y [MSTARS];
int mstar_bright1 [MSTARS];
int mstar_bright2 [MSTARS];
int mstar_speed;

#define MSHIP_BASE_X (200<<10)
#define MSHIP_BASE_Y (280<<10)


void init_menu_background(void)
{

 mship_x = MSHIP_BASE_X;
 mship_y = -500 << 10;
 mship_x_speed = 0;
 mship_phase = 0;
 mship_x_speed = 0;
 mship_y_speed = 5 << 10;
 mship_drive = 0;
 mship_frame = 0;
 next_move = 10;

 int i;

 for (i = 0; i < MSTARS; i ++)
 {
  mstar_x [i] = grand(640) << 10;
  mstar_y [i] = grand(480) << 10;
  mstar_bright1 [i] = COL_STAR2 + grand(5);
  mstar_bright2 [i] = mstar_bright1 [i] - 2;
 }
 mstar_speed = 0;

}

void run_menu_background(void)
{

 mship_x += mship_x_speed;
 mship_y += mship_y_speed;

 switch(mship_phase)
 {
  case 0:
   if (mship_y > 250 << 10)
    mship_phase = 1;
   break;
  case 1:
   if (mship_drive < 100)
    mship_drive ++;
   if (mship_frame < 100)
    mship_frame += 3;
   mship_y_speed -= 1 << 6;
   if (mship_y < 200 << 10)
    mship_phase = 2;
   mship_x_speed *= 980;
   mship_x_speed >>= 10;
   mship_y_speed *= 980;
   mship_y_speed >>= 10;
   break;
  case 2:
   if (mship_drive < 100)
    mship_drive ++;
   if (mship_frame < 100)
    mship_frame += 3;


   next_move --;
   if (next_move <= 0)
   {
    if (mship_x <= MSHIP_BASE_X)
     mship_x_speed += grand(1000);
    if (mship_x > MSHIP_BASE_X)
     mship_x_speed -= grand(1000);
//    if (mship_y <= MSHIP_BASE_Y)
//     mship_y_speed += grand(1000);
//    if (mship_y > MSHIP_BASE_Y)
     mship_y_speed -= grand(1000);
    next_move = 10 + grand(50);
   }
   if (mship_y < 150 << 10)
    mship_phase = 3;
   mship_x_speed *= 980;
   mship_x_speed >>= 10;
   mship_y_speed *= 980;
   mship_y_speed >>= 10;
   break;
  case 3:
   mship_drive -= 4;
   if (mship_drive < 0)
    mship_drive = 0;
   mship_frame -= 3;
   if (mship_frame < 0)
    mship_frame = 0;
   mship_y_speed += 1 << 8;
   if (mship_y > 300 << 10)
    mship_phase = 2;
   mship_x_speed *= 980;
   mship_x_speed >>= 10;
   mship_y_speed *= 980;
   mship_y_speed >>= 10;
   break;

 }

 mstar_speed += 50;
 if (mstar_speed > 4 << 10)
  mstar_speed = 4 << 10;

 int i;

 for (i = 0; i < MSTARS; i ++)
 {
  mstar_y [i] += mstar_speed;
  if (mstar_y [i] > 500 << 10)
  {
   mstar_x [i] = grand(640) << 10;
   mstar_y [i] = -20 << 10;
   mstar_bright1 [i] = COL_STAR2 + grand(5);
   mstar_bright2 [i] = mstar_bright1 [i] - 2;
  }
 }

}

RLE_SPRITE *RLE_ccircle_basic [3] [RLE_CCIRCLES];
RLE_SPRITE *RLE_ccircle_2cols [3] [RLE_CCIRCLES];

void display_menu_background(void)
{

 int i;

 for (i = 0; i < MSTARS; i ++)
 {
  vline(display, mstar_x [i] >> 10, mstar_y [i] >> 10, (mstar_y [i] - mstar_speed) >> 10, mstar_bright1 [i]);
  putpixel(display, mstar_x [i] >> 10, mstar_y [i] >> 10, mstar_bright1 [i]);
 }

 int sprite = PLAYER_RLE_MENU1;

 sprite += mship_frame / 10;
 if (sprite > PLAYER_RLE_MENU6)
  sprite = PLAYER_RLE_MENU6;

 draw_rle_sprite(display, RLE_player [sprite], (mship_x >> 10) - 48, (mship_y >> 10) - 30);

// line(display, (mship_x >> 10) - 50, (mship_y >> 10) - 33, 240, 200, COL_WHITE);

   int size = mship_drive / 5;

   if (size <= 0)
    return;

   size += grand(10);

   if (size >= RLE_CCIRCLES)
    size = RLE_CCIRCLES - 1;

   draw_trans_rle_sprite(display, RLE_ccircle_2cols [0] [size], (mship_x>>10)-size, (mship_y>>10) + 25);
   size /= 2;
   size += grand(5);
   if (size > 0)
    draw_trans_rle_sprite(display, RLE_ccircle_basic [0] [size], (mship_x>>10)-size, (mship_y>>10) + 29);
//   size /= 2;
//   size += grand(5);
   if (size > 0)
    draw_trans_rle_sprite(display, RLE_ccircle_2cols [0] [size], (mship_x>>10)-size, (mship_y>>10) + 25 + (size*2));

   size /= 2;
   if (size > 0)
    draw_trans_rle_sprite(display, RLE_ccircle_2cols [0] [size], (mship_x>>10)-size, (mship_y>>10) + 25 + (size*7));

//   draw_trans_rle_sprite(display, RLE_ccircle_basic [0] [size], x - size - 1, y - size - 1);

// may have returned before here.

}


