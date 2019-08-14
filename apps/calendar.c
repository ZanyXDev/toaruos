/* vim: tabstop=4 shiftwidth=4 noexpandtab
 */
#include <toaru/yutani.h>
#include <toaru/graphics.h>
#include <toaru/decorations.h>
#include <toaru/sdf.h>
#include <toaru/menu.h>

#include <sys/utsname.h>

static yutani_t * yctx;
static yutani_window_t * window = NULL;
static gfx_context_t * ctx = NULL;

static int32_t width = 300;
static int32_t height = 240;
static char *title_str;

static void redraw(void)
{
	struct decor_bounds bounds;
	decor_get_bounds(window, &bounds);

	draw_fill(ctx, rgb(204, 204, 204));

	window->decorator_flags |= DECOR_FLAG_NO_MAXIMIZE;
	render_decorations(window, ctx, title_str);

	flip(ctx);
	yutani_flip(yctx, window);

}

int main(int argc, char *argv[]) {
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

		window = yutani_window_create_flags(yctx, width + bounds.width, height + bounds.height, YUTANI_WINDOW_FLAG_DIALOG_ANIMATION);
		req_center_x = yctx->display_width / 2;
		req_center_y = yctx->display_height / 2;

		yutani_window_move(yctx, window, req_center_x - window->width / 2, req_center_y - window->height / 2);

		title_str = "Calendar APP";
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
