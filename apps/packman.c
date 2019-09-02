/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2019 Zanyxdev <zanyxdev@gmail.com>
 *
 * Pac-Man is a maze arcade game developed and released by Namco in 1980.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <toaru/yutani.h>
#include <toaru/graphics.h>
#include <toaru/decorations.h>
#include <toaru/menu.h>
#include <toaru/colors.h>

/* Pointer to graphics memory */
static yutani_t * yctx;
static yutani_window_t * window = NULL;
static gfx_context_t * ctx = NULL;

static int decor_left_width = 0;
static int decor_top_height = 0;
static int decor_right_width = 0;
static int decor_bottom_height = 0;
static int decor_width = 0;
static int decor_height = 0;

static char * title_str = "Pac-Man for ToaruOS";
static char * icon_path = "/usr/share/logo_login.bmp";

#define MIN_WIDTH  224
#define MIN_HEIGHT 288

/**
 * Resize window when asked by the compositor.
 */
static void resize_finish(int w, int h);

static void decors() {
	render_decorations(window, ctx, title_str);
}

void setup_decorations();
void create_window(yutani_t * yctx, int left, int top, int width , int height );

void redraw_window();

int main(int argc, char * argv[]) {

    int left, top, width , height;

	yctx = yutani_init();
	if (!yctx) {
		fprintf(stderr, "%s: failed to connect to compositor\n", argv[0]);
		return 1;
	}

	setup_decorations();

    width = MIN_WIDTH + decor_width;
    height = MIN_HEIGHT + decor_height;

    left= (yctx->display_width - width) / 2;
	top = (yctx->display_height - height)/ 2;

    /*Original display has 224×288  pixels*/
    create_window(yctx, left, top, width , height );
    
    int playing = 1;
	while (playing) {
        yutani_msg_t * m = yutani_poll(yctx);

    }
    yutani_close(yctx, window);

	return 0;
}

static void resize_finish(int w, int h) {

	if (w < MIN_WIDTH || h < MIN_HEIGHT) {
		yutani_window_resize_offer(yctx, window, w < MIN_WIDTH  ? MIN_WIDTH  : w, 
                                                 h < MIN_HEIGHT ? MIN_HEIGHT : h);
		return;
	}

	int width_changed = (window->width != (unsigned int)w);

	yutani_window_resize_accept(yctx, window, w, h);
	reinit_graphics_yutani(ctx, window);

	/* Redraw */
	redraw_window();
	yutani_window_resize_done(yctx, window);

	yutani_flip(yctx, window);
}

void setup_decorations(){
    struct decor_bounds bounds;
    
    /**
     *  Call @func init_decorations() call @func initialize_simple() and
     *  setup @code decor_get_bounds         = get_bounds_simple;
     *  @func static int get_bounds_simple(yutani_window_t * window, struct decor_bounds * bounds)
     *  always @return 0 and don't use yutani_window_t * window
     */
    init_decorations();
    
    decor_get_bounds(NULL, &bounds);

    decor_left_width = bounds.left_width;
	decor_top_height = bounds.top_height;
	decor_right_width = bounds.right_width;
	decor_bottom_height = bounds.bottom_height;
	decor_width = bounds.width;
	decor_height = bounds.height;
	
}


void create_window(yutani_t * yctx, int left, int top, int width , int height ){

    window = yutani_window_create(yctx, width , height );
	yutani_window_move(yctx, window, left, top);

	yutani_window_advertise_icon(yctx, window, title_str, icon_path);

	ctx = init_graphics_yutani(window);

	redraw_window();

	yutani_flip(yctx, window);
}

void redraw_window(){
	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	draw_fill(ctx, CHINESE_SILVER);
	decors();
	flip(ctx);
	yutani_flip(yctx, window);	
}

