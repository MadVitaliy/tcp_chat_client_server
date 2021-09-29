#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <utility> 
#include "Authorization.h"
#include "SimpleLogger.h"
#include "User.h"
#include "AccesMatrix.h"
#include "FileObject.h"

#pragma warning(disable: 4996)

SOCKET conections[100];
int counter = 0;

std::string authorizationLogger = "authorizationLOGS.txt";
std::string accesLogger = "accessLOGS.txt";
std::string accessMatrixPath = "accessMatrix.txt";

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

void sendInt(int index, int toSend) {
	send(conections[index], (char*)&toSend, sizeof(int), NULL);
	Sleep(5);
}

void sendString(int index, std::string toSend) {
	int toSend_size = toSend.size();
	sendInt(index, toSend_size);
	send(conections[index], toSend.c_str(), toSend_size, NULL);
	Sleep(10);
}

std::string recvString(int index) {
	int answer_size;
	std::string answer;
	//recieve answer size
	recv(conections[index], (char*)&answer_size, sizeof(int), NULL);
	if (answer_size != 0) {
		char *c_answer = new char[answer_size + 1];
		c_answer[answer_size] = '\0';
		//recieve answer
		recv(conections[index], c_answer, answer_size, NULL);
		//save into string
		answer = std::string(c_answer);
		delete[] c_answer;
	}
	return answer;
}

std::pair<std::string, std::string> recvLogPass(int index) {
	std::string login = recvString(index);
	std::string password = recvString(index);
	return std::make_pair(login, password);
}

void sendBool(int index, bool value) {
	int boolInInt;
	if (value) 
		boolInInt = 1;
	else 
		boolInInt = 0;
	send(conections[index], (char*)&boolInInt, sizeof(int), NULL);
}

std::pair<std::string, std::string> recvLiginFilename(int index) {
	//recieve login
	std::string login = recvString(index);
	std::string path = recvString(index);
	return std::make_pair(login, path);

}

void clientHandler(int index) {
	//User user;
	Packet packettype;
	std::string userComputerName = "";

	bool holdConnectionOpened = true;
	while (holdConnectionOpened) {
		recv(conections[index], (char*)&packettype, sizeof(Packet), NULL);
		switch (packettype) {
		case P_CollectUserData: 
		{
			int userComputerName_sizes;
			recv(conections[index], (char*)&userComputerName_sizes, sizeof(int), NULL);
			
			if (userComputerName_sizes != 0) {
				char *c_userComputerName = new char[userComputerName_sizes + 1];
				c_userComputerName[userComputerName_sizes] = '\0';
				//recieve userComputerName
				recv(conections[index], c_userComputerName, userComputerName_sizes, NULL);
				//save into string
				userComputerName = std::string(c_userComputerName);
				delete[] c_userComputerName;

				std::cout << "UserComputerName: " << userComputerName << std::endl;
			}

			SimpleLogger logger(authorizationLogger, userComputerName);
			logger.writeConnectionLog(true);
			break;
		}
		case P_Registration:
		{
			std::string login;
			std::string password;

			std::pair<std::string, std::string> logpass =recvLogPass(index);
			login = logpass.first;
			password = logpass.second;
			
			std::cout << "In case P_Registration recived" << std::endl;
			std::cout << "Login: " << login << std::endl;
			std::cout << "Password: " << password << std::endl;
			//trying to registrate password

			bool result;
			Authorization authorization("tyt_vodky_nema.txt");
			if (authorization.registrate(login, password)) {
				std::cout  << login << " registrated!" << std::endl;
				result = true;
			}
			else {
				std::cout << "Registration failed: login \"" << login << " \"is already taken!"<< std::endl;
				result = false;
			}
			sendBool(index, result);
			SimpleLogger logger(authorizationLogger, userComputerName);
			logger.writeRegistrationLog(login, result);
			
			if (result) {
				AccesMatrix accesMatrix(accessMatrixPath);
				accesMatrix.addSubject(login);
				accesMatrix.writeMatrix();
			}
			break;
		}
		case P_SingUp:
		{
			std::string login;
			std::string password;

			std::pair<std::string, std::string> logpass = recvLogPass(index);
			login = logpass.first;
			password = logpass.second;

			std::cout << "In case P_SingUp recived" << std::endl;
			std::cout << "Login: " << login << std::endl;
			std::cout << "Password: " << password << std::endl;
			//trying to registrate password

			
			Authorization authorization("tyt_vodky_nema.txt");

			// succes 0 -- successfull, 1 -- wrong login, 2 -- wrong password;
			bool resultForUser;
			int result = authorization.singUp(login, password);
			if (result == 0) {
				std::cout << login << " sing In!" << std::endl;
				resultForUser = true;
				//user = User(login, userComputerName);
			}
			else if(result == 1){
				std::cout << "Sing Up failed: wrong login!" << std::endl;
				resultForUser = false;
			}
			else {
				std::cout << "Sing Up failed: wrong password!" << std::endl;
				resultForUser = false;
			}
			sendBool(index, resultForUser);

			SimpleLogger logger(authorizationLogger, userComputerName);
			logger.writeSingInLog(login, result);

			break;
		}
		case P_CloseConnection: 
		{
			holdConnectionOpened = false;
			break;
		}
		case P_CreateFile:
		{
			std::string login = recvString(index);
			std::string path = recvString(index);

			int result = 0; //0--success 1--object already exists 2--failed hz
			AccesMatrix accesMatrix(accessMatrixPath);

			if (accesMatrix.getObjectIndex(path) == -1) {
				accesMatrix.addObject(path, login);
				FileObject fileObject(path);
				std::cout << std::endl << "File\""<< path <<"\"";
				if (fileObject.createFile())
					std::cout << " was created";
				else {
					std::cout << " was not created";
					result = 2;
				}
				std::cout << " by \"" << login << "\"" <<  std::endl;
				accesMatrix.writeMatrix();
			}
			else
				result = 1;

			send(conections[index], (char*)&result, sizeof(int), NULL);

			//0--success 1--object already exists 2--failed hz
			SimpleLogger logger(accesLogger, userComputerName);
			logger.createFileLog(result, login, path);

			break;
		}
		case P_ReadFile: 
		{
			//std::pair<std::string, std::string> temp = recvLiginFilename(index);
			std::string login = recvString(index);
			std::string path = recvString(index);
			
			std::string fileText;

			int answer = 0; //0--success 1--object doesnot exist 2--access denied
			AccesMatrix accesMatrix(accessMatrixPath);
			if (accesMatrix.getObjectIndex(path) != -1) {
				if (accesMatrix.canRead(login, path)) {

					FileObject fileObject(path);
					fileText = fileObject.readTextFile();
					std::cout << "User: " << login << " read file\"" << path << "\"";
					answer = 0;
				}
				else {
					answer = 2;
					std::cout << "User: " << login << "can not read file\"" << path << "\"" << "access denied";
				}
				
			}
			else {
				answer = 1;
				std::cout << "User: " << login << "file\"" << path << "\"" << "didnot open";

			}
			std::cout << std::endl;
			send(conections[index], (char*)&answer, sizeof(int), NULL);

			if (answer == 0) {
				sendString(index, fileText);
			
			}
			//0--success 1--object doesnot exist 2--access denied
			SimpleLogger logger(accesLogger, userComputerName);
			logger.readFileLog(answer, login, path);

			break;
		}
		case P_DeleteFile:
		{
			std::string login = recvString(index);
			std::string path = recvString(index);

			int result = 0; //0--success 1--object doesnot exists 2--failed hz
			AccesMatrix accesMatrix(accessMatrixPath);
			if (accesMatrix.getObjectIndex(path) != -1) {
				if (accesMatrix.canDelete(login, path)) {
					FileObject fileObject(path);
					
					if (fileObject.deleteFile()) {
						accesMatrix.deleteObject(path);
						accesMatrix.writeMatrix();
						std::cout << "User: " << login << "deleted file\"" << path << "\"";
						result = 0;
					}
					else {
						std::cout << "User: " << login << "did not delete file\"" << path << "\"";
						result = 1;
					}
				}
				else {
					result = 2;
					std::cout << "User: " << login << "can not delete file\"" << path << "\"" << "access denied";
				}
			}
			else {
				result = 1;
				std::cout << "User: " << login << "did not delete file\"" << path << "\"";
			}
			send(conections[index], (char*)&result, sizeof(int), NULL);

			SimpleLogger logger(accesLogger, userComputerName);
			logger.deleteFileLog(result, login, path);
			break;
		}
		case P_WriteFile:
		{
			std::string login = recvString(index);
			std::string path = recvString(index);
			std::string fileText;

			int answer = 0; //0--success 1--object doesnot exist 2--access denied
			AccesMatrix accesMatrix(accessMatrixPath);
			if (accesMatrix.getObjectIndex(path) != -1) {
				if (accesMatrix.canChange(login, path) || accesMatrix.isOwner(login, path)) {

					FileObject fileObject(path);
					fileText = fileObject.readTextFile();
					fileText = "You trr to change \"" + path + "\"";
					fileText += ", but developer went on a bender and did not finish text editor";
					answer = 0;
				}
				else {
					answer = 2;
					std::cout << "User: " << login << "can not change file\"" << path << "\"" << "access denied";
				}

			}
			else {
				answer = 1;
				std::cout << "User: " << login << "file\"" << path << "\"" << "didnot open";

			}
			std::cout << std::endl;
			send(conections[index], (char*)&answer, sizeof(int), NULL);

			if (answer == 0) {
				sendString(index, fileText);
				std::cout << "User: " << login << " changed file\"" << path << "\"";
			}

			SimpleLogger logger(accesLogger, userComputerName);
			logger.writeFileLog(answer, login, path);

			break;
		}
		case P_GetListOfFiles: {
			std::string login = recvString(index);

			AccesMatrix accesMatrix(accessMatrixPath);
			std::string result = accesMatrix.getAvailableObjects(login);
			sendString(index, result);

			std::cout << std::endl << result << std::endl << std::endl;
			break;
		}
		case P_GetUsers:
		{
			AccesMatrix accesMatrix(accessMatrixPath);
			std::string result = accesMatrix.getAllSubject();
			sendString(index, result);

			std::cout << std::endl << result << std::endl << std::endl;
			break;
		}
		case P_ChangeRight:
		{
			std::string login = recvString(index);
			std::string path = recvString(index);
			int action;
			recv(conections[index], (char*)&action, sizeof(int), NULL);
			std::string subject = recvString(index);

			bool access = false;

			AccesMatrix accesMatrix(accessMatrixPath);
			if (accesMatrix.canChangeAccess(login, path)) {
				switch (action)
				{
				case 0: 
				{
					if(accesMatrix.setViewRight(login,subject,path)) {
						access = true;
					}
					break;
				}
				case 1:
				{
					if (accesMatrix.setReadRight(login, subject, path)) {
						access = true;
					}
					break;
				}
				case 2:
				{
					if (accesMatrix.setChangeRight(login, subject, path)) {
						access = true;
					}
					break;
				}
				case 3:
				{
					if (accesMatrix.setDeleteRight(login, subject, path)) {
						access = true;
					}
					break;
				}
				case 4:
				{
					if (accesMatrix.isOwner(login,path)) {
						if (accesMatrix.setChangeAccessRight(login, subject, path)) {
							access = true;
						}
					}
					break;
				}
				case 5:
				{
					if (accesMatrix.deleteViewRight(login, subject, path)) {
						access = true;
					}
					break;
				}
				case 6:
				{
					if (accesMatrix.deleteReadRight(login, subject, path)) {
						access = true;
					}
					break;
				}
				case 7:
				{
					if (accesMatrix.deleteChangeRight(login, subject, path)) {
						access = true;
					}
					break;
				}
				case 8:
				{
					if (accesMatrix.deleteDeleteRight(login, subject, path)) {
						access = true;
					}
					break;
				}
				case 9:
				{
					if (accesMatrix.isOwner(login, path)) {
						if (accesMatrix.deleteChangeAccessRight(login, subject, path)) {
							access = true;
						}
					}
					break;
				}
				default:
					break;
				}
			}
			accesMatrix.writeMatrix();

			SimpleLogger logger(accesLogger, userComputerName);
			logger.changeRightLOG(action, login, subject, path, access);

			break;
		}
		default:
			std::cout << "Unrecognized packet: " << packettype << std::endl;
			break;
		}

	}
	closesocket(conections[index]);
	SimpleLogger logger(authorizationLogger, userComputerName);
	logger.writeConnectionLog(false);
	std::cout <<  "Connection " << index << " closed" << std::endl;
}

void testToOpenFile(std::string path) {
	std::ifstream file(path, std::fstream::in);
	if (file.is_open())
	{
		std::cout << std::endl << "File opened" << std::endl;

		file.close();
	}
	else {
		std::cout << std::endl << "File opene faild" << std::endl;
	}
}


int main() {
	std::cout << "Server program start up!" << std::endl;

	testToOpenFile(accessMatrixPath);

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2,1);
	if(WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Windows sock is not loaded" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.20");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET  sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "New connection is not set" << std::endl;
			exit(2);
		}
		else {
			std::cout << "New connection is set" << std::endl;

			conections[i] = newConnection;
			counter++;

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)(i), NULL, NULL);
		}
	}
	return 0;
}