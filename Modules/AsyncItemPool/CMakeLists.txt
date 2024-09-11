cmake_minimum_required(VERSION 3.28)

include("Cango")

project(Cango.CommonUtils.AsyncItemPool)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(Cango_CommonUtils_AsyncItemPool_DebugMode ON)
else()
	set(Cango_CommonUtils_AsyncItemPool_DebugMode OFF)
endif()

if ("${Cango_CommonUtils_AsyncItemPool_EnableLogLifetime}" STREQUAL "")
	set(Cango_CommonUtils_AsyncItemPool_EnableLogLifetime ${Cango_CommonUtils_AsyncItemPool_DebugMode})
endif()

if (Cango_CommonUtils_AsyncItemPool_EnableLogLifetime)
	set(Cango_CommonUtils_AsyncItemPool_ExtraLibs "Cango_CommonUtils_ScopeNotifier")
endif()

AddCXXModule(
	NAME "AsyncItemPool"
	NAMESPACE "Cango::CommonUtils"
	LINKS
		"Threads::Threads"
		${Cango_CommonUtils_AsyncItemPool_ExtraLibs}
)
if (Cango_CommonUtils_AsyncItemPool_EnableLogLifetime)
	target_compile_definitions(Cango_CommonUtils_AsyncItemPool
		PUBLIC -DCANGO_CommonUtils_AsyncItemPool_EnableLogLifetime)
endif()