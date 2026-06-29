#include "smush.h"
#include <ctype.h>
#include <stdio.h>


static struct rebltune game_rebltune[3][21] = {
	{
	{"1A",  100,  100,   60,  110,    0,    0,   15,    0,    0,    5,  500,  100, 2048},
	{"1B",  100,   16,  120,    0,    7,    0,   15,    0,   25,    5,  500,  100, 3072},
	{"2",     0,    0,    0,    0,    4,   15,   25,    0,   25,   10,  500,  100, 2048},
	{"3",    70,  100,  150,   90,    0,    0,   20,    0,    0,    5, 1000,  100, 2048},
	{"4A",    0,    0,    0,    0,    2,   11,    0,    4,   25,    5,  500,  750, 2048},
	{"4B",    0,    0,    0,    0,    3,   20,    0,    2,   50,    5,  500,  750, 2064},
	{"5A",   70,  150,   50,   25,   10,    0,   20,    0,   25,    5,  500,   15, 3072},
	{"5B",    0,    0,    0,    0,    5,    0,    0,    2,   25,    0,  500,   15, 2048},
	{"6",     0,    0,    0,    0,    2,   20,   20,    0,   25,    5,  500,  100, 2048},
	{"7",   100,  150,  150,   25,    7,    0,   12,    2,   50,    5,  500,  100, 3072},
	{"8",     0,    0,    0,    0,    0,    0,   30,    0,   25,    0, 1000,  100, 3074},
	{"9A",    0,    0,    0,    0,    4,    0,    0,   15,   25,    0, 1000,  100, 3074},
	{"9B",    0,    0,    0,    0,    0,    0,    0,   15,   25,    0, 1000,  100, 3098},
	{"10",    0,    0,    0,    0,    3,   10,    0,    5,   25,    5,  500,  200, 2048},
	{"11",   70,  150,  150,   25,   12,    0,   30,    0,   50,    5,  500,  200, 3072},
	{"12",    0,    0,    0,    0,    4,   13,    0,    5,   25,    5,  500,  100, 2048},
	{"13",  100,   16,  120,    0,   20,    0,   35,    8,   75,    5,  500,  100, 3072},
	{"14A",   0,    0,    0,    0,    0,   20,   35,    8,   25,    0, 1000,  100, 2048},
	{"14B",   0,    0,    0,    0,   10,   20,   35,    8,   25,    0, 1000,  100, 2048},
	{"15A",   0,    0,    0,    0,    4,    0,   28,    3,   25,    5,  500,  100, 2048},
	{"15B",   0,    0,    0,    0,    4,   10,   30,    3,   25,    5,  500,  100, 2048}
	}, {
	{"1A",  100,  105,   60,  115,    0,    0,   25,    0,    0,    5, 1000,  200, 2048},
	{"1B",  100,   18,  120,    0,    5,    0,   20,    0,   50,    5, 1000,  200, 3072},
	{"2",     0,    0,    0,    0,    2,   18,   30,    0,   50,   10, 1000,  200, 2048},
	{"3",    72,  105,  155,  105,    0,    0,   25,    0,    0,    5, 2000,  200, 2048},
	{"4A",    0,    0,    0,    0,    1,   25,    0,    6,   50,    5, 1000, 1500, 2048},
	{"4B",    0,    0,    0,    0,    1,   25,    0,    5,  100,    5, 1000, 1500, 2064},
	{"5A",   72,  165,  155,   30,    8,    0,   30,    0,   50,    5, 1000,   30, 3072},
	{"5B",    0,    0,    0,    0,    3,    0,    0,    5,   50,    5, 1000,   30, 2048},
	{"6",     0,    0,    0,    0,    1,   25,   30,    0,   50,    5, 1000,  200, 2048},
	{"7",   100,  160,  200,   35,    4,    0,   30,    4,  100,    5, 1000,  200, 3072},
	{"8",     0,    0,    0,    0,    0,    0,   36,    0,   50,    0, 2000,  200, 3074},
	{"9A",    0,    0,    0,    0,    2,    0,    0,   25,   50,    0, 2000,  200, 3078},
	{"9B",    0,    0,    0,    0,    0,    0,    0,   25,   50,    0, 2000,  200, 3098},
	{"10",    0,    0,    0,    0,    1,   16,    0,    5,   50,    5, 1000,  400, 2048},
	{"11",   72,  165,  155,   30,    7,    0,   36,    0,   50,    5, 1000,  400, 3072},
	{"12",    0,    0,    0,    0,    2,   20,    0,    5,   50,    5, 1000,  200, 2048},
	{"13",  100,   18,  120,    0,   18,    0,   36,   10,  100,    5, 1000,  200, 3072},
	{"14A",   0,    0,    0,    0,    0,   27,   36,   12,   50,    0, 2000,  200, 2048},
	{"14B",   0,    0,    0,    0,    5,   25,   36,   10,   50,    0, 2000,  200, 2048},
	{"15A",   0,    0,    0,    0,    3,    0,   36,    3,   50,    5, 1000,  200, 2048},
	{"15B",   0,    0,    0,    0,    3,   20,   34,    3,   50,    5, 1000,  200, 2048}
	},{
	{"1A",  105,  110,   65,  120,    0,    0,   30,    0,    0,   10, 1500,  500, 2050},
	{"1B",  100,   20,  150,    0,    1,    0,   25,    0,   75,   10, 1500,  500, 3074},
	{"2",     0,    0,    0,    0,    0,   20,   35,    0,   75,   10, 1500,  500, 2050},
	{"3",    75,  110,  160,  110,    0,    0,   28,    0,    0,   10, 3000,  500, 2050},
	{"4A",    0,    0,    0,    0,    1,   28,    0,    6,   75,   10, 1500, 2000, 2050},
	{"4B",    0,    0,    0,    0,    1,   28,    0,    6,  200,   10, 1500, 2000, 2064},
	{"5A",  110,  190,   55,   65,    3,    0,   33,    0,   75,   10, 1500,   75, 3074},
	{"5B",    0,    0,    0,    0,    1,    0,    0,    6,   75,   10, 1500,   75, 2050},
	{"6",     0,    0,    0,    0,    0,   28,   33,    0,   75,   10, 1500,  500, 2050},
	{"7",   100,  180,  250,   50,    3,    0,   33,    5,  100,   10, 1500,  500, 3074},
	{"8",     0,    0,    0,    0,    0,    0,   39,    0,   75,    0, 3000,  500, 3074},
	{"9A",    0,    0,    0,    0,    0,    0,    0,   30,   75,    0, 3000,  500, 3078},
	{"9B",    0,    0,    0,    0,    0,    0,    0,   30,   75,    0, 3000,  500, 3098},
	{"10",    0,    0,    0,    0,    0,   18,    0,    7,   75,   10, 1500, 1000, 2050},
	{"11",   75,  170,  160,   33,    3,    0,   39,    0,   75,   10, 1500, 1000, 3074},
	{"12",    0,    0,    0,    0,    0,   23,    0,    5,   75,   10, 1500,  500, 2050},
	{"13",  100,   20,  150,    0,   15,    0,   39,   12,  200,   10, 1500,  500, 3074},
	{"14A",   0,    0,    0,    0,    0,   28,   39,   12,   75,    0, 3000,  500, 2050},
	{"14B",   0,    0,    0,    0,    4,   28,   39,   12,   75,    0, 3000,  500, 2050},
	{"15A",   0,    0,    0,    0,    3,    0,   39,    4,   75,   10, 1500,  500, 2050},
	{"15B",   0,    0,    0,    0,    2,   22,   35,    4,   75,   10, 1500,  500, 2050}
	}
};


static char game_hiscore_names[10][10] = { "<Vince", "<Tamlynn", "<Chip", "<Brett",
					   "<Casey", "<Justin", "<Bill", "<Aaron",
					   "<Mary", "<Ron" };
static uint32_t game_hiscore_points[10] = { 0x2710, 0x2328, 0x1f40, 0x1b58,
					    0x1770, 0x1388, 0xfa0, 0xbb8,
					    0x7d0, 0x3e8 };
static uint8_t game_hiscore_difficulty[10] = { 2, 2, 2, 1, 1, 1, 0, 0, 0, 0 };

static char game_level_passwords[15][20] = { "FALCON", "BIGGS", "ACKBAR", "ANOAT",
					     "KAIBURR", "FORNAX", "YUZZEM", "MYNOCK",
					     "BESPIN", "BRIGIA", "DAGOBAH", "KESSEL",
					     "GREEDO", "MIMBAN", "ORGANA" };


uint8_t  game_difficulty_index;		/* 002ae972 */
uint8_t  game_input_flipy;		/* 002ae974 */
uint8_t  game_snddrv_volume;		/* 002ae977 */


uint8_t  DAT_002b5610;			/* 002b5610; GAM1 26 in pause called flag */
uint8_t  DAT_002b5611;

uint8_t  game_curr_difficulty;

struct anm_res game_anmres_laser2;	/* 002aed7f */
struct anm_res game_anmres_banking;	/* 002af185 */
struct anm_res game_anmres_exploding;	/* 002af58b */
struct anm_res game_anmres_bang;	/* 002af991 */
struct anm_res game_anmres_laser;	/* 002afd97 */

/* LASRSHOT, BOOM, EXPLODE, KLAXON, ALERT, BONUS, BLAST, none, none */
enum game_sound_id {
	LASRSHOT = 0, BOOM, EXPLODE, KLAXON, ALERT, BONUS, BLAST,
};
void *game_sound[10];			/* 002b019d - 002b01c4 */

uint8_t game_level_pws_encrypted = 0;	/* 002b01c5 */
int16_t game_levelfunc_retval;		/* 002b01c6 */
static const char game_levelpw_allowed_chars[33] = { "^`_ABCDEFGHIJKLMNOPQRSTUVWXYZ^'_\0" };	/* 002b01c8 */
int16_t  game_lvlres_need_change;	/* 002b01e9 */
char *game_lvlres_laser2file;		/* 002b01eb */
char *game_lvlres_bankingfile;		/* 002b01ef */
char *game_lvlres_explodingfile;	/* 002b01f3 */
char *game_lvlres_bangfile;		/* 002b01f7 */
char *game_lvlres_laserfile;		/* 002b01fb */
uint8_t  game_newanm_posted;		/* 002b01ff */
uint8_t  game_anmpos_updated;		/* 002b0200 */
char     *game_lvlres_sndfile;		/* 002b0201 */
int16_t  game_explosion_xpos[10];	/* 002b0205 */
int16_t  game_explosion_ypos[10];	/* 002b0219 */
int16_t  game_explosion_skipidx[10];	/* 002b022d */
int16_t  game_explosion_frmeidx[10];	/* 002b0241 */
uint16_t game_explosion_currcnt;	/* 002b0255 */
uint8_t  DAT_002b089e;			/* 002b089e */
uint8_t  DAT_002b089f;			/* 002b089f */

uint8_t  game_demo_mode;		/* 002b2612 */

char *	 game_newanm_filename;		/* 002b51b4 */
int16_t	 game_newanm_splice1;		/* 002b51b8 */
int16_t	 game_anmpos_splice2;		/* 002b51ba */
uint16_t game_newanm_anmflags;		/* 002b51bc */
int16_t	 game_newanm_splice2;		/* 002b51be */
int16_t	 game_anmpos_splice1;		/* 002b51c0 */
uint16_t game_lvlres_sndidx;		/* 002b51c2 */
uint16_t game_anmpos_anmflags;		/* 002b51c4 */

uint16_t game_level_status_flags;	/* 002b54e4 */

uint16_t game_target_xoff_lut[20];	/* 002b54ea - 002b5511 */
uint16_t game_target_yoff_lut[20];	/* 002b5512 - 002b5539 */
uint16_t game_target_frmeidx[20];	/* 002b553a - 002b5561 */
uint16_t game_target_currcnt;		/* 002b5562 */
uint16_t game_target_lastcnt;		/* 002b5564 */
uint16_t game_target_proximity;		/* 002b5568 */
uint16_t game_target_proximity_last;	/* 002b556a */
uint16_t game_target_lockon_icon_cycle;	/* 002b556c */
uint16_t game_difficulty_level_index;	/* 002b556e */
uint16_t game_player_health;		/* 002b5570 */
uint16_t game_player_lives;		/* 002b5572 */
uint32_t game_player_score;		/* 002b5574 */
uint16_t game_input_curr_key;		/* 002b5578 */
uint16_t DAT_002b557a;			/* 002b557a */
int16_t	 game_input_curr_x;		/* 002b557c */
int16_t	 game_input_curr_y;		/* 002b557e */
uint16_t game_input_curr_button1;	/* 002b5580 */
uint16_t game_input_curr_button2;	/* 002b5582 */
int16_t  game_joycfg_minmax[4];		/* 002b5584 */
uint16_t game_input_prev_button1;	/* 002b558c */
uint16_t game_input_prev_button2;	/* 002b558e */
int16_t  DAT_002b5590[10];		/* 002b5590 */
int16_t  DAT_002b55a4[10];		/* 002b55a4 */
int16_t  DAT_002b55b8[10];		/* 002b55b8 */
int16_t  DAT_002b55cc[10];		/* 002b55cc */
uint16_t game_player_targets_hit;	/* 002b55e0 */

uint16_t DAT_002b55e6;			/* 002b55e6 */
int16_t  DAT_002b55f6[2];		/* 002b55f6 */
int16_t  DAT_002b55fa[2];		/* 002b55fa */
int16_t  DAT_002b55fe[2];		/* 002b55fe */
int16_t  DAT_002b5602[2];		/* 002b5602 */
int16_t  DAT_002b5606[2];		/* 002b5606 */
int16_t  DAT_002b560a[2];		/* 002b560a */
uint8_t  game_state_flags[2];		/* 002b560e */
uint8_t  DAT_002b5610;			/* 002b5610 */
uint8_t  DAT_002b5611;			/* 002b5611 */
uint8_t  game_hide_table[300];		/* 002b5612 */
uint8_t  game_joy_secret_unlock;	/* 002b573e , set when doing up-down-left-right during lucasarts logo */
uint8_t  game_hiscore_changed;		/* 002b573f */
uint16_t game_curr_levelidx;		/* 002b5740 */
uint16_t DAT_002b5746;			/* 002b5746 */
uint8_t *game_postcb_dst;		/* 002b5748 */
uint8_t *game_postcb_statusbar;		/* 002b574c */
uint16_t game_postcb_currfrme;		/* 002b5750 */
uint16_t game_postcb_maxfrme;		/* 002b5752 */
uint16_t game_postcb_currfrme2;		/* 002b5754 */
struct anm_rect *game_postcb_viewport;	/* 002b5756 */
jmp_buf  game_jmpbuf_1;			/* 002b575a */
jmp_buf  game_jmpbuf_2;			/* 002b578e */
uint8_t  game_jmpbuf_2_valid;		/* 002b57c2 */
uint8_t  game_exit_loop_now_flag;	/* 002b57c3 */
game_level_fn_t   game_curr_levelfunc;	/* 002b57c4 */
uint8_t	 game_palette[768];		/* 002b57c8 - */

uint32_t DAT_002b6294;			/* 002b6294; STACK POINTER moved to in game_yield() */
uint32_t game_player_score_newlife;	/* 002b6298 */
uint8_t  game_hiscore_changed;		/* 002b732f */


uint16_t game_anmflags_initial;		/* 002b77ea */

struct anm_res game_anmres_DISPLAYNUT;
void *game_sound_LASRSHOT;
void *game_sound_EXPLODE;
void *game_sound_BOOM;
void *game_sound_KLAXON;
void *game_sound_LOCKON;
void *game_sound_ALERT;
void *game_sound_BONUS;
void *game_sound_BLAST;

void game_set_statusbar_changed(uint8_t v)
{
	AG(game_current_statusbar_changed_flag_at_call) = v;
}

void game_cfg_set_sndcard_params(int16_t base, int16_t irq, uint32_t bufsize,
				 int16_t cardtype,int16_t dmachan)
{
	AG(sou_soudrv_type) = cardtype;
	AG(sou_soudrv_iobase) = base;
	AG(sou_soudrv_dma) = dmachan;
	AG(sou_soudrv_irq) = irq;
	if (bufsize)
		AG(sou_soudrv_pref_bufsize) = bufsize;
}

void game_cfg_write_rebltune_txt(void)
{
	int i;
	FILE *f = fopen("REBLTUNE.TXT", "w");
	if (!f) {
		printf("can't open REBLTUNE.TXT\n");
		return;
	}
	fprintf(f, "Rebel Assault tuning parameters\n");
	fprintf(f, "\n");
	fprintf(f, "EASY\n");
	fprintf(f, "       ------ ship control -----         --- damage ---  -------- points -------\n");
	fprintf(f, " level  roll  lift  slide  drift snap  miss  wham  shot  kill  time level  bonus flags\n");
	fprintf(f, "\n");
	for (i = 0; i < 21; i++) {
		fprintf(f, "%6s %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd\n",
			game_rebltune[0][i].name,  game_rebltune[0][i].roll,
			game_rebltune[0][i].lift,  game_rebltune[0][i].slide,
			game_rebltune[0][i].drift, game_rebltune[0][i].snap,
			game_rebltune[0][i].miss,  game_rebltune[0][i].wham,
			game_rebltune[0][i].shot,  game_rebltune[0][i].kill,
			game_rebltune[0][i].time,  game_rebltune[0][i].level,
			game_rebltune[0][i].bonus, game_rebltune[0][i].flags);
	}
	fprintf(f, "\n");
	fprintf(f, "NORMAL\n");
	fprintf(f, "       ------ ship control -----         --- damage ---  -------- points -------\n");
	fprintf(f, " level  roll  lift  slide  drift snap  miss  wham  shot  kill  time level  bonus flags\n");
	fprintf(f, "\n");
	for (i = 0; i < 21; i++) {
		fprintf(f, "%6s %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd\n",
			game_rebltune[1][i].name,  game_rebltune[1][i].roll,
			game_rebltune[1][i].lift,  game_rebltune[1][i].slide,
			game_rebltune[1][i].drift, game_rebltune[1][i].snap,
			game_rebltune[1][i].miss,  game_rebltune[1][i].wham,
			game_rebltune[1][i].shot,  game_rebltune[1][i].kill,
			game_rebltune[1][i].time,  game_rebltune[1][i].level,
			game_rebltune[1][i].bonus, game_rebltune[1][i].flags);
	}
	fprintf(f, "\n");
	fprintf(f, "HARD\n");
	fprintf(f, "       ------ ship control -----         --- damage ---  -------- points -------\n");
	fprintf(f, " level  roll  lift  slide  drift snap  miss  wham  shot  kill  time level  bonus flags\n");
	fprintf(f, "\n");
	for (i = 0; i < 21; i++) {
		fprintf(f, "%6s %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd\n",
			game_rebltune[2][i].name,  game_rebltune[2][i].roll,
			game_rebltune[2][i].lift,  game_rebltune[2][i].slide,
			game_rebltune[2][i].drift, game_rebltune[2][i].snap,
			game_rebltune[2][i].miss,  game_rebltune[2][i].wham,
			game_rebltune[2][i].shot,  game_rebltune[2][i].kill,
			game_rebltune[2][i].time,  game_rebltune[2][i].level,
			game_rebltune[2][i].bonus, game_rebltune[2][i].flags);
	}
	fclose(f);
}

void game_cfg_read_rebltune_txt(void)
{
	char temp[152];
	size_t j;
	ssize_t r;
	int i;

	FILE *f = fopen("REBLTUNE.TXT", "r");
	if (!f) {
		printf("can't open REBLTUNE.TXT\n");
		return;
	}
	j = 149;
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	for (i = 0; i < 21; i++) {
		r = fscanf(f, "%6s %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd\n",
			 game_rebltune[0][i].name,  &game_rebltune[0][i].roll,
			&game_rebltune[0][i].lift,  &game_rebltune[0][i].slide,
			&game_rebltune[0][i].drift, &game_rebltune[0][i].snap,
			&game_rebltune[0][i].miss,  &game_rebltune[0][i].wham,
			&game_rebltune[0][i].shot,  &game_rebltune[0][i].kill,
			&game_rebltune[0][i].time,  &game_rebltune[0][i].level,
			&game_rebltune[0][i].bonus, &game_rebltune[0][i].flags);
	}
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	for (i = 0; i < 21; i++) {
		r = fscanf(f, "%6s %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd\n",
			 game_rebltune[1][i].name,  &game_rebltune[1][i].roll,
			&game_rebltune[1][i].lift,  &game_rebltune[1][i].slide,
			&game_rebltune[1][i].drift, &game_rebltune[1][i].snap,
			&game_rebltune[1][i].miss,  &game_rebltune[1][i].wham,
			&game_rebltune[1][i].shot,  &game_rebltune[1][i].kill,
			&game_rebltune[1][i].time,  &game_rebltune[1][i].level,
			&game_rebltune[1][i].bonus, &game_rebltune[1][i].flags);
	}
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	r = getline((char **)temp, &j, f);
	for (i = 0; i < 21; i++) {
		r = fscanf(f, "%6s %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd %5hd\n",
			 game_rebltune[2][i].name,  &game_rebltune[2][i].roll,
			&game_rebltune[2][i].lift,  &game_rebltune[2][i].slide,
			&game_rebltune[2][i].drift, &game_rebltune[2][i].snap,
			&game_rebltune[2][i].miss,  &game_rebltune[2][i].wham,
			&game_rebltune[2][i].shot,  &game_rebltune[2][i].kill,
			&game_rebltune[2][i].time,  &game_rebltune[2][i].level,
			&game_rebltune[2][i].bonus, &game_rebltune[2][i].flags);
	}
	fclose(f);
}

void game_write_hiscore_file(void)
{
	FILE *f = fopen("REBEL.PTS", "wb");
	if (!f)
		return;
	for (int i = 0; i < 10; i++)
		fprintf(f, "%s\n", game_hiscore_names[i]),
	fwrite((void *)game_hiscore_points, 10, 4, f);
	fwrite((void *)game_hiscore_difficulty, 10, 1, f);
	fclose(f);
}

void game_read_hiscore_file(void)
{
	size_t s;
	ssize_t r;
	FILE *f = fopen("REBEL.PTS", "rb");
	if (!f)
		return;
	for (int i = 0; i < 10; i++) {
		s = 10;
		r = getline((char **)&(game_hiscore_names[i]), &s, f);
		int t = strlen(game_hiscore_names[i]);
		game_hiscore_names[i][9] = 0;
		for (int j = 8; j; j--) {
			if ((game_hiscore_names[i][j] == '\n')
			   || (game_hiscore_names[i][j] == '\r')) {
				game_hiscore_names[i][j] = 0;
			}
		}
	}
	r = fread(game_hiscore_points, 10, 4, f);
	r = fread(game_hiscore_difficulty, 10, 1, f);
	fclose(f);
}

void game_main(int16_t debug)
{
	if (game_level_pws_encrypted) {
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 19; j++) {
				if (game_level_passwords[i][j] == 0)
					break;
				game_level_passwords[i][j] ^= 0xaa;
			}
		}
		game_level_pws_encrypted = 0;
	}
	game_read_hiscore_file();
	AG(smush_debug_enabled) = debug;
	AG(sou_music_enabled) = 1;
	AG(sou_sfx_enabled) = 1;
	AG(sou_voice_enabled) = 1;
	/* seems to be a bug: game_init() sets this based on sound card availability */
	//AG(anm_text_force_enabled) = 0;
	game_difficulty_index = 1;
	game_snddrv_volume = 127;
	game_input_flipy = 0;
	game_joycfg_minmax[0] = -128;
	game_joycfg_minmax[1] = -128;
	game_joycfg_minmax[2] = 127;
	game_joycfg_minmax[3] = 127;
	game_hiscore_changed = 0;
	DAT_002b5610 = 0;		/* GAME pause/gam1a ? */
	DAT_002b5611 = 0;
	game_curr_difficulty = 0;
	game_joy_secret_unlock = 0;	/* u-d-l-r joy move during LEC logo */
	game_curr_levelidx = 0;		/* level password index */
	anm_resource_load(&game_anmres_DISPLAYNUT, "SYS/DISPLAY.NUT", 1);
	res_resource_load("SYS/LASRSHOT.SAD", &game_sound_LASRSHOT);
	res_resource_load("SYS/EXPLODE.SAD",  &game_sound_EXPLODE);
	res_resource_load("SYS/BOOM.SAD",     &game_sound_BOOM);
	res_resource_load("SYS/KLAXON.SAD",   &game_sound_KLAXON);
	res_resource_load("SYS/LOCKON.SAD",   &game_sound_LOCKON);
	res_resource_load("SYS/ALERT.SAD",    &game_sound_ALERT);
	res_resource_load("SYS/BONUS.SAD",    &game_sound_BONUS);
	res_resource_load("SYS/BLAST.SAD",    &game_sound_BLAST);
	game_state_flags[0] = 0;
	game_state_flags[1] = 0;	/* both are written to with a single 16bit store */
	game_set_next_level(NULL);
	game_run("OPEN/O1CALIB.ANM", 1, game_anm_frme_render_postcb, game_anm_loop_cb,
		 game_anm_game_cb, game_hide_table, 0, 150000);
	res_resource_free(&game_sound_LASRSHOT);
	res_resource_free(&game_sound_EXPLODE);
	res_resource_free(&game_sound_BOOM);
	res_resource_free(&game_sound_KLAXON);
	res_resource_free(&game_sound_LOCKON);
	res_resource_free(&game_sound_ALERT);
	res_resource_free(&game_sound_BONUS);
	res_resource_free(&game_sound_BLAST);
	anm_resource_free(&game_anmres_DISPLAYNUT);
}

void game_clear_jmp2(void)
{
	game_jmpbuf_2_valid = 0;
}

/* yields to the other jmpbuf if set */
int16_t game_yield(void)
{
	int ret;

	ret = setjmp(game_jmpbuf_1);
	printf("game_yield: setjmp(game_jmpbuf_1) = %d\n", ret);
	if (ret != 0) {
		return game_levelfunc_retval;
	}
	if (game_jmpbuf_2_valid) {
		game_jmpbuf_2_valid = 0;
		printf("game_yield: longjmp(game_jmpbuf_2, 1)\n");
		longjmp(game_jmpbuf_2, 1);
	}
	game_jmpbuf_2_valid = 0;
#ifdef __WATCOMC__
	/* FIXME verify */
	_asm { mov dx, ds; mov eax, DAT_002b6294; mov ss,dx; mov esp,eax }
	/* FIXME */
#endif
	game_levelfunc_retval = game_curr_levelfunc();
	printf("game_yield: longjmp(game_jmpbuf_1, 2)\n");
	longjmp(game_jmpbuf_1, 2);
}

void game_switch_task(void)
{
	int ret;

	if ((tolower(game_input_curr_key) == 'j') && (0 == game_state_flags[0] & 0x20) && (0 == game_state_flags[1] & 1)) {
		game_input_curr_key = 0;
		game_options_joystick_screen();
	}
	if ((tolower(game_input_curr_key) == 'w') && (0 == game_state_flags[0] & 0x20) && (0 == game_state_flags[1] & 1)) {
		game_input_curr_key = 0;
		game_options_quit_screen();
	}
	if ((tolower(game_input_curr_key) == '0') && (0 == game_state_flags[0] & 0x20) && (0 == game_state_flags[1] & 1)) {
		game_input_curr_key = 0;
		game_options_config_screen(1, (game_player_score == 0));
	}
	if ((tolower(game_input_curr_key) == ' ') && (0 == game_state_flags[0] & 0x20) && (0 == game_state_flags[1] & 1)) {
		game_input_curr_key = 0;
		game_pause();
	}
	ret = setjmp(game_jmpbuf_2);
	printf("game_switch_task(): setjmp(game_jmpbuf_2) = %d\n", ret);
	if (ret == 0) {
		game_jmpbuf_2_valid = 1;
		printf("game_switch_task(): longjmp(game_jmpbuf_1, 1)\n");
		longjmp(game_jmpbuf_1, 1);
	}
	printf("game_switch_task(): returning to caller!\n");
}

void game_reset_lvldata(uint16_t diffidx)
{
	game_difficulty_level_index = diffidx;
	*(uint16_t *)&game_state_flags[0] = game_rebltune[game_difficulty_index][diffidx].flags; /* single 16bit move */
	game_player_targets_hit = 0;
//	game_gam93_hidetl_last_cleartested_bit = 0;
	game_level_status_flags = 0;
//	game_gam95_hidetl_last_cleartested_bit = 0;
	DAT_002b557a = 0;
//	game_target_proximity = 0;
//	game_lvl4_prot_target1 = 0;
//	game_lvl4_prot_target2 = 0;
	DAT_002b55f6[0] = 0;
	DAT_002b55f6[1] = 0;
	game_target_currcnt = 0;
	game_target_lastcnt = 0;
}

uint16_t game_enter_passcode_screen(void)
{
	char pwd_buffer[20] = {0};
	char cursor_char_str[2] = {0};
	uint8_t can_use_pwd = 3;
	uint8_t exit_flag = 0;
	int16_t cursor_anim_offset = 0;
	int idx;

	memcpy(pwd_buffer, "< ", 2);

	if (game_curr_levelidx != 0) {
		strcat(pwd_buffer, game_level_passwords[game_curr_levelidx - 1]);
		can_use_pwd = 1;
	}

	while ((exit_flag == 0) && (game_exit_loop_now_flag == 0)) {
		if (game_postcb_currfrme == (game_postcb_maxfrme - 0x33)) {
			game_anm_set_playpos(0x20, game_postcb_maxfrme - 0x33, 0);
		}

		txt_font_print(NULL, NULL, 160, 75, 640, 100, "Enter Passcode");
		txt_font_print(NULL, NULL, 160, 95, 640, 100, pwd_buffer);

		idx = (can_use_pwd == 0) ? 0x37 : (can_use_pwd - 1);
		cursor_char_str[0] = game_levelpw_allowed_chars[idx];
		txt_font_print(NULL, NULL, cursor_anim_offset + 145, 110, 640, 3, cursor_char_str);

		cursor_char_str[0] = game_levelpw_allowed_chars[can_use_pwd];
		txt_font_print(NULL, NULL, cursor_anim_offset + 160, 110, 640, (game_postcb_currfrme & 2), cursor_char_str);

		idx = (can_use_pwd < 29) ? (can_use_pwd + 1) : 0;
		cursor_char_str[0] = game_levelpw_allowed_chars[idx];
		txt_font_print(NULL, NULL, cursor_anim_offset + 175, 110, 640, 3, cursor_char_str);

		cursor_anim_offset = 0;

		if (((game_input_curr_x < 0x33) || ((game_postcb_currfrme & 1) == 0)) && (game_input_curr_x < 0x65)) {
			if (((game_input_curr_x < -0x32) && ((game_postcb_currfrme & 1) != 0)) || (game_input_curr_x < -100)) {
				can_use_pwd = (can_use_pwd == 0) ? 29 : can_use_pwd - 1;
				ctl_mouse_reset();
				cursor_anim_offset = -7;
			}
		} else {
			can_use_pwd = (can_use_pwd >= 29) ? 0 : can_use_pwd + 1;
			ctl_mouse_reset();
			cursor_anim_offset = 7;
		}

		if (game_input_curr_button1 != 0 && game_input_prev_button1 == 0) {
			int len = strlen(pwd_buffer);
			char c = game_levelpw_allowed_chars[can_use_pwd];

			if (c == '^') {
				if (len > 1)
					pwd_buffer[len - 1] = 0;
			} else if (c == '`') {
				exit_flag = 1;
			} else if (c == '_') {
				if (len < 9) {
					pwd_buffer[len + 0] = ' ';
					pwd_buffer[len + 1] = 0;

				}
			} else if (len < 9) {
				pwd_buffer[len + 0] = c;
				pwd_buffer[len + 1] = 0;
			}
		}

		if ((game_input_curr_button2 != 0) && (game_input_prev_button2 == 0))
			exit_flag = 1;

		if (game_input_curr_key != 0) {
			int len = strlen(pwd_buffer);
			if (isprint(game_input_curr_key)) {
				if (len < 9) {
					pwd_buffer[len + 0] = toupper(game_input_curr_key);
					pwd_buffer[len + 1] = 0;
				}
			} else if ((game_input_curr_key == '\r') || (game_input_curr_key == 10)) {
				exit_flag = 1;
			} else if ((game_input_curr_key == 8) && (len > 1)) {
				pwd_buffer[len - 1] = 0;
			}
			game_input_curr_key = 0;
		}
		game_switch_task();
	}

	for (int i = 0; i < (int)strlen(pwd_buffer); i++) {
		pwd_buffer[i] = toupper(pwd_buffer[i]);
	}

	for (uint16_t i = 0; i < 15; i++) {
		if (strcmp(&pwd_buffer[1], game_level_passwords[i]) == 0) {
			game_curr_levelidx = i + 1;
			return i + 1;
		}
	}
	return 0;
}

void game_pause(void)
{
	game_state_flags[0] |= 0x20;
	sou_pause();
	anm_cmd_pause(1);
	game_switch_task();
	game_switch_task();
	vid_palette_set_intensity(AG(anm_anim_header_current).animpal, 127);
	game_input_curr_key = 0;
	while (game_input_curr_key == 0) {
		game_switch_task();
	}
	game_input_curr_key = 0;
	vid_palette_set(AG(anm_anim_header_current).animpal);
	anm_cmd_pause(0);
	sou_resume();
	game_state_flags[0] &= 0xdf;
}

void game_options_joystick_screen(void)
{
	struct anm_rect viewport;
	int16_t state;

	/* returns 0 if the joystick is disabled (and mouse ENabled) */
	if (ctl_joy_calibrate_start() == 0) {
		return;
	}

	state = 0;
	viewport.x = 4;
	viewport.y = 4;
	viewport.w = 312;
	viewport.h = 192;

	if (game_input_curr_key == 0 || game_input_curr_key == 0x1B) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}

	game_state_flags[1] |= 1;
	sou_pause();
	anm_cmd_pause(1);
	game_switch_task();
	game_switch_task();

	vid_bitmap_clear(game_postcb_dst);
	vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
	vid_palette_set(game_palette);

	game_joycfg_minmax[0] = -50;
	game_joycfg_minmax[1] = -50;
	game_joycfg_minmax[2] = 50;
	game_joycfg_minmax[3] = 50;

	while ((state < 4) && (game_exit_loop_now_flag == 0)) {
		vid_bitmap_clear(game_postcb_dst);
		txt_font_print(NULL, &viewport, 160, 10, 640, 100, "JOYSTICK CALIBRATION");
		char *instruction;
		switch (state) {
			case 0:
			case 3:
				instruction = "<Center joystick";
				break;
			case 1:
				instruction = "<Move joystick to upper left";
				break;
			case 2:
				instruction = "<Move joystick to lower right";
				break;
		}

		txt_font_print(NULL, &viewport, 160, 25, 640, 100, instruction);
		txt_font_print(NULL, &viewport, 160, 40, 640, 100, "<and press fire.");

		if (state != 0) {
			int16_t disp_x = (game_input_curr_x / 4) + 160;
			int16_t disp_y = (game_input_curr_y / 4);
			txt_font_printf(NULL, &viewport, 160, 175, 640, 100, "(%d,%d)", disp_x, disp_y);
		}

		if (game_input_prev_button1 == 0 && game_input_curr_button1 != 0) {
			if (state == 0) {
				ctl_joy_center();
			}
			state++;
		}

		if (state != 0) {
			if (game_input_curr_x > game_joycfg_minmax[2]) {
				game_joycfg_minmax[2] = game_input_curr_x;
			}
			if (game_input_curr_x < game_joycfg_minmax[0]) {
				game_joycfg_minmax[0] = game_input_curr_x;
			}
			if (game_input_curr_y > game_joycfg_minmax[3]) {
				game_joycfg_minmax[3] = game_input_curr_y;
			}
			if (game_input_curr_y < game_joycfg_minmax[1]) {
				game_joycfg_minmax[1] = game_input_curr_y;
			}
		}
		vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);

		AG(anm_anmflags_global) |= 0x8000;
		game_switch_task();
		AG(anm_anmflags_global) &= 0x7FFF;
	}

	ctl_joy_calibrate_end();

	if (game_input_curr_key == 0 || game_input_curr_key == 27) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}

	vid_palette_set(AG(anm_anim_header_current).animpal);
	anm_cmd_pause(0);
	sou_resume();

	game_state_flags[1] &= 0xfe;
	game_input_curr_key = 0;
}

void game_options_config_screen(uint8_t clrscr, uint8_t diffchg_en)
{
	int16_t state = 1;
	int16_t debounce = 0;
	struct anm_rect viewport;

	viewport.x = 4;
	viewport.y = 4;
	viewport.w = 312;
	viewport.h = 192;

	if (game_input_curr_key == 0 || game_input_curr_key == 0x1b) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}

	game_state_flags[1] |= 1;	/* inhibit GAME probably */
	sou_pause();
	anm_cmd_pause(1);
	game_switch_task();
	game_switch_task();

	if (clrscr != 0) {
		vid_bitmap_clear(game_postcb_dst);
		vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
		vid_palette_set(game_palette);
	}

	while (state != 0) {
		if (clrscr != 0) {
			vid_bitmap_clear(game_postcb_dst);
		}

		txt_font_print(NULL, &viewport, 160, 30, 640, 100, "GAME OPTIONS");
		txt_font_print(NULL, &viewport, 160, 45, 640, 100, "<EXIT MENU");

		if (sou_engine_gethook(4) == 0) {
			txt_font_print(NULL, &viewport, 160, 60, 640, 100, "<ROOKIE1 IS MALE");
		} else {
			txt_font_print(NULL, &viewport, 160, 60, 640, 100, "<ROOKIE1 IS FEMALE");
		}

		if (AG(sou_music_enabled) == 0) {
			txt_font_print(NULL, &viewport, 160, 75, 640, 100, "<MUSIC IS OFF");
		} else {
			txt_font_print(NULL, &viewport, 160, 75, 640, 100, "<MUSIC IS ON");
		}

		if (AG(sou_sfx_enabled) == 0) {
			txt_font_print(NULL, &viewport, 160, 90, 640, 100, "<SFX AND VOICE ARE OFF");
		} else {
			txt_font_print(NULL, &viewport, 160, 90, 640, 100, "<SFX AND VOICE ARE ON");
		}

		if (AG(anm_text_force_enable) == 0) {
			txt_font_print(NULL, &viewport, 160, 105, 640, 100, "<DIALOGUE TEXT IS OFF");
		} else {
			txt_font_print(NULL, &viewport, 160, 105, 640, 100, "<DIALOGUE TEXT IS ON");
		}

		if (game_input_flipy == 0) {
			txt_font_print(NULL, &viewport, 160, 120, 640, 100, "<CONTROLS ARE NORMAL");
		} else {
			txt_font_print(NULL, &viewport, 160, 120, 640, 100, "<CONTROLS ARE Y-FLIPPED");
		}

		txt_font_printf(NULL, &viewport, 160, 135, 640, 100, "<VOLUME AT %hd PERCENT", (game_snddrv_volume * 100) / 127);

		if (diffchg_en != 0) {
			if (game_difficulty_index == 0) {
				txt_font_print(NULL, &viewport, 160, 150, 640, 100, "<DIFFICULTY IS EASY");
			} else if (game_difficulty_index == 1) {
				txt_font_print(NULL, &viewport, 160, 150, 640, 100, "<DIFFICULTY IS NORMAL");
			} else {
				txt_font_print(NULL, &viewport, 160, 150, 640, 100, "<DIFFICULTY IS HARD");
			}
		}

		int dispatch_action = 0;
		if (game_input_curr_button1 != 0 && game_input_prev_button1 == 0) {
			dispatch_action = 1;
		} else if (game_input_curr_key == '\n' || game_input_curr_key == '\r') {
			dispatch_action = 1;
		}

		if (dispatch_action) {
			game_input_curr_key = 0;
			switch(state) {
				case 1:
					state = 0;
					break;
				case 2:
					sou_engine_sethook(4, !sou_engine_gethook(4));
					if ((game_state_flags[0] & 0x40) == 0) {
						msc_memset(game_hide_table, 0, 150);
					}
					break;
				case 3:
					AG(sou_music_enabled) ^= 1;
					break;
				case 4:
					AG(sou_sfx_enabled) ^= 1;
					AG(sou_voice_enabled) ^= 1;
					break;
				case 5:
					AG(anm_text_force_enable) ^= 1;
					break;
				case 6:
					game_input_flipy ^= 1;
					break;
				case 7:
					while (game_input_curr_button1 != 0 && game_exit_loop_now_flag == 0) {
						if (game_input_curr_x > 80) {
							game_snddrv_volume += 5;
							if (game_snddrv_volume > 127) game_snddrv_volume = 127;
						} else if (game_input_curr_x < -80) {
							game_snddrv_volume -= 5;
							if (game_snddrv_volume < 0) game_snddrv_volume = 0;
						}

						sou_drv_set_volume(game_snddrv_volume);
						game_snddrv_volume = sou_drv_get_volume();

						txt_font_printf(NULL, &viewport, 160, 135, 640, 100, "<VOLUME AT %hd PERCENT", (game_snddrv_volume * 100) / 127);

						vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
						AG(anm_anmflags_global) |= 0x8000;

						game_input_curr_key = 0;
						game_switch_task();

						if (clrscr != 0) {
							vid_bitmap_clear(game_postcb_dst);
						}
					}
					break;
				case 8:
					game_difficulty_index++;
					if (game_difficulty_index > 2)
						game_difficulty_index = 0;
					break;
			}
		}

		if (game_input_curr_y <= 80 && game_input_curr_y >= -80) {
			debounce = 3;
		} else {
			debounce++;
			if (debounce > 3) {
				debounce = 0;
				game_input_curr_key = (game_input_curr_y > 0) ? 0x82 : 0x81;
				ctl_mouse_reset();
			}
		}

		if (game_input_curr_key == 0x82) {
			int16_t max_state = (diffchg_en != 0) ? 8 : 7;
			state++;
			if (state > max_state)
				state = 1;
		} else if (game_input_curr_key == 0x81) {
			int16_t max_state = (diffchg_en != 0) ? 8 : 7;
			state--;
			if (state < 1)
				state = max_state;
		}

		if (state != 0) {
			msc_draw_rect(game_postcb_dst, 10, (state * 15) + 29, 300, 15, 223, 320, 199);
		}

		if (game_exit_loop_now_flag != 0 || game_input_curr_key == 'o' || game_input_curr_key == 'O') {
			state = 0;
		}

		vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
		AG(anm_anmflags_global) |= 0x8000;

		game_input_curr_key = 0;
		game_switch_task();
	}

	if (clrscr != 0) {
		vid_bitmap_clear(game_postcb_dst);
		vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
	}

	AG(anm_anmflags_global) &= 0x7FFF;

	if (game_input_curr_key == 0 || game_input_curr_key == 0x1b) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}

	if (clrscr != 0) {
		vid_palette_set(AG(anm_anim_header_current).animpal);
	}

	anm_cmd_pause(0);
	sou_resume();
	game_state_flags[1] &= 0xfe;
}

void game_options_quit_screen(void)
{
	struct anm_rect vp;

	vp.x = 4;
	vp.y = 4;
	vp.w = 312;
	vp.h = 192;
	if ((game_input_curr_key == 0) || (game_input_curr_key == 27)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}
	game_state_flags[1] |= 1;
	sou_pause();
	anm_cmd_pause(1);
	game_switch_task();
	game_switch_task();
	vid_bitmap_clear(game_postcb_dst);
	vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
	vid_palette_set(game_palette);
	while (1) {
		game_switch_task();
		if ((game_exit_loop_now_flag != 0) || (game_input_curr_key != 0))
			break;
		vid_bitmap_clear(game_postcb_dst);
		txt_font_print(NULL, &vp, 160, 100, 640, 100 ,"Quit Game (Y/N)?");
		vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
		AG(anm_anmflags_global) |= 0x8000;
	}
	vid_bitmap_clear(game_postcb_dst);
	vid_blt_to_screen(game_postcb_dst, 4, 4, 4, 4, 316, 196, 320);
	AG(anm_anmflags_global) |= 0x8000;
	game_switch_task();
	AG(anm_anmflags_global) &= 0x7fff;
	if ((game_input_curr_key == 0) || (game_input_curr_key == 27)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}
	vid_palette_set(AG(anm_anim_header_current).animpal);
	anm_cmd_pause(0);
	sou_resume();
	game_state_flags[1] &= ~1;
	if (tolower(game_input_curr_key) == 'y') {
		anm_cmd_quit();
	}
	game_input_curr_key = 0;
}

void game_load_level_resources(char *l2f, char *bf, char *ef, char *bangf, char *lf)
{
	int i = 30;
	while ((DAT_002b5610 != 0) && i--)
		game_switch_task();

	game_lvlres_laser2file = l2f;
	game_lvlres_bankingfile = bf;
	game_lvlres_explodingfile = ef;
	game_lvlres_bangfile = bangf;
	game_lvlres_laserfile = lf;
	while ((game_lvlres_laser2file != 0) || (game_lvlres_bankingfile != 0)
		|| (game_lvlres_explodingfile != 0) || (game_lvlres_bangfile != 0)
		|| (game_lvlres_laserfile != 0)) {
		game_switch_task();
	}
}

void game_level_clear_resources(void)
{
	int i = 30;
	while ((DAT_002b5610 != 0) && i--)
		game_switch_task();

	game_lvlres_need_change = 1;
	while (game_lvlres_need_change != 0)
		game_switch_task();
}

void game_load_sound(char *filename,uint16_t bank)
{
	game_lvlres_sndidx = bank;
	game_lvlres_sndfile = filename;
}

void game_cmd_newanim(char *anmfile, uint16_t anm_flags, int16_t splicestart,
		      int16_t splicedelt, uint16_t difficulty_table_line_index)
{
	int i;

	if (splicestart != 0) {
		if (splicestart < 0) {
			if (AG(anm_anim_frme_cnt) <= game_postcb_currfrme + 7) {
				;
			} else {
				i = game_postcb_currfrme;
			}
		} else {
			if (AG(anm_anim_frme_cnt) <= splicestart + 7) {
				;
			} else {
				i = splicestart;
			}
		}
		AG(anm_anim_frme_cnt) = i + 7;
	}
	game_newanm_filename = anmfile;
	game_newanm_anmflags = anm_flags;
	game_newanm_splice1 = splicestart;
	game_newanm_splice2 = splicedelt;
	game_newanm_posted = 1;
	while ((game_newanm_posted) != 0) {
		game_switch_task();
	}
	while (0 != anm_anim_check()) {
		game_switch_task();
	}

	if (difficulty_table_line_index != 0xffff)
		game_reset_lvldata(difficulty_table_line_index);

}

void game_anm_set_playpos(uint16_t anm_flags, int16_t splice1, int16_t splice2)
{
	game_anmpos_anmflags = anm_flags;
	game_anmpos_splice1 = splice1;
	game_anmpos_splice2 = splice2;
	game_anmpos_updated = 1;
}

void game_update_score(uint8_t *dst, uint8_t *statusbar)
{
	uint8_t dispflag = 0;

	DAT_002b5610 = 0xFF;

	int diff = game_difficulty_index;
	int lvl = game_difficulty_level_index;
	int16_t health = game_player_health;

	if (health < (game_rebltune[diff][lvl].miss * 2) ||
		health < (game_rebltune[diff][lvl].wham * 2) ||
		health < (game_rebltune[diff][lvl].shot * 2))
	{
		if ((game_postcb_currfrme & 7) == 0) {
			dispflag = 1;
		}
	}

	/* looks like every 32 frames there's some bonus health regen */
	if ((game_postcb_currfrme & 0x1F) == 0) {
		if (health >= 0) {
			game_player_score += game_rebltune[diff][lvl].time;
			if (game_player_health < 0x62) {
				game_player_health++;
			}
			dispflag = 1;
		}
	}

	if (dispflag != 0) {
		game_statusbar_score_update_and_display(statusbar);
	}
}
void game_statusbar_score_update_and_display(uint8_t *statusbar)
{
	int m = game_difficulty_index;
	int n = game_difficulty_level_index;
	int p = game_player_health;
	struct anm_rect cr;

	if ((game_player_score_newlife / 10000) < (game_player_score / 10000)) {
		game_player_lives += 1;
		sou_engine_start(game_sound_BONUS, 7);
	}
	game_player_score_newlife = game_player_score;
	if (game_anmres_DISPLAYNUT.membase == NULL) {
		return;
	}

	vid_statusbar_clear_buf(statusbar);
	DAT_002b5611 = 1;
	cr.x = 4;
	cr.y = 0;
	cr.w = 312;
	cr.h = 20;
	anm_resource_render(statusbar, &game_anmres_DISPLAYNUT, 0, 5, 0, &cr, 0x181);
	if (0 < game_player_health) {
		msc_fill_rect(statusbar, 0x92 - game_player_health, 8, game_player_health, 5, 0, 320);
	}

	txt_font_printf(statusbar, &cr, 0x111, 5, 0x180, 100, "<<%06ld", game_player_score);

	if ((-1 < game_player_lives) && (game_player_lives < 5)) {
		msc_fill_rect(statusbar, game_player_lives * 10 + 186, 6, 51 - game_player_lives * 10, 9, 0, 320);
	}

	if (((p < (game_rebltune[m][n].miss * 2)) || (p < (game_rebltune[m][n].wham * 2)) || (p < (game_rebltune[m][n].shot * 2)))
	    && ((game_postcb_currfrme & 8) != 0)) {
		if ((game_rebltune[m][n].miss < p) && (game_rebltune[m][n].shot < p) && (game_rebltune[m][n].wham < p)) {
			txt_font_print_len(statusbar, &cr, 73, 7, 128, 100, 3, "<<[");
		} else {
			txt_font_print_len(statusbar, &cr, 73, 7, 128, 100, 3, "<<\\");
			if (0 == (game_postcb_currfrme & 7)) {
				sou_engine_start(game_sound_KLAXON, 4);
			}
		}
	}
	vid_statusbar_blt(statusbar);
}

/* called from anm_frme_render() when a "GAME" or "GAM2" chunk is encountered in
 * the current FRME.
 */
void game_anm_game_cb(uint8_t *dst, uint8_t *statusbar, struct anm_rect *viewport,
		      uint8_t *anm_hide_table, uint16_t anm_flags, uint32_t gam1,
		      uint32_t gam2, uint32_t gam3, uint32_t gam4,
		      uint32_t gam5, uint32_t gam6, uint32_t gam7)
{
	if (game_state_flags[1] & 0x01)		/* GAME inhibited */
		return;

	if ((game_state_flags[0] & 0020) == 0) {	/* pause test */
		/* TODO: dispatch.. */
	} else {
		if (gam1 < 11) {
			if (gam1 < 7)
				return;
			if (8 < gam1)
				return;
		} else if ((11 < gam1) && (gam1 != 26)) {
			return;
		}
		DAT_002b5610 = 1;		/* game 26 in pause called */
	}
}


int16_t game_get_angle(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t dx = x2 - x1;
	int16_t dy = y1 - y2;

	if (dy < 0) {
		dy = -dy;
		dx = -dx;
	}

	if (dx >= 0) {
		if (dy > 5 * dx)
			return 0;
		if (2 * dy > 3 * dx)
			return 1;
		if (3 * dy > 2 * dx)
			return 2;
		if (9 * dy > 2 * dx)
			return 3;

		return 4;
	} else {
		dx = -dx;
		if (dy > 5 * dx)
			return 0;
		if (2 * dy > 3 * dx)
			return -1;
		if (3 * dy > 2 * dx)
			return -2;
		if (9 * dy > 2 * dx)
			return -3;

		return -4;
	}
}

int16_t game_get_angle_and_anm_flip_flags(int16_t x1, int16_t y1, int16_t x2,
					  int16_t y2, uint16_t *af_X, uint16_t *af_Y)
{
	int16_t dx = x1 - x2;
	int16_t dy = y2 - y1;

	if (dy < 0) {
		dy = -dy;
		*af_Y = ANM_FLAG_FOB_FLIPY; /* 0x4000 */
	} else {
		*af_Y = 0;
	}

	// 2. Determine X-Quadrant (Horizontal Flip)
	if (dx < 0) {
		dx = -dx;
		*af_X = ANM_FLAG_FOB_FLIPX; /* 0x2000 */
	} else {
		*af_X = 0;
	}

	if (dy > 10 * dx) {
		return 0;
	} else if (3 * dy > 10 * dx) {
		return 1;
	} else if (dy > 2 * dx) {
		return 2;
	} else if (4 * dy > 5 * dx) {
		return 3;
	} else if (5 * dy > 4 * dx) {
		return 4;
	} else if (2 * dy > dx) {
		return 5;
	} else if (10 * dy > 3 * dx) {
		return 6;
	} else if (10 * dy > dx) {
		return 7;
	} else {
		return 8;
	}
}

int16_t game_anm_loop_cb(void)
{
	ctl_kbd_query(&game_input_curr_key);
	if (game_hiscore_changed) {
		game_write_hiscore_file();
		game_hiscore_changed = 0;
	}
	if (game_lvlres_need_change) {
		game_lvlres_need_change = 0;
		anm_resource_free(&game_anmres_laser2);
		anm_resource_free(&game_anmres_banking);
		anm_resource_free(&game_anmres_exploding);
		anm_resource_free(&game_anmres_bang);
		anm_resource_free(&game_anmres_laser);
	}
	if (game_lvlres_laser2file) {
		anm_resource_load(&game_anmres_laser2, game_lvlres_laser2file, 0x2d);
		game_lvlres_laser2file = NULL;
	}
	if (game_lvlres_bankingfile) {
		anm_resource_load(&game_anmres_banking, game_lvlres_bankingfile, 0x1b);
		game_lvlres_bankingfile = NULL;
	}
	if (game_lvlres_explodingfile) {
		anm_resource_load(&game_anmres_exploding, game_lvlres_explodingfile, 10);
		game_lvlres_explodingfile = NULL;
	}
	if (game_lvlres_bangfile) {
		anm_resource_load(&game_anmres_bang, game_lvlres_bangfile, 10);
		game_lvlres_bangfile = NULL;
	}
	if (game_lvlres_laserfile) {
		anm_resource_load(&game_anmres_laser, game_lvlres_laserfile, 0x1b);
		game_lvlres_laserfile = NULL;
	}
	if (game_lvlres_sndfile) {
		res_resource_load(game_lvlres_sndfile, &(game_sound[game_lvlres_sndidx]));
		game_lvlres_sndfile = NULL;
	}
	if (game_newanm_posted) {
		int ret = anm_cmd_new(game_newanm_filename, game_newanm_anmflags,
				      game_newanm_splice1, game_newanm_splice2);
		if (ret) {
			anm_cmd_new("OPEN/O1STUB.ANM", game_newanm_anmflags, 0, 0);
		}
		game_newanm_posted = 0;
	}
	if (game_anmpos_updated) {
		anm_cmd_restart(game_anmpos_anmflags, game_anmpos_splice1, game_anmpos_splice2);
		game_anmpos_updated = 0;
	}
	return 0;
}

int16_t game_anm_frme_render_postcb (uint8_t *dst, uint8_t *statusbar,
				     uint16_t currfrme, struct anm_rect *viewport,
				     uint16_t maxfrme, uint32_t frmesize)
{
	game_input_prev_button1 = game_input_curr_button1;
	game_input_prev_button2 = game_input_curr_button2;
	ctl_pointer_query(&game_input_curr_x, &game_input_curr_y,
			  &game_input_curr_button1, &game_input_curr_button2);
	if (game_curr_levelfunc != NULL) {
		if (game_joy_secret_unlock != 0) {
			if ((game_input_curr_key == '-') && (game_player_health != 0)) {
				game_player_health -= 10;
				sou_engine_sethook(1, 1);
				game_input_curr_key = 0;
			}
			if (game_input_curr_key == '+') {
				game_player_health = 100;
				game_input_curr_key = 0;
			}
		}
		game_postcb_dst = dst;
		game_postcb_statusbar = statusbar;
		game_postcb_currfrme = currfrme;
		game_postcb_maxfrme = maxfrme;
		game_postcb_viewport = viewport;
		game_yield();
	}

	game_postcb_currfrme2 = currfrme;
	if ((DAT_002b5610 != 0) && (DAT_002b089e != 0)) {
		DAT_002b089e = DAT_002b5611;
		game_statusbar_score_update_and_display(statusbar);
		DAT_002b5611 = DAT_002b089e;
	}
	DAT_002b089e = DAT_002b5611;
	if (DAT_002b5610 == 0) {
		if (DAT_002b089f != 0)  {
			game_target_currcnt = 0;
			for (int i = 0; i < 10; i++) {
				game_explosion_skipidx[i] = 0;
			}
			fob_camera_projection_disable_randomness();
			sou_engine_sethook(2, 0);
			sou_engine_sethook(3, 0);
			sou_engine_stop(4);
			sou_engine_stop(6);
			sou_engine_stop(5);
			sou_engine_stop(2);
			if ((game_state_flags[1] & 0x01) == 0) {
				anm_cmd_clearscreen();
			}
		}
	} else {
		vid_statusbar_set_changed();
	}

	DAT_002b089f = DAT_002b5610;
	DAT_002b5610 = 0;
	DAT_002b55e6 = 0;
	if (game_input_curr_key < 0x31) {
		if (game_input_curr_key == 0) {
			return 0;
		}
		if (game_input_curr_key != 0x1b) {
			return 0;
		}
	} else {
		if (0x39 < game_input_curr_key) {
			if (game_input_curr_key < 0x61) {
				return 0;
			}
			if ((0x66 < game_input_curr_key) && (game_input_curr_key != 'z')) {
				return 0;
			}
		}
		if (game_joy_secret_unlock == 0) {
			return 0;
		}

	}

	game_exit_loop_now_flag = 1;
	return 0;
}

int16_t game_run(char *animfile, int16_t repeatcnt, anm_frme_postcb_t postcb,
		 int16_t(*loop_cb)(void), anm_frme_gamecb_t game_callback,
		 int8_t *anm_hide_table_ptr, uint16_t initial_anm_flags,
		 uint32_t fle_bufsize)
{
	int i, j, ret;

	i = AG(game_cfg_maxfps);
	if (AG(game_current_statusbar_changed_flag_at_call))
		i = AG(game_cfg_maxfps_1315);

	AG(sys_timer_fps_tickthres) = AG(game_cfg_timerrate) / i;
	AG(sys_cpu_usagecap_val) = AG(sys_cfg_cddrvusage);
	game_clear_jmp2();
	AG(sys_abort_flag) = 0;
	game_anmflags_initial = initial_anm_flags;
	ret = anm_init(NULL, postcb, game_callback, anm_hide_table_ptr, fle_bufsize);
	if (ret != 0) {
		AG(sys_abort_flag) = 0;
		return 0;
	}
	for (i = 0; i < repeatcnt; i++) {
		ret = anm_cmd_new(animfile, 0x420, 0, 0);
		if (ret) {
			anm_terminate();
			AG(sys_abort_flag) = 0;
			return 0;
		}
		sys_timer_init(AG(game_cfg_timerrate));
		while ((AG(anm_streamer_handle) != 0) && (AG(anm_quit_flag) == 0)) {
			fle_streamer_acquire();
			ctl_mou_center();
			if (loop_cb)
				(*loop_cb)();
		}
		sys_timer_terminate();
	}
	anm_terminate();
	AG(sys_abort_flag) = 0;
	return 0;
}

/* XXX: from sys.c */
/* 002b239e */
static const uint16_t const sys_timer_abort_thresholds[9] = { 4, 3, 2, 3, 2, 2, 2, 2, 3 };
/* 002b23b0 */
static const uint16_t const sys_timer_abort_slacks[9] = { 1, 1, 1, 2, 2, 3, 4, 5, 6 };

void game_print_usagecap_string(void)
{
	int i = AG(sys_cpu_usagecap_val);
	uint16_t w2, w3;

	if ((i & 1) == 0) {
		i = i >> 1;
		w3 = sys_timer_abort_slacks[i];
		w2 = sys_timer_abort_thresholds[i];
	} else {
		i = i >> 1;
		w3 = sys_timer_abort_slacks[i] + sys_timer_abort_slacks[i + 1];
		w2 = sys_timer_abort_thresholds[i] + sys_timer_abort_thresholds[i + 1];
	}
	sprintf(AG(sys_config_cdusage_string), "CD-ROM DRIVE: %dX, %d PERCENT CPU",
		AG(game_param_gfxquality), (w3 * 100) / (w2 + w3));
	return;
}

void game_cfg_set_perf_params(uint16_t maxfps, uint16_t timerrate, uint16_t cddrvusage)
{
	if (maxfps) {
		AG(game_cfg_maxfps) = maxfps;
		AG(game_cfg_maxfps_1315) = (maxfps * 13) / 15;
	}
	if (timerrate) {
		AG(game_cfg_timerrate) = timerrate;
	}
	if ((cddrvusage != 0) && (cddrvusage < 10)) {
		AG(sys_cfg_cddrvusage) = cddrvusage - 1;
		AG(sys_cfg_cddrvusagecap_set) = 1;
	}
}

void game_screen_hiscore(void)
{
	/* FIXME */
}

void game_set_next_level(game_level_fn_t func)
{
	game_state_flags[0] = (game_difficulty_index == 2) ? 2 : 0;
	game_curr_levelfunc = func ? func : game_scene_calib_and_init;

	if (game_joy_secret_unlock == 0) {
		game_input_curr_key = 0;
		return;
	}
	switch (game_input_curr_key) {
	case '1': 	game_curr_levelfunc = game_level1_func; break;
	case '2': 	game_curr_levelfunc = game_level2_func; break;
	case '3': 	game_curr_levelfunc = game_level3_func; break;
	case '4': 	game_curr_levelfunc = game_level4_func; break;
	case '5': 	game_curr_levelfunc = game_level5_func; break;
	case '6': 	game_curr_levelfunc = game_level6_func; break;
	case '7': 	game_curr_levelfunc = game_level7_func; break;
	case '8': 	game_curr_levelfunc = game_level8_func; break;
	case '9': 	game_curr_levelfunc = game_level9_func; break;
	case 'a': 	game_curr_levelfunc = game_level10_func; break;
	case 'b': 	game_curr_levelfunc = game_level11_func; break;
	case 'c': 	game_curr_levelfunc = game_level12_func; break;
	case 'd': 	game_curr_levelfunc = game_level13_func; break;
	case 'e': 	game_curr_levelfunc = game_level14_func; break;
	case 'f': 	game_curr_levelfunc = game_level15_func; break;
	default:	game_curr_levelfunc = func; break;
	}
	game_input_curr_key = 0;
}

void game_level_render_introtext(char *line1, char *line2, uint16_t startfrme,
				 uint16_t untilfrme)
{
	while ((game_exit_loop_now_flag == 0)
		&& ((game_postcb_currfrme < game_postcb_maxfrme + -5))) {
		if (game_postcb_currfrme < untilfrme) {
			txt_font_print(NULL, NULL, 160, 10, 0x0200,  game_postcb_currfrme - startfrme, line1);
			txt_font_print(NULL, NULL, 160, 25, 0x0200, (game_postcb_currfrme - startfrme) - 15, line2);
		}
		game_switch_task();
	}
}

void game_level_end_print_summary(uint16_t frmestart, uint16_t frmeend, char *bonus1text,
		char *bonus1text2, uint16_t bonus1val2, uint16_t bonus1_score,
		char *bonus2_text, char *bonus2text2, uint16_t bonus2val2,
		uint16_t bonus2_score, uint16_t game_level_index_new)
{
	char completion_bonus_str[80];
	char bonus1_score_str[80];
	char bonus1_str[80];
	char bonus2_score_str[80];
	char bonus2_str[80];

	if (game_level_index_new != 0) {
		game_curr_levelidx = game_level_index_new;
	}

	int16_t level_bonus = game_rebltune[game_difficulty_index][game_difficulty_level_index].level;
	sprintf(completion_bonus_str, "Completions bonus: %d", level_bonus);

	if (bonus1text != NULL) {
		sprintf(bonus1_score_str, "Bonus: %d", bonus1_score);
		sprintf(bonus1_str, bonus1text2, bonus1val2);
	}

	if (bonus2_text != NULL) {
		sprintf(bonus2_score_str, "Bonus: %d", bonus2_score);
		sprintf(bonus2_str, bonus2text2, bonus2val2);
	}

	while (game_postcb_currfrme < frmeend) {
		txt_font_print(NULL, NULL, 160, 5, 0x200, game_postcb_currfrme - frmestart, "Chapter Complete");

		if (game_postcb_currfrme > frmestart + 15) {
			txt_font_print(NULL, NULL, 160, 25, 0x200, 100, completion_bonus_str);
		}

		if (bonus1text != NULL) {
			if (game_postcb_currfrme > frmestart + 40 && game_postcb_currfrme < frmestart + 70) {
				txt_font_print(NULL, NULL, 160, 50, 0x200, 100, bonus1text);
				txt_font_print(NULL, NULL, 160, 70, 0x200, 100, bonus1_str);
				txt_font_print(NULL, NULL, 160, 90, 0x200, 100, bonus1_score_str);
			}
		}

		if (bonus2_text != NULL) {
			if (game_postcb_currfrme > frmestart + 85 && game_postcb_currfrme < frmestart + 115) {
				txt_font_print(NULL, NULL, 160, 50, 0x200, 100, bonus2_text);
				txt_font_print(NULL, NULL, 160, 70, 0x200, 100, bonus2_str);
				txt_font_print(NULL, NULL, 160, 90, 0x200, 100, bonus2_score_str);
			}
		}

		if (game_level_index_new != 0) {
			if (game_postcb_currfrme > frmestart + 10) {
				txt_font_printf(NULL, NULL, 160, 115, 0x200, 100, "Password: %s", game_level_passwords[game_level_index_new - 1]);
			}
		}
		game_switch_task();
		if ((game_exit_loop_now_flag != 0) || (game_postcb_currfrme >= game_postcb_maxfrme - 5))
			break;
	}

	game_player_score += bonus1_score;
	game_player_score += bonus2_score;
	game_player_score += level_bonus;
}

int16_t game_scene_calib_and_init(void)
{
	msc_memcpy(game_palette, AG(anm_anim_header_current).animpal, 0x300);
	game_palette[0] = 0;
	game_palette[1] = 0;
	game_palette[2] = 0;
	while ((game_exit_loop_now_flag == 0) && (game_postcb_currfrme < game_postcb_maxfrme + -3)) {
		if (game_postcb_currfrme == 3) {
			sys_timer_set_usagecap(1);
		}
		game_print_usagecap_string();
		txt_font_printf(NULL, NULL, 0xa0, 0x28, 640, 0x96, "<%s", "Rebel Assault PC-CDROM V1.7");
		txt_font_printf(NULL, NULL, 0xa0, 0x46, 640, 0x96, "<<%s",	"CALIBRATING, PLEASE WAIT");
		txt_font_printf(NULL, NULL, 0x46, 0x5f,  0x80, 0x96, "<<%s", AG(sys_config_cdusage_string));
		txt_font_printf(NULL, NULL, 0x46, 0x73,  0x80, 0x96, "<<%s", AG(sou_soudrv_namestr));
		txt_font_printf(NULL, NULL, 0x46, 0x7d,  0x80, 0x96, "<<%s", AG(sou_soudrv_cfgstr1));
		txt_font_printf(NULL, NULL, 0x46, 0x87,  0x80, 0x96, "<<%s", AG(sou_soudrv_cfgstr2));
		txt_font_printf(NULL, NULL, 0x82, 0x9b,  0x80, 0x96, "<TIME %d", game_postcb_maxfrme - game_postcb_currfrme);
		game_switch_task();
	}
	sys_timer_set_usagecap(0);
	game_player_health = 0x62;
	game_player_lives = 3;
	game_player_score = 0;
	if (game_exit_loop_now_flag == 0) {
		game_options_joystick_screen();
	}
	game_set_next_level(game_level0_func);
	return 0;
}

int16_t game_level0_func(void)
{
	int16_t game_state;                /* SI */
	int16_t game_passcode_result = 0;  /* Stack[-0x18] local_18 */
	int16_t game_mouse_delay = 0;      /* Stack[-0x1c] local_1c */

	if ((game_input_curr_key == 0) || (game_input_curr_key == 0x1b)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}

	game_state = 0;
	sou_engine_stop_all();
	game_load_level_resources(NULL, NULL, NULL, NULL, NULL);
	game_cmd_newanim("OPEN/O1LOGO.ANM", 0x420, 0, 0, -1);
	game_switch_task();
	game_screen_hiscore();
	game_player_score = 0;

	do {
		/* up-down-left-right  unlocks something */
		if ((game_input_curr_button1 != 0) && (game_input_prev_button1 == 0) && (game_state <= 3)) {
			if (game_state == 0) {
				if (game_input_curr_y < -50)
					game_state++;
			} else if (game_state == 1) {
				if (game_input_curr_y > 50)
					game_state++;
				else
					game_state = 0;
			} else if (game_state == 2) {
				if (game_input_curr_x < -50)
					game_state++;
				else
					game_state = 0;
			} else if (game_state == 3) {
				if (game_input_curr_x > 50)
					game_state++;
				else
					game_state = 0;
			}
		}

		if (game_state == 4) {
			sou_engine_sethook(5, 1);
			game_joy_secret_unlock = 1;
			game_state = 5;
		}
		game_switch_task();

	} while ((game_exit_loop_now_flag == 0) && (game_postcb_currfrme < game_postcb_maxfrme - 3));

	if ((game_input_curr_key == 0) || (game_input_curr_key == 0x1b)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}

	/* Wait for the user to release the button */
	while (game_input_curr_button1 != 0) {
		game_switch_task();
	}

	game_demo_mode = 1;

	/* 2. Attract Mode / Demo Loop */
	while (game_demo_mode != 0) {
		if ((game_input_curr_button1 != 0) || (game_exit_loop_now_flag != 0) || (game_input_curr_key != 0)) {
			break;
		}

		/* Play High Score sequence */
		game_cmd_newanim("OPEN/O1SCORE.ANM", 0x420, 0, 0, -1);
		do {
			if ((game_input_curr_button1 != 0) || (game_exit_loop_now_flag != 0) || (game_input_curr_key != 0)) {
				game_demo_mode = 0;
				break;
			}

			if (game_postcb_currfrme > 20) {
				txt_font_print(NULL, NULL, 0x37, 0x7, 0x80, 0x64, "TOP PILOTS");
			}

			game_state = 0;
			while (game_state < 10) {
				int16_t game_text_y = (game_postcb_currfrme * 2) - (game_state * 16) - 40; /* EAX */
				int16_t game_text_x = (game_text_y * 14) + 25;                             /* EDI */

				txt_font_print(NULL, NULL, 0xa, game_text_x, 0x80, game_text_y, game_hiscore_names[game_state]);

				txt_font_printf(NULL, NULL, 0xd2, game_text_x, 0x84, game_hiscore_difficulty[game_state] + 0x7b,
						"<%ld %c", game_hiscore_points[game_state], game_hiscore_difficulty[game_state] + 0x7b);
				game_state++;
			}

			if ((game_input_curr_key == 'q') || (game_input_curr_key == 'Q')) {
				game_input_curr_key = 0;
			}

			game_switch_task();
		} while ((game_exit_loop_now_flag == 0) && (game_postcb_currfrme < game_postcb_maxfrme - 3));

		if (game_demo_mode == 0) {
			break;
		}

		if ((game_input_curr_button1 != 0) || (game_exit_loop_now_flag != 0) || (game_input_curr_key != 0)) {
			break;
		}

		/* Play Cutscene demo */
		sou_engine_stop_all();
		game_cmd_newanim("OPEN/O1OPEN.ANM", 0x420, 0, 0, -1);

		do {
			if ((game_input_curr_button1 != 0) || (game_exit_loop_now_flag != 0) || (game_input_curr_key != 0)) {
				game_demo_mode = 0;
				break;
			}

			if ((game_input_curr_key == 'q') || (game_input_curr_key == 'Q')) {
				game_input_curr_key = 0;
			}

			game_switch_task();
		} while ((game_exit_loop_now_flag == 0) && (game_postcb_currfrme < game_postcb_maxfrme - 3));
	}

	game_demo_mode = 0;

	if ((game_input_curr_key == 'q') || (game_input_curr_key == 'Q') || (game_exit_loop_now_flag != 0)) {
		game_input_curr_key = 0;
	}

	if ((game_input_curr_key == 0) || (game_input_curr_key == 0x1b)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}

	/* 3. Main Menu Initialization */
	sou_engine_set_volume(0);
	game_cmd_newanim("OPEN/O1OPTION.ANM", 0x420, 0, 0, -1);
	game_switch_task();

	game_state = 1;

	while ((game_state != 0) && (game_exit_loop_now_flag == 0)) {
		txt_font_print(NULL, NULL, 0xa0, 0x1e, 640, 0x64, "MAIN MENU");
		txt_font_print(NULL, NULL, 0xa0, 0x3c, 640, 0x64, "<START NEW GAME");
		txt_font_print(NULL, NULL, 0xa0, 0x4b, 640, 0x64, "<GAME OPTIONS");
		txt_font_print(NULL, NULL, 0xa0, 0x5a, 640, 0x64, "<ENTER PASSCODE");
		txt_font_print(NULL, NULL, 0xa0, 0x69, 640, 0x64, "<CONTINUE DEMO");
		txt_font_print(NULL, NULL, 0xa0, 0x78, 640, 0x64, "<EXIT TO DOS");

		/* Loop the background animation cleanly */
		if (game_postcb_currfrme == game_postcb_maxfrme - 51) {
			game_anm_set_playpos(0x20, game_postcb_maxfrme - 51, 0);
		}

		/* Handle Selection */
		if (((game_input_curr_button1 != 0) && (game_input_prev_button1 == 0)) ||
			(game_input_curr_key == '\n') || (game_input_curr_key == '\r')) {

			game_input_curr_key = 0;
			switch (game_state) {
				case 1: /* START NEW GAME */
					game_state = 0;
					break;
				case 2: /* GAME OPTIONS */
					game_cmd_newanim("OPEN/O1HWARE.ANM", 0x420, 0, 0, -1);
					while (game_postcb_currfrme < 15) {
						game_switch_task();
					}
					game_options_config_screen(0, 1);
					game_cmd_newanim("OPEN/O1OPTION.ANM", 0x420, 0, 0, -1);

					while (game_input_curr_button1 == 0) {
						game_switch_task();
					}
					while (game_input_curr_button1 != 0) {
						game_switch_task();
					}
					break;
				case 3: /* ENTER PASSCODE */
					while (game_input_curr_button1 == 0) {
						game_switch_task();
					}
					game_passcode_result = game_enter_passcode_screen();
					if (game_passcode_result != 0) {
						game_state = 0;
					}
					break;
				case 4: /* CONTINUE DEMO */
					break;
				case 5: /* EXIT TO DOS */
					anm_cmd_quit();
					break;
			}
		} else {
			/* Handle Mouse Deadzone and Debounce */
			if (game_input_curr_y <= 80 && game_input_curr_y >= -80) {
				game_mouse_delay = 3;
			} else {
				game_mouse_delay++;
				if (game_mouse_delay > 3) {
					game_mouse_delay = 0;

					bool is_down = (game_input_curr_y > 0);
					bool is_flipped = (game_input_flipy != 0);
					game_input_curr_key = (is_down != is_flipped) ? 0x82 : 0x81;

					ctl_mouse_reset();
				}
			}

			if (game_input_curr_key == 0x82) {
				game_state++;
				if (game_state > 5) game_state = 1;
				game_input_curr_key = 0;
			} else if (game_input_curr_key == 0x81) {
				game_state--;
				if (game_state < 1) game_state = 5;
				game_input_curr_key = 0;
			}

			/* draw box around selected menu item */
			msc_draw_rect(game_postcb_dst, 50, (game_state * 15) + 0x2c,
				      220, 15, 223, 320, 199);
		}
		game_switch_task();
	}

	/* Menu Fade Out */
	for (game_state = 0; game_state < 16; game_state++) {
		if (game_exit_loop_now_flag != 0)
			break;

		vid_palette_set_intensity(AG(anm_anim_header_current).animpal, (15 - game_state) << 4);

		if (game_postcb_currfrme == game_postcb_maxfrme - 51) {
			game_anm_set_playpos(0x20, game_postcb_maxfrme - 51, 0);
		}
		game_switch_task();
	}

	sou_engine_stop_all();
	sou_engine_set_volume(127);
	game_level_clear_resources();

	game_player_health = 0x62;
	game_player_lives = 3;
	game_player_score = 0;

	if (game_passcode_result == 0) {
		game_set_next_level(game_level1_func);
	} else if (game_passcode_result < 4) {
		game_difficulty_index = game_passcode_result - 1;
		game_exit_loop_now_flag = 1;
		game_set_next_level(game_cutscene1_func);
	} else if (game_passcode_result <= 6) {
		game_difficulty_index = game_passcode_result - 4;
		game_exit_loop_now_flag = 1;
		game_set_next_level(game_cutscene2_func);
	} else if (game_passcode_result < 10) {
		game_difficulty_index = game_passcode_result - 7;
		game_exit_loop_now_flag = 1;
		game_set_next_level(game_cutscene3_func);
	} else if (game_passcode_result <= 12) {
		game_difficulty_index = game_passcode_result - 10;
		game_exit_loop_now_flag = 1;
		game_set_next_level(game_level15_func);
	} else if (game_passcode_result <= 15) {
		game_difficulty_index = game_passcode_result - 13;
		game_exit_loop_now_flag = 1;
		game_set_next_level(game_cutscene4_func);
	}

	return 0;
}

int16_t game_level1_func(void)
{
	return 0;
}

int16_t game_level2_func(void)
{
	return 0;
}

int16_t game_level3_func(void)
{
	return 0;
}

int16_t game_cutscene1_func(void)
{
	uint16_t anm_flags;

	anm_flags = (game_exit_loop_now_flag == 1) ? 0x0420 : 0x0020;
	game_cmd_newanim("CUT1/C1BLOCK.ANM", anm_flags, 0 ,0, -1);
	if ((game_input_curr_key == 0) || (game_input_curr_key == 27)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}
	do {
		game_switch_task();
		if (game_exit_loop_now_flag != 0)
			break;
	} while ((int)game_postcb_currfrme < game_postcb_maxfrme - 1);
	if ((game_input_curr_key == 0) || (game_input_curr_key == 27)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}
	game_cmd_newanim("CUT1/C1DARTH1.ANM", 0x420, 0, 0, -1);
	do {
		game_switch_task();
		if (game_exit_loop_now_flag != 0)
			break;
	} while ((int)game_postcb_currfrme < game_postcb_maxfrme - 1);
	if ((game_input_curr_key == 0) || (game_input_curr_key == 27)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}
	game_cmd_newanim("CUT1/C1C3PO.ANM", 0x420, 0, 0, -1);
	do {
		game_switch_task();
		if (game_exit_loop_now_flag != 0)
			break;
	} while ((int)game_postcb_currfrme < game_postcb_maxfrme - 1);

	game_cmd_newanim("CUT1/C1DARTH2.ANM", 0x420, 0, 0, -1);
	do {
		game_switch_task();
		if (game_exit_loop_now_flag != 0)
			break;
	} while ((int)game_postcb_currfrme < game_postcb_maxfrme - 1);

	game_set_next_level(game_level4_func);

	return 0;
}

int16_t game_level4_func(void)
{
	return 0;
}

int16_t game_level5_func(void)
{
	return 0;
}

int16_t game_level6_func(void)
{
	return 0;
}

int16_t game_cutscene2_func(void)
{
	return 0;
}

int16_t game_level7_func(void)
{
	return 0;
}

int16_t game_level8_func(void)
{
	return 0;
}

int16_t game_level9_func(void)
{
	return 0;
}

int16_t game_level10_func(void)
{
	return 0;
}

int16_t game_cutscene3_func(void)
{
	if ((game_input_curr_key == 0) || (game_input_curr_key == 27)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}
	game_cmd_newanim("CUT3/C3BOOM.ANM", 0x420, 0, 0, -1);
	do {
		game_switch_task();
		if (game_exit_loop_now_flag != 0) break;
	} while (game_postcb_currfrme < game_postcb_maxfrme - 1);
	game_set_next_level(game_level11_func);
	return 0;
}

int16_t game_level11_func(void)
{
	return 0;
}

int16_t game_level12_func(void)
{
	return 0;
}

int16_t game_level13_func(void)
{
	return 0;
}

int16_t game_level14_func(void)
{
	return 0;
}

int16_t game_level15_func(void)
{
	return 0;
}

int16_t game_cutscene4_func(void)
{
	int16_t ret;

	if ((game_input_curr_key == 0) || (game_input_curr_key == 27)) {
		game_exit_loop_now_flag = 0;
		game_input_curr_key = 0;
	}
	game_cmd_newanim("FIN/FNFINAL.ANM", 0x420, 0, 0, -1);
	do {
		game_switch_task();
		if (game_exit_loop_now_flag)
			break;
	} while (game_postcb_currfrme < game_postcb_maxfrme);
	ret = 1;
	while (1) {
		ret = sou_engine_query(0);
		if ((ret == 0) || (game_exit_loop_now_flag != 0))
			break;
		game_switch_task();
	}
	game_set_next_level(game_level0_func);
}
