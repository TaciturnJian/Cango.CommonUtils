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
		"ScopeNotifier"
		"AsyncItemPool"
		"CallRateCounterX"
		"Configurations"
		"CounterX"
		"GlobalLogger"
		"IntervalSleeper"
		"ObjectOwnership"
		"JoinThreads"
		"FormatExtension"
)

AddCXXModule(
	NAME CommonUtils
	NAMESPACE Cango
	LINKS
		"fmt::fmt"
		"Boost::system"
		"Cango::CommonUtils::AsyncItemPool"
		"Cango::CommonUtils::CallRateCounterX"
		"Cango::CommonUtils::Configurations"
		"Cango::CommonUtils::CounterX"
		"Cango::CommonUtils::GlobalLogger"
		"Cango::CommonUtils::IntervalSleeper"
		"Cango::CommonUtils::ObjectOwnership"
		"Cango::CommonUtils::ScopeNotifier"
		"Cango::CommonUtils::JoinThreads"
		"Cango::CommonUtils::FormatExtension"
)	
