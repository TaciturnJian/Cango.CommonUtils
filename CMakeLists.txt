cmake_minimum_required(VERSION 3.28)

include(Cango)

project("Cango.CommonUtils")

set(RequiredLibraries
	"fmt::fmt"
	"spdlog::spdlog"
	"Boost::system"
	"Boost::filesystem"
	"Threads::Threads"
)
RequireLibraries(RequiredLibraries)

AddModulesInDirectory(
	DIRECTORY "Modules"
	NAMES
		"ScopeNotifier"
		"AsyncItemPool"
		"CallRateCounterX"
		"Configurations"
		"CounterX"
		"GlobalLogger"
		"IntervalSleeper"
		"ObjectOwnership"
		"JoinThreads"
)

AddCXXModule(
	NAME CommonUtils
	NAMESPACE Cango
	LINKS
		"Cango::CommonUtils::AsyncItemPool"
		"Cango::CommonUtils::CallRateCounterX"
		"Cango::CommonUtils::Configurations"
		"Cango::CommonUtils::CounterX"
		"Cango::CommonUtils::GlobalLogger"
		"Cango::CommonUtils::IntervalSleeper"
		"Cango::CommonUtils::ObjectOwnership"
		"Cango::CommonUtils::ScopeNotifier"
		"Cango::CommonUtils::JoinThreads"
)	
