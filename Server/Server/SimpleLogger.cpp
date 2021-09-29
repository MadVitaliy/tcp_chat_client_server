#include "SimpleLogger.h"

SimpleLogger::SimpleLogger(const std::string &path, const std::string &computerName) {
	this->path = path;
	this->computerName = computerName;
}

std::string SimpleLogger::currentTime() {
	// current date/time based on current system
	time_t now = time(0);
	// convert now to string form
	char timeBuffer[50] = { '\0' };
	ctime_s(timeBuffer, sizeof(timeBuffer), &now);
	//write date and time
	std::string resultTime(timeBuffer);
	return resultTime.substr(0, resultTime.length()-1);
}


void SimpleLogger::writeRegistrationLog(std::string userID, bool succes) {

	std::ofstream logFile(path, std::fstream::app);
	
	if (logFile.is_open())
	{
		//write date and time
		logFile << currentTime() << ": ";
		//write userID
		logFile << "REGISTRATION with login \""<< userID << "\": ";
		//write operation succes
		if(succes)
			logFile << "SUCCESFULL ";
		else
			logFile << "FAILD ";

		//write computername
		logFile << "[ " << computerName << " ]"<< std::endl;
		//GetComputerNameW(name, 255);
		logFile.close();
	}
}

// succes 0 -- successfull, 1 -- wrong login, 2 -- wrong password;
void SimpleLogger::writeSingInLog(std::string userID, int succes) {
	std::ofstream logFile(path, std::fstream::app);

	if (logFile.is_open())
	{
		//write date and time
		logFile << currentTime() << ":";
		//write userID
		logFile << "SING IN with login \"" << userID << "\": ";
		//write operation succes
		if (succes == 0)
			logFile << "SUCCESFULL ";
		else if (succes == 1)
			logFile << "FAILD: wrong login ";
		else if (succes == 2)
			logFile << "FAILD: wrong password ";

		//write computername
		logFile << "[ " << computerName << " ]" << std::endl;
		//GetComputerNameW(name, 255);
		logFile.close();
	}
}

//succes true -- connection open, false -- connection close
void SimpleLogger::writeConnectionLog(bool succes) {
	std::ofstream logFile(path, std::fstream::app);
	if (logFile.is_open())
	{
		logFile << "CONNECTION with [ " << computerName << " ] ";
		if (succes) 
			logFile << "OPENED";
		else
			logFile << "CLOSED";

		logFile << std::endl;
		logFile.close();
	}
}


//0--success 1--object already exists 2--failed hz
void SimpleLogger::createFileLog(int result, std::string who, std::string fileName) {
	std::ofstream logFile(path, std::fstream::app);
	if (logFile.is_open()) {
		logFile << currentTime() << ": ";
		logFile << "user \"" << who << "\" ";

		if (result == 0) {
			logFile << " created file successfully";
		}
		else if (result == 2) {
			logFile << "could not create file -- ta voobsche hz chogo";
		}
		else {
			logFile << "could not create file -- nsf or smelse";
		}
		logFile << "[ " << computerName << " ]" << std::endl;
		//GetComputerNameW(name, 255);
		logFile.close();
	}
}

//0--success 1--object doesnot exist 2--access denied
void SimpleLogger::readFileLog(int result, std::string who, std::string fileName) {
	std::ofstream logFile(path, std::fstream::app);
	if (logFile.is_open()) {
		logFile << currentTime() << ": ";
		logFile << "user \"" << who << "\" ";

		if (result == 0) {
			logFile << " read file successfully";
		}
		else if (result == 2) {
			logFile << "could not read file -- access denied";
		}
		else {
			logFile << "could not read file -- nsf or smelse";
		}
		logFile << "[ " << computerName << " ]" << std::endl;
		//GetComputerNameW(name, 255);
		logFile.close();
	}
}

//0--success 1--object doesnot exists 2--failed hz
void SimpleLogger::deleteFileLog(int result, std::string who, std::string fileName) {
	std::ofstream logFile(path, std::fstream::app);
	if (logFile.is_open()) {
		logFile << currentTime() << ": ";
		logFile << "user \"" << who << "\" ";

		if (result == 0) {
			logFile << " delete file successfully";
		}
		else if (result == 2) {
			logFile << "could not delete file -- access denied";
		}
		else {
			logFile << "could not delete file -- nsf or smelse";
		}
		logFile << "[ " << computerName << " ]" << std::endl;
		//GetComputerNameW(name, 255);
		logFile.close();
	}
}

//0--success 1--object doesnot exist 2--access denied
void SimpleLogger::writeFileLog(int result, std::string who, std::string fileName) {
	std::ofstream logFile(path, std::fstream::app);
	if (logFile.is_open()) {
		logFile << currentTime() << ": ";
		logFile << "user \"" << who << "\" ";
		
		if (result == 0) {
			logFile << " write file successfully";
		}
		else if (result == 2) {
			logFile << "could not write file -- access denied";
		}
		else {
			logFile << "could not write file -- nsf or smelse";
		}
		logFile << "[ " << computerName << " ]" << std::endl;
		//GetComputerNameW(name, 255);
		logFile.close();
	}
}


void SimpleLogger::changeRightLOG(
	int operation,
	std::string who,
	std::string whom,
	std::string fileName,
	bool access) {

	std::ofstream logFile(path, std::fstream::app);

	if (logFile.is_open())
	{
		logFile << currentTime() << ": ";

		logFile << "user \"" << who <<"\" ";

		if(!access)
			logFile << "could not ";

		switch (operation)
		{
		case 0:
			logFile << "set visble";
			break;
		case 1:
			logFile << "set read";
			break;
		case 2:
			logFile << "set change";
			break;
		case 3:
			logFile << "set delete";
			break;
		case 4:
			logFile << "set changeRight";
			break;
		case 5:
			logFile << "delete visble";
			break;
		case 6:
			logFile << "delete read";
			break;
		case 7:
			logFile << "delete change";
			break;
		case 8:
			logFile << "delete delete";
			break;
		case 9:
			logFile << "delete changeRight";
			break;
		default:
			break;
		}

		logFile << " for user \"" << whom << "\" ";
		logFile << "to file \"" << fileName << "\" ";
		logFile << "[ " << computerName << " ]" << std::endl;

		logFile.close();
	}
}