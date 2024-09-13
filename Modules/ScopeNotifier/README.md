# Cango.CommonUtils.ScopeNotifier

[[CangoCXXModuleDocumentInfo]]

|  Item   | Value                           |
|:-------:|:--------------------------------|
|  Name   | Cango.CommonUtils.ScopeNotifier |
| Version | 1.0.0                           |
|  Brief  | 作用域、生存期日志通知器                    |

## 依赖

|      Name      | Description |
|:--------------:|:------------|
| spdlog::spdlog | 日志库         |
| Boost::system  | 更好的 typeid  |

## 示例

### 为类型启用生存周期日志

代码示例：

```c++

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

```

可能的输出：

```text
[2024-09-13 15:36:17.792] [debug] (anonymous namespace)::TestData> 构造对象
[2024-09-13 15:36:17.792] [info] sizeof data: 4
[2024-09-13 15:36:17.792] [debug] (anonymous namespace)::TestData> 构造对象
[2024-09-13 15:36:17.792] [info] -----
[2024-09-13 15:36:17.792] [debug] (anonymous namespace)::TestData> 销毁对象
[2024-09-13 15:36:17.792] [debug] (anonymous namespace)::TestData> 销毁对象
```
