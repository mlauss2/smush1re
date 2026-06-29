/*
 * SMUSHv1 RE
 *
 * main()
 *
 */

#include "smush.h"

static char * game_fontfiles[3] = { "SYS/TITLFONT.NUT",
						"SYS/TALKFONT.NUT",
						"SYS/TECHFONT.NUT" };
static uint16_t game_fontglyphs[3] = { 122, 122, 122 };

int main(int argc, char **argv)
{
	int ret, debug;

	memset(&anm_globals, 0, sizeof(struct _anm_globals_v1));

	ret = game_init_platform(argc, argv);
	if (ret)
		return ret;

	/* initialize 320x200x8 video mode */
	vid_video_init();
	/* init mouse/joystick */
	ctl_init();
	/* populare font store */
	txt_load_fonts(game_fontfiles, game_fontglyphs, 3);
	if ((argc < 2) || (argv[1][0] != 'd') || (argv[1][1] != 0)) {
		debug = 0;
	} else {
		debug = 1;
	}
	game_main(debug);
	txt_free_fonts();
	ctl_terminate();
	vid_video_terminate();
	return printf("%s", AG(anm_errstr));
}
