#include "AccesMatrix.h"

AccesMatrix::AccesMatrix(std::string path) {
	this->_path = path;
	readMatrix();
}

AccesMatrix::~AccesMatrix() {
	delete[] _matrix;
}

void AccesMatrix::readMatrix() {
	std::ifstream file(_path, std::fstream::in);
	if (file.is_open())
	{
		file >> _numberOfSubject >> _numberOfObjects;
		std::cout << _numberOfSubject << " " << _numberOfObjects << std::endl;
		if (_numberOfSubject > 0) {

			std::cout << std::endl << "Subjects:" << std::endl;

			_subjects.reserve(_numberOfSubject);

			std::string subject;
			std::getline(file, subject);
			for (int i = 0; i < _numberOfSubject; i++) {
				std::getline(file, subject);
				_subjects.push_back(subject);
				std::cout << _subjects[i] << std::endl;
			}

			if (_numberOfObjects > 0) {
				std::cout << std::endl << "Objects:" << std::endl;

				_objects.reserve(_numberOfObjects);
				std::string object;
				std::getline(file, object);
				for (int i = 0; i < _numberOfObjects; i++) {
					std::getline(file, object);
					_objects.push_back(object);
					std::cout << _objects[i] << std::endl;
				}

				std::cout << std::endl << "Matrix" << std::endl;
				_matrix = new uint8_t[_numberOfSubject*_numberOfObjects];

				for (int i = 0; i < _numberOfSubject; i++) {
					for (int j = 0; j < _numberOfObjects; j++)
					{
						int temp;
						uint8_t utemp;
						file >> temp;
						utemp = temp;
						_matrix[i*_numberOfObjects + j] = utemp;
						//_matrix[i*_numberOfObjects + j] -= 48;
						std::cout << (int)_matrix[i*_numberOfObjects + j] << " ";
					}
					std::cout << std::endl;
				}
			}
		}


		file.close();
	}
	else {
		std::cout << "Cannot open matrix file" << std::endl;
	}
}
void AccesMatrix::writeMatrix() {
	std::ofstream file(_path, std::fstream::trunc);

	if (file.is_open())
	{
		file << _numberOfSubject << " " << _numberOfObjects << std::endl;

		if (_numberOfSubject > 0) {
			for (int i = 0; i < _numberOfSubject; i++) {
				file << _subjects[i] << std::endl;
			}
			file << std::endl;
			if (_numberOfObjects > 0) {
				for (int i = 0; i < _numberOfObjects; i++) {
					file << _objects[i] << std::endl;
				}
				file << std::endl;

				for (int i = 0; i < _numberOfSubject; i++) {
					for (int j = 0; j < _numberOfObjects; j++)
					{
						file << (int)_matrix[i*_numberOfObjects + j] << " ";
					}
					file << std::endl;
				}
			}
		}
		file.close();
	}
}



void addRawInMatrix(
	uint8_t *newMatrix,	//pointer to new bigger Matrix
	uint8_t *oldMatrix,	//pointer to old Matrix
	int commonWidth,	//width of both of them (matrices have equal width)
	int oldHeight,		//height of old Matrix, newHeight = oldHeight+1
	uint8_t symbol) {		//symbol to fill new raw
	for (int i = 0; i < oldHeight; i++) {
		for (int j = 0; j < commonWidth; j++) {
			newMatrix[i*commonWidth + j] = oldMatrix[i*commonWidth + j];
		}
	}

	for (int i = 0; i < commonWidth; i++) {
		newMatrix[oldHeight*commonWidth + i] = symbol;
	}
}

void addColumnInMatrix(
	uint8_t *newMatrix,	//pointer to new bigger Matrix
	uint8_t *oldMatrix,	//pointer to old Matrix
	int commonHeight,	//height of both of them (matrices have equal height)
	int oldWidth,		//width of old Matrix, newWidth = oldWidth+1
	uint8_t symbol) {		//symbol to fill new column

	for (int i = 0; i < oldWidth; i++) {
		for (int j = 0; j < commonHeight; j++) {
			newMatrix[j*(oldWidth + 1) + i] = oldMatrix[j*oldWidth + i];
		}
	}

	for (int i = 0; i < commonHeight; i++) {
		newMatrix[oldWidth*(i + 1) + i] = symbol;
	}
}

void deleteColumnInMatrix(
	uint8_t *newMatrix,	//pointer to new smaller Matrix
	uint8_t *oldMatrix,	//pointer to old Matrix
	int commonHeight,	//height of both of them (matrices have equal height)
	int oldWidth,		//width of old Matrix, newWidth = oldWidth-1
	int column) {		//column to be deleted

	int newWidth = oldWidth - 1;
	for (int i = 0; i < commonHeight; i++) {
		for (int j = 0; j < newWidth; j++) {
			if (j < column)
				newMatrix[i*newWidth + j] = oldMatrix[i*oldWidth + j];
			else
				newMatrix[i*newWidth + j] = oldMatrix[i*oldWidth + j + 1];
		}
	}
}


//return index of Subject in matrix, return -1 if subjectId not found
int AccesMatrix::getSubjectIndex(std::string subjectId) {
	auto itterator = std::find(_subjects.begin(), _subjects.end(), subjectId);
	if (itterator != _subjects.end()) {
		int indexOfSubject = itterator - _subjects.begin();
		return indexOfSubject;
	}
	else {
		return -1;
	}

}
int AccesMatrix::getObjectIndex(std::string objectId) {
	auto itterator = std::find(_objects.begin(), _objects.end(), objectId);
	if (itterator != _objects.end()) {
		int indexOfSubject = itterator - _objects.begin();
		return indexOfSubject;
	}
	else {
		return -1;
	}
}

void AccesMatrix::addSubject(std::string subjectId) {

	if (getSubjectIndex(subjectId) != -1) { //if user already exists exit;
		std::cout << "User \"" << subjectId << "\" already exists" << std::endl;
		return;
	}
	//add one subject
	int newNumberOfSubject = _numberOfSubject + 1;
	_subjects.push_back(subjectId);
	// and add one raw in matrix for it
	if (_numberOfObjects != 0) {
		uint8_t *newMatrix = new uint8_t[newNumberOfSubject*_numberOfObjects];
		addRawInMatrix(newMatrix, _matrix, _numberOfObjects, _numberOfSubject, 0);
		_numberOfSubject++;
		delete[] _matrix;
		_matrix = newMatrix;

	}
}




void AccesMatrix::addObject(std::string objectId, std::string ownerId) {
	//add one object

	int subjectIndex = getSubjectIndex(ownerId);
	if (subjectIndex == -1) { //if user doesnt exist exit;
		std::cout << "User \"" << ownerId << "\"  doesnt exist" << std::endl;
		return;
	}

	int objectIndex = getObjectIndex(objectId);
	if (objectIndex != -1) { //if file exists exit;
		std::cout << "Object \"" << objectId << "\"  already exists\"" << std::endl;
		return;
	}

	//add one object
	int newNumberOfObjects = _numberOfObjects + 1;
	_objects.push_back(objectId);
	// and add column  in matrix for it
	if (_numberOfSubject != 0) {
		uint8_t *newMatrix = new uint8_t[newNumberOfObjects*_numberOfSubject];
		addColumnInMatrix(newMatrix, _matrix, _numberOfSubject, _numberOfObjects, 0);
		_numberOfObjects++;
		delete[] _matrix;
		_matrix = newMatrix;
		_matrix[subjectIndex*_numberOfObjects + _numberOfObjects - 1] = 1;
	}

}
void AccesMatrix::deleteObject(std::string objectId) {
	auto itterator = std::find(_objects.begin(), _objects.end(), objectId);
	if (itterator != _objects.end()) {
		int indexToDelete = itterator - _objects.begin();
		_objects.erase(itterator);
		uint8_t *newMatrix = new uint8_t[(_numberOfObjects - 1)*_numberOfSubject];
		deleteColumnInMatrix(newMatrix, _matrix, _numberOfSubject, _numberOfObjects,  indexToDelete);
		_numberOfObjects--;
		delete[] _matrix;
		_matrix = newMatrix;
	}

}


uint8_t AccesMatrix::getRights(std::string subjectId, std::string objectId) {
	int subjectIndex = getSubjectIndex(subjectId);
	int objectIndex = getObjectIndex(objectId);
	return (uint8_t)_matrix[subjectIndex*_numberOfObjects + objectIndex];
}


bool AccesMatrix::isOwner(std::string subjectId, std::string objectId) {
	uint8_t rights = getRights(subjectId, objectId);
	return (rights & AM_OWNER);
}

bool AccesMatrix::isVisible(std::string subjectId, std::string objectId) {
	uint8_t rights = getRights(subjectId, objectId);
	return (rights & AM_VIEWRIGHT)|| (rights & AM_OWNER);
}

bool AccesMatrix::canRead(std::string subjectId, std::string objectId) {
	uint8_t rights = getRights(subjectId, objectId);
	return (rights & AM_READRIGHT)|| (rights & AM_OWNER);
}

bool AccesMatrix::canChange(std::string subjectId, std::string objectId) {
	uint8_t rights = getRights(subjectId, objectId);
	return (rights & AM_CHANGERIGHT)|| (rights & AM_OWNER);
}

bool AccesMatrix::canDelete(std::string subjectId, std::string objectId) {
	uint8_t rights = getRights(subjectId, objectId);
	uint8_t tempResult = (rights & AM_DELETRIGHT)|| (rights & AM_OWNER);
	bool boolResult = tempResult;
	return boolResult;
}

bool AccesMatrix::canChangeAccess(std::string subjectId, std::string objectId) {
	uint8_t rights = getRights(subjectId, objectId);
	return (rights & AM_CHANGEACCESSRIGHT)||(rights & AM_OWNER);
}

bool AccesMatrix::canChangeAccessOrOwner(std::string subjectId, std::string objectId) {
	uint8_t rights = getRights(subjectId, objectId);
	return (rights & AM_CHANGEACCESSRIGHT) || (rights & AM_OWNER);
}


//for next funcs
// ownerId -- who change right,
// subjectId -- to whom change right,
// objectId -- to change right

//return true if new right is set, else return false
bool AccesMatrix::setViewRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] |= (uint8_t)AM_VIEWRIGHT;
		return true;
	}
	else {
		return false;
	}
}

bool AccesMatrix::setReadRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] |= (uint8_t)(AM_READRIGHT | AM_VIEWRIGHT);
		return true;
	}
	else {
		return false;
	}
}

bool AccesMatrix::setChangeRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] |= (uint8_t)(AM_CHANGERIGHT | AM_VIEWRIGHT);
		return true;
	}
	else {
		return false;
	}
}

bool AccesMatrix::setDeleteRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] |= (uint8_t)(AM_DELETRIGHT | AM_VIEWRIGHT);
		return true;
	}
	else {
		return false;
	}
}

bool AccesMatrix::setChangeAccessRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] |= (uint8_t)(AM_CHANGEACCESSRIGHT | AM_VIEWRIGHT);
		return true;
	}
	else {
		return false;
	}
}



//return true if new right is deleted, else return false
bool AccesMatrix::deleteViewRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] &= (uint8_t)(~AM_VIEWRIGHT);
		return true;
	}
	else
		return false;
}

bool AccesMatrix::deleteReadRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] &= (uint8_t)(~AM_READRIGHT);
		return true;
	}
	else
		return false;
}

bool AccesMatrix::deleteChangeRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] &= (uint8_t)(~AM_CHANGERIGHT);
		return true;
	}
	else
		return false;
}

bool AccesMatrix::deleteDeleteRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] &= (uint8_t)(~AM_DELETRIGHT);
		return true;
	}
	else
		return false;
}

bool AccesMatrix::deleteChangeAccessRight(std::string ownerId, std::string subjectId, std::string objectId) {
	if (canChangeAccessOrOwner(ownerId, objectId)) {
		int subjectIndex = getSubjectIndex(subjectId);
		int objectIndex = getObjectIndex(objectId);
		_matrix[subjectIndex*_numberOfObjects + objectIndex] &= (uint8_t)(~AM_CHANGEACCESSRIGHT);
		return true;
	}
	else
		return false;
}


std::string AccesMatrix::getAvailableObjects(std::string subjectId) {

	std::stringstream result;
	int subjectIndex = getSubjectIndex(subjectId);
	int rights;
	for (int i = 0; i < _numberOfObjects; ++i) {
		rights = (int)_matrix[subjectIndex*_numberOfObjects + i];
		if (rights > 0) {
			result << (int)rights << " " << _objects[i] << '\n';
		}
	}
	return result.str();
}




std::string AccesMatrix::getAllSubject() {
	std::stringstream result;
	for (int i = 0; i < _numberOfSubject; ++i) {
		result << _subjects[i] << '\n';
	}
	return result.str();
}



void AccesMatrix::outputMatrix() {
	std::cout << std::endl;
	for (int i = 0; i < _numberOfSubject; i++)
	{
		for (int j = 0; j < _numberOfObjects; j++)
		{
			std::cout << (int)_matrix[i*_numberOfObjects + j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}