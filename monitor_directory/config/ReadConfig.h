#pragma once
#ifndef READ_CONFIG_H
#define READ_CONFIG_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>

using namespace std;

namespace config
{
	
	enum Param
	{
		EXCHANGE,
		ROUTEKEY,
		QUEUE,
		TIME_MONITOR,
		WARNING_THRESHOLD_1,
		WARNING_THRESHOLD_2,
		WARNING_THRESHOLD_3,
		DELETE_START_THRESHOLD,
		DELETE_STOP_THRESHOLD
	};

	class ReadConfig
	{
	private:
		static mutex myMutex;
		static ReadConfig *instance;
		ReadConfig();

		string dataPath;
		string exchange;
		string routekey;
		string queue;
		int timeMonitory;
		int warningThreshold_1;
		int warningThreshold_2;
		int warningThreshold_3;
		int deleteStartThreshold;
		int deleteStopThreshold;

		//  Cat string "str" thanh cac string con
		// voi string dan cach la "delim"
		vector<string> split(const string& str, const string& delim);

		//  Xoa bo ky tu " " thua
		void standardize(string& str);

		void setParam(Param param, string value);
		Param stringToParam(const string& str);
	public:

		ReadConfig(ReadConfig&) = delete;

		ReadConfig& operator=(ReadConfig&) = delete;

		static ReadConfig* getInstance();

		~ReadConfig();

		void read();

		void setDataPath(string dataPath);

		int getIntValue(Param param);
		string getStringValue(Param param);
	};
}

#endif

