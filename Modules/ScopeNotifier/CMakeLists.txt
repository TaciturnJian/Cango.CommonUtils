project(Cango.CommonUtils.ScopeNotifier)

AddCXXModule(
	NAME "ScopeNotifier"
	NAMESPACE "Cango_CommonUtils"
	LINKS
		"fmt::fmt"
		"spdlog::spdlog"
)
