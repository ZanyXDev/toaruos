/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2019 ZanyXDev
 *
 * gcal- Show calendar widget with nav bar into GUI window
 *
 */
#include <toaru/yutani.h>
#include <toaru/graphics.h>
#include <toaru/decorations.h>
#include <toaru/sdf.h>

#include <sys/time.h>

#define APP_VERSION   "0.0.1"
#define APP_COPYRIGHT "Copyright 2019 ZanyXDev <\033[3mzanyxdev@gmail.com\033[23m>"

#define CAL_WIDTH 300
#define CAL_HEIGHT 300
#define CAL_BACKGROUND premultiply(rgba(0,0,0,150))

static yutani_t * yctx;
static gfx_context_t * ctx;
static yutani_window_t * window;

static int display_width;
static int display_height;


static int center_x(int x) {
	int c_x = (display_width  - x) ;
	return (c_x > 0) ? c_x / 2 : 0;
}

static int center_y(int y) {
	int c_y = (display_height  - y) ;
	return (c_y > 0) ? c_y / 2 : 0;
}

void redraw(void){
	static int size = 16;
	//draw_fill(ctx, rgb(255,255,255));

	
	draw_rounded_rectangle(ctx,0,0, CAL_WIDTH, CAL_HEIGHT, 10, CAL_BACKGROUND);

	//decors();

	//draw_sdf_string(ctx, 30,30*1, "ABCDEFGHIJKLMNOPQRSTUVWXYZABC", size, rgb(0,0,0), SDF_FONT_THIN);
	//draw_sdf_string(ctx, 30,30*2, "abcdefghijklmnopqrstuvwxyzabc", size, rgb(0,0,0), SDF_FONT_THIN);
	//draw_sdf_string(ctx, 30,30*3, "ABCDEFGHIJKLMNOPQRSTUVWXYZABC", size, rgb(0,0,0), SDF_FONT_BOLD);
	//draw_sdf_string(ctx, 30,30*4, "abcdefghijklmnopqrstuvwxyzabc", size, rgb(0,0,0), SDF_FONT_BOLD);
	//draw_sdf_string(ctx, 30,30*5, "ABCDEFGHIJKLMNOPQRSTUVWXYZABC", size, rgb(0,0,0), SDF_FONT_OBLIQUE);
	//draw_sdf_string(ctx, 30,30*6, "abcdefghijklmnopqrstuvwxyzabc", size, rgb(0,0,0), SDF_FONT_OBLIQUE);
	//draw_sdf_string(ctx, 30,30*7, "ABCDEFGHIJKLMNOPQRSTUVWXYZABC", size, rgb(0,0,0), SDF_FONT_BOLD_OBLIQUE);
	draw_sdf_string(ctx, 30,30*8, "abcdefghijklmnopqrstuvwxyzabc", size, rgb(0,0,0), SDF_FONT_BOLD_OBLIQUE);
}

int main (int argc, char ** argv) {

	yctx = yutani_init(); //!< Init connection to Compositor
	if (!yctx) {
		fprintf(stderr, "%s: failed to connect to compositor\n", argv[0]);
		return 1;
	}
	
	/* For convenience, store the display size */
	display_width  = yctx->display_width;
	display_height = yctx->display_height;
	
	window = yutani_window_create(yctx, CAL_WIDTH, CAL_HEIGHT);
	yutani_window_move(yctx, window, center_x(CAL_WIDTH), center_y(CAL_HEIGHT));
	ctx = init_graphics_yutani(window);
	
	redraw();
	yutani_flip(yctx, window);
	
	int playing = 1;
	while (playing) {
			yutani_msg_t * m = yutani_poll(yctx);
			while (m) {
				switch (m->type) {
					case YUTANI_MSG_KEY_EVENT:
						{
							struct yutani_msg_key_event * ke = (void*)m->data;
							if (ke->event.action == KEY_ACTION_DOWN && ke->event.keycode == 'q') {
								playing = 0;
							}
						}
						break;
					case YUTANI_MSG_WINDOW_MOUSE_EVENT:
						{
							struct yutani_msg_window_mouse_event * me = (void*)m->data;
							if (me->command == YUTANI_MOUSE_EVENT_DOWN && me->buttons & YUTANI_MOUSE_BUTTON_LEFT) {
								yutani_window_drag_start(yctx, window);
							}
						}
						break;
					case YUTANI_MSG_WINDOW_CLOSE:
					case YUTANI_MSG_SESSION_END:
						playing = 0;
						break;
					default:
						break;
				}
				free(m);
				m = yutani_poll_async(yctx);
			}	
			redraw();
			yutani_flip(yctx,window);
	}
	
	yutani_close(yctx, window);
	return 0;
}