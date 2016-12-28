/*
 * Copyright (C) 2002-2016 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef WL_WUI_MAPVIEW_H
#define WL_WUI_MAPVIEW_H

#include <memory>
#include <queue>

#include <boost/function.hpp>
#include <boost/signals2.hpp>

#include "base/rect.h"
#include "base/vector.h"
#include "logic/widelands_geometry.h"
#include "ui_basic/panel.h"

class GameRenderer;
class InteractiveBase;

/**
 * Implements a view of a map. It is used to render a valid map on the screen.
 */
struct MapView : public UI::Panel {
	struct View {
		// Mappixel of top-left pixel of this MapView.
		Vector2f viewpoint;

		// Current zoom value.
		float zoom;
	};

	// Time in milliseconds since the game was launched. Animations always
	// happen in real-time, not in gametime. Therefore they are also not
	// affected by pause.
	struct TimestampedView {
		uint32_t t;
		View view;
	};

	struct TimestampedMouse {
		uint32_t t;
		Vector2f pixel;
	};

	MapView(UI::Panel* const parent,
	        const int32_t x,
	        const int32_t y,
	        const uint32_t w,
	        const uint32_t h,
	        InteractiveBase&);
	virtual ~MapView();

	/**
	 * Called when the view changed.  'jump' defines if the change should be
	 * considered a "jump" or a smooth scrolling event.
	 */
	// NOCOM(#sirver): change to using 'Transition'
	boost::signals2::signal<void(bool jump)> changeview;

	boost::signals2::signal<void()> fieldclicked;

	enum class Transition { Smooth, Jump };

	// Moves the mouse cursor so that it is directly above the given node. Does
	// nothing if the node is not currently visible on screen.
	void mouse_to_field(const Widelands::Coords& coords, const Transition& transition);
	void mouse_to_pixel(const Vector2i& pixel, const Transition& transition);

	// NOCOM(#sirver): document
	void set_viewpoint(const Vector2f& vp, const Transition& transition);
	void center_on_coords(const Widelands::Coords& coords, const Transition& transition);
	void center_on_map_pixel(const Vector2f& pos, const Transition& transition);

	Vector2f get_viewpoint() const;
	Rectf view_area() const;
	const View& view() const;
	float get_zoom() const;

	// Set the zoom to the new value without changing view_point. For the user
	// the view will perceivably jump.
	void set_zoom(float zoom);

	// Set the zoom to the 'new_zoom'. This keeps the map_pixel that is
	// displayed at 'panel_pixel' unchanging, i.e. the center of the zoom.
	void zoom_around(float new_zoom, const Vector2f& panel_pixel, const Transition& transition);

	// NOCOM(#sirver): document
	bool is_dragging() const;
	bool is_animating() const;
	bool is_visible(const Widelands::Coords& coords);

	// Implementing Panel.
	void draw(RenderTarget&) override;
	bool handle_mousepress(uint8_t btn, int32_t x, int32_t y) override;
	bool handle_mouserelease(uint8_t btn, int32_t x, int32_t y) override;
	bool
	handle_mousemove(uint8_t state, int32_t x, int32_t y, int32_t xdiff, int32_t ydiff) override;
	bool handle_mousewheel(uint32_t which, int32_t x, int32_t y) override;
	bool handle_key(bool down, SDL_Keysym code) override;

protected:
	InteractiveBase& intbase() const {
		return intbase_;
	}

	// Move the view by 'delta_pixels'.
	void pan_by(Vector2i delta_pixels);

private:
	void stop_dragging();

	// Returns the target view of the last entry in 'view_plans_' or (now,
	// 'view_') if we are not animating.
	TimestampedView animation_target_view() const;

	// Returns the target mouse position 'mouse_plans_' or (now,
	// current mouse) if we are not animating.
	TimestampedMouse animation_target_mouse() const;

	// Move the sel to the given mouse position. Does not honour sel freeze.
	void track_sel(const Vector2i& m);

	Vector2f to_panel(const Vector2f& map_pixel) const;
	Vector2f to_map(const Vector2i& panel_pixel) const;

	std::unique_ptr<GameRenderer> renderer_;
	InteractiveBase& intbase_;
	View view_;
	Vector2i last_mouse_pos_;
	bool dragging_;

	// The queue of plans to execute as animations.
	std::deque<std::deque<TimestampedView>> view_plans_;
	std::deque<std::deque<TimestampedMouse>> mouse_plans_;
};

#endif  // end of include guard: WL_WUI_MAPVIEW_H
