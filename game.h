/*
 * SMUSHv1 RE
 *
 * game - RA1 game related generic code.
 */

#ifndef _SMUSHV1_GAME_H_
#define _SMUSHV1_GAME_H_


struct rebltune {
	char name[5];
	int16_t roll;
	int16_t lift;
	int16_t slide;
	int16_t drift;
	int16_t snap;
	int16_t miss;
	int16_t wham;
	int16_t shot;
	int16_t kill;
	int16_t time;
	int16_t level;
	int16_t bonus;
	int16_t flags;
};

/* Level function prototypes */
typedef int16_t(*game_level_fn_t)(void);


/* per-platform commandline argument parser and rebltune.txt reader/writer.
 * Original does a hard "exit(0)" when "/?" is specified.  We return 1 here.
 */
int game_init_platform(int argc, char **argv);						/* 002a06bc */

/* deinit. in DOS, this restores the int21h and int23h handlers */
void game_deinit_platform(void);							/* 002a0be4 unreferenced */

void game_set_statusbar_changed(uint8_t v);

void game_cfg_set_sndcard_params(int16_t base, int16_t irq, uint32_t bufsize,
				 int16_t cardtype,int16_t dmachan);

void game_main(int16_t debug);


void game_cfg_read_rebltune_txt(void);
void game_cfg_write_rebltune_txt(void);

void game_write_hiscore_file(void);
void game_read_hiscore_file(void);

void game_switch_task(void);								/* 00292500 */
void game_reset_lvldata(uint16_t diffidx);						/* 002925fa */
void game_enter_passcode_screen(void);							/* 00292687 */
void game_pause(void);									/* 00292a46 */
void game_options_joystick_screen(void);						/* 00292acc */
void game_options_config_screen(void);							/* 00292e4c */
void game_options_quit_screen(void);							/* 00293553 */
void game_load_level_resources(char *l2f, char *bf, char *ef, char *bangf, char *lf);	/* 00293719 */
void game_level_clear_resources(void);							/* 0029379b */
void game_load_sound(char *filename,uint16_t bank);					/* 002937d8 - unused */

void game_cmd_newanim(char *anmfile, uint16_t anm_flags, int16_t splicestart,
		      int16_t splicedelt, uint16_t difficulty_table_line_index);	/* 002937ef */

void game_anm_set_playpos(uint16_t anm_flags, int16_t splice1, int16_t splice2);	/* 002938a1 */

void game_statusbar_score_update_and_display(uint8_t *statusbar);			/* 00293988 */

void game_anm_game_cb(uint8_t *dst, uint8_t *statusbar, struct anm_rect *viewport,
		      uint8_t *anm_hide_table, uint16_t anm_flags, uint32_t gam1,
		      uint32_t gam2, uint32_t gam3, uint32_t gam4,
		      uint32_t gam5, uint32_t gam6, uint32_t gam7);			/* 00293bd0 */


int16_t game_anm_loop_cb(void);								/* 002970ce */
int16_t game_anm_frme_render_postcb (uint8_t *dst, uint8_t *statusbar,
				     uint16_t currfrme, struct anm_rect *viewport,
				     uint16_t maxfrme, uint32_t frmesize);		/* 002972c5 */

int16_t game_run(char *animfile, int16_t repeatcnt, anm_frme_postcb_t postcb,
		 int16_t(*loop_cb)(void), anm_frme_gamecb_t game_callback,
		 int8_t *anm_hide_table_ptr, uint16_t initial_anm_flags,
		 uint32_t fle_bufsize);							/* 0029c672 */

void game_cfg_set_perf_params(uint16_t maxfps, uint16_t timerrate, uint16_t cddrvusage);/* 0029cb0a */

void game_screen_hiscore(void);				/* 002a354f */
void game_set_next_level(game_level_fn_t func);		/* 002a396a */
int16_t game_scene_calib_and_init(void);		/* 002a3e77 */
int16_t game_level0_func(void);				/* 002a4062, intro screen */
int16_t game_level1_func(void);				/* 002a48b2 */
int16_t game_level2_func(void);				/* 002a4f64 */
int16_t game_level3_func(void);				/* 002a5300 */
int16_t game_cutscene1_func(void);			/* 002a558a */
int16_t game_level4_func(void);				/* 002a56f1 */
int16_t game_level5_func(void);				/* 002a5c0c */
int16_t game_level6_func(void);				/* 002a61ea */
int16_t game_cutscene2_func(void);			/* 002a6563 */
int16_t game_level7_func(void);				/* 002a65de */
int16_t game_level8_func(void);				/* 002a6d58 */
int16_t game_level9_func(void);				/* 002a74c7 */
int16_t game_level10_func(void);			/* 002a81fd */
int16_t game_cutscene3_func(void);			/* 002a8762 */
int16_t game_level11_func(void);			/* 002a87c7 */
int16_t game_level12_func(void);			/* 002a8b34 */
int16_t game_level13_func(void);			/* 002a81fd */
int16_t game_level14_func(void);			/* 002a94ff */
int16_t game_level15_func(void);			/* 002a9ac1 */
int16_t game_cutscene4_func(void);			/* 002aa12b, finale and end credits */

#endif
