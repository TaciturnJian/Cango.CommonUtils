cmake_minimum_required(VERSION 3.28)

include("Cango")

project("Cango.CommonUtils")

macro(Cango_CommonUtils_DependentScopeNotifier moduleName)
	cmake_dependent_option(Cango_CommonUtils_${moduleName}_EnableScopeNotifier
		"enable scope notifier for ${moduleName}" YES
		"Cango_CommonUtils_EnableScopeNotifier" NO
	)
	message(STATUS "${PROJECT_NAME}> ${moduleName}_EnableScopeNotifier: ${Cango_CommonUtils_${moduleName}_EnableScopeNotifier}")
	if (Cango_CommonUtils_${moduleName}_EnableScopeNotifier)
		list(APPEND Cango_CommonUtils_ScopeNotifier_ExtraCompileDefinitionLines
			"-DCANGO_COMMON_UTILS_ENABLE_SCOPE_NOTIFIER_FOR_${moduleName}"
		)
	endif()
endmacro()

macro(Cango_CommonUtils_AboutScopeNotifier)
	option(Cango_CommonUtils_EnableScopeNotifier "enable scope notifier for most of the utils" YES)
	message(STATUS "${PROJECT_NAME}> EnableScopeNotifier: ${Cango_CommonUtils_EnableScopeNotifier}")

	set(Cango_CommonUtils_ScopeNotifier_ExtraCompileDefinitions "")

	Cango_CommonUtils_DependentScopeNotifier("AsyncItemPool")
	Cango_CommonUtils_DependentScopeNotifier("IntervalSleeper")
	Cango_CommonUtils_DependentScopeNotifier("ObjectOwnership")
endmacro()

Cango_CommonUtils_AboutScopeNotifier()

# add "stdc++_libbacktrace" from /lib/gcc/x86_64-linux-gnu/13/libstdc++_libbacktrace.a
add_library(libbacktrace STATIC IMPORTED)
set_target_properties(libbacktrace PROPERTIES
	IMPORTED_LOCATION "/lib/gcc/x86_64-linux-gnu/13/libstdc++_libbacktrace.a"
)


AddCXXModule(
	NAME "CommonUtils"
	NAMESPACE "Cango"
	HEADER_DIR "Headers"
	SOURCE_DIR "Sources"
	TESTER_DIR "Testers"
	CXX_STANDARD 26
	LINKS
		"fmt::fmt"
		"spdlog::spdlog"
		"Boost::system"
		"Boost::filesystem"
		"Threads::Threads"
		"libbacktrace"
)

if (Cango_CommonUtils_EnableScopeNotifier)
	foreach(compileDefinitionLine IN LISTS Cango_CommonUtils_ScopeNotifier_ExtraCompileDefinitionLines)
		target_compile_definitions(Cango_CommonUtils PUBLIC ${compileDefinitionLine})
	endforeach()
endif()
