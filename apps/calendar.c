/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2019 ZanyXDev
 *
 * calendar - show a window with a calendar widget
 */
#include <toaru/yutani.h>
#include <toaru/graphics.h>
#include <toaru/decorations.h>
#include <toaru/sdf.h>
#include <toaru/menu.h>

static yutani_t * yctx;
static yutani_window_t * window = NULL;
static gfx_context_t * ctx = NULL;

static int32_t width = 300; //<! window with
static int32_t height = 240; //<! window height
static char *title_str; //<! window title string

static int decor_left_width = 0;
static int decor_top_height = 0;
static int decor_right_width = 0;
static int decor_bottom_height = 0;
static int decor_width = 0;
static int decor_height = 0;

#define APP_VERSION "0.0.1"
#define APP_COPYRIGHT "Copyright 2019 zanyxdev <\033[3mzanyxdev@gmail.com\033[23m>"


static void redraw(void){
	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	draw_fill(ctx, rgb(204, 204, 204));
	window->decorator_flags |= DECOR_FLAG_NO_MAXIMIZE;
	render_decorations(window, ctx, title_str);

	char * day_names = "Su Mo Tu We Th Fr Sa";
	draw_sdf_string(ctx, decor_left_width +10, decor_top_height + 10, day_names, 16, rgb(255, 255, 255), SDF_FONT_THIN);

	flip(ctx);
	yutani_flip(yctx, window);
}

int main(int argc, char *argv[]) {
		int req_center_x, req_center_y;
	

		yctx = yutani_init();
		if (!yctx)		{
			fprintf(stderr, "%s: failed to connect to compositor\n", argv[0]);
			return 1;
		}

		init_decorations();

		struct decor_bounds bounds;
		decor_get_bounds(NULL, &bounds);

		decor_left_width = bounds.left_width;
		decor_top_height = bounds.top_height;
		decor_right_width = bounds.right_width;
		decor_bottom_height = bounds.bottom_height;
		decor_width = bounds.width;
		decor_height = bounds.height;

		window = yutani_window_create_flags(yctx, width + bounds.width, height + bounds.height, YUTANI_WINDOW_FLAG_DIALOG_ANIMATION);
		req_center_x = yctx->display_width / 2;
		req_center_y = yctx->display_height / 2;

	 	title_str ="Calendar widget";

		yutani_window_move(yctx, window, req_center_x - window->width / 2, req_center_y - window->height / 2);
		yutani_window_advertise_icon(yctx, window, title_str, "calendar");
		ctx = init_graphics_yutani_double_buffer(window);

		redraw();

		int playing = 1;
		while (playing)
		{
			yutani_msg_t *m = yutani_poll(yctx);
			while (m)
			{
				if (menu_process_event(yctx, m)){
					redraw();
				}
				free(m);
				m = yutani_poll_async(yctx);
			}
		}
		yutani_close(yctx, window);

		return 0;
}
