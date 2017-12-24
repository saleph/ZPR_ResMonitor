#ifndef INCLUDE_CONFIGURATIONPARSER_HPP_
#define INCLUDE_CONFIGURATIONPARSER_HPP_

#include <vector>
#include <memory>
#include <fstream>
#include "configrawdata.hpp"
#include "flex.hpp"

class ConfigurationParser {
public:
	ConfigurationParser(const std::string& confFile) :
			configurationFile(confFile, std::ifstream::in) {
		lexer = std::unique_ptr < yyFlexLexer
				> (new yyFlexLexer(configurationFile, std::cerr));
	}

	void run() {
		while (lexer->yylex() != 0) {
		}
	}

	std::vector<TriggerType> &getTriggerTypes() {
		return lexer->getTriggerTypes();
	}

	std::vector<LogType> &getLogTypes() {
		return lexer->getLogTypes();
	}

private:
	std::unique_ptr<yyFlexLexer> lexer;
	std::ifstream configurationFile;
};

#endif /* INCLUDE_CONFIGURATIONPARSER_HPP_ */
