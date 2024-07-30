cmake_minimum_required(VERSION 3.28)

include(Cango)

project("Cango.CommonUtils")

AddModulesInDirectory(
	DIRECTORY "Modules"
	NAMES
		"AsyncItemPool"
		"CounterX"
		"IntervalSleeper"
		"Configurations"
)

AddCXXModule(
	NAME CommonUtils
	NAMESPACE Cango
	LINKS
		"fmt::fmt"
		"Cango_CommonUtils_AsyncItemPool"
		"Cango_CommonUtils_CounterX"
		"Cango_CommonUtils_IntervalSleeper"
		"Cango_CommonUtils_Configurations"
)	
