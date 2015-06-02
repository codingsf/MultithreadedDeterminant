#include "ArgumentParser.h"
#include <regex>
#include <iostream>

ArgumentParser::ArgumentParser(const std::vector<ArgumentDefinition>& validArguments)
	: m_ValidArguments(validArguments)
	, m_HasError(false)
{
}
std::map<std::string, std::string> ArgumentParser::ParseArgs(int argc, char** argv)
{
	using namespace std;
	map<string, string> values;
	for (int i = 1; i < argc; i++)
	{
		string arg = argv[i];
		arg = arg.substr(1);
		auto finder = [&arg](ArgumentDefinition definition) -> bool
		{
			return definition.Argument == arg;
		};
		auto definition = std::find_if(m_ValidArguments.begin(), m_ValidArguments.end(), finder);
		if (definition != m_ValidArguments.end())
		{
			if (!definition->HasValues)
			{
				values[arg] = "";
				continue;
			}

			if (i == argc - 1)
			{
				cerr << "Missing value for argument: " << arg << endl;
				m_HasError = true;
				return values;
			}
			string value = argv[++i];
			regex regularExpression(definition->ValueRegex);
			bool isValid = definition->ValueRegex.empty() ||
				regex_match(value, regularExpression);
			if (isValid)
			{
				values[arg] = value;
			}
			else
			{
				cerr << "Invalid value for argument: " << arg << " (" << value << ")" << endl;
				m_HasError = true;
			}
		}
		else
		{
			cerr << "Unknown argument: " << arg << endl;
			m_HasError = true;
		}
	}
	return values;
}

bool ArgumentParser::HasError() const
{
	return m_HasError;
}