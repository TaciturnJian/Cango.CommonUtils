# Format

Format.md [2024/9/6]@Sango

## 前言 

`std::format` 都出来好久了，但是咱就是说能不能有个比较好的总结。  
要么缺代码，要么格式化表占一大堆篇幅，唉。  

## 使用 `std::format` 格式化字符串

```c++

auto hello_world = std::format("Hello, {}!", "world");
std::cout << hello_world << std::endl;

```

## 特化 `std::formatter` 以支持自定义类型

```c++

// 需要格式化的类型
struct MyType {
    int Key;
    int Value;
};


#ifdef SIMPLE_FORMATTER
template<>
struct std::formatter<MyType> {
    // 指示格式化器如何解析格式字符串，在此函数中，可以解析格式保存为状态
    constexpr auto parse(const format_parse_context& ctx) {
        return ctx.begin();
    }

    // 指示格式化器如何格式化类型，在此函数中，可以利用解析时得到的状态进行格式化
    auto format(const MyType& mt, format_context& ctx) const {
        return format_to(ctx.out(), "Key: {}, Value: {}", mt.Key, mt.Value);
    }
};
#else
template<>
struct std::formatter<MyType> {
    enum class FormatType {
        KeyAndValue, // by default
        KeyOnly, // -> 'k'
        ValueOnly // -> 'v'
    };
    
    FormatType Type{KeyAndValue};
    
    constexpr auto parse(const format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        switch (*it) {
        case 'k':
            Type = FormatType::KeyOnly;
            break;
        case 'v':
            Type = FormatType::ValueOnly;
            break;
        default:
            Type = FormatType::KeyAndValue;
            break;
        }
        return ++it;
    }
    
    auto format(const MyType& mt, format_context& ctx) const {
        switch (Type) {
        case FormatType::KeyOnly:
            return format_to(ctx.out(), "{}", mt.Key);
        case FormatType::ValueOnly:
            return format_to(ctx.out(), "{}", mt.Value);
        default:
            return format_to(ctx.out(), "{}={}", mt.Key, mt.Value);
        }
    }
};
#endif

int main() {
    MyType mt{1, 2};
    std::cout << std::format("{}", mt) << std::endl;
    #ifndef SIMPLE_FORMATTER
    std::cout << std::format("{:k}", mt) << std::endl;
    std::cout << std::format("{:v}", mt) << std::endl;
    #endif
    return 0;
}

```

## 继承 `std::formatter` 以支持自定义类型

```c++

struct MyType {
    double Key;
    double Value;
};

template<>
struct std::formatter<MyType> : std::formatter<double> {
    // 当前结构体在解析时记录了用户的小数格式化要求，现在在格式化时应用到自定义类型上

    auto format(const MyType& mt, format_context& ctx) const {
        std::formatter<double>::format(mt.Key, ctx);
        std::format_to(ctx.out(), ","); // separator
        return std::formatter<double>::format(mt.Value, ctx);
    }
};

```
