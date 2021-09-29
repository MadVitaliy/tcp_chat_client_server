
#include "FileObject.h"
FileObject::FileObject(std::string path) {
	this->_path = path;
}

bool FileObject::createFile() {
	std::ofstream file(_path); //создать
	if (file.is_open()) {
		file.close();
		return true;
	}
	else
		return false;
}

bool FileObject::deleteFile() {
	return !(bool)std::remove(_path.c_str());
}

std::string FileObject::readTextFile() {
	std::stringstream result;
	std::string raw;

	std::ifstream file(_path, std::fstream::in);
	if (file.is_open()) {
		while (std::getline(file, raw)) {
			result << raw;
			result << '\n';
		}
		file.close();

		return result.str();
	}
	else {
		return "";
	}
}
bool FileObject::writeTextFile(std::string str) {

	std::ofstream file(_path, std::fstream::trunc);
	if (file.is_open()) {
		file << str;
		return true;
	}
	else {
		return false;
	}
}