/*
* FileOper.h
*
*  Created on: Sep 6, 2018
*      Author: root
*/


#pragma once
#ifndef FILEOPER_H_
#define FILEOPER_H_


#include <iostream>

using namespace std;

class FileOper {
public:
	static int fileReader(string filename, char ** lpbuf, int *bufsize);
	static int fileWriter(string filename, const char * lpdate, int datesize);
};



#endif /* FILEOPER_H_ */

