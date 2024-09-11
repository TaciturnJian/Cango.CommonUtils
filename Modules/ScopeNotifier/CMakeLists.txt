project(Cango.CommonUtils.ScopeNotifier)

AddCXXModule(
	NAME "ScopeNotifier"
	NAMESPACE "Cango_CommonUtils"
	LINKS
		"spdlog::spdlog"
		"Boost::system"
)
