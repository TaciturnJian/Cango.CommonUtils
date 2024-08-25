#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <spdlog/spdlog.h>


using namespace Cango;

namespace {
	class TestModule {
		CANGO_SCOPE("TestModule");

	public:
		void Test() const {
			CANGO_SCOPE("1");
			{
				CANGO_SCOPE("2");
				{
					CANGO_SCOPE("3");
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
