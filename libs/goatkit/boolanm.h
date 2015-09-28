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
#ifndef BOOLANIM_H_
#define BOOLANIM_H_

class BoolAnim {
private:
	mutable float value;
	mutable float trans_dir;	// transition direction (sign)
	long trans_start;	// transition start time
	long trans_dur;

	long (*get_msec)();

	void update(long tm) const;

public:
	BoolAnim(bool st = false);

	void set_transition_duration(long dur);
	long get_transition_duration() const;

	void set_time_callback(long (*time_func)());

	void set(bool st);

	void change(bool st);
	void change(bool st, long trans_start);

	bool get_state() const;
	bool get_state(long tm) const;

	float get_value() const;
	float get_value(long tm) const;

	// transition direction (-1, 0, 1)
	float get_dir() const;
	float get_dir(long tm) const;

	operator bool() const;	// equivalent to get_state
	operator float() const;	// equivalent to get_value
};

#endif	// BOOLANIM_H_
