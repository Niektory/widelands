/*
 * Copyright (C) 2010-2017 by the Widelands Development Team
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

#include "wui/multiplayersetupgroup.h"

#include <memory>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "ai/computer_player.h"
#include "base/i18n.h"
#include "base/log.h"
#include "base/wexception.h"
#include "graphic/graphic.h"
#include "graphic/playercolor.h"
#include "graphic/text_constants.h"
#include "logic/game.h"
#include "logic/game_settings.h"
#include "logic/map_objects/tribes/tribe_descr.h"
#include "logic/map_objects/tribes/tribes.h"
#include "logic/player.h"
#include "logic/widelands.h"
#include "ui_basic/button.h"
#include "ui_basic/checkbox.h"
#include "ui_basic/dropdown.h"
#include "ui_basic/icon.h"
#include "ui_basic/scrollbar.h"
#include "ui_basic/textarea.h"

#define AI_NAME_PREFIX "ai" AI_NAME_SEPARATOR

struct MultiPlayerClientGroup : public UI::Box {
	MultiPlayerClientGroup(UI::Panel* const parent,
	                       Widelands::PlayerNumber id,
	                       int32_t const /* x */,
	                       int32_t const /* y */,
	                       int32_t const w,
	                       int32_t const h,
	                       GameSettingsProvider* const settings)
	   : UI::Box(parent, 0, 0, UI::Box::Horizontal, w, h),
	     type_icon(nullptr),
	     type(nullptr),
	     s(settings),
	     id_(id),
	     save_(-2) {
		set_size(w, h);
		name = new UI::Textarea(this, 0, 0, w - h - UI::Scrollbar::kSize * 11 / 5, h);
		add(name);
		// Either Button if changeable OR text if not
		if (id == settings->settings().usernum) {  // Our Client
			type = new UI::Button(
			   this, "client_type", 0, 0, h, h, g_gr->images().get("images/ui_basic/but1.png"), "");
			type->sigclicked.connect(
			   boost::bind(&MultiPlayerClientGroup::toggle_type, boost::ref(*this)));
			add(type);
		} else {  // just a shown client
			type_icon = new UI::Icon(
			   this, 0, 0, h, h, g_gr->images().get("images/wui/fieldaction/menu_tab_watch.png"));
			add(type_icon);
		}
	}

	/// Switch human players and spectator
	void toggle_type() {
		UserSettings us = s->settings().users.at(id_);
		int16_t p = us.position;
		if (p == UserSettings::none())
			p = -1;

		for (++p; p < static_cast<int16_t>(s->settings().players.size()); ++p) {
			if (s->settings().players.at(p).state == PlayerSettings::State::kHuman ||
			    s->settings().players.at(p).state == PlayerSettings::State::kOpen) {
				s->set_player_number(p);
				return;
			}
		}
		s->set_player_number(UserSettings::none());
	}

	/// Care about visibility and current values
	void refresh() {
		UserSettings us = s->settings().users.at(id_);
		if (us.position == UserSettings::not_connected()) {
			name->set_text((boost::format("<%s>") % _("free")).str());
			if (type)
				type->set_visible(false);
			else
				type_icon->set_visible(false);
		} else {
			name->set_text(us.name);
			if (save_ != us.position) {
				const Image* position_image;
				std::string temp_tooltip;
				if (us.position < UserSettings::highest_playernum()) {
					position_image =
					   playercolor_image(us.position, "images/players/genstats_player.png");
					temp_tooltip =
					   (boost::format(_("Player %u")) % static_cast<unsigned int>(us.position + 1))
					      .str();
				} else {
					position_image = g_gr->images().get("images/wui/fieldaction/menu_tab_watch.png");
					temp_tooltip = _("Spectator");
				}

				// Either Button if changeable OR text if not
				if (id_ == s->settings().usernum) {
					type->set_pic(position_image);
					type->set_tooltip(temp_tooltip);
					type->set_visible(true);
				} else {
					type_icon->set_icon(position_image);
					type_icon->set_tooltip(temp_tooltip);
					type_icon->set_visible(true);
				}
				save_ = us.position;
			}
		}
	}

	UI::Textarea* name;
	UI::Icon* type_icon;
	UI::Button* type;
	GameSettingsProvider* const s;
	Widelands::PlayerNumber const id_;
	int16_t save_;  // saved position to check rewrite need.
};

struct MultiPlayerPlayerGroup : public UI::Box {
	MultiPlayerPlayerGroup(UI::Panel* const parent,
	                       Widelands::PlayerNumber id,
	                       int32_t const /* x */,
	                       int32_t const /* y */,
	                       int32_t const w,
	                       int32_t const h,
	                       GameSettingsProvider* const settings,
	                       NetworkPlayerSettingsBackend* const npsb)
	   : UI::Box(parent, 0, 0, UI::Box::Horizontal, w, h),
	     player(nullptr),
	     init(nullptr),
	     s(settings),
	     n(npsb),
	     id_(id),
	     type_dropdown_(this, 0, 0, 50, 200, h, _("Player Type"), UI::DropdownType::kPictorial),
	     tribes_dropdown_(this, 0, 0, 50, 200, h, _("Tribe"), UI::DropdownType::kPictorial),
	     last_state_(PlayerSettings::State::kClosed) {
		set_size(w, h);
		tribes_dropdown_.set_visible(false);
		tribes_dropdown_.set_enabled(false);
		tribes_dropdown_.selected.connect(
		   boost::bind(&MultiPlayerPlayerGroup::set_tribe_or_shared_in, boost::ref(*this)));

		const Image* player_image = playercolor_image(id, "images/players/player_position_menu.png");
		assert(player_image);
		player = new UI::Icon(this, 0, 0, h, h, player_image);
		add(player);

		type_dropdown_.set_enabled(false);
		type_dropdown_.selected.connect(
		   boost::bind(&MultiPlayerPlayerGroup::set_type, boost::ref(*this)));

		add(&type_dropdown_);
		add(&tribes_dropdown_);
		init = new UI::Button(this, "player_init", 0, 0, w - 4 * h, h,
		                      g_gr->images().get("images/ui_basic/but1.png"), "");
		init->sigclicked.connect(
		   boost::bind(&MultiPlayerPlayerGroup::toggle_init, boost::ref(*this)));
		add(init);
		team = new UI::Button(
		   this, "player_team", 0, 0, h, h, g_gr->images().get("images/ui_basic/but1.png"), "");
		team->sigclicked.connect(
		   boost::bind(&MultiPlayerPlayerGroup::toggle_team, boost::ref(*this)));
		add(team);

		subscriber_ = Notifications::subscribe<NoteGameSettings>([this](const NoteGameSettings& note) {
			if (id_ == note.position) {
				refresh();
			}
		});

		// Init dropdowns
		refresh();
		layout();
	}

	void layout() override {
		const int max_height = g_gr->get_yres() * 3/ 4;
		type_dropdown_.set_height(max_height);
		tribes_dropdown_.set_height(max_height);
		UI::Box::layout();
	}

	/// This will update the game settings for the type with the value
	/// currently selected in the type dropdown.
	void set_type() {
		if (type_dropdown_.has_selection()) {
			const std::string& selected = type_dropdown_.get_selected();
			PlayerSettings::State state = PlayerSettings::State::kComputer;
			if (selected == "closed") {
				state = PlayerSettings::State::kClosed;
			} else if (selected == "open") {
				state = PlayerSettings::State::kOpen;
			} else if (selected == "shared_in") {
				state = PlayerSettings::State::kShared;
			} else {
				if (selected == AI_NAME_PREFIX "random") {
					n->set_player_ai(id_, "", true);
				} else {
					if (boost::starts_with(selected, AI_NAME_PREFIX)) {
						std::vector<std::string> parts;
						boost::split(parts, selected, boost::is_any_of(AI_NAME_SEPARATOR));
						assert(parts.size() == 2);
						n->set_player_ai(id_, parts[1], false);
					} else {
						throw wexception("Unknown player state: %s\n", selected.c_str());
					}
				}
			}
			n->set_player_state(id_, state);
		}
	}

	/// Update the type dropdown from the server settings if the server setting changed.
	/// This will keep the host and client UIs in sync.
	void update_type_dropdown(const PlayerSettings& player_setting) {
		if (type_dropdown_.is_expanded() || type_dropdown_.empty()) {
			return;
		}
		if (player_setting.state == PlayerSettings::State::kHuman) {
			type_dropdown_.set_image(g_gr->images().get("images/wui/stats/genstats_nrworkers.png"));
			type_dropdown_.set_tooltip(_("Human"));
		} else if (player_setting.state == PlayerSettings::State::kClosed) {
			type_dropdown_.select("closed");
		} else if (player_setting.state == PlayerSettings::State::kOpen) {
			type_dropdown_.select("open");
		} else if (player_setting.state == PlayerSettings::State::kShared) {
			type_dropdown_.select("shared_in");
		} else {
			if (player_setting.state == PlayerSettings::State::kComputer) {
				if (player_setting.ai.empty()) {
					type_dropdown_.set_errored(_("No AI"));
				} else {
					if (player_setting.random_ai) {
						type_dropdown_.select(AI_NAME_PREFIX "random");
					} else {
						const ComputerPlayer::Implementation* impl =
							ComputerPlayer::get_implementation(player_setting.ai);
						type_dropdown_.select((boost::format(AI_NAME_PREFIX "%s") % impl->name).str());
					}
				}
			}
		}
	}

	/// Fill the type dropdown
	void rebuild_and_update_type_dropdown(const PlayerSettings& player_setting) {
		if (type_dropdown_.empty()) {
			type_dropdown_.clear();
			// AIs
			for (const auto* impl : ComputerPlayer::get_implementations()) {
				type_dropdown_.add(_(impl->descname), (boost::format(AI_NAME_PREFIX "%s") % impl->name).str(),
				                   g_gr->images().get(impl->icon_filename), false, _(impl->descname));
			}
			/** TRANSLATORS: This is the name of an AI used in the game setup screens */
			type_dropdown_.add(_("Random AI"), AI_NAME_PREFIX "random",
			                   g_gr->images().get("images/ai/ai_random.png"), false, _("Random AI"));

			// Slot state
			type_dropdown_.add(_("Shared in"), "shared_in",
									 g_gr->images().get("images/ui_fsmenu/shared_in.png"), false,
									 _("Shared in"));

			// Do not close a player in savegames or scenarios
			if (!s->settings().savegame && (!s->settings().scenario || s->settings().players.at(id_).closeable)) {
				type_dropdown_.add(_("Closed"), "closed", g_gr->images().get("images/ui_basic/stop.png"),
										 false, _("Closed"));
			}

			type_dropdown_.add(_("Open"), "open", g_gr->images().get("images/ui_basic/continue.png"),
			                   false, _("Open"));

			type_dropdown_.set_enabled(s->can_change_player_state(id_));
		}
		update_type_dropdown(player_setting);
	}

	/// This will update the game settings for the tribe or shared_in with the value
	/// currently selected in the tribes dropdown.
	void set_tribe_or_shared_in() {
		tribes_dropdown_.set_disable_style(s->settings().players[id_].state ==
		                                         PlayerSettings::State::kShared ?
		                                      UI::ButtonDisableStyle::kPermpressed :
		                                      UI::ButtonDisableStyle::kMonochrome);
		if (tribes_dropdown_.has_selection()) {
			if (s->settings().players[id_].state == PlayerSettings::State::kShared) {
				n->set_shared_in(
				   id_, boost::lexical_cast<unsigned int>(tribes_dropdown_.get_selected()));
			} else {
				n->set_tribe(id_, tribes_dropdown_.get_selected());
			}
		}
	}

	/// Toggle through the initializations
	void toggle_init() {
		n->toggle_init(id_);
	}

	/// Toggle through the teams
	void toggle_team() {
		n->toggle_team(id_);
	}

	/// Helper function to cast shared_in for use in the dropdown.
	const std::string shared_in_as_string(Widelands::PlayerNumber shared_in) {
		return boost::lexical_cast<std::string>(static_cast<unsigned int>(shared_in));
	}

	/// Rebuild the tribes dropdown from the server settings. This will keep the host and client UIs in sync.
	void rebuild_tribes_dropdown(const GameSettings& settings) {
		const PlayerSettings& player_setting = settings.players[id_];
		tribes_dropdown_.clear();

		// We need to see the playercolor if setting shared_in is disabled
		tribes_dropdown_.set_disable_style(player_setting.state == PlayerSettings::State::kShared ?
														  UI::ButtonDisableStyle::kPermpressed :
														  UI::ButtonDisableStyle::kMonochrome);

		if (player_setting.state == PlayerSettings::State::kShared) {
			for (size_t i = 0; i < settings.players.size(); ++i) {
				if (i != id_) {
					// Do not add players that are also shared_in or closed.
					const PlayerSettings& other_setting = settings.players[i];
					if (other_setting.state  == PlayerSettings::State::kShared || other_setting.state  == PlayerSettings::State::kClosed) {
						continue;
					}

					const Image* player_image =
						playercolor_image(i, "images/players/player_position_menu.png");
					assert(player_image);
					const std::string player_name =
						/** TRANSLATORS: This is an option in multiplayer setup for sharing
							another player's starting position. */
						(boost::format(_("Shared in Player %u")) % static_cast<unsigned int>(i + 1))
							.str();
					tribes_dropdown_.add(
						player_name, shared_in_as_string(i + 1), player_image, false, player_name);
				}
			}
			int shared_in = 0;
			while (shared_in == id_) {
				++shared_in;
			}
			tribes_dropdown_.select(shared_in_as_string(shared_in + 1));
			tribes_dropdown_.set_enabled(tribes_dropdown_.size() > 1);
		} else {
			{
				i18n::Textdomain td("tribes");
				for (const TribeBasicInfo& tribeinfo : Widelands::get_all_tribeinfos()) {
					tribes_dropdown_.add(_(tribeinfo.descname), tribeinfo.name,
												g_gr->images().get(tribeinfo.icon), false,
												tribeinfo.tooltip);
				}
			}
			tribes_dropdown_.add(pgettext("tribe", "Random"), "random",
										g_gr->images().get("images/ui_fsmenu/random.png"), false,
										_("The tribe will be selected at random"));
			if (player_setting.random_tribe) {
				tribes_dropdown_.select("random");
			} else {
				tribes_dropdown_.select(player_setting.tribe);
			}
		}
		const bool has_access = player_setting.state == PlayerSettings::State::kShared ? s->can_change_player_init(id_) : s->can_change_player_tribe(id_);
		if (tribes_dropdown_.is_enabled() != has_access) {
			tribes_dropdown_.set_enabled(has_access && tribes_dropdown_.size() > 1);
		}
		if (player_setting.state == PlayerSettings::State::kClosed ||
		    player_setting.state == PlayerSettings::State::kOpen) {
			return;
		}
		if (!tribes_dropdown_.is_visible()) {
			tribes_dropdown_.set_visible(true);
		}
	}

	/// Refresh all user interfaces
	void refresh() {
		const GameSettings& settings = s->settings();

		if (id_ >= settings.players.size()) {
			set_visible(false);
			return;
		}

		n->refresh(id_);

		set_visible(true);

		const PlayerSettings& player_setting = settings.players[id_];

		rebuild_and_update_type_dropdown(player_setting);
		rebuild_tribes_dropdown(settings);

		// Trigger update for the other players for shared_in mode when slots open and close
		if (last_state_ != player_setting.state) {
			last_state_ = player_setting.state;
			 for (Widelands::PlayerNumber p = 0; p < s->settings().players.size(); ++p) {
				 if (p != id_) {
					 Notifications::publish(NoteGameSettings(p));
				 }
			 }
		}

		const bool initaccess = s->can_change_player_init(id_);
		if (player_setting.state == PlayerSettings::State::kClosed || player_setting.state == PlayerSettings::State::kOpen) {
			team->set_visible(false);
			team->set_enabled(false);
			tribes_dropdown_.set_visible(false);
			tribes_dropdown_.set_enabled(false);
			init->set_visible(false);
			init->set_enabled(false);
			return;
		} else if (player_setting.state == PlayerSettings::State::kShared) {
			team->set_visible(false);
			team->set_enabled(false);
		} else {
			if (player_setting.team) {
				team->set_title(std::to_string(static_cast<unsigned int>(player_setting.team)));
			} else {
				team->set_title("–");
			}
			team->set_visible(true);
			team->set_enabled(s->can_change_player_team(id_));
		}
		init->set_enabled(initaccess);
		init->set_visible(true);

		if (settings.scenario)
			init->set_title(_("Scenario"));
		else if (settings.savegame)
			/** Translators: This is a game type */
			init->set_title(_("Saved Game"));
		else {
			i18n::Textdomain td("tribes");  // for translated initialisation
			for (const TribeBasicInfo& tribeinfo : settings.tribes) {
				if (tribeinfo.name == player_setting.tribe) {
					init->set_title(
					   _(tribeinfo.initializations.at(player_setting.initialization_index).descname));
					init->set_tooltip(
					   _(tribeinfo.initializations.at(player_setting.initialization_index).tooltip));
					break;
				}
			}
		}
	}

	UI::Icon* player;
	UI::Button* init;
	UI::Button* team;
	GameSettingsProvider* const s;
	NetworkPlayerSettingsBackend* const n;
	Widelands::PlayerNumber const id_;

	UI::Dropdown<std::string>
	   type_dropdown_;  /// Select who owns the slot (human, AI, open, closed, shared-in).
	UI::Dropdown<std::string> tribes_dropdown_;  /// Select the tribe or shared_in player.
	PlayerSettings::State last_state_;  /// The dropdowns for the other slots need updating if this changes

	std::unique_ptr<Notifications::Subscriber<NoteGameSettings>> subscriber_;
};

MultiPlayerSetupGroup::MultiPlayerSetupGroup(UI::Panel* const parent,
                                             int32_t const x,
                                             int32_t const y,
                                             int32_t const w,
                                             int32_t const h,
                                             GameSettingsProvider* const settings,
                                             uint32_t /* butw */,
                                             uint32_t buth)
   : UI::Panel(parent, x, y, w, h),
     s(settings),
     npsb(new NetworkPlayerSettingsBackend(s)),
     clientbox(this, 0, buth, UI::Box::Vertical, w / 3, h - buth),
     playerbox(this, w * 6 / 15, buth, UI::Box::Vertical, w * 9 / 15, h - buth),
     buth_(buth) {
	int small_font = UI_FONT_SIZE_SMALL * 3 / 4;

	// Clientbox and labels
	labels.push_back(new UI::Textarea(
	   this, UI::Scrollbar::kSize * 6 / 5, buth / 3, w / 3 - buth - UI::Scrollbar::kSize * 2, buth));
	labels.back()->set_text(_("Client name"));
	labels.back()->set_fontsize(small_font);

	labels.push_back(new UI::Textarea(
	   this, w / 3 - buth - UI::Scrollbar::kSize * 6 / 5, buth / 3, buth * 2, buth));
	labels.back()->set_text(_("Role"));
	labels.back()->set_fontsize(small_font);

	clientbox.set_size(w / 3, h - buth);
	clientbox.set_scrolling(true);

	// Playerbox and labels
	labels.push_back(new UI::Textarea(this, w * 6 / 15, buth / 3, buth, buth));
	labels.back()->set_text(_("Start"));
	labels.back()->set_fontsize(small_font);

	labels.push_back(new UI::Textarea(this, w * 6 / 15 + buth, buth / 3 - 10, buth, buth));
	labels.back()->set_text(_("Type"));
	labels.back()->set_fontsize(small_font);

	labels.push_back(new UI::Textarea(this, w * 6 / 15 + buth * 2, buth / 3, buth, buth));
	labels.back()->set_text(_("Tribe"));
	labels.back()->set_fontsize(small_font);

	labels.push_back(new UI::Textarea(
	   this, w * 6 / 15 + buth * 3, buth / 3, w * 9 / 15 - 4 * buth, buth, UI::Align::kCenter));
	labels.back()->set_text(_("Initialization"));
	labels.back()->set_fontsize(small_font);

	labels.push_back(new UI::Textarea(this, w - buth, buth / 3, buth, buth, UI::Align::kRight));
	labels.back()->set_text(_("Team"));
	labels.back()->set_fontsize(small_font);

	playerbox.set_size(w * 9 / 15, h - buth);
	multi_player_player_groups.resize(kMaxPlayers);
	for (Widelands::PlayerNumber i = 0; i < multi_player_player_groups.size(); ++i) {
		multi_player_player_groups.at(i) =
		   new MultiPlayerPlayerGroup(&playerbox, i, 0, 0, playerbox.get_w(), buth, s, npsb.get());
		playerbox.add(multi_player_player_groups.at(i));
	}
	subscriber_ = Notifications::subscribe<NoteGameSettings>([this](const NoteGameSettings& /* note */) {
		// Keep track of who is visible
		for (Widelands::PlayerNumber i = 0; i < multi_player_player_groups.size(); ++i) {
			const bool should_be_visible = i < s->settings().players.size();
			const bool is_visible = multi_player_player_groups.at(i)->is_visible();
			if (should_be_visible != is_visible) {
				multi_player_player_groups.at(i)->set_visible(should_be_visible);
			}
		}
	});
	refresh();
}

MultiPlayerSetupGroup::~MultiPlayerSetupGroup() {
}

/**
 * Update display and enabled buttons based on current settings.
 */
void MultiPlayerSetupGroup::refresh() {
	const GameSettings& settings = s->settings();

	// Update / initialize client groups
	if (multi_player_client_groups.size() < settings.users.size()) {
		multi_player_client_groups.resize(settings.users.size());
	}
	for (uint32_t i = 0; i < settings.users.size(); ++i) {
		if (!multi_player_client_groups.at(i)) {
			multi_player_client_groups.at(i) =
			   new MultiPlayerClientGroup(&clientbox, i, 0, 0, clientbox.get_w(), buth_, s);
			clientbox.add(
			   &*multi_player_client_groups.at(i), UI::Box::Resizing::kAlign, UI::Align::kCenter);
		}
		multi_player_client_groups.at(i)->refresh();
	}
}
