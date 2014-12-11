// Global variables:
//  at the moment these are all defined in main.c.
//  but they have to be externed here so all modules with #include globvars.h can access them.

extern struct playerstruct player [2];

extern struct arenastruct arena;

extern struct ebulletstruct ebullet [NO_EBULLETS];

extern unsigned char counter;

extern struct cloudstruct cloud [NO_CLOUDS];

extern struct pbulletstruct pbullet [NO_PBULLETS];

extern struct starstruct star [NO_STARS];

extern struct hsstruct hs [1];

extern struct optionstruct options;

extern struct partstruct part [NO_PARTS];

extern struct partstruct map [NO_PARTS];

extern int glob;
