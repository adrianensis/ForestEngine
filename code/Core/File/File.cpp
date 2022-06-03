#include "Core/File/File.hpp"
#include "Core/Log/Log.hpp"


void File::init() {

}
void File::readFile(const String& path, std::function<void(std::ifstream& file)> callback)
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

void File::writeFile(const String& path, std::function<void(std::ofstream& file)> callback)
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
