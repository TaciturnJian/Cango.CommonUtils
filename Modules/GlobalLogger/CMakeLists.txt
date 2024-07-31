project(Cango.CommonUtils.GlobalLogger)

AddCXXModule(
	NAME "GlobalLogger"
	NAMESPACE "Cango_CommonUtils"
	LINKS
		"fmt::fmt"
		"spdlog::spdlog"
)
