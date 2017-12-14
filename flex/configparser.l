/* environment preparation */
%top{
	#include "configrawdata.hpp"
	#include <vector>
		
	// triggerTypes	
	TriggerType currentTriggerType;
	std::vector<TriggerType> triggerTypes;
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
%s logEnv triggerEnv setEnv
%option stack
%%

	/* ============================================================================================================= */
	/* ============================================== PARSING ====================================================== */
	/* ============================================================================================================= */
	/* environment distinction */
{ws}+				/* consume whitespaces at the beggining */
\n+					/* consume whitespaces at the beggining */
{log}{ws}+			yy_push_state(logEnv);
{trigger}{ws}+		yy_push_state(triggerEnv);


	/* ======================================================== */
	/* trigger parsing */
<triggerEnv>{
	{cpu}{ws}+		currentTriggerType.resource = TriggerType::Resource::CPU;
	{memory}{ws}+	currentTriggerType.resource = TriggerType::Resource::MEMORY;
	{disk}{ws}+		currentTriggerType.resource = TriggerType::Resource::DISK;
}

	/* ======================================================== */
	/* log parsing */
<logEnv>{
	{cpu}{ws}+		currentLogType.resource = LogType::Resource::CPU;
	{memory}{ws}+	currentLogType.resource = LogType::Resource::MEMORY;
	{disk}{ws}+		currentLogType.resource = LogType::Resource::DISK;

}

<<EOF>>				{
					clearHelperVectors();
					}

%%

