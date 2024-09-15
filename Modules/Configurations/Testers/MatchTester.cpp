#include <concepts>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <thread>
#include <type_traits>
#include <Cango/CommonUtils/Configurations.hpp>

// c++ 23

/*
namespace {

    /// @brief 将源与 宿|转换器 结合成为一个 任务|新源
    /// @tparam TSource 源，要求可以调用 TSource() 并返回一个对象
    /// @tparam TSinkOrTransformer 宿，要求可以调用 TSink(TSource())
    /// @param source 物品源的引用
    /// @param sot 物品 宿|转换器 的引用
    /// @return 一个可以直接调用的任务，或者一个新的可以直接调用的源，取决于 sinkOrTransformer
    template<typename TSource, typename TSinkOrTransformer>
    requires std::invocable<TSource>
        && std::invocable<TSinkOrTransformer, std::invoke_result_t<TSource>>
    auto operator >>(TSource&& source, TSinkOrTransformer&& sot) {
        return [source = std::forward<TSource>(source), sot = std::forward<TSinkOrTransformer>(sot)] {
            if constexpr (std::same_as<std::invoke_result_t<TSinkOrTransformer, std::invoke_result_t<TSource>>, std::void_t<>>)
                sot(source());
            else
                return sot(source());
        };
    }

    template<typename T>
    struct optional_pass {
        using type = T;

        std::function<void()> Handle;

        void operator()() const {
            if (Handle) Handle();
        }
    };

    template<typename TSink, typename TOptionalPass, typename TObject = typename TOptionalPass::type>
    requires std::invocable<TSink, TObject>
    auto operator+(const TSink& sink, const TOptionalPass& pass) {
        return [&sink, &pass] (const std::optional<TObject> input) {
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
    auto operator >>(const TTransformer& transformer, const TSink& sink) {
        return [&transformer, &sink] (const auto& input) {
            sink(transformer(input));
        };
    }

    /// @brief 将两个宿结合成为一个新宿，接收对象后，依次送到两个宿中
    template<typename TSinkFirst, typename TSinkNext>
    auto operator |(const TSinkFirst& first, const TSinkNext& next) {
        return [&first, &next] (const auto& input) {
            first(input);
            next(input);
        };
    }

    /// @brief 将两个宿结合成为一个新宿，接收对象后，同时送到两个宿中
    template<typename TSinkA, typename TSinkB>
    auto operator ||(const TSinkA& a, const TSinkB& b) {
        return [&a, &b] (const auto& input) {
            std::thread t1{[&a, &input]{ a(input); }};
            std::thread t2{[&b, &input]{ b(input); }};
            t1.join();
            t2.join();
        };
    }

    template<std::invocable TTaskFirst, std::invocable TTaskNext>
    auto operator &(TTaskFirst&& first, TTaskNext&& next) {
        return [first = std::forward<TTaskFirst>(first), next = std::forward<TTaskNext>(next)] {
            first();
            next();
        };
    }

    template<std::invocable TTaskA, std::invocable TTaskB>
    auto operator &&(const TTaskA& a, const TTaskB& b) {
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
    auto operator <<(TCondition&& whileCondition, TTask&& task) {
        return [while_condition = std::forward<TCondition>(whileCondition), task = std::forward<TTask>(task)] {
            while(while_condition()) task();
        };
    }

    template<std::integral TCount, typename TTask>
        requires std::invocable<TTask>
    auto operator <<(const TCount count, TTask&& task) {
        // 若传入的任务为一个万能引用，则将其复制转发进新任务中
        // 不符合左值与右值的引用，采用复制手段确保任务在上下文中可用
        return [count, task = std::forward<TTask>(task)] {for(TCount i = 0; i < count; ++i) task();};
    }

    /// @brief 将任务与条件结合成为一个新任务，执行任务，当条件满足时循环
    template<typename TTask, typename TCondition>
    requires std::invocable<TTask> &&
        std::convertible_to<std::invoke_result_t<TCondition>, bool> &&
            std::invocable<TCondition>
    auto operator >> (const TTask& task, const TCondition& flow_while) {
        return [&task, &flow_while] {do {task();} while(flow_while());};
    }

    struct int_source {
        mutable int i{};

        auto operator()() const -> std::optional<int> {
            return i++;
        }
    };
}
*/

int main() {
    Cango::FileConfigure config{"config.json"};

    if (!config.Load()) {
        config->put("name", "Cango");
        (void)config.Save();
    }

    std::string value{};
    if (!config.Read("name", value))
        spdlog::warn("cannot read name");
    else
        spdlog::info("name: {}", value);

    return 0;
}
