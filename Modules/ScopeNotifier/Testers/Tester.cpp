#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <spdlog/spdlog.h>

int main() {
	Cango::ScopeNotifier notifier{"测试", spdlog::default_logger(), spdlog::level::info};
}
