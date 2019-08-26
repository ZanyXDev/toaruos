/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2019 Zanyxdev <zanyxdev@gmail.com>
 */

/* This definition will eventually change with the rest of the widget toolkit */
#include <toaru/graphics.h>
#include <toaru/sdf.h>
#include <toaru/icon_cache.h>
#include <stdbool.h>

typedef enum aligment_flags{
	AlignLeft = 0x0001,
	AlignLeading = AlignLeft,
	AlignRight = 0x0002,
	AlignTrailing = AlignRight,
	AlignHCenter = 0x0004,
	AlignJustify = 0x0008,
	AlignAbsolute = 0x0010,
	AlignHorizontal_Mask = AlignLeft | AlignRight | AlignHCenter | AlignJustify | AlignAbsolute,
	AlignTop = 0x0020,
	AlignBottom = 0x0040,
	AlignVCenter = 0x0080,
	AlignBaseline = 0x0100,
	AlignVertical_Mask = AlignTop | AlignBottom | AlignVCenter | AlignBaseline,
	AlignCenter = AlignVCenter | AlignHCenter
} aligment_flags_t;

typedef enum widget_types{
	LABEL,
	PUSH_BUTTON,
	PUSH_BUTTON_FLAT
} widget_types_t;

struct TWidget
{
	int x;
	int y;
	int width;
	int height;
	int font_size; 
	int font_type; // sdf_font
	uint32_t color;
	uint32_t bg_color;
	widget_types_t types;
	aligment_flags_t align;
	char * title;
	void * pixmap;
	int hilight;
};

extern void t_widget_draw(gfx_context_t * ctx, struct TWidget * widget);
