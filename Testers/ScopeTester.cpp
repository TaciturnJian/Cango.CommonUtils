#include <spdlog/spdlog.h>

// 在调试模式下启用记录生存周期
#ifdef _DEBUG
#include <Cango/CommonUtils/ScopeNotifier.hpp>
#define EXAMPLE_TEST_DATA_ENABLE_LOG_LIFETIME : Cango::EnableLogLifetime<TestData>
#else
#define EXAMPLE_TEST_DATA_ENABLE_LOG_LIFETIME
#endif

namespace {
	struct TestData EXAMPLE_TEST_DATA_ENABLE_LOG_LIFETIME
	{
		int Value;
	};
}

int main() {
	spdlog::set_level(spdlog::level::debug);			// 降低日志等级到调试模式，启用日志生存周期的输出
	TestData data{};									// 数据 1
	spdlog::info("sizeof data: {}", sizeof(data)); // 测试数据的内存布局不会改变
	TestData _{};										// 数据 2
	spdlog::info("-----");							// 分割线，观察生存周期输出
}
