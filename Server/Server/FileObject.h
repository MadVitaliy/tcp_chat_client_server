#pragma once

#include <string>
#include <fstream>
#include <sstream>

class FileObject {
private:
	std::string _path;

public:
	FileObject(std::string path);

	bool createFile();
	bool deleteFile();
	std::string readTextFile();
	bool writeTextFile(std::string str);
};

