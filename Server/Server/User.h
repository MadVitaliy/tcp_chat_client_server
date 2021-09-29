#pragma once
#include <string>
class User
{
private:
	std::string id;//login
	std::string computerName;
	bool authorizated = false;
public:
	User(std::string id, std::string computerName);
	//User();
	std::string getId();
	std::string getComputerName();
	bool isAuthorizated();
};

