#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <spdlog/spdlog.h>

int main() {
	spdlog::set_level(spdlog::level::trace);

	Cango::ScopeNotifier notifier1{"测试1"};
	{
		Cango::ScopeNotifier notifier2{"测试2"};
		{
			Cango::ScopeNotifier notifier3{"测试3"};
		}
	}
}
