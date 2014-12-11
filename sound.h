
void init_sound(void);

void play_basicwfv(int sample, int f, int v);
void play_effect(int sample);
void play_effectwfv(int sample, int f, int v);
void play_effectwfvx(int sample, int f, int v, int x);
void play_effectwfvxy(int sample, int f, int v, int x, int y);
void play_effectwfvxy_xs_ys(int sample, int f, int v, int x, int y, int xs, int ys);
void run_voices(void);
void init_voices(void);

void init_beats(void);
void build_tracks(void);
void play_tracks(void);


enum
{
WAV_NONE,
WAV_FIRE,
//WAV_BLOP,
WAV_BANG1,
//WAV_BANG2,
WAV_BANG3,
WAV_BANG4,
//WAV_BANG5,
WAV_MGUN,
WAV_DZAP,
WAV_PHASEY,
WAV_WHOOSH,
WAV_HIGHZAP,
WAV_BURST,
WAV_HIT,
WAV_ROTARY,
WAV_WORMS,
WAV_MINE,
WAV_BEAM,
WAV_WORM2,
WAV_BASIC,
WAV_BEAM1,
WAV_SHIELD,
WAV_KIL,
WAV_BLIP,
WAV_SPAWNA,
//WAV_CLICKY,
WAV_FLYS,
WAV_SELECT0,
WAV_SELECT1,
WAV_3CANNON,
WAV_DRIVE,

//BEAT_CHORDA,
//BEAT_GETA,
//BEAT_GETB,
//BEAT_DRONEA,
BEAT_RADARJAM,
//BEAT_SHOTA,
BEAT_AMBIA,

/*BEAT_CYMBAL1,
BEAT_DRUM2,
BEAT_DRUM4,
BEAT_FLUTE,
BEAT_FLUTE2_L,
BEAT_FLUTE2_S,
BEAT_FLUTE_L,
BEAT_WALKER,
BEAT_THING4,
BEAT_SYNTH,
BEAT_SQUARE,*/
BEAT_NOISEA,
BEAT_NOISEB,

BEAT_WAH1,
BEAT_WAH2,
BEAT_WEIRD,
//BEAT_MILLIPEDE,
BEAT_SPARKLE,
//BEAT_TODDLE,
//BEAT_WARM2,
//BEAT_WARM3,
BEAT_WARM4,
BEAT_TOLL,
BEAT_LINE1,
BEAT_LINE2,
BEAT_LINE3,
BEAT_AMBI,
BEAT_AMBI2,
//BEAT_LOOPA,
BEAT_DRONG,
BEAT_DRONG2,
BEAT_DRONG3,
BEAT_ULTRA,

BEAT_NASICA,
//BEAT_LOOPA2,
BEAT_ORGAN,
BEAT_STRINGY,
BEAT_GATE1,
BEAT_GATE2,
BEAT_GATE3,


BEAT_EMPTY,
BEAT_FINISHED,
NO_WAVS
};

