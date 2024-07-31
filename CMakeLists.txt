cmake_minimum_required(VERSION 3.28)

include(Cango)

project("Cango.CommonUtils")

set(RequiredLibraries
	"fmt::fmt"
	"spdlog::spdlog"
	"Boost::system"
	"Threads::Threads"
)
RequireLibraries(RequiredLibraries)

AddModulesInDirectory(
	DIRECTORY "Modules"
	NAMES
		"AsyncItemPool"
		"CallRateCounterX"
		"Configurations"
		"CounterX"
		"GlobalLogger"
		"IntervalSleeper"
)

AddCXXModule(
	NAME CommonUtils
	NAMESPACE Cango
	LINKS
		"fmt::fmt"
		"spdlog::spdlog"
		"Boost::system"
		"Threads::Threads"
		"Cango_CommonUtils::AsyncItemPool"
		"Cango_CommonUtils::CallRateCounterX"
		"Cango_CommonUtils::Configurations"
		"Cango_CommonUtils::CounterX"
		"Cango_CommonUtils::GlobalLogger"
		"Cango_CommonUtils::IntervalSleeper"
)	
