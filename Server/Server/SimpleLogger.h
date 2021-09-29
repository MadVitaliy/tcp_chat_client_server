#pragma once
#include <string>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <Windows.h>
#include <TCHAR.H>


class SimpleLogger
{
private:
	std::string path;
	std::string computerName;
	std::string currentTime();
	//std::string computerName();
public:
	SimpleLogger(const std::string &path, const std::string &computerName);
	void writeRegistrationLog(std::string userID, bool succes);
	// succes 0 -- successfull, 1 -- wrong login, 2 -- wrong password;
	void writeSingInLog(std::string userID, int succes);  
	//succes true -- connection open, false -- connection close
	void writeConnectionLog(bool succes);


	//0--success 1--object already exists 2--failed hz
	void createFileLog(int result, std::string who, std::string fileName);
	//0--success 1--object doesnot exist 2--access denied
	void readFileLog(int result, std::string who, std::string fileName);
	//0--success 1--object doesnot exists 2--access denied
	void deleteFileLog(int result, std::string who, std::string fileName);
	//0--success 1--object doesnot exist 2--access denied
	void writeFileLog(int result, std::string who, std::string fileName);

	void changeRightLOG(
		int operation,
		std::string who,
		std::string whom,
		std::string fileName,
		bool access);

};

