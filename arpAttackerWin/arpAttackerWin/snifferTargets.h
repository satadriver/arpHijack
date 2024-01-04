#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "ClientAddress.h"
#include "PublicUtils.h"

using namespace std;


class SnifferTargets {
public:
	static int SnifferTargets::snifferHosts(vector < CLIENTADDRESSES> &target);
	static int SnifferTargets::snifferHostsMain(void * param,vector <CLIENTADDRESSES> &target);
};