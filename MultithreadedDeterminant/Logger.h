#pragma once
#include <string>

class Logger
{
public:
	Logger(bool runInQuietMode);
	static Logger& Instance();
	void ImportantInfo(const std::string&);
	void Info(const std::string&);
	void Error(const std::string&);
private:
	static Logger* g_Logger;
	bool m_IsInQuiteMode;
};


