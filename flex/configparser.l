/* environment preparation */
%top{
	#include "configrawdata.hpp"
	#include <vector>
	#include <cmath>
#include <iostream>
	
	// duration helpers
	const long SEC_IN_MIN = 60;
	const long SEC_IN_HOUR = SEC_IN_MIN * 60;
	const long SEC_IN_DAY = SEC_IN_HOUR * 24;
	// sumaric duration time in seconds for current element
	long currentDurationTime;
	// last read value - eg. number of hours. Will be multiplied by
	// specifc multipler (for hour - 3600) and added to currentDurationTime
	long lastReadDurationElement;
		
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
		std::cout << "trigger times: ";
		for (auto i : triggerDurationTimes)
			std::cout << i << " ";
		std::cout << std::endl;
	}

	void produceTriggersCrossproduct() {
		for (auto &&res : triggerResourceValues) {
			for (auto &&duration : triggerDurationTimes) {
				currentTriggerType.value = res;
				currentTriggerType.duration = duration;
				triggerTypes.push_back(currentTriggerType);
			}
		}
		triggerResourceValues.clear();
		triggerDurationTimes.clear();
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
MB			(?i:"mb"|"MB/s"|"mbps")
GB			(?i:"gb"|"gb/s"|"gbps")
sec			(?i:"s")
min			(?i:"m")
hour		(?i:"h")
day			(?i:"d")
for			(?i:"for")


	/* ======================================================== */
	/* starting condition setup and stack enable */
%s logEnv triggerEnv resourceSetEnv resourceSingleNoEnv triggerDurationSetEnv triggerDurationSingleNoEnv logDurationSetEnv logDurationSingleNoEnv
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
		/*======= resource type */
	{cpu}{ws}+		currentTriggerType.resource = TriggerType::Resource::CPU;
	{memory}{ws}+	currentTriggerType.resource = TriggerType::Resource::MEMORY;
	{disk}{ws}+		currentTriggerType.resource = TriggerType::Resource::DISK;
	
		/*======= over/under */
	{under}{ws}+"{"{ws}*	currentTriggerType.fluctuationType = TriggerType::FluctuationType::UNDER; yy_push_state(resourceSetEnv);
	{under}{ws}+			currentTriggerType.fluctuationType = TriggerType::FluctuationType::UNDER; yy_push_state(resourceSingleNoEnv);
	{over}{ws}+"{"{ws}*		currentTriggerType.fluctuationType = TriggerType::FluctuationType::OVER; yy_push_state(resourceSetEnv);
	{over}{ws}+				currentTriggerType.fluctuationType = TriggerType::FluctuationType::OVER; yy_push_state(resourceSingleNoEnv);
	
		/* bracket with set of values */
	<resourceSetEnv>{
		{number}		currentResourceValue.value = atol(yytext);
		{ws}*"%"		currentResourceValue.unitType = ResourceValue::ResourceUnit::PERCENT;
		{ws}*{MB}		currentResourceValue.unitType = ResourceValue::ResourceUnit::MB;
		{ws}*{GB}		currentResourceValue.unitType = ResourceValue::ResourceUnit::GB;
		{ws}*","{ws}*	triggerResourceValues.push_back(currentResourceValue);
		
		{ws}*"}"{ws}*	triggerResourceValues.push_back(currentResourceValue); yy_pop_state();
	}
	
		/* plain value */
	<resourceSingleNoEnv>{
		{number}			currentResourceValue.value = atol(yytext);
		{ws}*"%"{ws}+		currentResourceValue.unitType = ResourceValue::ResourceUnit::PERCENT; triggerResourceValues.push_back(currentResourceValue); yy_pop_state();
		{ws}*{MB}{ws}+		currentResourceValue.unitType = ResourceValue::ResourceUnit::MB; triggerResourceValues.push_back(currentResourceValue); yy_pop_state(); 
		{ws}*{GB}{ws}+		currentResourceValue.unitType = ResourceValue::ResourceUnit::GB; triggerResourceValues.push_back(currentResourceValue); yy_pop_state();
	}
	
		/*======= fluctuation duration */
		/* durations set */
	{ws}*{last}{ws}+"{"{ws}*		yy_push_state(triggerDurationSetEnv);
		/* single duration */
	{ws}*{last}{ws}+				yy_push_state(triggerDurationSingleNoEnv);
	
	<triggerDurationSetEnv>{
		{number}		lastReadDurationElement = atol(yytext);
		{ws}*"s"		currentDurationTime += lastReadDurationElement; 				/* no of seconds in seconds */
		{ws}*"m"		currentDurationTime += lastReadDurationElement * SEC_IN_MIN;	/* no of seconds in minute */
		{ws}*"h"		currentDurationTime += lastReadDurationElement * SEC_IN_HOUR;	/* no of seconds in hour */
		{ws}*"d"		currentDurationTime += lastReadDurationElement * SEC_IN_DAY;	/* no of seconds in day */
		{ws}*","{ws}*	triggerDurationTimes.push_back(currentDurationTime); currentDurationTime = 0;
		
			/* END OF LINE PROCCESSING: pop from stack triggerDurationSetEnv and triggerEnv */
		{ws}*"}"{ws}*\n*	{
			triggerDurationTimes.push_back(currentDurationTime); 
			currentDurationTime = 0; 
			yy_pop_state(); yy_pop_state(); /* back to INITIAL */
			produceTriggersCrossproduct();
		}
	}
	
	/* plain value */
	<triggerDurationSingleNoEnv>{
		{number}		lastReadDurationElement = atol(yytext);
		{ws}*"s"		currentDurationTime += lastReadDurationElement; 				/* no of seconds in seconds */
		{ws}*"m"		currentDurationTime += lastReadDurationElement * SEC_IN_MIN;	/* no of seconds in minute */
		{ws}*"h"		currentDurationTime += lastReadDurationElement * SEC_IN_HOUR;	/* no of seconds in hour */
		{ws}*"d"		currentDurationTime += lastReadDurationElement * SEC_IN_DAY;	/* no of seconds in day */
		
			/* END OF LINE PROCCESSING: pop from stack triggerDurationSetEnv and triggerEnv */
		{ws}*|\n*		{
			triggerDurationTimes.push_back(currentDurationTime); 
			currentDurationTime = 0; 
			yy_pop_state(); yy_pop_state(); /* back to INITIAL */
			produceTriggersCrossproduct();
		}
	}
	
}

	/* ======================================================== */
	/* log parsing */
<logEnv>{
	{cpu}{ws}+		currentLogType.resource = LogType::Resource::CPU;
	{memory}{ws}+	currentLogType.resource = LogType::Resource::MEMORY;
	{disk}{ws}+		currentLogType.resource = LogType::Resource::DISK;
	
	{for}{ws}+"{"{ws}*	yy_push_state(logDurationSetEnv);
	{for}{ws}+			yy_push_state(logDurationSingleNoEnv);

	\n				yy_pop_state();
}

	/* ======================================================== */
	/* start environment distinction */
{ws}+				/* consume whitespaces at the beggining */
\n+					/* consume whitespaces at the beggining */
{log}{ws}+			yy_push_state(logEnv);
{trigger}{ws}+		yy_push_state(triggerEnv);
<<EOF>>				yyterminate();
					

%%


int main( int /* argc */, char** /* argv */ )
{
    FlexLexer* lexer = new yyFlexLexer;
    int i = 0;
    while(lexer->yylex() != 0) {
    	
    }
    //logTypesValue();
    std::cout << "trig no: " << triggerTypes.size() << std::endl;
    for (auto &&trig : triggerTypes) {
    	std::cout << trig.value.value << " ";
    }
    std::cout << std::endl;
    //std::cout << "trig no: " << logTypes.size() << std::endl;
    
    return 0;
}


