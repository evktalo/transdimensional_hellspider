
#include "config.h"

#include "allegro.h"

#include "globvars.h"

#include "stuff.h"

#include "hs_build.h"
#include "pbullet.h"
#include "ebullet.h"


void init_level(void)
{

// int i;

 init_pbullets();
 init_ebullets();

 arena.end_stage = 0;

 init_hs_at_stage_1(0);

}

