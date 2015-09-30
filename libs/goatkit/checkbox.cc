/*
GoatKit - a themable/animated widget toolkit for games
Copyright (C) 2014  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "checkbox.h"
#include "boolanm.h"

namespace goatkit {

struct CheckBoxImpl {
	BoolAnim checked;
};

CheckBox::CheckBox()
{
	cbox = new CheckBoxImpl;
	cbox->checked = false;
	cbox->checked.set_transition_duration(60);
}

CheckBox::~CheckBox()
{
	delete cbox;
}

const char *CheckBox::get_type_name() const
{
	return "checkbox";
}


void CheckBox::check()
{
	cbox->checked.change(true);
}

void CheckBox::uncheck()
{
	cbox->checked.change(false);
}

float CheckBox::get_checked() const
{
	return cbox->checked.get_value();
}

bool CheckBox::is_checked() const
{
	return cbox->checked.get_state();
}

void CheckBox::toggle()
{
	if(is_checked()) {
		uncheck();
	} else {
		check();
	}
}

void CheckBox::set_toggle_transition(long msec)
{
	cbox->checked.set_transition_duration(msec);
}

long CheckBox::get_toggle_transition() const
{
	return cbox->checked.get_transition_duration();
}

void CheckBox::on_click()
{
	toggle();

	Event ev;
	ev.type = EV_CHANGE;
	handle_event(ev);
}

}	// namespace goatkit
