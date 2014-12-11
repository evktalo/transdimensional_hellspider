


#include "config.h"

#include "allegro.h"

//#include <string.h>
#include <math.h>

#include "globvars.h"
#include "stuff.h"
#include "game.h"
#include "display_init.h"
#include "sound.h"
#include "menu.h"

// timer interupt functions and variables:
void framecount(void);

volatile int framecounter;
volatile int frames_per_second;

void tickover(void);

volatile unsigned char ticked;
int slacktime;

void init_at_startup(void);
void begin_game(void);


struct playerstruct player [2];
struct arenastruct arena;
struct starstruct star [NO_STARS];
struct cloudstruct cloud [NO_CLOUDS];
struct hsstruct hs [1];
struct pbulletstruct pbullet [NO_PBULLETS];
struct optionstruct options;
struct partstruct part [NO_PARTS];
struct partstruct map [NO_PARTS];
struct ebulletstruct ebullet [NO_EBULLETS];
int glob;

void framecount(void)
{
   frames_per_second = framecounter;
   framecounter = 0;
}
END_OF_FUNCTION (framecount);

/*
Is called every 25ms; lets the game know that it's time to move on to the next frame.
*/
void tickover(void)
{
 ticked ++;
}
END_OF_FUNCTION (tickover);



int main(void)
{

/*
The following code is for use if your system is set up to prevent an application writing to its own directory.
Normally this will prevent highscores and key configurations being saved, but the following code puts the
initfile in a better place. This may be a default on some systems (eg Unixes and possibly Windows Vista)
and can be set on some other systems.

The only problem with this is that you'll have to manually type or copy the various other options (windowed,
vsync, joystick buttons etc) into the initfile in this new location (or just copy the initfile across, or
just rely on the default values and let the game make a new initfile).

Thanks to Thomas Harte for this code! I had no idea it was necessary (it isn't on my Windows XP computer, and
I haven't tested it).

*/




int allint =  allegro_init();
   if (allint == -1)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Failed to initialise Allegro! This isn't going to work, sorry.");
      exit(1);
   }

//#define UNIX_OSX_VISTA_ETC

#ifdef UNIX_OSX_VISTA_ETC

   {

//     char *HPath = getenv("HOME");

// use this if for some reason you're running Windows Vista:
        char *HPath = getenv("APPDATA");

	 char ConfigPath[2048];

	 sprintf(ConfigPath, "%s/.Hspider", HPath);
	 set_config_file(ConfigPath);

   }
#else
   set_config_file("init.txt");
#endif

   install_keyboard();
   install_timer();

   three_finger_flag = 0;
   key_led_flag = 0;

 init_at_startup();

// now we've initialised, let's play the game!
// game_loop();
 startup_menu();

 return 0;

}

END_OF_MAIN();

/*
Self-explanatory.
*/
void init_at_startup(void)
{

   LOCK_FUNCTION (framecount);
   LOCK_FUNCTION (tickover);
   LOCK_VARIABLE (ticked);
   LOCK_VARIABLE (frames_per_second);
   LOCK_VARIABLE (framecounter);
   LOCK_VARIABLE (turns_per_second);
   LOCK_VARIABLE (turncounter);

   install_int (framecount, 1000);
   install_int (tickover, 20);

   set_color_depth(8);

   int randseed = get_config_int("Misc", "Seed", 0);
   srand(randseed);

   options.windowed = get_config_int("Misc", "Windowed", 0);


 int windowed;
 switch(options.windowed)
 {
    default:
     case 1: windowed = GFX_AUTODETECT_WINDOWED; break;
     case 0: windowed = GFX_AUTODETECT_FULLSCREEN; break;
 }

//   windowed  = GFX_AUTODETECT_WINDOWED;
//   windowed = GFX_AUTODETECT_FULLSCREEN;

//   if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0)
   if (set_gfx_mode(windowed, 640, 480, 0, 0) != 0)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set 640x480 display mode\n%s\n", allegro_error);
      exit(1);
   }

 init_trig();

 prepare_display();
 init_sound();
/*
 player[0].ckey [CKEY_UP] = KEY_UP;
 player[0].ckey [CKEY_DOWN] = KEY_DOWN;
 player[0].ckey [CKEY_LEFT] = KEY_LEFT;
 player[0].ckey [CKEY_RIGHT] = KEY_RIGHT;
 player[0].ckey [CKEY_FIRE1] = KEY_Z;
 player[0].ckey [CKEY_FIRE2] = KEY_X;
 player[0].ckey [CKEY_FIRE3] = KEY_C;
*/

 player[0].ckey [CKEY_UP] = get_config_int("Misc", "key_up", KEY_UP);
 player[0].ckey [CKEY_DOWN] = get_config_int("Misc", "key_down", KEY_DOWN);
 player[0].ckey [CKEY_LEFT] = get_config_int("Misc", "key_left", KEY_LEFT);
 player[0].ckey [CKEY_RIGHT] = get_config_int("Misc", "key_right", KEY_RIGHT);
 player[0].ckey [CKEY_FIRE1] = get_config_int("Misc", "key_fire1", KEY_Z);
 player[0].ckey [CKEY_FIRE2] = get_config_int("Misc", "key_fire2", KEY_X);

 options.joy_stick = get_config_int("Misc", "joy_stick", 0);

 options.joy_sensitivity = get_config_int("Misc", "joy_sensitivity", 70);
 options.init_joystick = get_config_int("Misc", "joy_init", 1);
 options.joystick = 0;
 options.key_or_joy = 0; // don't put in initfile!

 options.sfx_volume = get_config_int("Misc", "sfx_volume", 70);
 options.ambience_volume = get_config_int("Misc", "ambience_volume", 100);
 options.run_vsync = get_config_int("Misc", "run_vsync", 0);

// set_config_int("Misc", "Tourist", 3);
// set_config_int("Misc", "joy_stick", 0);

 if (options.init_joystick)
  init_joystick();

 if (options.joystick == 1) // set in init_joystick
 {
  options.joy_button [0] = get_config_int("Misc", "joy_button_1", 0);
  if (options.joy_button [0] > joy[0].num_buttons)
   options.joy_button [0] = joy[0].num_buttons - 1;
  options.joy_button [1] = get_config_int("Misc", "joy_button_2", 1);
  if (options.joy_button [1] > joy[0].num_buttons)
   options.joy_button [1] = joy[0].num_buttons - 1;
  options.joy_button [2] = get_config_int("Misc", "joy_button_engine", 3);
  if (options.joy_button [2] > joy[0].num_buttons)
   options.joy_button [2] = joy[0].num_buttons - 1;
  options.joy_button [3] = get_config_int("Misc", "joy_button_brake", 2);
  if (options.joy_button [3] > joy[0].num_buttons)
   options.joy_button [3] = joy[0].num_buttons - 1;
 }

 ticked = 0;


}



