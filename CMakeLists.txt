cmake_minimum_required(VERSION 3.28)

include(Cango)

project("Cango.CommonUtils")

AddModulesInDirectory(
	DIRECTORY "Modules"
	NAMES
		"AsyncItemPool"
		"CounterX"
		"IntervalSleeper"
)

AddCXXModule(
	NAME CommonUtils
	NAMESPACE Cango
	LINKS
		"Cango_CommonUtils_AsyncItemPool"
		"Cango_CommonUtils_CounterX"
		"Cango_CommonUtils_IntervalSleeper"
)	
