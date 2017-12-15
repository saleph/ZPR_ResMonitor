/* environment preparation */
%top{
	#include "configrawdata.hpp"
	#include <vector>
	#include <cmath>
#include <iostream>
		
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
	
	void logTypesValue() {
		for (auto i : triggerResourceValues)
			std::cout << "trigger: " << i.value << std::endl;
	}
}
	

	/* ======================================================== */
	/* constans and tokens preparation */
ws    		[ \t]
nonws 		[^ \t\n]
word  		{ws}*{nonws}+
number		[0-9]+

trigger		(?i:"trigger")
log			(?i:"log")
cpu			(?i:"cpu")
memory		(?i:"memory")
disk		(?i:"disk")
over		(?i:"over")
under		(?i:"under")
last		(?i:"last for"|"last")
resolution	(?i:"resolution")
MB			(?i:"mb"|"mb/s"|"mbps")
GB			(?i:"gb"|"gb/s"|"gbps")


	/* ======================================================== */
	/* starting condition setup and stack enable */
%s logEnv triggerEnv setEnv singleNoEnv
%option stack noyywrap c++ debug
%%

	/* ============================================================================================================= */
	/* ============================================== PARSING ====================================================== */
	/* ============================================================================================================= */
	/* ======================================================== */
	/* trigger parsing */
	/* trigger cpu over {70%, 80%, 90%} last for {20s, 1m} */
	/* trigger memory under 200MB last for 2h30m */
<triggerEnv>{
		/* resource type */
	{cpu}{ws}+		currentTriggerType.resource = TriggerType::Resource::CPU;
	{memory}{ws}+	currentTriggerType.resource = TriggerType::Resource::MEMORY;
	{disk}{ws}+		currentTriggerType.resource = TriggerType::Resource::DISK;
	
		/* over/under */
	{under}{ws}+	currentTriggerType.fluctuationType = TriggerType::FluctuationType::UNDER;
	{over}{ws}+		currentTriggerType.fluctuationType = TriggerType::FluctuationType::OVER;
	
	
		/* bracket with set of values */
	"{"{ws}*		yy_push_state(setEnv);
	
		/* plain value */
	{number}		currentResourceValue.value = atol(yytext); yy_push_state(singleNoEnv); logTypesValue();
	
		/* bracket with set of values */
	<setEnv>{
		{number}		currentResourceValue.value = atol(yytext); logTypesValue();
		{ws}*"%"		currentResourceValue.unitType = ResourceValue::ResourceUnit::PERCENT;
		{ws}*{MB}		currentResourceValue.unitType = ResourceValue::ResourceUnit::MB;
		{ws}*{GB}		currentResourceValue.unitType = ResourceValue::ResourceUnit::GB;
		{ws}*","		triggerResourceValues.push_back(currentResourceValue);
		
		{ws}*"}"{ws}*	triggerResourceValues.push_back(currentResourceValue); yy_pop_state();
	}
	
		/* plain value */
	<singleNoEnv>{
		{ws}*"%"{ws}+		currentResourceValue.unitType = ResourceValue::ResourceUnit::PERCENT; yy_pop_state();
		{ws}*{MB}{ws}+		currentResourceValue.unitType = ResourceValue::ResourceUnit::MB; yy_pop_state(); 
		{ws}*{GB}{ws}+		currentResourceValue.unitType = ResourceValue::ResourceUnit::GB; yy_pop_state();
	}
	
	[^\n]	
	
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
<<EOF>>				clearHelperVectors(); yyterminate();
					

%%


int main( int /* argc */, char** /* argv */ )
{
    FlexLexer* lexer = new yyFlexLexer;
    while(lexer->yylex() != 0)
        ;
    std::cout << "trig no: " << triggerTypes.size() << std::endl;
    std::cout << "trig no: " << logTypes.size() << std::endl;
    
    return 0;
}


