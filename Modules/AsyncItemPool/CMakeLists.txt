project(Cango.CommonUtils.AsyncItemPool)

AddCXXModule(
	NAME "AsyncItemPool"
	NAMESPACE "Cango_CommonUtils"
	LINKS
		"Threads::Threads"
		"Cango_CommonUtils_ScopeNotifier"
)
