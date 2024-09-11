project(Cango.CommonUtils.ScopeNotifier)

AddCXXModule(
	NAME "ScopeNotifier"
	NAMESPACE "Cango::CommonUtils"
	LINKS
		"spdlog::spdlog"
		"Boost::system"
)
