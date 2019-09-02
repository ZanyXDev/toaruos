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

void key_event_action(struct yutani_msg_key_event * me);

void move_up();
void move_down();
void move_left();
void move_right();
void restart();
void show_score();

int main(int argc, char * argv[]) {

    int left, top, width , height;

	yctx = yutani_init();
	if (!yctx) {
		fprintf(stderr, "%s: failed to connect to compositor\n", argv[0]);
		return 1;
	}

	setup_decorations();

    width = MIN_WIDTH * 2 + decor_width;
    height = MIN_HEIGHT * 2 + decor_height;

    left= (yctx->display_width - width) / 2;
	top = (yctx->display_height - height)/ 2;

    /*Original display has 224×288  pixels*/
    create_window(yctx, left, top, width , height );
    
    int playing = 1;
	while (playing) {
        yutani_msg_t * m = yutani_poll(yctx);
		while (m) {
			if (menu_process_event(yctx, m)) {
				/* just decorations should be fine */
				decors();
				flip(ctx);
				yutani_flip(yctx, window);
			}
			switch (m->type){
				case YUTANI_MSG_KEY_EVENT:
					{
						struct yutani_msg_key_event * key_event = (void*)m->data;
						key_event_action( key_event );
					}
					break;
				case YUTANI_MSG_RESIZE_OFFER:
					{
						struct yutani_msg_window_resize * wr = (void*)m->data;
						if (wr->wid == window->wid) {
							resize_finish(wr->width, wr->height);
						}
					}
					break;	
				case YUTANI_MSG_WINDOW_MOUSE_EVENT:
					{
						fprintf(stderr, "\nYUTANI_MSG_WINDOW_MOUSE_EVENT");
						
						struct yutani_msg_window_mouse_event * me = (void*)m->data;
						int result = decor_handle_event(yctx, m);
						switch (result) {
							case DECOR_CLOSE:
								playing = 0;
								break;
							case DECOR_RIGHT:
								/* right click in decoration, show appropriate menu */
								decor_show_default_menu(window, window->x + me->new_x, window->y + me->new_y);
								break;
							default:
								/* Other actions */
								break;
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

	draw_fill(ctx, DIM_GRAY);
	//window->decorator_flags |= DECOR_FLAG_NO_MAXIMIZE;
	decors();

	flip(ctx);
	yutani_flip(yctx, window);

}

void key_event_action(struct yutani_msg_key_event * ke){
	if (ke->event.action != KEY_ACTION_DOWN) return;

	switch (ke->event.keycode){
		case 'w':
			move_up();
			break;	
		case 's':
			move_down();
			break;
		case 'a':
			move_left();
			break;
		case 'd':
			move_right();
			break;
		case 'r':
			if (ke->event.modifiers & YUTANI_KEY_MODIFIER_CTRL ) {
				restart();					
			}			
			break;
		case 't':
			if (ke->event.modifiers & YUTANI_KEY_MODIFIER_CTRL ) {
				show_score();				
			}						
			break;	
		default:
			break;
		}
}

void move_up(){
	fprintf(stderr,"\nkey pressed W");
}
void move_down(){
	fprintf(stderr,"\nkey pressed S");
}
void move_left(){
	fprintf(stderr,"\nkey pressed A");
}
void move_right(){
fprintf(stderr,"\nkey pressed D");
}
void restart(){
fprintf(stderr,"\nkey pressed CTRL-R");
}
void show_score(){
fprintf(stderr,"\nkey pressed CTRL-T");
}