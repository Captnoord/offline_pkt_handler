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

#include "EvemuPCH.h"
#include "FileParser.h"
#include "ascent_getopt.h"

int main(int argc, char ** argv)
{
	char * in_file_path = NULL;     /* required */
	char * out_file_path = NULL;    /* optional */

	if (argc <=1)
	{
		printf("Usage: %s [--in <filename>] [--out <filename>] \n", argv[0]);
		return 1;
	}

	struct ascent_option longopts[] =
	{
		{ "in",				ascent_required_argument,		NULL,					'i'		},
		{ "out",			ascent_optional_argument,		NULL,					'o'		},
		{ 0, 0, 0, 0 }
	};

	/* assuming we did right click open with */
	if (argc != 2)
	{
		char c;
		while ((c = (char)ascent_getopt_long_only(argc, argv, ":f:", longopts, NULL)) != -1)
		{
			switch (c)
			{
			case 'i':
				/* input filepath was set */
				in_file_path = new char[strlen(ascent_optarg)+1];
				strcpy(in_file_path,ascent_optarg);
				break;
			case 'o':
				/* output filepath was set */
				out_file_path = new char[strlen(ascent_optarg)+1];
				strcpy(out_file_path,ascent_optarg);
				break;
			case 0:
				break;
			default:
				printf("Usage: %s [--in <filename>] opt [--out <filename>] opt [--alt]\n", argv[0]);
				return 1;
			}
		}
	}
	else
	{
		/* paranoid checks I know */
		if (argv[1] == NULL)
			return 1;

		if (*argv[1] == '\0')
			return 1;
		size_t arg_len = strlen(argv[1]);

		if (arg_len < 2)
			return 1;
		
		/* input filepath was set */
		in_file_path = new char[arg_len+1];
		strcpy(in_file_path,argv[1]);
	}

    TimeMeasure stop_watch;
    stop_watch.get_time();

    HandleFile(in_file_path, out_file_path);

	printf("File parsing took: %f ms\n", stop_watch.get_delta_time() * 1000.0);

    // dono if I should delete this one....:P
	//delete PyMarshalStringTable::getSingletonPtr();

	if (out_file_path)
		delete [] out_file_path;
	if (in_file_path)
		delete [] in_file_path;

	//_CrtDumpMemoryLeaks();
	//system("pause");

	return 0;
}
