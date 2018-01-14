#include <iostream>
#include <fstream>
#include "ResUsageProvider.hpp"
#include <iostream>
#include <thread>

#ifdef __linux__
#include "LinuxResProvider.hpp"

#elif _WIN32
#include "WindowsResProvider.hpp"
#endif
/*
int main()
{
	#ifdef __linux__
	std::unique_ptr<ResUsageProvider> resProvider = std::make_unique<LinuxResProvider>();
	#elif _WIN32
	std::unique_ptr<ResUsageProvider> resProvider = std::make_unique<WindowsResProvider>();
	#endif

	std::this_thread::sleep_for(std::chrono::seconds(1));

	RamState ramState = resProvider->getRamState();

	std::cout<<"Zasoby RAM: "<< std::endl;
	std::cout << "Total RAM: "<<ramState.totalMB()<<std::endl;
	std::cout<<"RAM used [MB]: "<<ramState.currMBUsed()<<std::endl;
	std::cout<<"RAM used [%]: "<<ramState.currPercentageUsed()<<std::endl;
	std::cout<<"App use [MB]: "<<ramState.currMonitorMBUsed()<<std::endl;
	std::cout<<"App use [%]: "<<ramState.currMonitorPercentageUsed()<<std::endl;
	std::cout<<std::endl;

	for(int i = 0; i < 5; ++i)
	{
		CpuState cpuState = resProvider->getCpuState();
		std::cout<<"Zasoby CPU: "<<std::endl;
		std::cout<<"CPU used [%]: "<<cpuState.currPercentageUsed()<<std::endl;
		std::cout<<"CPU used by app [%]: "<<cpuState.currMonitorPercentageUsed()<<std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	HddState hddState = resProvider->getHddState();
	std::cout<<"Self - Read KB/s: "<<hddState.currKBsUsedRead()<<std::endl;
	std::cout<<"Self - Write KB/s: "<<hddState.currKBsUsedWrite()<<std::endl;
	std::cout<<"System - Read KB/s: "<<hddState.currMonitorKBsUsedRead()<<std::endl;
	std::cout<<"System - Write KB/s: "<<hddState.currMonitorKBsUsedWrite()<<std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	hddState = resProvider->getHddState();
	std::cout<<"Self - Read KB/s: "<<hddState.currKBsUsedRead()<<std::endl;
	std::cout<<"Self - Write KB/s: "<<hddState.currKBsUsedWrite()<<std::endl;
	std::cout<<"System - Read KB/s: "<<hddState.currMonitorKBsUsedRead()<<std::endl;
	std::cout<<"System - Write KB/s: "<<hddState.currMonitorKBsUsedWrite()<<std::endl;
	getchar();
	return 0;
}
*/
