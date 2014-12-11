/*

This function is straight out of Excellent Bifurcation.
I haven't fixed it up for this example at all.
Use the colour enum in palette.h.

*/


#include "config.h"

#include "allegro.h"


#include "palette.h"
#include "globvars.h"

#include "stuff.h"

COLOR_MAP trans_table;
COLOR_MAP alpha_table;
//COLOR_MAP trans_table2;

//int blend_function(int base, int trans, RGB *rgbl);
int blend_function(int trans, int base, RGB *rgbl);
int blend_function2(int trans, int base, RGB *rgbl);
int alpha_blend_function(int trans, int base, RGB *rgbl);
int limit_colour(int colour_input);
void halfway_colour(int base, int target, int r, int g, int b);
void setcol(int col, int r, int g, int b);

RGB other_palet [1324];
RGB palet [256];
//RGB palet2 [256];
//RGB light_palet [256];
//RGB dark_palet [256];
RGB other_palet2 [2048];
// what seems to be a weird bug in Allegro forces me to put padding around
//  the palet array, or it becomes corrupted.

/*


IMPORTANT TO REMEMBER:

    I created the palette for the GIMP by saving a 16x16 image with all colours
    as a gif then importing the palette from that. This let me get the colours
    in the right order!


 In the GIMP, must go to image|mode|indexed colour and change the settings there.






*/


void special_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal);
void special_create_color_table2(COLOR_MAP *table, AL_CONST PALETTE pal);
int base_colour(int y);

void colour_table(const char *which_call);





void reset_palette(void)
{
 vsync();
 set_palette(palet);
}

void init_palette(int stage)
{

int i;


 for (i = 0; i < 256; i ++)
 {

     palet [i].r = 0;
     palet [i].g = 0;
     palet [i].b = 0;

  }


 for (i = 1; i < 17; i ++)
 {

     palet [i].r = limit_colour(20 + i * 2);
     palet [i].g = limit_colour(20 + i * 2);
     palet [i].b = limit_colour(20 + i * 2);

 }

  palet[17].r = 60;
  palet[17].g = 20;
  palet[17].b = 60;



/*
  setcol(COL_DEEP1, 19,19,19);
  setcol(COL_DEEP2, 9,9,19);
  setcol(COL_DEEP3, 11,11,23);

  setcol(COL_BACK1, 12,12,12);
  setcol(COL_BACK2, 17,17,19);
  setcol(COL_BACK3, 22,22,24);
  setcol(COL_BACK4, 27,27,29);

  setcol(COL_ENTITY1, 12,28,18);
  setcol(COL_ENTITY2, 14,34,21);
  setcol(COL_ENTITY3, 16,38,23);
  setcol(COL_ENTITY4, 18,42,27);

  setcol(COL_GUARD1, 25,25,26);
  setcol(COL_GUARD2, 29,29,32);
  setcol(COL_GUARD3, 33,33,46);
  setcol(COL_GUARD4, 40,40,50);

  setcol(COL_WILD1, 22,28,18);
  setcol(COL_WILD2, 24,34,21);
  setcol(COL_WILD3, 26,38,23);
  setcol(COL_WILD4, 28,42,27);
*/

/*  setcol(COL_DEEP1, 7,11,24);
  setcol(COL_DEEP2, 17,17,27);
  setcol(COL_DEEP3, 26,26,29);*/


  setcol(COL_BACK, 2,2,2);

  for (i = 0; i < 7; i ++)
  {
//   setcol(COL_STAR1 + i, 5 + i*9,5 + i*9,5 + i*9);
   setcol(COL_STAR1 + i, 12+i*5, 3+i*8, 5+i*6);
   if (stage < 13)
    setcol(COL_STAR1 + i, 12+i*5, 3+i*6, 10+i*5);
   if (stage < 7)
    setcol(COL_STAR1 + i, 3+i*8, 3+i*8, 15+i*6);
  }
/*  setcol(COL_STAR2, 8,8,8);
  setcol(COL_STAR3, 11,11,11);
  setcol(COL_STAR4, 14,14,14);
  setcol(COL_STAR5, 18,18,18);
  setcol(COL_STAR6, 23,23,23);
  setcol(COL_STAR7, 27,27,27);
  setcol(COL_STAR8, 31,31,31);*/


  setcol(COL_OUTLINE, 7,7,7);
  setcol(COL_DGREY, 17,17,17);
  setcol(COL_LGREY, 32,32,32);
  setcol(COL_WHITE, 42,42,42);

/*  setcol(COL_HS_INNER, 9,9,9);
  setcol(COL_HS_INNER_EDGE, 39,29,9);
  setcol(COL_HS_OUTER, 39,9,9);
  setcol(COL_HS_OUTER_EDGE, 42,35,13);*/

int stage_colour [24] [6] [3] =
{
/* {
  {32,20,20}, // inner edge
  {13,13,13}, // inner
  {12,12,12}, // outer edge
  {36,13,13}, // outer
  {25,20,20}, // inner line
  {39,16,16} // hs hurt
 }, // redback*/





 {
  {22,22,32}, // inner edge
  {29,29,39}, // inner
  {12,12,29}, // outer edge
  {25,25,34}, // outer
  {24,24,34}, // inner line
  {33,33,43} // hs hurt
//  {38,38,49} // hs hurt
 }, // light blue
 {
  {32,17,22}, // inner edge
  {39,23,29}, // inner
  {14,12,29}, // outer edge
  {27,25,34}, // outer
  {34,19,24}, // inner line
  {43,28,43} // hs hurt
//  {49,30,38} // hs hurt
 }, // orangey and blue
 {
  {32,17,22}, // inner edge
  {39,23,29}, // inner
  {26,12,15}, // outer edge
  {34,25,27}, // outer
  {34,19,24}, // inner line
  {44,26,32} // hs hurt
//  {49,30,34} // hs hurt
 }, // orangey


 {
  {11,24,11}, // inner edge
  {10,27,10}, // inner
  {9,22,12}, // outer edge
  {20,30,22}, // outer
  {11,21,11}, // inner line
  {13,34,13} // hs hurt
 }, // light green
 {
  {19,29,24}, // inner edge
  {10,27,10}, // inner
  {9,22,12}, // outer edge
  {15,30,16}, // outer
  {22,31,25}, // inner line
  {13,40,18} // hs hurt
 }, // light green bright lines
 {
  {32,20,20}, // inner edge
  {13,13,13}, // inner
  {12,12,12}, // outer edge
  {36,15,13}, // outer
  {25,20,20}, // inner line
  {41,18,16} // hs hurt
 }, // redback

 {
  {25,23,20}, // inner edge
  {38,32,26}, // inner
  {15,21,13}, // outer edge
  {25,33,24}, // outer
  {15,12,9}, // inner line
  {41,35,30} // hs hurt
 }, // lemon lime

 {
  {22,22,32}, // inner edge
  {29,29,37}, // inner
  {20,20,10}, // outer edge
  {31,26,32}, // outer
  {24,24,32}, // inner line
  {37,26,22} // hs hurt
 }, // blue and orange

 {
  {12,12,35}, // inner edge
  {21,21,42}, // inner
  {12,12,29}, // outer edge
  {18,18,32}, // outer
  {17,17,34}, // inner line
  {25,25,42} // hs hurt
 }, // dark blue

 {
  {22,18,29}, // inner edge
  {33,20,35}, // inner
  {14,10,25}, // outer edge
  {26,17,30}, // outer
  {23,19,29}, // inner line
  {36,25,37} // hs hurt
 }, // purple



 {
  {23,23,23}, // inner edge
  {30,30,30}, // inner
  {17,17,17}, // outer edge
  {28,28,28}, // outer
  {12,12,12}, // inner line
  {35,35,35} // hs hurt
 }, // grey

 {
  {25,23,20}, // inner edge
  {35,31,26}, // inner
  {19,17,13}, // outer edge
  {31,28,24}, // outer
  {15,12,9}, // inner line
  {38,35,30} // hs hurt
 }, // gold

 {
  {11,24,16}, // inner edge
  {10,27,20}, // inner
  {9,22,16}, // outer edge
  {12,29,23}, // outer
  {11,21,16}, // inner line
  {13,34,19} // hs hurt
 }, // greyish bluey green

 {
  {32,12,22}, // inner edge
  {37,17,27}, // inner
  {26,9,15}, // outer edge
  {34,19,27}, // outer
  {34,12,24}, // inner line
  {44,21,32} // hs hurt
//  {49,30,34} // hs hurt
 }, // pink

 {
  {32,12,22}, // inner edge
  {37,17,27}, // inner
  {14,10,25}, // outer edge
  {26,17,30}, // outer
  {34,12,24}, // inner line
  {25,25,42} // hs hurt
 }, // purple and pink

 {
  {32,24,20}, // inner edge
  {13,13,13}, // inner
  {12,12,12}, // outer edge
  {36,19,13}, // outer
  {25,22,20}, // inner line
  {41,22,16} // hs hurt
 }, // orangeback


 {
  {21,21,21}, // inner edge
  {29,29,29}, // inner
  {12,12,29}, // outer edge
  {25,25,34}, // outer
  {24,24,34}, // inner line
  {38,38,38} // hs hurt
//  {38,38,49} // hs hurt
 }, // grey and light blue

 {
  {25,23,20}, // inner edge
  {35,31,26}, // inner
  {12,12,29}, // outer edge
  {25,25,34}, // outer
  {24,24,34}, // inner line
  {38,38,38} // hs hurt
//  {38,38,49} // hs hurt
 }, // grey and gold

 {
  {25,9,9}, // inner edge
  {35,11,11}, // inner
  {29,12,12}, // outer edge
  {32,15,15}, // outer
  {24,14,14}, // inner line
  {38,18,18} // hs hurt
//  {38,38,49} // hs hurt
 }, // deeper red?

 {
  {22,25,32}, // inner edge
  {29,32,39}, // inner
  {12,15,29}, // outer edge
  {25,28,34}, // outer
  {24,28,34}, // inner line
  {33,36,43} // hs hurt
//  {38,38,49} // hs hurt
 }, // turquoise?

 {
  {32,14,22}, // inner edge
  {39,19,29}, // inner
  {14,9,29}, // outer edge
  {27,20,34}, // outer
  {34,12,24}, // inner line
  {43,23,43} // hs hurt
//  {49,30,38} // hs hurt
 }, // orangey and blue v2 (less green)

 {
  {13,33,13}, // inner edge
  {5,5,5}, // inner
  {19,32,19}, // outer edge
  {5,9,5}, // outer
  {12,27,14}, // inner line
  {26,26,10} // hs hurt
 }, // reverse green

 {
  {19,22,53}, // inner edge
  {5,5,5}, // inner
  {16,19,55}, // outer edge
  {5,5,9}, // outer
  {12,22,57}, // inner line
  {16,19,60} // hs hurt
 }, // reverse blue

 {
  {33,33,33}, // inner edge
  {5,5,5}, // inner
  {29,29,29}, // outer edge
  {9,9,9}, // outer
  {36,36,36}, // inner line
  {24,24,24} // hs hurt
 }, // reverse grey

// {
//    {0,0,0}
// }
/* {
  {,,,}, // inner edge
  {,,,}, // inner
  {,,,}, // outer edge
  {,,,}, // outer
  {,,,}, // inner line
  {,,,} // hs hurt
 }, */


};

stage --;

int s = stage;

 switch(stage)
 {
  case -2:
  case -1: // startup
  case 0: // menu
   setcol(COL_HS_INNER_EDGE, stage_colour [0] [0] [0], stage_colour [0] [0] [1], stage_colour [0] [0] [2]);
   setcol(COL_HS_INNER, stage_colour [0] [1] [0], stage_colour [0] [1] [1], stage_colour [0] [1] [2]);
   setcol(COL_HS_OUTER_EDGE, stage_colour [0] [2] [0], stage_colour [0] [2] [1], stage_colour [0] [2] [2]);
   setcol(COL_HS_OUTER, stage_colour [0] [3] [0], stage_colour [0] [3] [1], stage_colour [0] [3] [2]);
   setcol(COL_HS_INNER_LINE, stage_colour [0] [4] [0], stage_colour [0] [4] [1], stage_colour [0] [4] [2]);
   setcol(COL_HS_HURT, stage_colour [0] [5] [0], stage_colour [0] [5] [1], stage_colour [0] [5] [2]);
   break;
//  case 1:
  default:
   if (stage_colour [s] [0] [0] == 0)
    s = 0;
   setcol(COL_HS_INNER_EDGE, stage_colour [s] [0] [0], stage_colour [s] [0] [1], stage_colour [s] [0] [2]);
   setcol(COL_HS_INNER, stage_colour [s] [1] [0], stage_colour [s] [1] [1], stage_colour [s] [1] [2]);
   setcol(COL_HS_OUTER_EDGE, stage_colour [s] [2] [0], stage_colour [s] [2] [1], stage_colour [s] [2] [2]);
   setcol(COL_HS_OUTER, stage_colour [s] [3] [0], stage_colour [s] [3] [1], stage_colour [s] [3] [2]);
   setcol(COL_HS_INNER_LINE, stage_colour [s] [4] [0], stage_colour [s] [4] [1], stage_colour [s] [4] [2]);
   setcol(COL_HS_HURT, stage_colour [s] [5] [0], stage_colour [s] [5] [1], stage_colour [s] [5] [2]);
   break;
/*  setcol(COL_HS_INNER_EDGE, 22,22,32);
  setcol(COL_HS_INNER, 29,29,39);
  setcol(COL_HS_OUTER_EDGE, 12,12,29);
  setcol(COL_HS_OUTER, 25,25,34);
  setcol(COL_HS_INNER_LINE, 24,24,34);
  setcol(COL_HS_HURT, 38,38,49);
  break;
  case 2:
   setcol(COL_HS_INNER_EDGE, 32,17,22);
   setcol(COL_HS_INNER, 39,23,29);
   setcol(COL_HS_OUTER_EDGE, 26,12,15);
   setcol(COL_HS_OUTER, 34,25,27);
   setcol(COL_HS_INNER_LINE, 34,19,24);
   setcol(COL_HS_HURT, 49,30,34);
   break;
  case 3:
   setcol(COL_HS_INNER_EDGE, 22,32,22);
   setcol(COL_HS_INNER, 23,39,23);
   setcol(COL_HS_OUTER_EDGE, 12,29,12);
   setcol(COL_HS_OUTER, 24,35,25);
   setcol(COL_HS_INNER_LINE, 22,35,22);
   setcol(COL_HS_HURT, 34,49,34);
   break;

  case 7:
   setcol(COL_HS_INNER_EDGE, 32,17,22);
   setcol(COL_HS_INNER, 39,23,29);
   setcol(COL_HS_OUTER_EDGE, 14,12,29);
   setcol(COL_HS_OUTER, 27,25,34);
   setcol(COL_HS_INNER_LINE, 34,19,24);
   setcol(COL_HS_HURT, 49,30,38);
   break;*/
 }
/*
  setcol(COL_HS_INNER_EDGE, 42,42,42);
  setcol(COL_HS_INNER, 11,11,11);
  setcol(COL_HS_OUTER_EDGE, 40,40,40);
  setcol(COL_HS_OUTER, 9,9,9);
  setcol(COL_HS_INNER_LINE, 44,44,44);
  setcol(COL_HS_HURT, 39,12,12);*/


 setcol(COL_BRIGHT, 55, 40, 10);

 setcol(255, 63, 63, 63);
  palet[0].r = 0;
  palet[0].g = 0;
  palet[0].b = 0;

  int current_base [21] [3];

  for (i = 0; i < 21; i ++)
  {
   current_base [i] [0] = palet[i].r;
   current_base [i] [1] = palet[i].g;
   current_base [i] [2] = palet[i].b;
  }

 int base [3], j;

    for (i = 1; i < 20; i ++)
    {
      for (j = 0; j < 3; j ++)
      {
       base [j] = current_base [i] [j];
//       if (base [j] < 15)
//        base[j] = 15;
       if (base [j] < 6)
        base[j] = 6;
      }

      setcol(TRANS_RED1 + i-1, base [0] + 18, base [1] + 1, base [2] - 7);
      setcol(TRANS_RED2 + i-1, base [0] + 31, base [1] + 5, base [2] / 2);
      setcol(TRANS_RED3 + i-1, base [0] / 3 + 42, base [1] / 3 + 25, base [2] / 4);

      setcol(TRANS_GREEN1 + i-1, base [0] + 1, base [1] + 5, base [2] - 5);
      setcol(TRANS_GREEN2 + i-1, base [0] + 3, base [1] + 10, base [2] / 2);
      setcol(TRANS_GREEN3 + i-1, base [0] / 3 + 30, base [1] / 3 + 30, base [2] / 4 + 10);

      setcol(TRANS_YELLOW4 + i-1, base [0] / 4 + 45, base [1] / 4 + 39, base [2] / 5 + 15);

      setcol(TRANS_BLUE1 + i-1, base [0] + 1, base [1] + 1, base [2] + 15);
      setcol(TRANS_BLUE2 + i-1, base [0] / 2 + 18, base [1] / 2 + 18, base [2] / 2 + 30);
      setcol(TRANS_BLUE3 + i-1, base [0] / 4 + 36, base [1] / 4 + 36, base [2] / 4 + 50);
      setcol(TRANS_BLUE4 + i-1, base [0] / 5 + 51, base [1] / 5 + 51, base [2] / 5 + 65);

    }



// to delete!!!
/* for (i = 1; i < 17; i ++)
 {
   for (j = 0; j < 10; j ++)
   {
     palet [i + j * 16].r = limit_colour(20 + i * 2);
     palet [i + j * 16].g = limit_colour(0 + j * 2);
     palet [i + j * 16].b = limit_colour(0 + j * 2);
   }
 }*/

/*

BITMAP *bmp = create_bitmap(16, 16);
   set_palette(palet);

for (i = 0; i < 16; i ++)
{
 for (j = 0; j < 16; j ++)
 {
     putpixel(bmp, i, j, i + j * 16);
 }

}

save_bitmap("spal.bmp", bmp, palet);

*/

 setcol(WRITE_IN, 55, 55, 55);
 setcol(WRITE_OUT, 20, 20, 20);
   vsync();
   set_palette(palet);

  if (stage == -1)
  {
   special_create_color_table(&trans_table, palet);
   special_create_color_table2(&alpha_table, palet);
  }

   color_map = &trans_table;


}

void setcol(int col, int r, int g, int b)
{
  palet[col].r = limit_colour(r);
  palet[col].g = limit_colour(g);
  palet[col].b = limit_colour(b);


}

int limit_colour(int colour_input)
{

 if (colour_input < 0) return 0;
 if (colour_input > 63) return 63;
 return colour_input;

}

/*
int blend_function(int trans, int base, RGB *rgbl)
{

 if (base <= COLOUR_11)
  return trans + base;

// int low_colour = base % 12;
// int low_trans = trans / 12;
 int trans_strength = (trans / 12 - 1) % 6;

 if (trans == TRANS_PURPLE1)
  trans_strength = 10;
 if (trans == TRANS_PURPLE2)
  trans_strength = 11;

 int base_trans_strength = (base / 12 - 1) % 6;
 if (trans == TRANS_PURPLE1)
  base_trans_strength = 10;
 if (trans == TRANS_PURPLE2)
  base_trans_strength = 11;

 if (trans_strength >= base_trans_strength)
  return trans + (base % 12);
   else return base;

 return trans;

}
*/

int blend_function(int trans, int base, RGB *rgbl)
{

 int save_trans = 0;
 int save_base = 0;

 if (trans == CONVERT_WHITE_TO_GREY)
 {
  if (base == WRITE_IN)
   return COL_LGREY;
  return base;
 }

 if (base == WRITE_IN)
  base = COL_WHITE;
 if (base == WRITE_OUT)
  base = COL_DGREY;

 if (trans == COL_HS_OUTER
  || trans == COL_HS_OUTER_EDGE)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
//  save_trans += 1;
  save_base = (base - 0) % 19;
  switch(save_base)
  {
   case COL_HS_INNER:
   case COL_HS_INNER_EDGE:
   case COL_HS_OUTER:
   case COL_HS_INNER_LINE:
   case COL_HS_HURT:
    return save_base + save_trans;
   default: return trans + save_trans;
  }

 }

 if (trans == COL_HS_INNER
  || trans == COL_HS_HURT
  || trans == COL_HS_INNER_EDGE)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
//  save_trans += 1;
  save_base = (base - 0) % 19;
  switch(save_base)
  {
   case COL_HS_INNER:
   case COL_HS_HURT:
   case COL_HS_INNER_LINE:
    return save_base + save_trans;
   default: return trans + save_trans;
  }

 }

 if (trans == COL_HS_INNER_LINE)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
  save_base = (base - 0) % 19;
  return trans + save_trans;
 }

 if (trans == COL_HS_INNER_TRANS)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
  save_base = (base - 0) % 19;
  return COL_HS_HURT + save_trans;
 }

 if (trans == COL_WHITE)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
  save_base = (base - 0) % 19;
  return trans + save_trans;

 }

 if (trans == COL_LGREY)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
  save_base = (base - 0) % 19;
  switch(save_base)
  {
   case COL_WHITE:
   case COL_DGREY:
    return save_base + save_trans;
   default: return trans + save_trans;
  }

 }

 if (trans == COL_DGREY)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
  save_base = (base - 0) % 19;
  switch(save_base)
  {
   case COL_WHITE:
   case COL_LGREY:
//   case COL_OUTLINE:
    return save_base + save_trans;
   default: return trans + save_trans;
  }

 }

 if (trans == COL_OUTLINE)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
  save_base = (base - 0) % 19;
  return save_base + save_trans;
 }



/*
 if (trans == COL_HS_OUTER
  || trans == COL_HS_OUTER_EDGE)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
//  save_trans += 1;
  save_base = (base - 0) % 19;
  switch(save_base)
  {
   case COL_HS_INNER:
   case COL_HS_INNER_EDGE:
   case COL_HS_OUTER:
   case COL_HS_INNER_LINE:
   case COL_HS_HURT:
    return save_base + save_trans;
   default: return trans + save_trans;
  }

 }

 if (trans == COL_HS_INNER
  || trans == COL_HS_HURT
  || trans == COL_HS_INNER_EDGE)
 {
  save_trans = (base - 1) / 19;
  save_trans *= 19;
//  save_trans += 1;
  save_base = (base - 0) % 19;
//  save_base2 = base - 1;// % 19;
  switch(save_base)
  {
   case COL_HS_INNER:
   case COL_HS_HURT:
   case COL_HS_INNER_LINE:
    return save_base + save_trans;
   default: return trans + save_trans;
  }

 }
*/
 if (base < TRANS_RED1)
  return trans + base - 1;

 int trans_strength = (trans - 1) / 19;

 int base_trans_strength = (base - 1) / 19;

 if (trans_strength > base_trans_strength)
  return trans + ((base - 1) % 19);
   else return base;

// return trans;

}

int alpha_blend_function(int trans, int base, RGB *rgbl)
{

  if (base > 16)
  {
   return base;
  }

 int alpha = (trans - 1) / 16;
 int trans_colour = (trans - 1) % 16;

 if (alpha == 0 || alpha > 9)
  return base;

 if (alpha == 9)
  return trans_colour;

 int output;

 output += trans_colour * alpha;
 output += base * (9 - alpha);
 output /= 9;

 if (output < 1) output = 1;
 if (output > 16) output = 16;

 return output;

}



/*
This function had to be modified from the allegro create_color_table
because the allegro version used bestfit_color, whereas we need
specific color values
*/
void special_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal)
{
   int x, y, z;
   RGB c;

   for (x = 0; x < PAL_SIZE; x++)
   {
      for (y = 0; y < PAL_SIZE; y++)
      {
       z = blend_function(x, y, &c);

       table->data [x] [y] = z;
      }

   }
}

void special_create_color_table2(COLOR_MAP *table, AL_CONST PALETTE pal)
{
   int x, y, z;
   RGB c;

   for (x = 0; x < PAL_SIZE; x++)
   {
      for (y = 0; y < PAL_SIZE; y++)
      {
       z = alpha_blend_function(x, y, &c);

       table->data [x] [y] = z;
      }

   }
}


