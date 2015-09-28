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
#include "boolanm.h"

static long default_get_msec();

BoolAnim::BoolAnim(bool st)
{
	set(st);
	trans_start = 0;
	trans_dur = 500;
	get_msec = default_get_msec;
}

void BoolAnim::update(long tm) const
{
	if(trans_dir == 0.0) return;

	float dt = (tm - trans_start) / 1000.0;
	float t = dt / (trans_dur / 1000.0);

	if(trans_dir > 0.0) {
		value = t;
	} else {
		value = 1.0 - t;
	}

	if(value < 0.0) {
		value = 0.0;
		trans_dir = 0.0;
	} else if(value > 1.0) {
		value = 1.0;
		trans_dir = 0.0;
	}
}

void BoolAnim::set_transition_duration(long dur)
{
	trans_dur = dur;
}

long BoolAnim::get_transition_duration() const
{
	return trans_dur;
}

void BoolAnim::set_time_callback(long (*time_func)())
{
	get_msec = time_func;
}

void BoolAnim::set(bool st)
{
	value = st ? 1.0 : 0.0;
	trans_dir = 0.0;
}

void BoolAnim::change(bool st)
{
	change(st, get_msec());
}

void BoolAnim::change(bool st, long tm)
{
	trans_dir = st ? 1.0 : -1.0;
	trans_start = tm;
}

bool BoolAnim::get_state() const
{
	return get_state(get_msec());
}

bool BoolAnim::get_state(long tm) const
{
	update(tm);

	// if we're not in transition use the value (should be 0 or 1)
	if(trans_dir == 0.0) {
		return value > 0.5;
	}

	// if we're in transition base it on the direction of the transition
	return trans_dir > 0.0;
}

float BoolAnim::get_value() const
{
	return get_value(get_msec());
}

float BoolAnim::get_value(long tm) const
{
	update(tm);
	return value;
}

float BoolAnim::get_dir() const
{
	return get_dir(get_msec());
}

float BoolAnim::get_dir(long tm) const
{
	update(tm);
	return trans_dir;
}

BoolAnim::operator bool() const
{
	return get_state();
}

BoolAnim::operator float() const
{
	return get_value();
}

#ifdef WIN32
#include <windows.h>

static long default_get_msec()
{
	return GetTickCount();
}
#else
#include <sys/time.h>

static long default_get_msec()
{
	static struct timeval tv0;
	struct timeval tv;

	gettimeofday(&tv, 0);
	if(tv0.tv_sec == 0 && tv0.tv_usec == 0) {
		tv0 = tv;
		return 0;
	}
	return (tv.tv_sec - tv0.tv_sec) * 1000 + (tv.tv_usec - tv0.tv_usec) / 1000;
}
#endif
