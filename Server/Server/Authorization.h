#pragma once

#include<unordered_map>
#include<string>
#include <fstream>
#include<vector>
class Authorization
{
private:
	std::string hashPath;
	std::unordered_map<std::string, int> hashTable;
	//std::vector<User> users;
	int hashFunc(const std::string &pass);
public:
	Authorization(const std::string &path);
	bool registrate(const std::string &login, const std::string &pass);
	// succes 0 -- successfull, 1 -- wrong login, 2 -- wrong password;
	int singUp(const std::string &login, const std::string &pass);
};

