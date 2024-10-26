#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <stacktrace>

namespace {
    void foo() {
        Cango::ScopeNotifier notifier{"foo"};
        try {
            throw std::runtime_error{"foo"};
        } catch (const std::exception& e) {
            spdlog::error("Exception: {}.\nStack trace:\n{}", e.what(), std::to_string(std::stacktrace::current()));
        }
    }
}

int main() {
    spdlog::set_level(spdlog::level::debug);

    Cango::ScopeNotifier notifier{"main"};
    foo();

    return 0;
}