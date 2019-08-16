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
#include <toaru/button.h>
#include <toaru/colors.h>
#include <time.h>
#include <sys/time.h>

#define APP_VERSION "0.0.1"
#define APP_COPYRIGHT "Copyright 2019 zanyxdev <\033[3mzanyxdev@gmail.com\033[23m>"

#define BUTTON_HEIGHT 28
#define BUTTON_WIDTH 28
#define BUTTON_PADDING 7

static yutani_t * yctx;
static yutani_window_t * window = NULL;
static gfx_context_t * ctx = NULL;

static int32_t main_window_width = 300; //<! window with
static int32_t main_window_height = 240; //<! window height
static char *title_str; //<! window title string

static int decor_left_width = 0;
static int decor_top_height = 0;
static int decor_right_width = 0;
static int decor_bottom_height = 0;
static int decor_width = 0;
static int decor_height = 0;

struct TTKButton _left = {0};
struct TTKButton _center = {0};
struct TTKButton _right = {0};

const char *month_names[] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
};

void setup_buttons(void){
	struct timeval now;
	struct tm *timeinfo;
	char month_year[140] ={0};

	/* Get the current time for the clock */
	gettimeofday(&now, NULL);
	timeinfo = localtime((time_t *)&now.tv_sec);
	sprintf(month_year, "%s, %d", month_names[timeinfo->tm_mon], timeinfo->tm_year + 1900);
	fprintf(stderr, "%s %d\n", month_year, draw_sdf_string_width(month_year, 16, SDF_FONT_THIN) );

	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	_left.width = BUTTON_WIDTH;
	_left.title = "<";
	_left.height = BUTTON_HEIGHT;
	_left.x = bounds.left_width + BUTTON_PADDING;
	_left.y = bounds.top_height + BUTTON_PADDING;

	_center.width = 140;//draw_sdf_string_width(month_year, 16, SDF_FONT_THIN) + 32;
	_center.title = month_year;
	_center.height = BUTTON_HEIGHT;
	_center.x = bounds.left_width + BUTTON_WIDTH + BUTTON_PADDING*2;
	_center.y = bounds.top_height + BUTTON_PADDING;
	fprintf(stderr, "_center.title:%s\n", _center.title);

	_right.width = BUTTON_WIDTH;
	_right.title = ">";
	_right.height = BUTTON_HEIGHT;
	_right.x = ctx->width - bounds.right_width - BUTTON_WIDTH - BUTTON_PADDING;
	_right.y = bounds.top_height + BUTTON_PADDING;
	fprintf(stderr, "bounds[right_width:%d,bottom_height:%d] Button_Right[x:%d,y:%d]", bounds.right_width, bounds.bottom_height, _right.x, _right.y);
}

static void redraw(void){
	draw_fill(ctx,  DIM_GRAY);

	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	

	//draw_rectangle_solid(ctx, decor_left_width , decor_top_height , decor_width , decor_height , GRAY);

	//char * day_names = "Su Mo Tu We Th Fr Sa";
	//draw_sdf_string(ctx, decor_left_width +10, decor_top_height + 10, day_names, 16, rgb(255, 255, 255), SDF_FONT_THIN);

	ttk_button_draw(ctx, &_left);
	ttk_button_draw(ctx, &_center);
	ttk_button_draw(ctx, &_right);

	window->decorator_flags |= DECOR_FLAG_NO_MAXIMIZE;
	render_decorations(window, ctx, title_str);

	flip(ctx);
	yutani_flip(yctx, window);
}

int in_button(struct TTKButton *button, struct yutani_msg_window_mouse_event *me)
{
	if (me->new_y >= button->y && me->new_y < button->y + button->height)
	{
		if (me->new_x >= button->x && me->new_x < button->x + button->width)
		{
			return 1;
		}
	}
	return 0;
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

		window = yutani_window_create_flags(yctx, main_window_width + bounds.width, main_window_height + bounds.height, YUTANI_WINDOW_FLAG_DIALOG_ANIMATION);
		req_center_x = yctx->display_width / 2;
		req_center_y = yctx->display_height / 2;

	 	title_str ="Calendar widget";

		yutani_window_move(yctx, window, req_center_x - window->width / 2, req_center_y - window->height / 2);
		yutani_window_advertise_icon(yctx, window, title_str, "calendar");
		ctx = init_graphics_yutani_double_buffer(window);

		setup_buttons();

		redraw();

		int playing = 1;
		while (playing) {
			/* Respond to Yutani events */
			yutani_msg_t *m = yutani_poll(yctx);
			while (m) {
				menu_process_event(yctx, m);
				switch (m->type) {
					/* Mouse movement / click */
				case YUTANI_MSG_WINDOW_MOUSE_EVENT:
					//panel_check_click((struct yutani_msg_window_mouse_event *)m->data);
					break;
				case YUTANI_MSG_KEY_EVENT:
					//handle_key_event((struct yutani_msg_key_event *)m->data);
					break;
				break;
				case YUTANI_MSG_RESIZE_OFFER: 
				{
					struct yutani_msg_window_resize *wr = (void *)m->data;
					//resize_finish(wr->width, wr->height);
				}
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
