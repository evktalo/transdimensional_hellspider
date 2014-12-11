#include "config.h"

#include "allegro.h"

#include <string.h>

#include "sound.h"

#include "globvars.h"

#include "math.h"
#include "stuff.h"

// # is octave
enum
{
NOTE_0C,
NOTE_0CS,
NOTE_0D,
NOTE_0DS,
NOTE_0E,
NOTE_0F,
NOTE_0FS,
NOTE_0G,
NOTE_0GS,
NOTE_0A,
NOTE_0AS,
NOTE_0B,
NOTE_1C,
NOTE_1CS,
NOTE_1D,
NOTE_1DS,
NOTE_1E,
NOTE_1F,
NOTE_1FS,
NOTE_1G,
NOTE_1GS,
NOTE_1A,
NOTE_1AS,
NOTE_1B,
NOTE_2C,
NOTE_2CS,
NOTE_2D,
NOTE_2DS,
NOTE_2E,
NOTE_2F,
NOTE_2FS,
NOTE_2G,
NOTE_2GS,
NOTE_2A,
NOTE_2AS,
NOTE_2B,
NOTE_3C,
NOTE_3CS,
NOTE_3D,
NOTE_3DS,
NOTE_3E,
NOTE_3F,
NOTE_3FS,
NOTE_3G,
NOTE_3GS,
NOTE_3A,
NOTE_3AS,
NOTE_3B,
NOTE_4C,
NOTE_4CS,
NOTE_4D,
NOTE_4DS,
NOTE_4E,
NOTE_4F,
NOTE_4FS,
NOTE_4G,
NOTE_4GS,
NOTE_4A,
NOTE_4AS,
NOTE_4B,
NOTE_ENDNOTE

};


#define NO_TONES 57
#define BASE_TONE 200
int tone [NOTE_ENDNOTE];

SAMPLE *sounds [NO_WAVS];

void load_sample_in(int samp, const char *sfile);
void new_voice_wvfxy_xs_ys(int sample, int vol, int freq, int x, int y, int x_speed, int y_speed);
int fix_freq(int new_freq);
int position_pan(int x, int y);
int position_vol(int x, int y, int base_vol);
int doppler_shift(int x, int y, int xs, int ys, int base_freq);

/*
Call at startup of program.
*/
void init_sound(void)
{

// first we'll populate the tone array with the right frequencies:
   int i, j;
   float t;
   for (i = 0; i < NOTE_ENDNOTE; i ++)
   {
    t = (float) BASE_TONE;
    for (j = 0; j < i; j ++)
    {
     t *= (float) 1.059463094359;//(1000 + (1000 / 18)) / 1000;
    }
    tone [i] = t;
    // saves me from having to remember how to use the pow function
   }

   reserve_voices(16, 0); // not sure this is necessary
   if (install_sound (DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == -1)
   {
    exit(1); // don't bother with proper error handling yet. This should just disable sound.
   }
   set_volume(255, 0);

   load_sample_in(WAV_FIRE, "fire");
//   load_sample_in(WAV_BLOP, "blop");
   load_sample_in(WAV_BANG1, "bang1");
//   load_sample_in(WAV_BANG2, "bang2");
   load_sample_in(WAV_BANG3, "bang3");
   load_sample_in(WAV_BANG4, "bang4");
//   load_sample_in(WAV_BANG5, "bang5");
   load_sample_in(WAV_MGUN, "mgun");
   load_sample_in(WAV_DZAP, "dzap");
   load_sample_in(WAV_PHASEY, "phasey");
   load_sample_in(WAV_WHOOSH, "whoosh");
   load_sample_in(WAV_HIGHZAP, "highzap");
   load_sample_in(WAV_BURST, "burst");
   load_sample_in(WAV_HIT, "hit");
   load_sample_in(WAV_ROTARY, "rotary");
   load_sample_in(WAV_WORMS, "worms");
   load_sample_in(WAV_MINE, "mine");
   load_sample_in(WAV_BEAM, "beam");
   load_sample_in(WAV_WORM2, "worm2");
   load_sample_in(WAV_BASIC, "basic");
   load_sample_in(WAV_BEAM1, "beam1");
   load_sample_in(WAV_SHIELD, "shield");
   load_sample_in(WAV_KIL, "kil");
   load_sample_in(WAV_BLIP, "blip");
   load_sample_in(WAV_SPAWNA, "spawnA"); // chaos mouth
//   load_sample_in(WAV_CLICKY, "clicky"); // hs basic
   load_sample_in(WAV_FLYS, "flys");
   load_sample_in(WAV_SELECT0, "select0");
   load_sample_in(WAV_SELECT1, "select1");
   load_sample_in(WAV_3CANNON, "3cannon");
   load_sample_in(WAV_DRIVE, "drive");

//   load_sample_in(BEAT_CHORDA, "beat//chordA");
//   load_sample_in(BEAT_GETA, "beat//getA");
//   load_sample_in(BEAT_GETB, "beat//getB");
//   load_sample_in(BEAT_DRONEA, "beat//droneA");
   load_sample_in(BEAT_RADARJAM, "beat//radarJam");
//   load_sample_in(BEAT_SHOTA, "beat//shotA");
   load_sample_in(BEAT_NOISEA, "beat//noiseA");
   load_sample_in(BEAT_NOISEB, "beat//noiseB");
   load_sample_in(BEAT_AMBIA, "beat//ambiA");

//   load_sample_in(BEAT_CYMBAL1, "beat//cymbal1");
//   load_sample_in(BEAT_DRUM2, "beat//drum2");
//   load_sample_in(BEAT_DRUM4, "beat//drum4");
//   load_sample_in(BEAT_FLUTE, "beat//flute");
//   load_sample_in(BEAT_FLUTE2_L, "beat//flute2_l");
   //load_sample_in(BEAT_FLUTE2_S, "beat//flute2_s");
   //load_sample_in(BEAT_FLUTE_L, "beat//flute_l");
//   load_sample_in(BEAT_WALKER, "beat//walker");
//   load_sample_in(BEAT_THING4, "beat//thing4");
   //load_sample_in(BEAT_SYNTH, "beat//synth");
//   load_sample_in(BEAT_SQUARE, "beat//square");

   load_sample_in(BEAT_WAH1, "beat//wah1");
   load_sample_in(BEAT_WAH2, "beat//wah2");
   load_sample_in(BEAT_WEIRD, "beat//weird");
   load_sample_in(BEAT_SPARKLE, "beat//sparkle");
//   load_sample_in(BEAT_MILLIPEDE, "beat//millipede");
//   load_sample_in(BEAT_TODDLE, "beat//toddle");

//   load_sample_in(BEAT_WARM2, "beat//warm2");
//   load_sample_in(BEAT_WARM3, "beat//warm3");
   load_sample_in(BEAT_WARM4, "beat//warm4");
   load_sample_in(BEAT_TOLL, "beat//toll");
   load_sample_in(BEAT_LINE1, "beat//line1");
   load_sample_in(BEAT_LINE2, "beat//line2");
   load_sample_in(BEAT_LINE3, "beat//line3");

   load_sample_in(BEAT_AMBI, "beat//AmbiLoo");
   load_sample_in(BEAT_AMBI2, "beat//AmbiLoo2");
//   load_sample_in(BEAT_LOOPA, "beat//LoopA");

   load_sample_in(BEAT_DRONG, "beat//drong");
   load_sample_in(BEAT_DRONG2, "beat//drong2");
   load_sample_in(BEAT_DRONG3, "beat//drong3");
   load_sample_in(BEAT_ULTRA, "beat//ultra");

   load_sample_in(BEAT_NASICA, "beat//nasica");
//   load_sample_in(BEAT_LOOPA2, "beat//LoopA2");
   load_sample_in(BEAT_ORGAN, "beat//organ");
   load_sample_in(BEAT_GATE1, "beat//gate1");
   load_sample_in(BEAT_GATE2, "beat//gate2");
   load_sample_in(BEAT_GATE3, "beat//gate3");

//   load_sample_in(BEAT_, "beat//");

// to add a wave file, add a new WAV_??? entry to sound.h and add a line here for it. The sound is now ready!

}


/*
Loads a .wav file. Should be in the .wavs subdirectory
*/
void load_sample_in(int samp, const char *sfile)
{

 char sfile_name [70];

 strcpy(sfile_name, ".//wavs//");
 strcat(sfile_name, sfile);
 strcat(sfile_name, ".wav");

 sounds [samp] = load_sample(sfile_name);

 if (sounds [samp] == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error: Unable to load sound file: %s", sfile_name);
      exit(1);
 }
}

/*
Pass e.g. WAV_FIRE to this and it plays it
*/
void play_effect(int sample)
{

// play_sample(sounds [sample], 200, 127, 1000, 0);
 new_voice_wvfxy_xs_ys(sample, 200, 1000, 0, 0, 0, 0);

}

void play_basicwfv(int sample, int f, int v)
{

    play_sample(sounds [sample], v, 127, f, 0);

}

void play_effectwfv(int sample, int f, int v)
{

// play_sample(sounds [sample], v, 127, f, 0);

 v *= options.sfx_volume;
 v /= 100;

 new_voice_wvfxy_xs_ys(sample, v, f, 0, 0, 0, 0);


}

/*
plays sample at frequency f (normal is 1000), volume v (255 is max), x position x (middle is 127)
*/
void play_effectwfvx(int sample, int f, int v, int x)
{

// int pan = x / 2500; // this gives us a # from 0 to 255, from the left of the screen to the right. For stereo

// play_sample(sounds [sample], v, pan, f, 0);

// new_voice_wvfxy_xs_ys(sample, v, f, x, 0, 0, 0);

}


void play_effectwfvxy(int sample, int f, int v, int x, int y)
{

// int pan = 127;//x / 2500; // this gives us a # from 0 to 255, from the left of the screen to the right. For stereo

// play_sample(sounds [sample], v, pan, f, 0);

 v *= options.sfx_volume;
 v /= 100;

 new_voice_wvfxy_xs_ys(sample, v, f, x, y, 0, 0);

}

void play_effectwfvxy_xs_ys(int sample, int f, int v, int x, int y, int xs, int ys)
{

// int pan = 127;//x / 2500; // this gives us a # from 0 to 255, from the left of the screen to the right. For stereo

// play_sample(sounds [sample], v, pan, f, 0);

 v *= options.sfx_volume;
 v /= 100;

 new_voice_wvfxy_xs_ys(sample, v, f, x, y, xs, ys);

}


#define VOICES 25

int voice_in_use [VOICES];
int voice_freq [VOICES];
int voice_vol [VOICES];
int voice_x [VOICES];
int voice_y [VOICES];
int voice_x_speed [VOICES];
int voice_y_speed [VOICES];
int voice_index [VOICES];
int voice_player_location [VOICES];

void init_voices(void)
{
 int v;

 for (v = 0; v < VOICES; v ++)
 {
  voice_in_use [v] = 0;
 }

}

void new_voice_wvfxy_xs_ys(int sample, int vol, int freq, int x, int y, int x_speed, int y_speed)
{

 int v;

// if (sample != WAV_PHASEY)
//  return;

 for (v = 0; v < VOICES; v ++)
 {
  if (voice_in_use [v] == 0)
   break;
  if (v == VOICES - 1)
   return; // oh well, you're not hearing that sound
 }

 voice_index [v] = allocate_voice(sounds [sample]); //play_sample(sounds [sample], vol, 127, freq, 0);

 if (voice_index [v] == -1)
  return;

 voice_x [v] = x;
 voice_y [v] = y;
 voice_x_speed [v] = x_speed;
 voice_y_speed [v] = y_speed;
 voice_freq [v] = freq;
 voice_vol [v] = vol;
 if (x == 0 && y == 0 && x_speed == 0)
 {
  voice_player_location [v] = 1;
  voice_set_volume(voice_index [v], vol);
  voice_set_frequency(voice_index [v], fix_freq(freq));
  voice_set_pan(voice_index [v], 127);
 }
   else
   {
    voice_player_location [v] = 0;
    voice_set_volume(voice_index [v], position_vol(x, y, vol));
    voice_set_frequency(voice_index [v], fix_freq(doppler_shift(x, y, x_speed, y_speed, freq)));
    voice_set_pan(voice_index [v], position_pan(x, y));
   }


 voice_in_use [v] = 1;

 voice_start(voice_index [v]);

}

int position_pan(int x, int y)
{

 float angle = atan2(player[0].y - y, player[0].x - x) - angle_to_radians(player[0].angle);

 return 127 - fypart(angle, 127);
}

int position_vol(int x, int y, int base_vol)
{

 int dist = hypot(player[0].y - y, player[0].x - x);

 dist /= 8000;

 int vol = base_vol - dist;

 if (vol < 0)
  return 0;

 return vol;

}

int doppler_shift(int x, int y, int xs, int ys, int base_freq)
{

 int speed = hypot(player[0].y_speed - ys, player[0].x_speed - xs);
/*
 int x1 = (player[0].x - x);
 int y1 = (player[0].y - y);

// unsigned int dist1 = (x1 * x1) + (y1 * y1);
 unsigned int dist1 = hypot(y1, x1);//(x1 * x1) + (y1 * y1);
// if ((x < 0) ^ (y < 0))
//  dist1 *= -1;

 x1 = x1 + ((xs - player[0].x_speed));
 y1 = y1 + ((ys - player[0].y_speed));

 unsigned int dist2 = hypot(y1, x1);//((x1 + xs - player[0].x_speed) * (x1 + xs - player[0].x_speed)) + ((y1 + ys - player[0].y_speed) * (y1 + ys - player[0].y_speed));
*/
// if ((x+xs < 0) ^ (y+ys < 0))
//  dist2 *= -1;

// speed /= 40;

 float position_angle = atan2(player[0].y - y, player[0].x - x);
 float travel_angle = atan2(ys - player[0].y_speed, xs - player[0].x_speed);
 int angle_difference = radians_to_angle(position_angle - travel_angle) & ANGLE_MASK;

 int shift = xpart(angle_difference, speed / 70);

 int new_freq = base_freq;

/* if (dist1 < dist2)
  new_freq += shift;
   else
    new_freq -= shift;*/

 new_freq += shift;

/* part[50].x = shift;
 part[50].y = base_freq;
 part[50].old_x = dist1 / 1000;
 part[50].old_y = dist2 / 1000;
 part[50].hp = new_freq;
*/

 if (new_freq < 200)
  return 200;

 return new_freq;

}

int fix_freq(int new_freq)
{

 return (44100 * new_freq) / 1000;

}

void run_voices(void)
{

 int v;

 for (v = 0; v < VOICES; v ++)
 {
  if (voice_in_use [v] == 0)
   continue;

  if (voice_get_position(voice_index [v]) == -1)
  {
   deallocate_voice(voice_index [v]);
   voice_in_use [v] = 0;
   continue;
  }

  voice_x [v] += voice_x_speed [v];
  voice_y [v] += voice_y_speed [v];
  if (!voice_player_location [v])
  {
   voice_set_pan(voice_index [v], position_pan(voice_x [v], voice_y [v]));
   voice_set_volume(voice_index [v], position_vol(voice_x [v], voice_y [v], voice_vol [v]));
   voice_set_frequency(voice_index [v], fix_freq(doppler_shift(voice_x [v], voice_y [v], voice_x_speed [v], voice_y_speed [v], voice_freq [v])));
  }

 }

}

/*

****************************************************************************************************************


"music"


****************************************************************************************************************

*/

#define L0 128
#define L1 64
#define L2 32
#define L3 21
#define L4 16
#define L6 11
#define L8 8
#define L12 6
#define L16 4
#define L32 2
#define L64 1
// avoid using 3 and 6 as they're not exact!

#define NO_TRACKS 5

int track_phrase [NO_TRACKS];
int track_pos [NO_TRACKS];
int track_wait [NO_TRACKS];

//int reel_repeats [REELS_PLAYED];
//int max_length;

int get_tone(int base);
void track_finished(void);

enum
{
PHRASE_1_0,
PHRASE_1_1,
PHRASE_1_2,
PHRASE_1_3,
PHRASE_1_4,

PHRASE_2_1,
PHRASE_2_2,
PHRASE_2_3,
PHRASE_2_4,
PHRASE_2_5,
PHRASE_2_6,

PHRASE_3_1,
PHRASE_3_2,
PHRASE_3_3,

PHRASE_4_1,

PHRASE_5_1,
PHRASE_5_2,
PHRASE_5_3,
PHRASE_5_4,
PHRASE_5_5,
PHRASE_5_6,

PHRASE_6_1,
PHRASE_6_2,
PHRASE_6_3,

PHRASE_7_1,
//PHRASE_7_2,
//PHRASE_7_3,
//PHRASE_7_4,

PHRASE_8_1,

PHRASE_EMPTY,
PHRASE_END_SONG,
NO_PHRASES

};

#define PHRASE_LENGTH 40

enum
{
PC_BEAT1,
PC_BEAT2,
PC_NOTE1,
PC_NOTE2,
PC_LENGTH,
PC_VOL,
PHRASE_CONTENTS
};

int phrase [NO_PHRASES] [PHRASE_LENGTH] [PHRASE_CONTENTS] =
{
 {
  {BEAT_NOISEA, -1, NOTE_2C, NOTE_3C, L2 + L4, 100},
  {BEAT_NOISEA, -1, NOTE_1C, NOTE_3C, L2 + L4, 100},
  {BEAT_FINISHED}
 },
 {
  {BEAT_NOISEA, -1, NOTE_2C, NOTE_3C, L2, 100},
  {BEAT_NOISEB, -1, NOTE_2C, NOTE_3C, L4, 100},
  {BEAT_NOISEA, -1, NOTE_1C, NOTE_3C, L2, 100},
  {BEAT_NOISEB, -1, NOTE_1C, NOTE_3C, L4, 100},
  {BEAT_FINISHED}
 },
 {
//  {BEAT_RADARJAM, -1, NOTE_2C, NOTE_3C, L2, 100},
  {BEAT_RADARJAM, -1, NOTE_1C, -1, L2, 150},
  {BEAT_RADARJAM, -1, NOTE_2C, -1, L4, 150},
  {BEAT_RADARJAM, -1, NOTE_1G, -1, L2, 150},
  {BEAT_RADARJAM, -1, NOTE_2C, -1, L2, 150},
  {BEAT_RADARJAM, -1, NOTE_1E, -1, L4, 150},
  {BEAT_FINISHED}

 },
 {
  {BEAT_AMBIA, -1, NOTE_3C, -1, L2+L4+L2+L4 + L2+L4+L2+L4, 150},
  {BEAT_FINISHED}
 },
 {
  {BEAT_AMBIA, -1, NOTE_2C, -1, L2+L4+L2+L4 + L2+L4+L2+L4, 150},
  {BEAT_FINISHED}
 },

// 2_1
 {
  {BEAT_SPARKLE, -1, NOTE_2G, -1, L0, 250},
  {BEAT_SPARKLE, -1, NOTE_2G, -1, L0, 250},
  {BEAT_SPARKLE, -1, NOTE_2G, -1, L0, 250},
  {BEAT_SPARKLE, -1, NOTE_2G, -1, L0, 250},
  {BEAT_FINISHED}
 },
// 2_2
 {
  {BEAT_WEIRD, -1, NOTE_1G, -1, L0+L0, 250},
  {BEAT_WEIRD, -1, NOTE_1G, -1, L0+L0, 250},
  {BEAT_WEIRD, -1, NOTE_1G, -1, L0+L0, 250},
  {BEAT_WEIRD, -1, NOTE_1G, -1, L0+L0, 250},
  {BEAT_FINISHED}

 },
 {
  {BEAT_AMBIA, -1, NOTE_1C, -1, L0, 70},
  {BEAT_AMBIA, -1, NOTE_1G, -1, L0, 70},
  {BEAT_AMBIA, -1, NOTE_1B, -1, L0, 70},
  {BEAT_FINISHED}

 },
 {
  {BEAT_AMBIA, -1, NOTE_0G, -1, L0, 70},
  {BEAT_AMBIA, -1, NOTE_1G, -1, L0, 70},
  {BEAT_AMBIA, -1, NOTE_1A, -1, L0, 70},
  {BEAT_FINISHED}

 },
 {
  {BEAT_EMPTY, -1, NOTE_3C, -1, L16, 170},
  {BEAT_WARM4, -1, NOTE_3C, -1, L2, 170},
  {BEAT_WARM4, -1, NOTE_3G, -1, L2, 160},
  {BEAT_WARM4, -1, NOTE_3C, -1, L1, 150},

  {BEAT_WARM4, -1, NOTE_3G, -1, L2, 140},
  {BEAT_WARM4, -1, NOTE_3B, -1, L2, 130},
  {BEAT_WARM4, -1, NOTE_3G, -1, L1, 120},

  {BEAT_WARM4, -1, NOTE_2B, -1, L2, 110},
  {BEAT_WARM4, -1, NOTE_3FS, -1, L2, 100},
  {BEAT_WARM4, -1, NOTE_2B, -1, L1, 90},

  {BEAT_WARM4, -1, NOTE_2B, -1, L2, 80},
  {BEAT_WARM4, -1, NOTE_3FS, -1, L2, 70},
  {BEAT_WARM4, -1, NOTE_3B, -1, L1, 60},

  {BEAT_FINISHED}
 },
 {
  {BEAT_EMPTY, -1, NOTE_3C, -1, L16, 170},
  {BEAT_WARM4, -1, NOTE_2G, -1, L2, 170},
  {BEAT_WARM4, -1, NOTE_2B, -1, L2, 160},
  {BEAT_WARM4, -1, NOTE_2G, -1, L1, 150},

  {BEAT_WARM4, -1, NOTE_3G, -1, L2, 140},
  {BEAT_WARM4, -1, NOTE_3B, -1, L2, 130},
  {BEAT_WARM4, -1, NOTE_3G, -1, L1, 120},

  {BEAT_WARM4, -1, NOTE_2A, -1, L2, 110},
  {BEAT_WARM4, -1, NOTE_3G, -1, L2, 105},
  {BEAT_WARM4, -1, NOTE_2A, -1, L1, 100},

  {BEAT_WARM4, -1, NOTE_2A, -1, L2, 90},
  {BEAT_WARM4, -1, NOTE_3G, -1, L2, 80},
  {BEAT_WARM4, -1, NOTE_3A, -1, L1, 70},

  {BEAT_FINISHED}
 },



 {
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 210},
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_2C, -1, L4, 210},

  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 210},
  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_1G, -1, L4, 210},

  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 210},
  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_1AS, -1, L4, 210},

  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 210},
  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 110},
  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 140},
  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 180},
  {BEAT_WAH2, -1, NOTE_1F, -1, L4, 210},

//  {BEAT_WAH1, -1, NOTE_2C, -1, L0, 210},
//  {BEAT_WAH1, -1, NOTE_2C, -1, L0, 210},
  {BEAT_FINISHED}
 },

#define BEATY1 BEAT_LINE1
#define BEATY2 BEAT_LINE2
#define BEATY3 BEAT_LINE3

 {
  {BEATY1, -1, NOTE_2C, -1, L4, 50},
  {BEATY3, -1, NOTE_3C, -1, L4, 120},
  {BEATY2, -1, NOTE_2G, -1, L4, 50},
  {BEATY3, -1, NOTE_3D, -1, L4, 120},
  {BEATY2, -1, NOTE_2F, -1, L4, 180},
  {BEATY2, -1, NOTE_2G, -1, L4, 70},
  {BEATY1, -1, NOTE_2C, -1, L4, 90},
  {BEATY1, -1, NOTE_1AS, -1, L4, 110},

  {BEATY1, -1, NOTE_2C, -1, L4, 50},
  {BEATY3, -1, NOTE_3C, -1, L4, 120},
  {BEATY2, -1, NOTE_2G, -1, L4, 50},
  {BEATY3, -1, NOTE_3G, -1, L4, 120},
  {BEATY2, -1, NOTE_2F, -1, L4, 180},
  {BEATY1, -1, NOTE_2C, -1, L4, 70},
  {BEATY2, -1, NOTE_2AS, -1, L4, 90},
  {BEATY3, -1, NOTE_3C, -1, L4, 110},

  {BEATY1, -1, NOTE_2G, -1, L4, 50},
  {BEATY3, -1, NOTE_3C, -1, L4, 120},
  {BEATY1, -1, NOTE_2C, -1, L4, 50},
  {BEATY2, -1, NOTE_2F, -1, L4, 120},
  {BEATY2, -1, NOTE_2G, -1, L4, 180},
  {BEATY3, -1, NOTE_3C, -1, L4, 70},
  {BEATY3, -1, NOTE_3DS, -1, L4, 90},
  {BEATY3, -1, NOTE_3C, -1, L4, 110},

  {BEATY3, -1, NOTE_3C, -1, L4, 50},
  {BEATY2, -1, NOTE_2G, -1, L4, 120},
  {BEATY1, -1, NOTE_2C, -1, L4, 50},
  {BEATY3, -1, NOTE_3C, -1, L4, 120},
  {BEATY2, -1, NOTE_2G, -1, L4, 180},
  {BEATY3, -1, NOTE_3G, -1, L4, 70},
  {BEATY3, -1, NOTE_3C, -1, L4, 90},
  {BEATY1, -1, NOTE_2C, -1, L4, 110},

/*  {BEATY1, -1, NOTE_2C, -1, L4, 50},
  {BEATY2, -1, NOTE_3C, -1, L4, 120},
  {BEATY2, -1, NOTE_2AS, -1, L4, 50},
  {BEATY2, -1, NOTE_2G, -1, L4, 120},
  {BEATY1, -1, NOTE_2F, -1, L4, 180},
  {BEATY1, -1, NOTE_2DS, -1, L4, 70},
  {BEATY1, -1, NOTE_2C, -1, L4, 90},
  {BEATY1, -1, NOTE_1AS, -1, L4, 110},

  {BEATY1, -1, NOTE_1G, -1, L4, 50},
  {BEATY2, -1, NOTE_2AS, -1, L4, 120},
  {BEATY2, -1, NOTE_2G, -1, L4, 50},
  {BEATY1, -1, NOTE_2DS, -1, L4, 120},
  {BEATY1, -1, NOTE_2F, -1, L4, 180},
  {BEATY1, -1, NOTE_2DS, -1, L4, 70},
  {BEATY1, -1, NOTE_1G, -1, L4, 90},
  {BEATY1, -1, NOTE_1AS, -1, L4, 110},

  {BEATY1, -1, NOTE_1DS, -1, L4, 50},
  {BEATY1, -1, NOTE_1G, -1, L4, 120},
  {BEATY1, -1, NOTE_1AS, -1, L4, 50},
  {BEATY1, -1, NOTE_2C, -1, L4, 120},
  {BEATY2, -1, NOTE_2G, -1, L4, 180},
  {BEATY1, -1, NOTE_2F, -1, L4, 70},
  {BEATY1, -1, NOTE_2DS, -1, L4, 90},
  {BEATY1, -1, NOTE_1AS, -1, L4, 110},

  {BEATY1, -1, NOTE_1F, -1, L4, 50},
  {BEATY1, -1, NOTE_1G, -1, L4, 120},
  {BEATY1, -1, NOTE_1AS, -1, L4, 50},
  {BEATY1, -1, NOTE_2C, -1, L4, 120},
  {BEATY2, -1, NOTE_2AS, -1, L4, 180},
  {BEATY2, -1, NOTE_2G, -1, L4, 70},
  {BEATY1, -1, NOTE_2F, -1, L4, 90},
  {BEATY1, -1, NOTE_1AS, -1, L4, 110},
*/

  /*  {BEAT_TOLL, -1, NOTE_2C, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1G, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1AS, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1F, -1, L0, 210},

  {BEAT_TOLL, -1, NOTE_2C, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1G, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1AS, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1F, -1, L0, 210},

  {BEAT_TOLL, -1, NOTE_2C, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1G, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1AS, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1F, -1, L0, 210},

  {BEAT_TOLL, -1, NOTE_2C, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1G, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1AS, -1, L0, 210},
  {BEAT_TOLL, -1, NOTE_1F, -1, L0, 210},*/
  {BEAT_FINISHED}

 },
 {
  {BEATY3, -1, NOTE_3C, -1, L4, 50},
  {BEATY2, -1, NOTE_2AS, -1, L4, 120},
  {BEATY2, -1, NOTE_2G, -1, L4, 50},
  {BEATY1, -1, NOTE_2DS, -1, L4, 120},
  {BEATY1, -1, NOTE_2F, -1, L4, 180},
  {BEATY1, -1, NOTE_2DS, -1, L4, 70},
  {BEATY1, -1, NOTE_2C, -1, L4, 90},
  {BEATY1, -1, NOTE_1AS, -1, L4, 110},

  {BEATY1, -1, NOTE_2C, -1, L4, 50},
  {BEATY2, -1, NOTE_2G, -1, L4, 120},
  {BEATY3, -1, NOTE_3C, -1, L4, 50},
  {BEATY3, -1, NOTE_3DS, -1, L4, 120},
  {BEATY3, -1, NOTE_3F, -1, L4, 180},
  {BEATY1, -1, NOTE_2DS, -1, L4, 70},
  {BEATY3, -1, NOTE_3C, -1, L4, 90},
  {BEATY2, -1, NOTE_2G, -1, L4, 110},

  {BEATY1, -1, NOTE_1C, -1, L4, 50},
  {BEATY1, -1, NOTE_1G, -1, L4, 120},
  {BEATY1, -1, NOTE_1AS, -1, L4, 50},
  {BEATY1, -1, NOTE_2C, -1, L4, 120},
  {BEATY1, -1, NOTE_2DS, -1, L4, 180},
  {BEATY2, -1, NOTE_2AS, -1, L4, 70},
  {BEATY3, -1, NOTE_3C, -1, L4, 90},
  {BEATY3, -1, NOTE_3DS, -1, L4, 110},

  {BEATY3, -1, NOTE_3G, -1, L4, 50},
  {BEATY3, -1, NOTE_3C, -1, L4, 120},
  {BEATY2, -1, NOTE_2AS, -1, L4, 50},
  {BEATY1, -1, NOTE_2C, -1, L4, 120},
  {BEATY1, -1, NOTE_2F, -1, L4, 180},
  {BEATY1, -1, NOTE_2DS, -1, L4, 70},
  {BEATY1, -1, NOTE_2C, -1, L4, 90},
  {BEATY1, -1, NOTE_1AS, -1, L4, 110},

  {BEAT_FINISHED}
 },
/*
 {
  {BEAT_FADING, -1, NOTE_2G, -1, L1 + L2, 60},
  {BEAT_FADING, -1, NOTE_2C, -1, L1 + L2, 90},
  {BEAT_FADING, -1, NOTE_1G, -1, L1 + L2, 120},
  {BEAT_FADING, -1, NOTE_2G, -1, L1 + L2, 170},
  {BEAT_FINISHED}
 },
*/
 {
  {BEAT_AMBI, -1, NOTE_2C, -1, L0 + L0 + L0, 60},
  {BEAT_AMBI, -1, NOTE_1A, -1, L0 + L0 + L0, 120},
  {BEAT_AMBI, -1, NOTE_2C, -1, L0 + L0 + L0, 180},
  {BEAT_AMBI2, -1, NOTE_2C, -1, L0 + L0 + L0, 160},
  {BEAT_AMBI, -1, NOTE_1G, -1, L0 + L0 + L0, 60},
  {BEAT_AMBI2, -1, NOTE_2D, -1, L0 + L0 + L0, 120},
  {BEAT_AMBI, -1, NOTE_1A, -1, L0 + L0 + L0, 180},
  {BEAT_AMBI, -1, NOTE_2C, -1, L0 + L0 + L0, 160},
  {BEAT_FINISHED}
 },

 {
  {BEAT_DRONG, -1, NOTE_2C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_1AS, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_1AS, -1, L4, 120},
  {BEAT_FINISHED}
 },

 {
  {BEAT_DRONG, -1, NOTE_1G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_1AS, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2DS, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L4, 120},
  {BEAT_FINISHED}
 },

 {
  {BEAT_DRONG, -1, NOTE_3C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_3G, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3G, -1, L4, 120},
  {BEAT_FINISHED}
 },

 {
  {BEAT_DRONG, -1, NOTE_2A, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_1G, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_1F, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_1G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_1AS, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_1G, -1, L4, 120},
//  {BEAT_DRONG2, -1, NOTE_2C, -1, L4, 120},
  {BEAT_FINISHED}
 },

 {
  {BEAT_EMPTY, -1, NOTE_2G, -1, L8 + L8 + L4, 80},
  {BEAT_DRONG3, -1, NOTE_2G, -1, L8 + L8 + L4, 80},
  {BEAT_DRONG3, -1, NOTE_1G, -1, L8 + L8 + L4 + L8 + L8 + L4, 80},
/*  {BEAT_DRONG3, -1, NOTE_2G, -1, L4 + L8 +L8, 80},
  {BEAT_DRONG3, -1, NOTE_1G, -1, L8 + L8 + L4, 120},
  {BEAT_DRONG3, -1, NOTE_2C, -1, L4 + L8 , 110},
  {BEAT_DRONG3, -1, NOTE_1G, -1, L8 + L8 + L4, 120},*/
  {BEAT_FINISHED}
 },


 {
  {BEAT_DRONG, -1, NOTE_2E, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2E, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2E, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2E, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2E, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2E, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L4, 120},
  {BEAT_FINISHED}
 },

/*

 {
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2AS, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_3G, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_4C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3DS, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_4C, -1, L4, 120},
  {BEAT_FINISHED}
 },


 {
  {BEAT_DRONG, -1, NOTE_1G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_1AS, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2C, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L8, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2DS, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_2G, -1, L4, 120},
  {BEAT_DRONG, -1, NOTE_3C, -1, L4, 120},
  {BEAT_FINISHED}
 },
*/
 {
  {BEAT_NASICA, -1, NOTE_2C, -1, L0, 120},
  {BEAT_NASICA, -1, NOTE_2C, -1, L0, 120},
  {BEAT_NASICA, -1, NOTE_2C, -1, L0, 120},
  {BEAT_NASICA, -1, NOTE_2C, -1, L0, 120},
  {BEAT_FINISHED}
 },

 {
  {BEAT_ORGAN, -1, NOTE_1C, -1, L0 + L0, 120},
  {BEAT_ORGAN, -1, NOTE_1C, -1, L0 + L0, 120},
  {BEAT_ORGAN, -1, NOTE_1C, -1, L0 + L0, 120},
  {BEAT_ORGAN, -1, NOTE_1C, -1, L0 + L0, 120},
  {BEAT_ORGAN, -1, NOTE_1C, -1, L0 + L0, 120},
  {BEAT_ORGAN, -1, NOTE_1C, -1, L0 + L0, 120},
  {BEAT_FINISHED}
 },

 {
  {BEAT_ULTRA, -1, NOTE_2G, -1, L4, 20},
  {BEAT_ULTRA, -1, NOTE_2C, -1, L4, 30},
  {BEAT_ULTRA, -1, NOTE_2A, -1, L4, 40},
  {BEAT_ULTRA, -1, NOTE_2C, -1, L4, 60},
  {BEAT_ULTRA, -1, NOTE_2AS, -1, L4, 80},
  {BEAT_ULTRA, -1, NOTE_2C, -1, L4, 90},
  {BEAT_ULTRA, -1, NOTE_2B, -1, L4, 100},
  {BEAT_ULTRA, -1, NOTE_2CS, -1, L4, 120},
  {BEAT_ULTRA, -1, NOTE_3C,-1 , L4, 120},
  {BEAT_ULTRA, -1, NOTE_2D, -1, L4, 120},
  {BEAT_ULTRA, -1, NOTE_2B, -1, L4, 90},
  {BEAT_ULTRA, -1, NOTE_2DS, -1, L4, 80},
  {BEAT_ULTRA, -1, NOTE_2AS, -1, L4, 60},
  {BEAT_ULTRA, -1, NOTE_2E, -1, L4, 40},
  {BEAT_ULTRA, -1, NOTE_2A, -1, L4, 30},
  {BEAT_ULTRA, -1, NOTE_2F, -1, L4, 20},
/*  {BEAT_ULTRA, -1, NOTE_2C, -1, L4, 10},
  {BEAT_ULTRA, -1, NOTE_2A, -1, L8, 30},
  {BEAT_ULTRA, -1, NOTE_2E, -1, L4, 40},
  {BEAT_ULTRA, -1, NOTE_3C, -1, L8, 60},
  {BEAT_ULTRA, -1, NOTE_2A, -1, L4, 80},
  {BEAT_ULTRA, -1, NOTE_2D, -1, L4, 90},
  {BEAT_ULTRA, -1, NOTE_2E, -1, L4, 100},
  {BEAT_ULTRA, -1, NOTE_2C, -1, L4, 120},
  {BEAT_ULTRA, -1, NOTE_1G, -1, L4, 120},
  {BEAT_ULTRA, -1, NOTE_1A, -1, L4, 120},
  {BEAT_ULTRA, -1, NOTE_2F, -1, L4, 90},
  {BEAT_ULTRA, -1, NOTE_2G, -1, L4, 80},
  {BEAT_ULTRA, -1, NOTE_2C, -1, L4, 60},
  {BEAT_ULTRA, -1, NOTE_2A, -1, L4, 40},
  {BEAT_ULTRA, -1, NOTE_1A, -1, L4, 30},
  {BEAT_ULTRA, -1, NOTE_2C, -1, L4, 10},*/
  {BEAT_FINISHED}
 },


 {
  {BEAT_GATE1, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE1, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE1, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE1, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE2, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE2, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE1, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE1, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE3, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE3, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE2, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_GATE2, -1, NOTE_2C, -1, L1 + L2 + L8 + L16 + L32, 120},
  {BEAT_FINISHED}
 },

 {
  {BEAT_EMPTY, -1, NOTE_2C, -1, L0, 120},
  {BEAT_FINISHED}
 },


};

enum
{
 SONG_1_1,
 SONG_2_1,
 SONG_3_1,
 SONG_4_1,
 SONG_5_1,
 SONG_6_1,
 SONG_7_1,
 SONG_8_1,
 NO_SONGS


};

#define SONG_LENGTH 40
#define SONG_SIZE 5

int which_song;
int song_pos;

int song [NO_SONGS] [SONG_LENGTH] [SONG_SIZE] =
{
 {
  {PHRASE_1_0, PHRASE_EMPTY},
  {PHRASE_1_0, PHRASE_EMPTY},

  {PHRASE_1_1, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_EMPTY},

  {PHRASE_1_1, PHRASE_1_2, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_2, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_2, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_2, PHRASE_EMPTY},

  {PHRASE_1_1, PHRASE_1_3, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_3, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_3, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_3, PHRASE_EMPTY},

  {PHRASE_1_1, PHRASE_1_2, PHRASE_1_3, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_2, PHRASE_1_3, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_2, PHRASE_1_3, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_2, PHRASE_1_3, PHRASE_EMPTY},

  {PHRASE_1_1, PHRASE_1_4, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_4, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_4, PHRASE_EMPTY},
  {PHRASE_1_1, PHRASE_1_4, PHRASE_EMPTY},

  {PHRASE_1_4, PHRASE_EMPTY},
  {PHRASE_1_4, PHRASE_EMPTY},
  {PHRASE_1_4, PHRASE_EMPTY},
  {PHRASE_1_4, PHRASE_EMPTY},

  {PHRASE_END_SONG}
 },

 {
  {PHRASE_2_1, PHRASE_EMPTY},
  {PHRASE_2_1, PHRASE_2_2, PHRASE_EMPTY},
  {PHRASE_2_1, PHRASE_2_2, PHRASE_2_3, PHRASE_EMPTY},
  {PHRASE_2_1, PHRASE_2_2, PHRASE_2_4, PHRASE_EMPTY},
  {PHRASE_2_1, PHRASE_2_2, PHRASE_2_3, PHRASE_2_5, PHRASE_EMPTY},
  {PHRASE_2_1, PHRASE_2_2, PHRASE_2_4, PHRASE_2_6, PHRASE_EMPTY},
  {PHRASE_2_1, PHRASE_2_2, PHRASE_2_3, PHRASE_2_5, PHRASE_EMPTY},
  {PHRASE_2_1, PHRASE_2_2, PHRASE_2_4, PHRASE_2_6, PHRASE_EMPTY},
  {PHRASE_END_SONG}
 },

 {
  {PHRASE_3_2, PHRASE_EMPTY},
  {PHRASE_3_1, PHRASE_EMPTY, PHRASE_EMPTY},
  {PHRASE_3_1, PHRASE_3_2, PHRASE_EMPTY},
  {PHRASE_3_1, PHRASE_3_3, PHRASE_EMPTY},
//  {PHRASE_3_1, PHRASE_EMPTY},
  {PHRASE_END_SONG}

 },

 {
  {PHRASE_4_1, PHRASE_EMPTY},
  {PHRASE_END_SONG}

 },

 {
  {PHRASE_5_1, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_2, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_2, PHRASE_EMPTY},
  {PHRASE_5_3, PHRASE_5_2, PHRASE_EMPTY},
  {PHRASE_5_3, PHRASE_5_2, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_4, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_4, PHRASE_EMPTY},
  {PHRASE_5_6, PHRASE_5_4, PHRASE_EMPTY},
  {PHRASE_5_6, PHRASE_5_4, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_4, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_2, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_5_4, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_5_4, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_6, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_5_1, PHRASE_5_6, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_5_2, PHRASE_5_4, PHRASE_EMPTY},
  {PHRASE_5_2, PHRASE_5_4, PHRASE_EMPTY},
  {PHRASE_5_4, PHRASE_5_6, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_5_4, PHRASE_5_6, PHRASE_5_5, PHRASE_EMPTY},
  {PHRASE_END_SONG}

 },

 {
  {PHRASE_6_1, PHRASE_EMPTY},
  {PHRASE_6_1, PHRASE_6_2, PHRASE_EMPTY},
  {PHRASE_6_1, PHRASE_6_2, PHRASE_6_3, PHRASE_EMPTY},
  {PHRASE_6_1, PHRASE_6_2, PHRASE_6_3, PHRASE_EMPTY},
  {PHRASE_END_SONG}

 },

 {
  {PHRASE_7_1, PHRASE_EMPTY},
//  {PHRASE_7_1, PHRASE_7_2, PHRASE_EMPTY},
  {PHRASE_END_SONG}

 },

 {
  {PHRASE_8_1, PHRASE_EMPTY},
//  {PHRASE_7_1, PHRASE_7_2, PHRASE_EMPTY},
  {PHRASE_END_SONG}

 },

};


void init_beats(void)
{

// int i;




}

void build_tracks(void)
{

 int i;

 which_song = SONG_1_1;

 switch((int) ((hs[0].stage-1) / 3))
 {
  case 0: which_song = SONG_3_1; break;
  case 1: which_song = SONG_1_1; break;
  case 2: which_song = SONG_2_1; break;
  case 3: which_song = SONG_5_1; break;
  case 4: which_song = SONG_7_1; break;
  case 5: which_song = SONG_8_1; break;
  case 6: which_song = SONG_6_1; break;
  case 7: which_song = SONG_4_1; break;
//  case 8: which_song = SONG_1_1; break;
 }
/*
1 is clicky + radarjam
2 is sparkle
3 is melody + wah wah
4 is ambiloo
5 is contrapuntal drong melody
6 is haunting organ + nasica
7 is gate
8 is silence
*/

 song_pos = 0;

 int finished = 0;

 for (i = 0; i < NO_TRACKS; i ++)
 {
   track_pos [i] = 0;
   if (finished == 0)
   {
    track_phrase [i] = song [which_song] [song_pos] [i];
    if (track_phrase [i] == PHRASE_EMPTY)
    {
     track_wait [i] = -1;
     finished = 1;
    }
     else
      track_wait [i] = 1;
   }
    else
    {
     track_phrase [i] = PHRASE_EMPTY;
     track_wait [i] = -1;
    }
 }



}

int get_tone(int base)
{

 if (grand(6) == 0)
  return base + pos_or_neg(4);

 if (grand(6) == 0)
  return base + pos_or_neg(5);

 if (grand(6) == 0)
  return base + pos_or_neg(7);

 if (grand(8) == 0)
  return base + pos_or_neg(9);

 if (grand(10) == 0)
  return base + pos_or_neg(1);

 return base;

}

void play_tracks(void)
{
 int i, j;

 for (i = 0; i < NO_TRACKS; i ++)
 {
  if (track_phrase [i] == PHRASE_EMPTY)
   continue;
  track_wait [i] --;
  if (track_wait [i] == 0)
  {
   if (phrase [track_phrase [i]] [track_pos [i]] [0] == BEAT_FINISHED)
   {
    if (i == 0)
    {
     track_finished();
     return;
    }
      else
       track_wait [i] = -1;
   }
    else
    {
     if (phrase [track_phrase [i]] [track_pos [i]] [PC_BEAT1] != BEAT_EMPTY)
     {
//      play_effectwfv();
      new_voice_wvfxy_xs_ys(phrase [track_phrase [i]] [track_pos [i]] [PC_BEAT1],
       (phrase [track_phrase [i]] [track_pos [i]] [PC_VOL] * options.sfx_volume) / 100,
       tone [phrase [track_phrase [i]] [track_pos [i]] [PC_NOTE1]],
       0, 0, 0, 0);

     }
     track_wait [i] = phrase [track_phrase [i]] [track_pos [i]] [PC_LENGTH];
     track_pos [i] ++;
    }
  }


 }


}

void track_finished(void)
{


 song_pos ++;

 if (song [which_song] [song_pos] [0] == PHRASE_END_SONG)
  song_pos = 0;

 int i, finished = 0;

 for (i = 0; i < NO_TRACKS; i ++)
 {
   track_pos [i] = 0;
   if (finished == 0)
   {
    track_phrase [i] = song [which_song] [song_pos] [i];
    if (track_phrase [i] == PHRASE_EMPTY)
    {
     track_wait [i] = -1;
     finished = 1;
    }
     else
      track_wait [i] = 1;
   }
    else
    {
     track_phrase [i] = PHRASE_EMPTY;
     track_wait [i] = -1;
    }
 }

}

