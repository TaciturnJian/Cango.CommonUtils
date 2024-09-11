project(Cango.CommonUtils.IntervalSleeper)

AddCXXModule(
	NAME "IntervalSleeper"
	NAMESPACE "Cango::CommonUtils"
	LINKS
		"Threads::Threads"
		"Cango_CommonUtils_ScopeNotifier"
)
