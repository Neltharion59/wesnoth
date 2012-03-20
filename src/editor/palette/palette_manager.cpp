/* $Id: editor_palettes.cpp 53522 2012-03-13 18:19:16Z fendrin $ */
/*
  Copyright (C) 2003 - 2012 by David White <dave@whitevine.net>
  Part of the Battle for Wesnoth Project http://www.wesnoth.org/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY.

  See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-editor"

#include "palette_manager.hpp"
#include "widgets/widget.hpp"

#include "tooltips.hpp"
#include "editor/action/mouse/mouse_action.hpp"

namespace editor {

palette_manager::palette_manager(editor_display& gui, const size_specs &sizes, const config& cfg
		, mouse_action** active_mouse_action)
		: gui::widget(gui.video()),
		  gui_(gui),
		  size_specs_(sizes),
		  palette_start_(0),
		  mouse_action_(active_mouse_action),
		  terrain_palette_(new terrain_palette(gui, sizes, cfg, active_mouse_action)),
		  unit_palette_(new unit_palette(gui,sizes,cfg,active_mouse_action)),
		  empty_palette_(new empty_palette(gui,sizes,cfg,active_mouse_action))
{
	terrain_palette_->setup(cfg);
	unit_palette_->setup(cfg);
	set_dirty();
}

void palette_manager::set_group(size_t index)
{
	active_palette().set_group(index);
	scroll_top();
}

void palette_manager::adjust_size()
{
	scroll_top();

	SDL_Rect rect = create_rect(size_specs_.palette_x
			, size_specs_.palette_y
			, size_specs_.palette_w
			, size_specs_.palette_h);

	set_location(rect);
	palette_start_ = size_specs_.palette_y;

//TODO
//	rect.y = items_start_;
//	rect.h = space_for_items;
	bg_register(rect);

	(*mouse_action_)->get_palette().adjust_size(size_specs_);

	set_dirty();
}

void palette_manager::scroll_down()
{
	bool scrolled = (*mouse_action_)->get_palette().scroll_down();

	if (scrolled) {

		SDL_Rect rect = create_rect(size_specs_.palette_x
				, size_specs_.palette_y
				, size_specs_.palette_w
				, size_specs_.palette_h);

		bg_restore(rect);
		set_dirty();
	}
}

void palette_manager::scroll_up()
{
	bool scrolled_up = (*mouse_action_)->get_palette().scroll_up();
	if(scrolled_up) {

		SDL_Rect rect = create_rect(size_specs_.palette_x
				, size_specs_.palette_y
				, size_specs_.palette_w
				, size_specs_.palette_h);

		bg_restore(rect);
		set_dirty();
	}
}

void palette_manager::scroll_top()
{
	SDL_Rect rect = create_rect(size_specs_.palette_x
			, size_specs_.palette_y
			, size_specs_.palette_w
			, size_specs_.palette_h);

	active_palette().set_start_item(0);
	bg_restore(rect);
	set_dirty();
}

common_palette& palette_manager::active_palette()
{
	return (*mouse_action_)->get_palette();
}

void palette_manager::scroll_bottom()
{
	unsigned int old_start   = active_palette().num_items();
	unsigned int items_start = active_palette().start_num();
	while (old_start != items_start) {
		old_start = items_start;
		scroll_down();
	}
}

void palette_manager::draw(bool force)
{
	if (!dirty() && !force) {
		return;
	}

	const SDL_Rect &loc = location();

	SDL_Rect palrect;
	palrect.x = loc.x;
	palrect.y = palette_start_;
	palrect.w = size_specs_.palette_w;
	palrect.h = size_specs_.palette_h;
	tooltips::clear_tooltips(palrect);

	active_palette().draw(force);

	update_rect(loc);
	set_dirty(false);
}

void palette_manager::handle_event(const SDL_Event& event) {

	if (event.type == SDL_MOUSEMOTION) {
		// If the mouse is inside the palette, give it focus.
		if (point_in_rect(event.button.x, event.button.y, location())) {
			if (!focus(&event)) {
				set_focus(true);
			}
		}
		// If the mouse is outside, remove focus.
		else {
			if (focus(&event)) {
				set_focus(false);
			}
		}
	}
	if (!focus(&event)) {
		return;
	}
	int mousex, mousey;
	SDL_GetMouseState(&mousex,&mousey);
	const SDL_MouseButtonEvent mouse_button_event = event.button;
	if (mouse_button_event.type == SDL_MOUSEBUTTONDOWN) {
		if (mouse_button_event.button == SDL_BUTTON_LEFT) {
			left_mouse_click(mousex, mousey);
		}
		if (mouse_button_event.button == SDL_BUTTON_RIGHT) {
			right_mouse_click(mousex, mousey);
		}
		if (mouse_button_event.button == SDL_BUTTON_WHEELUP) {
			scroll_up();
		}
		if (mouse_button_event.button == SDL_BUTTON_WHEELDOWN) {
			scroll_down();
		}
		if (mouse_button_event.button == SDL_BUTTON_WHEELLEFT) {
			//TODO
//			set_group( (active_palette().active_group_index() -1) % (groups_.size() -1) );
//			gui_.set_terrain_report(active_group_report());
		}
		if (mouse_button_event.button == SDL_BUTTON_WHEELRIGHT) {
			//TODO
//			set_group( (active_palette().active_group_index() +1) % (groups_.size() -1) );
//			gui_.set_terrain_report(active_group_report());
		}
	}
	if (mouse_button_event.type == SDL_MOUSEBUTTONUP) {
		if (mouse_button_event.button == SDL_BUTTON_LEFT) {
			//TODO What is missing here?
		}
	}
}

void palette_manager::left_mouse_click(const int mousex, const int mousey)
{
	if (
	active_palette().left_mouse_click(mousex, mousey) ) {
		set_dirty();
		gui_.invalidate_game_status();
	}
}

void palette_manager::right_mouse_click(const int mousex, const int mousey)
{
	if (
	active_palette().right_mouse_click(mousex, mousey) ) {
		set_dirty();
		gui_.invalidate_game_status();
	}
}


} //Namespace editor

