cmake_minimum_required(VERSION 3.28)

include(Cango)

project("Cango.CommonUtils")

AddModulesInDirectory(
	DIRECTORY "Modules"
	NAMES
		"AsyncItemPool"
		"CallRateCounterX"
		"Configurations"
		"CounterX"
		"IntervalSleeper"
)

AddCXXModule(
	NAME CommonUtils
	NAMESPACE Cango
	LINKS
		"fmt::fmt"
		"Cango_CommonUtils::AsyncItemPool"
		"Cango_CommonUtils::CallRateCounterX"
		"Cango_CommonUtils::Configurations"
		"Cango_CommonUtils::CounterX"
		"Cango_CommonUtils::IntervalSleeper"
)	
