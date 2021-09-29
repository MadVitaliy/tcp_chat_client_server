#include "User.h"

User::User(std::string id, std::string computerName) {
	this->id = id;
	this->computerName = computerName;
}
std::string User::getId() { return id; }
std::string User::getComputerName() { return computerName; }
bool User::isAuthorizated() { return authorizated; }