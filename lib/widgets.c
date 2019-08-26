/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2019 ZanyXDev <zanyxdev@gmail.com>

 * t_widget base class of widgets ( Button, PushButton, Label 
 */
#include <toaru/widgets.h>



void draw_label(gfx_context_t *ctx, struct TWidget *widget)
{
    int label_width = draw_sdf_string_width(widget->title, widget->font_size, widget->font_type);
    int pos_x = 0 ;
    int pos_y = 0;

    switch (widget->align)
    {
    case AlignLeft:
        /* code */
        break;
    case AlignCenter:
        pos_x = widget->width +  (widget->width - label_width) / 2;
        pos_y = widget->height + (widget->height - widget->font_size) / 2;
        //@ToDo if hiLight add 2 pixel to x nad y coordinates
        break;
    default:
        break;
    }
    draw_sdf_string(ctx, pos_x, pos_y, widget->title, widget->font_size, widget->color, widget->font_type);
}

void draw_push_button(gfx_context_t *ctx, struct TWidget *widget, bool flat)
{

}

void t_widget_draw(gfx_context_t *ctx, struct TWidget *widget){  

    if (!ctx) {
       return;
    }

    if ((widget->height == 0) || (widget->width == 0)) {
        return;
    } 

    switch (widget->types){
    case LABEL:
        draw_label(ctx, widget);
        break;
    case PUSH_BUTTON:
        draw_push_button(ctx, widget, false);
        break;
    default:
        break;
    }
}
