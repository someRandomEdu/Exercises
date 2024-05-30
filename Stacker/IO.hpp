#ifndef IO_DEFINED
#define IO_DEFINED

#include <cstdio>	
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

using std::string;

namespace Lib::IO
{	
	struct File final
	{
	private:
		std::FILE *stream;

	public:
		~File()
		{
			if (stream != nullptr)
			{
				std::fclose(stream);
			}
		}
	};

	struct Path final
	{
	private:
		std::vector<string> names;

	public:
	};
}

#endif // !IO_DEFINED
