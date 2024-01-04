#pragma once

#ifndef CONFIG_H_H_H
#define CONFIG_H_H_H
#include <string>

#include <vector>
#include "PublicUtils.h"

using namespace std;

class Config {
public:
	static int getAttackTarget(string fn, vector<CLIENTADDRESSES> &targets,int * speed,int*arpdelay);

	static int Config::getAttackTargetFromCmd(char * buf, vector<CLIENTADDRESSES> & targets, int * speed, int * arprepeat);

	static int addTarget(unsigned int ip,vector <CLIENTADDRESSES>& list);

	static int Config::addTarget(vector<CLIENTADDRESSES> & targets, unsigned int recverip, unsigned char mac[MAC_ADDRESS_SIZE]);
};


#endif