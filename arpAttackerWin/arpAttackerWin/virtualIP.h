#pragma once

#include "ClientAddress.h"
#include "PublicUtils.h"
#include <vector>
#include <iostream>
#include <string>
using namespace std;

class VirtualIP {
public:
	static unsigned int makeVirtualIP(void * param, vector <CLIENTADDRESSES> &targets);

};
