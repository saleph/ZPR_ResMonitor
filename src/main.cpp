//#define BOOST_TEST_MODULE My Test
//#include <boost/test/included/unit_test.hpp>
//
//BOOST_AUTO_TEST_CASE(first_test)
//{
//  int i = 1;
//  BOOST_TEST(i);
//  BOOST_TEST(i == 2);
//}

#include <iostream>
#include <fstream>
#include "LinuxResProvider.hpp"
#include <unistd.h>

int main()
{
	LinuxResProvider linuxRes;
	RamState ramState = linuxRes.getRamState();

	std::cout<<"Zasoby RAM: "<< std::endl;
	std::cout << "Total RAM: "<<ramState.totalMB()<<std::endl;
	std::cout<<"RAM used [MB]: "<<ramState.currMBUsed()<<std::endl;
	std::cout<<"RAM used [%]: "<<ramState.currPercentageUsed()<<std::endl;
	std::cout<<"App use [MB]: "<<ramState.currMonitorMBUsed()<<std::endl;
	std::cout<<"App use [%]: "<<ramState.currMonitorPercentageUsed()<<std::endl;
	std::cout<<std::endl;

	for(int i = 0; i < 5; ++i)
	{
		CpuState cpuState = linuxRes.getCpuState();
		std::cout<<"Zasoby CPU: "<<std::endl;
		std::cout<<"CPU used [%]: "<<cpuState.currPercentageUsed()<<std::endl;
		std::cout<<"CPU used by app [%]: "<<cpuState.currMonitorPercentageUsed()<<std::endl;
		sleep(1);
	}

	HddState hddState = linuxRes.getHddState();
	std::cout<<"Self - Read KB/s: "<<hddState.currMonitorKBsUsedRead()<<std::endl;
	std::cout<<"Self - Write KB/s: "<<hddState.currMonitorKBsUsedWrite()<<std::endl;
	sleep(1);
	std::cout<<"System - Read KB/s: "<<hddState.currKBsUsedRead()<<std::endl;
	std::cout<<"System - Write KB/s: "<<hddState.currMonitorKBsUsedWrite()<<std::endl;
	return 0;
}
