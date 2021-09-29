#include "Authorization.h"

Authorization::Authorization(const std::string &path)
{
	hashPath = path;
	std::ifstream table(hashPath, std::fstream::in | std::fstream::binary);
	if (table.is_open())
	{
		std::string log;
		int len = 0;
		int hash = 0;
		int level = 0;
		while (!table.eof())
		{
			table.read(reinterpret_cast<char*>(&len), sizeof(int));
			log.resize(len);
			table.read(const_cast<char*>(log.data()), len);
			table.read(reinterpret_cast<char*>(&hash), sizeof(int));
			table.read(reinterpret_cast<char*>(&level), sizeof(int));
			hashTable.insert({ log,hash });
		}
		table.close();
	}
}


bool Authorization::registrate(const std::string &login, const std::string &pass)
{
	if (hashTable.find(login) == hashTable.end() && login.size() != 0)
	{
		int hash = hashFunc(pass);
		std::ofstream table(hashPath, std::fstream::out | std::fstream::binary | std::fstream::app);
		int level = 0;
		if (table.is_open())
		{
			int len = login.length();
			table.write(reinterpret_cast<char*>(&len), sizeof(int));
			//table.write(reinterpret_cast<char*>(login.c_str()), len);
			table.write(const_cast<char*>(login.c_str()), len);
			table.write(reinterpret_cast<char*>(&hash), sizeof(int));
			table.write(reinterpret_cast<char*>(&level), sizeof(int));
			table.close();
		}
		else
		{
			return false;
		}
		//hashTable.insert({login, hash});
		//users.push_back(user(0));

		return true;
	}
	else
	{
		return false;
	}

}

int Authorization::hashFunc(const std::string &pass)
{
	int hash = 1;
	for (auto & it : pass)
	{
		hash += static_cast<int>(it);
	}
	hash *= static_cast<int>('s');
	hash -= static_cast<int>('a');
	hash *= static_cast<int>('l');
	hash -= static_cast<int>('t');
	hash = hash % 314156;
	return hash;
}

// succes 0 -- successfull, 1 -- wrong login, 2 -- wrong password;
int Authorization::singUp(const std::string &login, const std::string &pass)
{
	auto finded = hashTable.find(login);
	if (finded != hashTable.end())
	{
		int hash = hashFunc(pass);
		if (hash == finded->second)
		{
			return 0;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 1;
	}
}