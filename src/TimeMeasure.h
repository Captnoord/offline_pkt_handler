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
 * class TimeMeasure
 *
 * TimeMeasure that is designed kinda like a stop watch.
 *
 * @note change this so it also supports Linux
 */
class TimeMeasure
{
public:
    TimeMeasure() : m_sampled_time(0.0)
	{
        int64 freq;

        // Check if we have a performance counter
        if( QueryPerformanceFrequency( (LARGE_INTEGER *)&freq ) )
        {
            // Counter resolution is 1 / counter frequency
            m_resolution = 1.0 / (double)freq;

            // Set start time for timer
            QueryPerformanceCounter( (LARGE_INTEGER *)&m_start_time );
        }
	}

    double get_time()
    {
        //store so we can easily do something like delta time
        m_sampled_time = _get_time();

        // Calculate the current time in seconds
        return m_sampled_time;
    }

    double get_delta_time()
    {
        return _get_time() - m_sampled_time;
    }

protected:
    ASCENT_INLINE double _get_time()
    {
        double  t;
        int64 t_64;

        QueryPerformanceCounter( (LARGE_INTEGER *)&t_64 );
        t = (double)(t_64 - m_start_time);

        /* Calculate the current time */
        return t * m_resolution;
    }

private:
    double       m_resolution;
    double       m_sampled_time;
    int64        m_start_time;
};
#endif // _TIMEMEASURE_H
