/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * This file is game running of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2019 ZanyXDev
 *
 * 2048 is perhaps one of the most addictive mathematical puzzle games.
 * For starters, it is played on a 4 × 4 game board which begins with
 * an initial configuration of two tiles, of value 2 or 4, placed at
 * arbitrary locations on the grid.
 */

#include <toaru/yutani.h>
#include <toaru/graphics.h>
#include <toaru/decorations.h>
#include <toaru/sdf.h>
#include <sys/utsname.h>

static int32_t width = 320;
static int32_t height = 240;

static yutani_t * yctx;
static yutani_window_t * window = NULL;
static gfx_context_t * ctx = NULL;
static sprite_t logo;

static char * title_str;
static char * icon_path;

static int center_x(int x) {
        return (width - x) / 2;
}

static void draw_string(int y, const char * string, int font, uint32_t color) {

        struct decor_bounds bounds;
        decor_get_bounds(window, &bounds);

        draw_sdf_string(ctx, bounds.left_width + center_x(draw_sdf_string_width(string, 16, font)), bounds.top_height + 10 + logo.height + 10 + y, string, 16, color, font);
}

static void moveCellsUp(){

}

static void moveCellsDown(){

}

static void moveCellsLeft(){

}

static void moveCellsRight(){

}

static void redraw(void) {
        static char * test_str = "Test string";
        static char * test2_str = "Test 2string";
        struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	draw_fill(ctx, rgb(204,204,204));
        draw_sprite(ctx, &logo, bounds.left_width + 5, bounds.top_height + 5);
	draw_string(0, test2_str, SDF_FONT_BOLD, rgb(0,0,0));

        window->decorator_flags |= DECOR_FLAG_NO_MAXIMIZE;
	render_decorations(window, ctx, title_str);

	flip(ctx);
	yutani_flip(yctx, window);
}

int main(int argc, char * argv[]) {

        yctx = yutani_init(); // setup connection to compositor
        
        if (!yctx) {
                fprintf(stderr, "%s: failed to connect to compositor\n", argv[0]);
                return 1;
        }

        int req_center_x = yctx->display_width / 2;
        int req_center_y = yctx->display_height / 2;

        init_decorations();

        struct decor_bounds bounds;
        decor_get_bounds(NULL, &bounds);

        window = yutani_window_create(yctx, width + bounds.width, height + bounds.height);
        yutani_window_move(yctx, window, req_center_x - window->width / 2, req_center_y - window->height / 2);

        title_str = "2048 game for ToaruOS";
	yutani_window_advertise(yctx, window, title_str);
        
        ctx = init_graphics_yutani_double_buffer(window);
        
        icon_path = "/usr/share/logo_login.bmp";
        load_sprite(&logo, icon_path);
	logo.alpha = ALPHA_EMBEDDED;

        redraw();
        
        int playing = 1;
	while (playing) {
             yutani_msg_t * m = yutani_poll(yctx); // get event message
             while (m) {
                if (menu_process_event(yctx, m)) {
			redraw();
		}
                switch (m->type) {
                   case YUTANI_MSG_KEY_EVENT:
			{
		 	        struct yutani_msg_key_event * ke = (void*)m->data;
                                if (ke->event.action == KEY_ACTION_DOWN ) {
                                        switch (ke->event.keycode)
                                        {
                                        case KEY_CTRL_ARROW_UP:
                                                moveCellsUp();
                                        break;
                                        case KEY_CTRL_ARROW_DOWN:
                                                moveCellsDown();
                                        break;
                                        case KEY_CTRL_ARROW_LEFT:
                                                moveCellsLeft();
                                        break;
                                        case KEY_CTRL_ARROW_RIGHT:
                                                moveCellsRight();
                                        break;
                                        case 'q':
                                               playing = 0;
                                                break;
                                        
                                        default:
                                                break;
                                        }
                                }			        
		        }
			break;    
                    case YUTANI_MSG_WINDOW_FOCUS_CHANGE:
			{
			  struct yutani_msg_window_focus_change * wf = (void*)m->data;
			  yutani_window_t * win = hashmap_get(yctx->windows, (void*)wf->wid);
			  if (win) {
			      win->focused = wf->focused;
			      redraw();
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

        yutani_close(yctx, window); // close window 
	return 0;
}
