project(Cango.CommonUtils.GlobalLogger)

AddCXXModule(
	NAME "GlobalLogger"
	NAMESPACE "Cango::CommonUtils"
	LINKS
		"fmt::fmt"
		"spdlog::spdlog"
)
