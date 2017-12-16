/* environment preparation */
%top{
	#include "configrawdata.hpp"
	#include <vector>
	#include <cmath>
	#include <iostream>
	#include <fstream>
	
	// duration helpers
	static const long SEC_IN_MIN = 60;
	static const long SEC_IN_HOUR = SEC_IN_MIN * 60;
	static const long SEC_IN_DAY = SEC_IN_HOUR * 24;
	// sumaric duration time in seconds for current element
	static long currentDurationTime;
	// last read value - eg. number of hours. Will be multiplied by
	// specifc multipler (for hour - 3600) and added to currentDurationTime
	static long lastReadDurationElement;
		
	// triggerTypes	
	static TriggerType currentTriggerType;
	static std::vector<TriggerType> triggerTypes;
	static ResourceValue currentResourceValue;
	static std::vector<ResourceValue> triggerResourceValues;
	static std::vector<long> triggerDurationTimes;
	
	// logTypes
	static LogType currentLogType;
	static std::vector<LogType> logTypes;
	static std::vector<long> logDurations;
	static std::vector<long> logResolutions;
	
	static void clearHelperVectors() {
		triggerResourceValues.clear();
		triggerDurationTimes.clear();
		logDurations.clear();
		logResolutions.clear();
	}
	
	static void logTypesValue() {
		std::cout << "trigger times: ";
		for (auto i : triggerDurationTimes)
			std::cout << i << " ";
		std::cout << std::endl;
	}

	static void produceTriggersCrossproduct() {
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
	
	static void produceLogsCrossproduct() {
		for (auto &&duration : logDurations) {
			for (auto &&res : logResolutions) {
				currentLogType.resolution = res;
				currentLogType.duration = duration;
				logTypes.push_back(currentLogType);
			}
		}
		logDurations.clear();
		logResolutions.clear();
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
%s logEnv triggerEnv resourceSetEnv resourceSingleNoEnv triggerDurationSetEnv triggerDurationSingleNoEnv logDurationSetEnv logDurationSingleNoEnv logResolutionSetEnv logResolutionSingleNoEnv
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
	/*======= resource type */
	{cpu}{ws}+		currentLogType.resource = LogType::Resource::CPU;
	{memory}{ws}+	currentLogType.resource = LogType::Resource::MEMORY;
	{disk}{ws}+		currentLogType.resource = LogType::Resource::DISK;

	/*======= log duration time */
	{for}{ws}+"{"{ws}*	yy_push_state(logDurationSetEnv);
	{for}{ws}+			yy_push_state(logDurationSingleNoEnv);

	/* durations set */
	<logDurationSetEnv>{
			{number}		lastReadDurationElement = atol(yytext);
			{ws}*"s"		currentDurationTime += lastReadDurationElement; 				/* no of seconds in seconds */
			{ws}*"m"		currentDurationTime += lastReadDurationElement * SEC_IN_MIN;	/* no of seconds in minute */
			{ws}*"h"		currentDurationTime += lastReadDurationElement * SEC_IN_HOUR;	/* no of seconds in hour */
			{ws}*"d"		currentDurationTime += lastReadDurationElement * SEC_IN_DAY;	/* no of seconds in day */
			{ws}*","{ws}*	logDurations.push_back(currentDurationTime); currentDurationTime = 0;
			
			{ws}*"}"{ws}+	logDurations.push_back(currentDurationTime); currentDurationTime = 0; yy_pop_state(); 
		}
		
	/* plain value */
	<logDurationSingleNoEnv>{
		{number}		lastReadDurationElement = atol(yytext);
		{ws}*"s"		currentDurationTime += lastReadDurationElement; 				/* no of seconds in seconds */
		{ws}*"m"		currentDurationTime += lastReadDurationElement * SEC_IN_MIN;	/* no of seconds in minute */
		{ws}*"h"		currentDurationTime += lastReadDurationElement * SEC_IN_HOUR;	/* no of seconds in hour */
		{ws}*"d"		currentDurationTime += lastReadDurationElement * SEC_IN_DAY;	/* no of seconds in day */
		
		{ws}+			logDurations.push_back(currentDurationTime); currentDurationTime = 0; yy_pop_state();
	}
	
	/*======= log resolution */
	{resolution}{ws}+"{"{ws}*	yy_push_state(logResolutionSetEnv);
	{resolution}{ws}+			yy_push_state(logResolutionSingleNoEnv);
	
	/* resolution set */
	<logResolutionSetEnv>{
			{number}		lastReadDurationElement = atol(yytext);
			{ws}*"s"		currentDurationTime += lastReadDurationElement; 				/* no of seconds in seconds */
			{ws}*"m"		currentDurationTime += lastReadDurationElement * SEC_IN_MIN;	/* no of seconds in minute */
			{ws}*"h"		currentDurationTime += lastReadDurationElement * SEC_IN_HOUR;	/* no of seconds in hour */
			{ws}*"d"		currentDurationTime += lastReadDurationElement * SEC_IN_DAY;	/* no of seconds in day */
			{ws}*","{ws}*	logResolutions.push_back(currentDurationTime); currentDurationTime = 0;

			/* END OF LINE PROCCESSING: pop from stack logResolutionSetEnv and logEnv */
			{ws}*"}"{ws}*\n*	{
				logResolutions.push_back(currentDurationTime); 
				currentDurationTime = 0; 
				yy_pop_state(); yy_pop_state(); /* back to INITIAL */
				produceLogsCrossproduct();
			}
		}
		
	/* plain resolution value */
	<logResolutionSingleNoEnv>{
		{number}		lastReadDurationElement = atol(yytext);
		{ws}*"s"		currentDurationTime += lastReadDurationElement; 				/* no of seconds in seconds */
		{ws}*"m"		currentDurationTime += lastReadDurationElement * SEC_IN_MIN;	/* no of seconds in minute */
		{ws}*"h"		currentDurationTime += lastReadDurationElement * SEC_IN_HOUR;	/* no of seconds in hour */
		{ws}*"d"		currentDurationTime += lastReadDurationElement * SEC_IN_DAY;	/* no of seconds in day */

		/* END OF LINE PROCCESSING: pop from stack logResolutionSingleNoEnv and logEnv */
		{ws}*|\n*		{
			logResolutions.push_back(currentDurationTime); 
			currentDurationTime = 0; 
			yy_pop_state(); yy_pop_state(); /* back to INITIAL */
			produceLogsCrossproduct();
		}
	}
	
}

	/* ======================================================== */
	/* start environment distinction */
{ws}+				/* consume whitespaces at the beggining */
\n+					/* consume whitespaces at the beggining */
{log}{ws}+			yy_push_state(logEnv);
{trigger}{ws}+		yy_push_state(triggerEnv);
<<EOF>>				yyterminate();
					

%%
