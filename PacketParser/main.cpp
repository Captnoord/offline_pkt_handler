//#include <vld.h>

#include "EvemuPCH.h"
#include "FileParser.h"
#include "ascent_getopt.h"

/* Visual Studio debug stuff
   --in D:\\Games\\CCP\\Dump\\Dump-2009-3-16-12-35-29(PID9944).bin
   --alt --in D:\\Games\\CCP\\EVE\\bulkdata\\15a6.cache

   
   
   //Dump-2009-5-4-17-38-33(PID5424)_bulk_data_login.bin
   Dump-2009-5-14-18-34-1(PID1272).bin
*/

int main(int argc, char ** argv)
{
	// test case for RleModule
	//size_t size = 22;
	//uint8 test_data_source[] = {0xA7, 0x09, 0x85, 0xF1, 0x42, 0x0F, 0x85, 0xB0, 0x93, 0x24, 0x96, 0x62, 0x02, 0x82, 0x10, 0x07, 0x08, 0x05, 0x0A, 0x0A, 0x87, 0x20};

	//size_t guessedSize = 0x100;
	
	/*size_t size = 2;
	uint8 test_data_source[] = {0xB7, 0x02};
	uint8 test_data_dest[1024];
	size_t outsize = 1024;

	// 0x8b 0x09

	RleModule::decode(test_data_source, size, test_data_dest, &outsize);
	//RleModule::zerounpackex((char*)test_data_dest, outsize, (char*)test_data_source, size);
	

	std::vector<uint8> unfuckedup;
	RleModule::UnpackZeroCompressed(test_data_source, size, unfuckedup);

	printf("unpacked mine:\n");
	//HexAsciiModule::print_hexview(stdout, test_data_dest, guessedSize);
	HexAsciiModule::print_hexview(stdout, test_data_dest, outsize+10);
	
	printf("unpacked there's:\n");
	HexAsciiModule::print_hexview(stdout, &unfuckedup[0], unfuckedup.size());

	printf("packed source:\n");
	HexAsciiModule::print_hexview(stdout, test_data_source, size);

	std::vector<uint8> fuckedup;
	size_t dst_len = 0;
	RleModule::PackZeroCompressed(test_data_dest, outsize, fuckedup, dst_len);

	printf("packed repacked:\n");
	HexAsciiModule::print_hexview(stdout, &fuckedup[0], dst_len);
	return 0;*/



	char * in_file_path = NULL; /* required */
	char * out_file_path = NULL; /* optional */
	bool is_in_file_cache = false;

	if (argc <=1)
	{
		printf("Usage: %s [--in <filename>] [--out <filename>] [--alt]\n", argv[0]);
		return 1;
	}

	struct ascent_option longopts[] =
	{
		{ "in",				ascent_required_argument,		NULL,					'i'		},
		{ "out",			ascent_optional_argument,		NULL,					'o'		},
		{ "alt",			ascent_no_argument,				NULL,					'a'		},
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
			case 'a':
				/* the input file is from a alternative source (example: bulk data) */
				is_in_file_cache = true;
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

	//new PyMarshalStringTable;

	DWORD tiet = GetTickCount();

	if (is_in_file_cache == false)
		ParseFile(in_file_path, out_file_path);
	else
		ParseCacheFile(in_file_path, out_file_path);

	printf("File parsing took: %u ms\n", GetTickCount() - tiet);

	delete PyMarshalStringTable::getSingletonPtr();

	if (out_file_path)
		delete [] out_file_path;
	if (in_file_path)
		delete [] in_file_path;

	//sBufferPool.print_stats();

	_CrtDumpMemoryLeaks();

	//system("pause");

	return 0;
}
