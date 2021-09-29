#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <utility>
#include <sstream>

class AccesMatrix
{
private:
	int _numberOfSubject;
	int _numberOfObjects;
	std::vector<std::string> _subjects;
	std::vector<std::string> _objects;
	uint8_t *_matrix; //_numberOfSubjec*_numberOfSubjec
	std::string _path;

	const uint8_t AM_OWNER = 0x01;                  //0000 0001
	const uint8_t AM_VIEWRIGHT = 0x02;              //0000 0010
	const uint8_t AM_READRIGHT = 0x04;              //0000 0100
	const uint8_t AM_CHANGERIGHT = 0x08;            //0000 1000
	const uint8_t AM_DELETRIGHT = 0x10;             //0001 0000
	const uint8_t AM_CHANGEACCESSRIGHT = 0x20;      //0010 0000
	const uint8_t AM_REZ1 = 0x40;                   //0100 0000
	const uint8_t AM_REZ2 = 0x80;                   //1000 0000

public:
	AccesMatrix(std::string path);
	~AccesMatrix();
	void readMatrix();
	void writeMatrix();

	int getSubjectIndex(std::string subjectId);
	int getObjectIndex(std::string objectId);

	void addSubject(std::string subjectId);
	void addObject(std::string objectId, std::string ownerId);
	void deleteObject(std::string objectId);

	int getNumberOfSubject() { return _numberOfSubject; }
	int getNumberOfObjects() { return _numberOfObjects; }


	uint8_t getRights(std::string subjectId, std::string objectId);
	uint8_t setAccessLevel(std::string subjectId, std::string objectId, uint8_t bit);

	bool isOwner(std::string subjectId, std::string objectId);
	bool isVisible(std::string subjectId, std::string objectId);
	bool canRead(std::string subjectId, std::string objectId);
	bool canChange(std::string subjectId, std::string objectId);
	bool canDelete(std::string subjectId, std::string objectId);
	bool canChangeAccess(std::string subjectId, std::string objectId);
	bool canChangeAccessOrOwner(std::string subjectId, std::string objectId);

	//for next funcs
	// ownerId -- who change right,
	// subjectId -- to whom change right,
	// objectId -- to change right

	//return true if new right is set, else return false
	bool setViewRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool setReadRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool setChangeRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool setDeleteRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool setChangeAccessRight(std::string ownerId, std::string subjectId, std::string objectId);


	//return true if new right is deleted, else return false
	bool deleteViewRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool deleteReadRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool deleteChangeRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool deleteDeleteRight(std::string ownerId, std::string subjectId, std::string objectId);
	bool deleteChangeAccessRight(std::string ownerId, std::string subjectId, std::string objectId);


	//create string, that has pair right-file of all available in each raw
	std::string getAvailableObjects(std::string subjectId);
	std::string getAllSubject();

	void outputMatrix();
};

