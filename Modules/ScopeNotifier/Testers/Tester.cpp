#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <spdlog/spdlog.h>


using namespace Cango;

namespace {
	class TestModule {
		DebugScopeNotifier("测试模块");

	public:
		void Test() const {
			DebugScopeNotifier("1");
			{
				DebugScopeNotifier("2");
				{
					DebugScopeNotifier("3");
				}
			}
		}
	};
}


int main() {
	spdlog::set_level(spdlog::level::trace);

	TestModule module{};
	module.Test();
}
