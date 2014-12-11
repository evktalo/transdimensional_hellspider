
void init_palette(int stage);



enum
{
COL_NULL,
COL_BACK,
COL_STAR1,
COL_STAR2,
COL_STAR3,
COL_STAR4,
COL_STAR5,
COL_STAR6,
COL_STAR7,
COL_OUTLINE,
COL_DGREY,
COL_LGREY,
COL_WHITE,
COL_HS_INNER,
COL_HS_INNER_EDGE,
COL_HS_OUTER,
COL_HS_OUTER_EDGE,
COL_HS_INNER_LINE,
COL_HS_HURT

};

#define COL_HS_INNER_TRANS 21



#define TRANS_RED1 20
#define TRANS_GREEN1 39
#define TRANS_BLUE1 58
#define TRANS_RED2 77
#define TRANS_GREEN2 96
#define TRANS_BLUE2 115
#define TRANS_RED3 134
#define TRANS_GREEN3 153
#define TRANS_BLUE3 172
#define TRANS_YELLOW4 191
#define TRANS_RED4 191
#define TRANS_GREEN4 191
// yellow is top colour for both red and green
#define TRANS_BLUE4 210


#define COL_BRIGHT 252
#define WRITE_OUT 254
#define WRITE_IN 253

#define CONVERT_WHITE_TO_GREY 252

// don't think these do anything at the moment:
#define TRANS_REVERSE 225
#define TRANS_DARKEN 226

#define FIX_BITMAP 244

int colour_to_trans(int y);






