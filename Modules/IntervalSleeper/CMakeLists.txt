project(Cango.CommonUtils.IntervalSleeper)

AddCXXModule(
	NAME "IntervalSleeper"
	NAMESPACE "Cango_CommonUtils"
	LINKS
		"Threads::Threads"
		"Cango_CommonUtils_ScopeNotifier"
)
