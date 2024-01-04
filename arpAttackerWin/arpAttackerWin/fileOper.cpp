/*
* fileOper.cpp
*
*  Created on: Nov 2, 2018
*      Author: root
*/




/*
* FileOper.cpp
*
*  Created on: Sep 6, 2018
*      Author: root
*/


#include "fileOper.h"
#include <stdio.h>



int FileOper::fileReader(string filename, char ** lpbuf, int *bufsize) {
	int ret = 0;

	FILE * fp = fopen(filename.c_str(), "rb");
	if (fp <= 0)
	{
		return -1;
	}

	ret = fseek(fp, 0, SEEK_END);

	int filesize = ftell(fp);

	ret = fseek(fp, 0, SEEK_SET);

	*bufsize = filesize;

	*lpbuf = new char[filesize + 1024];

	ret = fread(*lpbuf, 1, filesize, fp);
	fclose(fp);
	if (ret == 0)
	{
		delete[] lpbuf;
		return -1;
	}

	*(*lpbuf + filesize) = 0;
	return filesize;
}




int FileOper::fileWriter(string filename, const char * lpdata, int datasize) {
	int ret = 0;

	FILE * fp = fopen(filename.c_str(), "ab+");
	if (fp <= 0)
	{
		return -1;
	}

	ret = fwrite(lpdata, 1, datasize, fp);
	fclose(fp);
	if (ret == 0)
	{
		return -1;
	}

	return datasize;
}
