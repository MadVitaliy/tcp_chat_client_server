#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <sstream>
#include <windows.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

#define CONNECT_TO_SERVER

enum Narrow {
	N_Up,
	N_Dowm,
	N_Left,
	N_Right,
	Enter,
	Selse  // something else
};

void setCursor(int x, int y) {
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void drawMarker() {
	_putch('<');
	_putch('-');
	_putch('-');
}

void eraseMarker() {
	for (int i = 0; i < 3; i++) {
		_putch('\b');
		_putch(' ');
		_putch('\b');
	}
}

Narrow getNarrow() {
	char c1 = static_cast<char>(_getch()); 
	//std::cout << (int)c1;
	char c2;
	if (c1 == -32) {
		c2 = static_cast<char>(_getch());
		//std::cout << (int)c2;
		switch (c2)
		{
		case 72:
			return Narrow(N_Up);
			break;
		case 80:
			return Narrow(N_Dowm);
			break;
		case 75:
			return Narrow(N_Left);
			break;
		case 77:
			return Narrow(N_Right);
			break;
		default:
			break;
		}
	}
	else if (c1 == '\r') {
		return Narrow(Enter);
	}

	return Narrow(Selse);
}

int menu(std::vector<std::string> &list) {
	int maxLenghth = 0;
	
	system("CLS");
	for (std::string item : list) {
		std::cout << item << std::endl;

		if (item.length() > maxLenghth) 
			maxLenghth = item.length();
	}

	Narrow narrow = Selse;
	const int gap = 3;
	int xOfset = maxLenghth + gap;
	int listLength = list.size();
	int iterator = 0;
	int lastItaratorValue = -1;
	bool choosed = false;
	do {
		setCursor(xOfset, iterator);
		drawMarker();
		narrow = getNarrow();
		switch (narrow)
		{
		case N_Up:
			if (iterator > 0) {
				iterator--;
				eraseMarker();
			}
			break;
		case N_Dowm:
			if (iterator < listLength - 1) {
				iterator++;
				eraseMarker();
			}
			break;
		case Enter:
			choosed = true;
			break;
		default:
			break;
		}
		//std::cout << iterator;
	} while (!choosed);
	system("CLS");
	return iterator;
}

std::string getPassword(int pas_len) {
	char c;
	 //for VISUAL STUDIO

	char *password = new char[pas_len];
	int i = 0;

	while (true) {
		if (i <= pas_len - 1) {
			c = static_cast<char>(_getch());
			if (c == 8) {
				_putch('\b');
				_putch(' ');
				_putch('\b');
				if (i != 0)
					i--;
			}
			else if (c == '\r') {
				_putch('\n');
				break;
			}
			else {
				if (i < pas_len - 1) {
					password[i] = c;
					_putch('*');
					i++;
				}
			}
		}
	}
	password[i] = '\0';
	std::string result(password);
	delete[] password;
	return result;
}

void getLogPass(std::string &login, std::string &password) {
	std::cout << "Login: " << std::endl;
	std::getline(std::cin, login);
	std::cout << "Password: " << std::endl;
	password = getPassword(20);
}

void pressAnyButton() {
	std::cout << "Press any button to continue" << std::endl;
	_getch();
	//prev two raw wil be raplaced with system("pause");
}

SOCKET Connection;

enum Packet {
	P_CollectUserData,
	P_Registration,
	P_SingUp,
	P_CloseConnection,
	P_Eror,
	P_CreateFile,
	P_ReadFile,
	P_DeleteFile,
	P_WriteFile,
	P_GetFile,
	P_GetListOfFiles,
	P_GetUsers,
	P_ChangeRight
};

void sendInt(int toSend) {
	send(Connection, (char*)&toSend, sizeof(int), NULL);
	Sleep(5);
}

void sendString(std::string toSend) {
	int toSend_size = toSend.size();
	sendInt(toSend_size);
	send(Connection, toSend.c_str(), toSend_size, NULL);
	Sleep(10);
}

std::string recvString() {
	int answer_size;
	std::string answer;
	//recieve answer size
	recv(Connection, (char*)&answer_size, sizeof(int), NULL);
	if (answer_size != 0) {
		char *c_answer = new char[answer_size + 1];
		c_answer[answer_size] = '\0';
		//recieve answer
		recv(Connection, c_answer, answer_size, NULL);
		//save into string
		answer = std::string(c_answer);
		delete[] c_answer;
	}
	return answer;
}

void sendLogPass(std::string login, std::string password) {
	sendString(login);
	sendString(password);
}


std::string getComputerName() {
	char buffer[256] = { '\0' };
	unsigned long size = 256;
	GetComputerNameA(buffer, &size);
	return std::string(buffer);
}

std::vector<std::pair<uint8_t, std::string>> getListOfFiles(std::string login) {

	Packet packet = P_GetListOfFiles;
	//send packet type
	send(Connection, (char*)&packet, sizeof(Packet), NULL);

	//send login
	sendString(login);

	std::string answer = recvString();
	std::vector<std::pair<uint8_t, std::string>> result;
	std::pair<uint8_t, std::string> tempPair;
	//std::cout << "String answer" << std::endl;
//	std::cout << answer << std::endl << std::endl;

	std::stringstream parse;
	parse.str(answer);

	std::string tempstr1;
	std::string tempstr2;
	int rights;

	
	while (true) {

		if (std::getline(parse, tempstr1, ' ')) {
			if (std::getline(parse, tempstr2, '\n')) {
				rights = std::stoi(tempstr1);
				tempPair = std::make_pair((uint8_t)rights, tempstr2);
				result.push_back(tempPair);

				//std::cout << "rights: ";
				//std::cout << rights << std::endl;
				//std::cout << "tempstr2: ";
				//std::cout << tempstr2 << std::endl << std::endl;
			}
			else 
				break;
		}
		else
			break;
	}
	return result;
}

const uint8_t AM_OWNER = 0x01;                  //0000 0001
const uint8_t AM_VIEWRIGHT = 0x02;              //0000 0010
const uint8_t AM_READRIGHT = 0x04;              //0000 0100
const uint8_t AM_CHANGERIGHT = 0x08;            //0000 1000
const uint8_t AM_DELETRIGHT = 0x10;             //0001 0000
const uint8_t AM_CHANGEACCESSRIGHT = 0x20;      //0010 0000

bool isOwner(uint8_t rights) {return (rights & AM_OWNER);}
bool isVisible(uint8_t rights) {return (rights & AM_VIEWRIGHT);}
bool canRead(uint8_t rights) {return (rights & AM_READRIGHT);}
bool canChange(uint8_t rights) {return (rights & AM_CHANGERIGHT);}
bool canDelete(uint8_t rights) {return (rights & AM_DELETRIGHT);}
bool canChangeAccess(uint8_t rights) {return (rights & AM_CHANGEACCESSRIGHT);}



std::string createFilenameAndRightsString(std::pair<uint8_t, std::string> pair) {
	std::stringstream result;

	result << pair.second << "  ";
	if (isOwner(pair.first)) {
		result << "OWNER";
	}
	else {
		if (canRead(pair.first)) {
			result << "-rd ";
		}
		if (canChange(pair.first)) {
			result << "-ch ";
		}
		if (canDelete(pair.first)) {
			result << "-dl";
		}
		if (canChangeAccess(pair.first)) {
			result << "-chac";
		}
	}
	return result.str();
}


void createNewFile(std::string login) {
	system("CLS");
	bool exit = false;
	int result = 0;
	std::string path;
	
	std::cout << "Enter file name:" << std::endl;
	std::cin >> path;

	Packet packet = P_CreateFile;
	//send packet type
	send(Connection, (char*)&packet, sizeof(Packet), NULL);

	//send login
	sendString(login);

	//send login
	sendString(path);

	
	

	recv(Connection, (char*)&result, sizeof(int), NULL);

	if (result == 0) {
		std::cout << "File created successfully" << std::endl;
		exit = true;
	}
	else if (result == 1) {
		std::cout << "File already exist" << std::endl;
	} else
		std::cout << "File was not created" << std::endl;
	pressAnyButton();
}

void readFile(std::string login, std::string fileName) {
	Packet packet = P_ReadFile;
	//send packet type
	send(Connection, (char*)&packet, sizeof(Packet), NULL);

	//send login
	sendString(login);
	//send fileName
	sendString(fileName);
	

	int answer;
	std::string fileText;
	recv(Connection, (char*)&answer, sizeof(int), NULL);

	if (answer == 0) {
		fileText = recvString();
		std::cout << fileText << std::endl;
	}
	else if (answer == 1) {
		std::cout << "File doesnot exist" << std::endl;
	}
	else
		std::cout << "Access denied" << std::endl;
	pressAnyButton();


}

void changeFile(std::string login, std::string fileName) {
	Packet packet = P_WriteFile;
	//send packet type
	send(Connection, (char*)&packet, sizeof(Packet), NULL);

	//send login
	sendString(login);
	//send fileName
	sendString(fileName);


	int answer;
	std::string fileText;
	recv(Connection, (char*)&answer, sizeof(int), NULL);

	if (answer == 0) {
		fileText = recvString();
		std::cout << fileText << std::endl;
	}
	else if (answer == 1) {
		std::cout << "File doesnot exist" << std::endl;
	}
	else
		std::cout << "Access denied" << std::endl;
	pressAnyButton();

}

void deleteFile(std::string login, std::string fileName) {
	int result = 0;
	Packet packet = P_DeleteFile;
	//send packet type
	send(Connection, (char*)&packet, sizeof(Packet), NULL);

	//send login
	sendString(login);
	//send login
	sendString(fileName);
	recv(Connection, (char*)&result, sizeof(int), NULL);

	if (result == 0) {
		std::cout << "File deleted successfully" << std::endl;
	}
	else if (result == 1) {
		std::cout << "File doesnot exist" << std::endl;
	}
	else
		std::cout << "File was not deleted" << std::endl;
	pressAnyButton();
}

std::vector<std::string> getAllUsers() {
	Packet packet = P_GetUsers;
	//send packet type
	send(Connection, (char*)&packet, sizeof(Packet), NULL);

	std::string answer = recvString();
	std::vector<std::string> result;
	std::string temStr;

	std::stringstream parse;
	parse.str(answer);
	
	while (std::getline(parse, temStr, '\n')) {
		result.push_back(temStr);
	}
	
	return result;
}

void changeFileRights(std::string login, std::string fileName) {
	std::vector<std::string> actionsList;
	actionsList.push_back("Set visible");
	actionsList.push_back("Set read right");
	actionsList.push_back("Set change right");
	actionsList.push_back("Set delete right");
	actionsList.push_back("Set change file\'s rights");

	actionsList.push_back("Set invisible");
	actionsList.push_back("Delete read right");
	actionsList.push_back("Delete change right");
	actionsList.push_back("Delete delete right");
	actionsList.push_back("Delete change file\'s rights");
	actionsList.push_back("Exit");

	std::vector<std::string> allUsers = getAllUsers();
	allUsers.push_back("Exit");

	int userIndex;
	int action;
	do {
		userIndex = menu(allUsers);
		if (userIndex != allUsers.size() - 1) {
			do {
				action = menu(actionsList);
				if (action != 10) {
					Packet packet = P_ChangeRight;
					//send packet type
					send(Connection, (char*)&packet, sizeof(Packet), NULL);
					//send login
					sendString(login);
					//send fileName
					sendString(fileName);
					//send what action we want to do
					sendInt(action);
					//send user to aplly new right
					sendString(allUsers[userIndex]);
				}
			} while (action != 10);
		}
	} while (userIndex != allUsers.size()-1);
}

void personalMenu(std::string login) {
	int action;
	std::vector<std::string> fileMenuList;
	fileMenuList.push_back(std::string("Read file"));
	fileMenuList.push_back(std::string("Change file"));
	fileMenuList.push_back(std::string("Delete file"));
	fileMenuList.push_back(std::string("Change file\'s rights"));
	fileMenuList.push_back(std::string("Exit"));

	int userChoise;
	std::vector<std::string> personalMenuList; //(std::string("dfa"), std::string("dfa"))
	personalMenuList.push_back(std::string("Create file"));
	personalMenuList.push_back(std::string("View available files"));
	personalMenuList.push_back(std::string("Exit"));
	do {
		userChoise = menu(personalMenuList);

		switch (userChoise)
		{
		case 0:
		{
			createNewFile(login);
			break;
		}
		case 1:
		{
			std::vector<std::pair<uint8_t, std::string>> listOfFiles = getListOfFiles(login);

			int numberOfFile;
			std::vector<std::string> listOfFilesMenu;
			for (std::pair<uint8_t, std::string> pair : listOfFiles)
			{
				listOfFilesMenu.push_back(createFilenameAndRightsString(pair));
			}
			listOfFilesMenu.push_back("Exit");
			numberOfFile = menu(listOfFilesMenu);

			if (numberOfFile != listOfFiles.size()) {
				std::string fileName = listOfFiles[numberOfFile].second;
				action = menu(fileMenuList);
				switch (action)
				{
				case 0: 
				{
					readFile(login, fileName);
					break;
				}
				case 1:
				{
					changeFile(login, fileName);
					break;
				}
				case 2:
				{
					deleteFile(login, fileName);
					break;
				}
				case 3:
				{	
					changeFileRights(login, fileName);

					break;
				}
				case 4:
				{
					break;
				}
				default:
					break;
				}
			}
			

			break;
		}
		case 2:
			break;	
		default:
			break;
		}


	} while (userChoise != 2);

}

int main() {
	std::cout << "Hello, client!" << std::endl;

#ifdef CONNECT_TO_SERVER
	////////////// CONECTION /////////////////////////
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Windows sock is not loaded" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.20");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Erroe: failed connect to server" << std::endl;
		return 1;

	}
	//std::cout << "connected" << std::endl;
	////////////// CONECTION END /////////////

	////////////// COLLECT USER DATA /////////

	Packet packet= P_CollectUserData;
	//send packet type
	send(Connection, (char*)&packet, sizeof(Packet), NULL);
	std::string computerName = getComputerName();

	//send computerName
	sendString(computerName);
	////////////// COLLECT USER DATA END /////////
	
	

#endif

	////////////// SING UP OR REGISTRATION ///////////////
	int userChoise;
	std::vector<std::string> firstMenu; //(std::string("dfa"), std::string("dfa"))
	firstMenu.push_back(std::string("Sing Up"));
	firstMenu.push_back(std::string("Registrate"));
	firstMenu.push_back(std::string("Exit"));
	firstMenu.push_back(std::string("About"));
	
	do {
	userChoise = menu(firstMenu);

	switch (userChoise)
	{
	case 0:
		{
		//////////////  SING UP  ///////////////
		int numberOfTries = 4;
		std::string login;
		std::string password;
		Packet packettype = P_SingUp;
		int serverAnswer;

		for (int i = 0; i < numberOfTries; i++) {


			getLogPass(login, password);

			//login = "Vyny";
			//password = "123";
			//std::cout << "L: " << login << std::endl;
			//std::cout << "P: " << password << std::endl;
			
			//send packet type
			send(Connection, (char*)&packettype, sizeof(Packet), NULL);
			sendLogPass(login, password);

			
			recv(Connection, (char*)&serverAnswer, sizeof(int), NULL);
			if (serverAnswer == 1) {
				std::cout << "Loged in succesfully" << std::endl;
				pressAnyButton();
				break;
			}
			else if (serverAnswer == 0){
				std::cout << "Login or password is invalid, Check and try again" << std::endl;
				pressAnyButton();
			}

		}
		////////////// SING UP END   ///////////////
		if (serverAnswer == 1) {
			personalMenu(login);
		}
		
		
		break;
		}	
	case 1:
		{
		//////////////  REGISTRATION  ///////////////
		std::string login;
		std::string password;
		getLogPass(login, password);
		std::cout << "L: " << login << std::endl;
		std::cout << "P: " << password << std::endl;

		Packet packettype = P_Registration;
		//send packet type
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);

		sendLogPass(login, password);
		
		//recieve serwer anwer
		int serverAnswer;
		recv(Connection, (char*)&serverAnswer, sizeof(int), NULL);
		if (serverAnswer == 1)
			std::cout << "Registrated succesfully" << std::endl;
		else if (serverAnswer == 0)
			std::cout << "Registration faild, try another login" << std::endl;


		pressAnyButton();
		//prev two raw wil be raplaced with system("pause");

		break;
		//////////////  REGISTRATION  END///////////////
		}
	case 2:
	{
		Packet packettype = P_CloseConnection;
		//send packet type
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);
		break;
	}
	case 3:
		//write some heartbreaking story after programm will be wrote;
		break;
	default:
		break;
	}


	////////////// SING UP OR REGISTRATION  END///////////////
	} while (userChoise != 2);
	system("pause");
	
	return 0;
}