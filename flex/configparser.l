/* environment preparation */
%top{
	#include "configrawdata.hpp"
	#include <vector>
#include <cmath>
		
	// triggerTypes	
	TriggerType currentTriggerType;
	std::vector<TriggerType> triggerTypes;
	ResourceValue currentResourceValue;
	std::vector<ResourceValue> triggerResourceValues;
	std::vector<long> triggerDurationTimes;
	
	// logTypes
	LogType currentLogType;
	std::vector<LogType> logTypes;
	std::vector<long> logDurations;
	std::vector<long> logResolutin;
	
	void clearHelperVectors() {
		triggerResourceValues.clear();
		triggerDurationTimes.clear();
		logDurations.clear();
		logResolutin.clear();
	}
}
	

	/* ======================================================== */
	/* constans and tokens preparation */

ws    		[ \t]
nonws 		[^ \t\n]
word  		{ws}*{nonws}+
number		[0-9]+

trigger		"trigger"
log			"log"
cpu			"cpu"
memory		"memory"
disk		"disk"
over		"over"
under		"under"
last		"last for"|"last"
resolution	"resolution"


	/* ======================================================== */
	/* starting condition setup and stack enable */
%s logEnv triggerEnv setEnv singleNoEnv noyywrap
%option stack c++
%%

	/* ============================================================================================================= */
	/* ============================================== PARSING ====================================================== */
	/* ============================================================================================================= */
	/* ======================================================== */
	/* trigger parsing */
	/* trigger cpu over {70%, 80%, 90%} last for {20s, 1m} */
	/* trigger memory under 200MB last for 2h30m */
<triggerEnv>{
	{cpu}{ws}+		currentTriggerType.resource = TriggerType::Resource::CPU;
	{memory}{ws}+	currentTriggerType.resource = TriggerType::Resource::MEMORY;
	{disk}{ws}+		currentTriggerType.resource = TriggerType::Resource::DISK;
	
	{under}{ws}+	currentTriggerType.fluctuationType = TriggerType::FluctuationType::UNDER;
	{over}{ws}+		currentTriggerType.fluctuationType = TriggerType::FluctuationType::OVER;
	
		/* bracket with set of values */
	"{"{ws}*		yy_push_state(setEnv);
	<setEnv>{
		
	}
	
		/* plain value */
	{number}		yy_push_state(singleNoEnv); currentResourceValue.value = atol(yytext);
	
	\n				yy_pop_state();
}

	/* ======================================================== */
	/* log parsing */
<logEnv>{
	{cpu}{ws}+		currentLogType.resource = LogType::Resource::CPU;
	{memory}{ws}+	currentLogType.resource = LogType::Resource::MEMORY;
	{disk}{ws}+		currentLogType.resource = LogType::Resource::DISK;

	\n				yy_pop_state();
}

	/* ======================================================== */
	/* environment distinction */
{ws}+				/* consume whitespaces at the beggining */
\n+					/* consume whitespaces at the beggining */
{log}{ws}+			yy_push_state(logEnv);
{trigger}{ws}+		yy_push_state(triggerEnv);
<<EOF>>				clearHelperVectors();
					

%%


int main( int /* argc */, char** /* argv */ )
{
    FlexLexer* lexer = new yyFlexLexer;
    while(lexer->yylex() != 0)
        ;
    return 0;
}


