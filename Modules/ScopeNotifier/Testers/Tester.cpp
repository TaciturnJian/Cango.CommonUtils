#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <spdlog/spdlog.h>


using namespace Cango;

namespace {
	struct TestData
#ifdef _DEBUG
	 : EnableLogLifetime<TestData> 
#endif
	{
		int Value;
	};
}


int main() {
	spdlog::set_level(spdlog::level::trace);
	TestData data{};
	spdlog::info("sizeof data: {}", sizeof(data));
	TestData _{};
	spdlog::info("-----");
}
