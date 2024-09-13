#include <concepts>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <thread>
#include <type_traits>

// c++ 23

namespace {
    /// @brief 将源与 宿|转换器 结合成为一个 任务|新源
    /// @tparam TSource 源，要求可以调用 TSource() 并返回一个对象
    /// @tparam TSinkOrTransformer 宿，要求可以调用 TSink(TSource())
    /// @param source 物品源的引用
    /// @param sinkOrTransformer 物品 宿|转换器 的引用
    /// @return 一个可以直接调用的任务，或者一个新的可以直接调用的源，取决于 sinkOrTransformer
    template<typename TSource, typename TSinkOrTransformer>
    requires std::invocable<TSource>
        && std::invocable<TSinkOrTransformer, std::invoke_result_t<TSource>>
    auto operator >>(TSource&& source, TSinkOrTransformer&& sinkOrTransformer) {
        return [&source, &sinkOrTransformer] {
            if constexpr (std::same_as<std::invoke_result_t<TSinkOrTransformer, std::invoke_result_t<TSource>>, std::void_t<>>)
                sinkOrTransformer(source());
            else
                return sinkOrTransformer(source());
        };
    }

    template<typename T>
    struct optional_pass {
        using type = T;

        std::function<void()> Handle{};

        void operator()() const {
            if (Handle) Handle();
        }
    };

    template<typename TSink, typename TOptionalPass, typename TObject = typename TOptionalPass::type>
    requires std::invocable<TSink, TObject>
    auto operator+(TSink&& sink, const TOptionalPass& pass) {
        return [&sink, &pass] (const std::optional<TObject>& input) {
            if (input) sink(*input);
            else pass();
        };
    }

    /// @brief 将转换器与宿结合成为一个新宿
    /// @tparam TTransformer 转换器，要求可以调用 TTransformer(input)
    /// @tparam TSink 宿，要求可以调用 TSink(TTransformer(input))
    /// @param transformer 转换器的引用
    /// @param sink 宿的引用
    /// @return 一个新宿，接收的类型为转换器的输入的类型
    template<typename TTransformer, typename TSink>
    auto operator >>(TTransformer&& transformer, TSink&& sink) {
        return [&transformer, &sink] (const auto& input) {
            sink(transformer(input));
        };
    }

    /// @brief 将两个宿结合成为一个新宿，接收对象后，依次送到两个宿中
    template<typename TSinkFirst, typename TSinkNext>
    auto operator |(TSinkFirst&& first, TSinkNext&& next) {
        return [&first, &next] (const auto& input) {
            first(input);
            next(input);
        };
    }

    /// @brief 将两个宿结合成为一个新宿，接收对象后，同时送到两个宿中
    template<typename TSinkA, typename TSinkB>
    auto operator ||(TSinkA&& a, TSinkB&& b) {
        return [&a, &b] (const auto& input) {
            std::thread t1{[&a, &input]{ a(input); }};
            std::thread t2{[&b, &input]{ b(input); }};
            t1.join();
            t2.join();
        };
    }

    template<std::invocable TTaskFirst, std::invocable TTaskNext>
    auto operator &(TTaskFirst&& first, TTaskNext&& next) {
        return [&first, &next] {
            first();
            next();
        };
    }

    template<std::invocable TTaskA, std::invocable TTaskB>
    auto operator &&(TTaskA&& a, TTaskB&& b) {
        return [&a, &b] {
            std::thread t1{[&a]{ a(); }};
            std::thread t2{[&b]{ b(); }};
            t1.join();
            t2.join();
        };
    }

    /// @brief 将条件与任务结合成为一个新任务，当条件满足时，循环执行任务
    template<typename TCondition, typename TTask>
    requires std::invocable<TCondition> &&
        std::convertible_to<std::invoke_result_t<TCondition>, bool> &&
        std::invocable<TTask>
    auto operator <<(TCondition&& flow_while, TTask&& task) {
        return [&flow_while, &task] {while(flow_while()) task();};
    }

    template<std::integral TCount, typename TTask>
    requires std::invocable<TTask>
    auto operator <<(TCount count, TTask&& task) {
        return [&task, count] {for(TCount i = 0; i < count; ++i) task();};
    }

    /// @brief 将任务与条件结合成为一个新任务，执行任务，当条件满足时循环
    template<typename TTask, typename TCondition>
    requires std::invocable<TTask> &&
        std::convertible_to<std::invoke_result_t<TCondition>, bool> &&
            std::invocable<TCondition>
    auto operator >> (TTask&& task, TCondition&& flow_while) {
        return [&task, &flow_while] {do {task();} while(flow_while());};
    }
}

int main() {
    auto source = [] -> std::optional<int> {
        thread_local int i = 0;
        if (++i % 3 == 0) return std::nullopt;
        return i;
    };

    auto transformer = [] (const int& i) {
        return i * 2;
    };

    auto sink1 = [] (const int i) {
        static int id = 0;
        std::ostringstream stream{};
        stream << id++ << " = " << i << std::endl;
        std::cout << stream.str();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    };

    auto sink2 = [] (const int i) {
        static int id = 0;
        std::ostringstream stream{};
        stream << id-- << " = " << i << std::endl;
        std::cout << stream.str();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    };

    auto monitor = []<typename T>(T&& what) {
        std::cout << "monitor!\n";
        return std::forward<T>(what);
    };

    auto empty_input_handle = [] {
        std::cout << "empty input!\n";
    };

    /*(
        // 任务1 从源中获取物品，后打印监控消息，发送到 sink1，如果 std::optional 为空，则调用 empty_input_handle
        (10 << ( source >> monitor >> sink1 + optional_pass<int>{empty_input_handle} ))
        // 并行执行
        &&
        // 任务2
        (100 << ( source >> sink2 + optional_pass<int>{empty_input_handle} ))
    )();*/ // ok

    // 段错误
    auto task1 = 100 << (source >> monitor >> sink1 + optional_pass<int>{empty_input_handle});
    auto task2 = 100 << (source >> sink2 + optional_pass<int>{empty_input_handle});
    ( task1 & task2 )();

    std::this_thread::sleep_for(std::chrono::seconds{2});

    return 0;
}
