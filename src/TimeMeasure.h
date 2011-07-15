/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2011 The EVEmu Team
	For the latest information visit http://evemu.org
	------------------------------------------------------------------------------------
	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU Lesser General Public License as published by the Free Software
	Foundation; either version 2 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place - Suite 330, Boston, MA 02111-1307, USA, or go to
	http://www.gnu.org/copyleft/lesser.txt.
	------------------------------------------------------------------------------------
	Author:		Captnoord
*/

#ifndef _TIMEMEASURE_H
#define _TIMEMEASURE_H

/**
 * @note change this to performance counters for windows..
 */
class TimeMeasure
{
public:
	TimeMeasure()
	{
		mTimeStamp = GetTickCount();
	}

	void tick()
	{
		mTimeStamp = GetTickCount();
	}

	static void printTimeDiff(TimeMeasure & time)
	{
		DWORD timeDiff = GetTickCount() - time.getTick();
		printf("TimeMeasure diff: %u ms\n", timeDiff);
	}

	uint32 getTick()
	{
		return mTimeStamp;
	}
private:
	DWORD mTimeStamp;
};
#endif // _TIMEMEASURE_H
