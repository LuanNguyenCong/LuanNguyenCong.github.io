#include "ReadConfig.h"
using namespace std;

ReadConfig* ReadConfig::instance = NULL;

ReadConfig::ReadConfig(/* args */)
{
}

ReadConfig::~ReadConfig()
{
}


ReadConfig* ReadConfig::getInstance()
{
	if (instance == NULL)
	{
		instance = new ReadConfig();
	}
	return instance;
}

int ReadConfig::getNumDigit()
{
	return this->numDigit;
}

int ReadConfig::getNumThread()
{
	return this->numThread;
}

void ReadConfig::setDataPath(string dataPath)
{
	this->dataPath = dataPath;
}

vector<string> ReadConfig::split(const string& str, const string& delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos) pos = str.length();
		string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

void ReadConfig::standardize(string& str)
{
	while (str.at(0) == ' ') str = str.substr(1, str.length() - 1);
	while (str.at(str.length() - 1) == ' ') str = str.substr(0, str.length() - 1);
}

void ReadConfig::read()
{
	IOFile fileConfig(dataPath);
	vector<string> data = fileConfig.readString();

	while (!data.empty())
	{
		string item = data.back();
		data.pop_back();
		vector<string> param = this->split(item, "=");

		string key = param.at(0);
		standardize(key);
		string value = param.at(1);
		standardize(value);

		if (key == "num_Threads")
			this->numThread = atoi(value.c_str());
		else if (key == "num_Digits")
			this->numDigit = atoi(value.c_str());
	}
}