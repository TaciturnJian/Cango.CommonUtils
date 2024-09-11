project(Cango.CommonUtils.Configurations)

AddCXXModule(
	NAME "Configurations"
	NAMESPACE "Cango::CommonUtils"
	LINKS 
		"fmt::fmt"
		"spdlog::spdlog"
		"Boost::system"
		"Boost::filesystem"
)
