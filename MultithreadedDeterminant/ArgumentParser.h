#pragma once
#include <map>
#include <vector>
#include <string>

struct ArgumentDefinition
{
	std::string Argument;
	bool HasValues;
	std::string ValueRegex;
};

class ArgumentParser
{
public:
	ArgumentParser(const std::vector<ArgumentDefinition>& validArguments);
	std::map<std::string, std::string> ParseArgs(int argc, char** argv);
	bool HasError() const;

private:
	std::vector<ArgumentDefinition> m_ValidArguments;
	bool m_HasError;
};

