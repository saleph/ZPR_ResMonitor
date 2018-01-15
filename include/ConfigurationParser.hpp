#ifndef INCLUDE_CONFIGURATIONPARSER_HPP_
#define INCLUDE_CONFIGURATIONPARSER_HPP_

#include <vector>
#include <memory>
#include <fstream>
#include "configrawdata.hpp"
#include "flex.hpp"

/**
 * @brief Configuration files parser.
 * Produces vectors of TriggerTypes and LogTypes from specified file/stream.
 */
class ConfigurationParser {
public:
    /// Construct parser from file passed as filepath
	explicit ConfigurationParser(const std::string& confFile)
		: configurationFile(confFile, std::ifstream::in)
	{
		lexer = std::make_unique<yyFlexLexer>(configurationFile, std::cerr);
	}

    /// Construct parser from common istream (stringstream for example).
	explicit ConfigurationParser(std::istream& stream)
	{
		lexer = std::make_unique<yyFlexLexer>(stream, std::cerr);
	}

    /// Executes parser and prepares data vectors
	void run() {
        // clear both vector used in parser
        lexer->resetLexer();

        // start lexer
		while(lexer->yylex() != 0) { }

        // copy global vectors
        triggerTypes = lexer->getTriggerTypes();
        logTypes = lexer->getLogTypes();
	}

	const std::vector<TriggerType> &getTriggerTypes() {
		return triggerTypes;
	}

	const std::vector<LogType> &getLogTypes() {
        return logTypes;
    }

private:
	std::unique_ptr<yyFlexLexer> lexer;
	std::ifstream configurationFile;
    std::vector<TriggerType> triggerTypes;
    std::vector<LogType> logTypes;
};


#endif /* INCLUDE_CONFIGURATIONPARSER_HPP_ */
