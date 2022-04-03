#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Core/File/File.hpp"
#include "Core/Log/Log.hpp"
#endif

class File
{
public:
	CPP static void init() {}
	CPP static void readFile(const std::string& path, std::function<void(std::ifstream& file)> callback)
	{
		std::ifstream file;
		file.open(path);

		if(file.good() && !file.fail())
		{
			if (callback)
			{
				callback(file);
			}
		}

		file.close();
	}

	CPP static void writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback)
	{
		std::ofstream file;
		file.open(path);

		if(file.good() && !file.fail())
		{
			if (callback)
			{
				callback(file);
			}
		}

		file.close();
	}
};