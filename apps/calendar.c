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
#define BTN_BACKGROUND premultiply(rgba(0, 0, 0, 150))

static yutani_t *yctx;
static yutani_window_t *window = NULL;
static gfx_context_t *ctx = NULL;

static int32_t main_window_width = 300;  //<! window with
static int32_t main_window_height = 240; //<! window height
static char *title_str;					 //<! window title string

static int decor_left_width = 0;
static int decor_top_height = 0;
static int decor_right_width = 0;
static int decor_bottom_height = 0;
static int decor_width = 0;
static int decor_height = 0;

struct TTKButton _left = {0};
struct TTKButton _center = {0};
struct TTKButton _right = {0};

void set_current_month_year(char *pbuffer)
{
	struct timeval now;
	struct tm *timeinfo;

	// allocated dynamically during the function call. (Can be inefficient with a bad compiler)
	const char *month_names[] = {"January", "February", "March", "April", "May", "June",
								 "July", "August", "September", "October", "November", "December"};

	if (!pbuffer)
		return; // bad input. Let junk deal with junk data.

	/* Get the current time for the clock */
	gettimeofday(&now, NULL);
	timeinfo = localtime((time_t *)&now.tv_sec);

	sprintf(pbuffer, "%s, %d", month_names[timeinfo->tm_mon], timeinfo->tm_year + 1900);
}

void setup_buttons(void)
{
	static char center_title[16];

	set_current_month_year(center_title);

	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	_left.width = BUTTON_WIDTH;
	_left.title = "<";
	_left.height = BUTTON_HEIGHT;
	_left.x = bounds.left_width + BUTTON_PADDING;
	_left.y = bounds.top_height + BUTTON_PADDING;

	_center.width = ctx->width - bounds.right_width - bounds.left_width - BUTTON_WIDTH * 2 - BUTTON_PADDING * 2;
	_center.title = center_title;
	_center.height = BUTTON_HEIGHT;
	_center.x = bounds.left_width + BUTTON_WIDTH + BUTTON_PADDING;
	_center.y = bounds.top_height + BUTTON_PADDING;

	_right.width = BUTTON_WIDTH;
	_right.title = ">";
	_right.height = BUTTON_HEIGHT;
	_right.x = ctx->width - bounds.right_width - BUTTON_WIDTH - BUTTON_PADDING;
	_right.y = bounds.top_height + BUTTON_PADDING;
}

static void redraw(void)
{
	draw_fill(ctx, DIM_GRAY);

	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

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

/* Callback for mouse events */
static void do_mouse_events(struct yutani_msg_window_mouse_event *evt)
{
	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	if (evt->new_y < bounds.top_height)
		return;
	switch (evt->command)
	{
	case YUTANI_MOUSE_EVENT_DOWN:

		break;
	case YUTANI_MOUSE_EVENT_RAISE:
	case YUTANI_MOUSE_EVENT_CLICK:
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	int req_center_x, req_center_y;

	yctx = yutani_init();
	if (!yctx)
	{
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
	fprintf(stderr, "window->height:%ld, window->width:%ld\n", window->height, window->width);
	req_center_x = yctx->display_width / 2;
	req_center_y = yctx->display_height / 2;

	title_str = "Calendar widget";

	yutani_window_move(yctx, window, req_center_x - window->width / 2, req_center_y - window->height / 2);
	yutani_window_advertise_icon(yctx, window, title_str, "calendar");
	ctx = init_graphics_yutani_double_buffer(window);

	fprintf(stderr, "ctx->height:%ld, ctx->width:%ld\n", ctx->height, ctx->width);
	setup_buttons();

	redraw();

	struct TTKButton *_down_button = NULL;
	int playing = 1;

	while (playing)
	{
		/* Respond to Yutani events */
		yutani_msg_t *m = yutani_poll(yctx);
		while (m)
		{
			menu_process_event(yctx, m);
			switch (m->type)
			{
				/* Mouse movement / click */
			case YUTANI_MSG_WINDOW_MOUSE_EVENT:
			{
				struct yutani_msg_window_mouse_event *mouse_event = (void *)m->data;
				if (mouse_event->wid == window->wid)
				{
					int result = decor_handle_event(yctx, m);
					switch (result)
					{
					case DECOR_CLOSE:
						playing = 0;
						break;
					case DECOR_RIGHT:
						/* right click in decoration, show appropriate menu */
						decor_show_default_menu(window, window->x + mouse_event->new_x, window->y + mouse_event->new_y);
						break;
					default:
						/* Other actions */
						break;
					}
					do_mouse_events(mouse_event);
				}
				break;
			}
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
			case YUTANI_MSG_WINDOW_FOCUS_CHANGE:
			{
				struct yutani_msg_window_focus_change *wf = (void *)m->data;
				yutani_window_t *win = hashmap_get(yctx->windows, (void *)wf->wid);
				if (win)
				{
					win->focused = wf->focused;
					redraw();
				}
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
