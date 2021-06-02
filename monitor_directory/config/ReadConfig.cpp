#include "ReadConfig.h"

namespace config
{
	ReadConfig* ReadConfig::instance = NULL;

	mutex ReadConfig::myMutex;

	ReadConfig::ReadConfig(/* args */)
	{
	}

	ReadConfig::~ReadConfig()
	{
	}

	ReadConfig* ReadConfig::getInstance()
	{
		lock_guard<mutex> myLock(myMutex);
		if (instance == NULL)
		{
			instance = new ReadConfig();
		}
		return instance;
	}

	void ReadConfig::setDataPath(string dataPath)
	{
		this->dataPath = dataPath;
	}

	void ReadConfig::setParam(Param param, string value)
	{
		switch (param)
		{
		case EXCHANGE:
			exchange = value;
			break;
		
		case ROUTEKEY:
			routekey = value;
			break;
		
		case QUEUE:
			queue = value;
			break;

		case TIME_MONITOR:
			timeMonitory = atoi(value.c_str());
			break;

		case WARNING_THRESHOLD_1:
			warningThreshold_1 = atoi(value.c_str());
			break;

		case WARNING_THRESHOLD_2:
			warningThreshold_2 = atoi(value.c_str());
			break;

		case WARNING_THRESHOLD_3:
			warningThreshold_3 = atoi(value.c_str());
			break;

		case DELETE_START_THRESHOLD:
			deleteStartThreshold = atoi(value.c_str());
			break;

		case DELETE_STOP_THRESHOLD:
			deleteStopThreshold = atoi(value.c_str());
			break;
		}
	}

	Param ReadConfig::stringToParam(const string& str)
	{
		if (str == "Exchange") return EXCHANGE;
		else if (str == "Routekey") return ROUTEKEY;
		else if (str == "Queue") return QUEUE;
		else if (str == "Time monitory") return TIME_MONITOR;
		else if (str == "Warning threshold 1") return WARNING_THRESHOLD_1;
		else if (str == "Warning threshold 2") return WARNING_THRESHOLD_2;
		else if (str == "Warning threshold 3") return WARNING_THRESHOLD_3;
		else if (str == "Delete start threshold") return DELETE_START_THRESHOLD;
		else if (str == "Delete stop threshold") return DELETE_STOP_THRESHOLD;
		cout << "ERROR: Read Config khong co param '" << str << "'" << endl;
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
		ifstream fileConfig(dataPath);

		if (!fileConfig)
		{
			cout << "ERROR: Read Config duong dan file config sai" << endl;
			return;
		}

		string strLine, strData;
		while (!fileConfig.eof())
		{
			getline(fileConfig, strLine);				// Doc 1 dong trong file config
			if (strLine.length() > 0 && strLine[0] != '#')
			{
				strData = split(strLine, "#")[0];		// Loai bo phan conment
				if (strData.length() > 0)
				{
					// Lay gia tri trong file config
					vector<string> param = this->split(strData, "=");

					string key = param.at(0);
					standardize(key);
					string value = param.at(1);
					standardize(value);

					setParam(stringToParam(key), value);
				}
			}
		}
	}

	string ReadConfig::getStringValue(Param param)
	{
		myMutex.lock();
		string rs;
		switch (param)
		{
		case EXCHANGE:
			rs = exchange;
			break;
		case ROUTEKEY:
			rs = routekey;
			break;
		case QUEUE:
			rs = queue;
			break;
		default:
			cout << "ERROR: Read Config khong get dc kieu du lieu" << endl;
			break;
		}
		myMutex.unlock();
		return rs;
	}

	int ReadConfig::getIntValue(Param param)
	{
		myMutex.lock();
		int rs;
		switch (param)
		{
		case TIME_MONITOR:
			rs = timeMonitory;
			break;
		case WARNING_THRESHOLD_1:
			rs = warningThreshold_1;
			break;
		case WARNING_THRESHOLD_2:
			rs = warningThreshold_2;
			break;
		case WARNING_THRESHOLD_3:
			rs = warningThreshold_3;
			break;
		case DELETE_START_THRESHOLD:
			rs = deleteStartThreshold;
			break;
		case DELETE_STOP_THRESHOLD:
			rs = deleteStopThreshold;
			break;
		default:
			cout << "ERROR: Read Config khong get dc kieu du lieu" << endl;
			break;
		}
		myMutex.unlock();
		return rs;
	}

}