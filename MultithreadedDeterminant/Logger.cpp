#include "Logger.h"

#include <iostream>

Logger* Logger::g_Logger;

Logger::Logger(bool runInQuietMode)
	: m_IsInQuiteMode(runInQuietMode)
{
	if (g_Logger)
	{
		throw std::runtime_error("A logger already exists!");
	}
	g_Logger = this;
}

Logger& Logger::Instance()
{
	return *g_Logger;
}

void Logger::Info(const std::string& message)
{
	if (!m_IsInQuiteMode)
	{
		std::cout << message << std::endl;
	}
}

void Logger::ImportantInfo(const std::string& message)
{
	std::cout << message << std::endl;
}

void Logger::Error(const std::string& error)
{
	std::cerr << error << std::endl;
}