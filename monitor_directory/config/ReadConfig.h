#pragma once
#ifndef READ_CONFIG_H
#define READ_CONFIG_H

#include "IOFile.h"
#include <string>

using namespace std;

class ReadConfig
{
private:
	static ReadConfig *instance;
	ReadConfig();

	std::string dataPath;

	//	Cac thong so
	int timeMonitor;
	int warningThreshold_1;
	int warningThreshold_2;
	int warningThreshold_3;
	int deleteStartThreshold;
	int deleteStopThreshold;

	//      Cat string "str" thanh cac string con
	//  voi string dan cach la "delim"
	vector<string> split(const string& str, const string& delim);

	//  Xoa bo ky tu " " thua
	void standardize(string& str);
public:

	ReadConfig(ReadConfig&) = delete;

	ReadConfig& operator=(ReadConfig&) = delete;

	static ReadConfig *getInstance();

	~ReadConfig();

	void read();

	void setDataPath(string dataPath);
	int getWarningThreshold_1();
	int warningThreshold_2();
	int WarningThreshold_3();
	int getDeleteStartThreshold();
	int getDeleteStopThreshold();
};

#endif

