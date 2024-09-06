# CRTP 

CRTP.md [2024/8/25]@Sango

## 前言

[CRTP C++参考中文版](https://zh.cppreference.com/w/cpp/language/crtp)

作者更愿意称呼 CRTP 为“奇异模板继承”，官方译名为“奇特重现模板模式”，与他人沟通时请使用英文或官方译名。  
CRTP 对应英文只有一个，"Curiously Recurring Template Pattern".

## 从问题开始，到CRTP的解决方案

接口(`Interface`)与实现(`Implementation`)的设计，我们有这样几种实现：

### 在 C/C++ 中，调用子类中指向实现函数的指针

```c

// 接口定义文件
#define nullptr ((void*)(0))
#define FOOD_ID_CAT_FOOD 10
typedef bool (*CanConsumePtr)(int foodID);
struct AnimalInterface {
	CanConsumePtr CanConsume;
};

bool CanConsume(void* animalInterface, const int foodID) {
	AnimalInterface* animal = (AnimalInterface*)animalInterface;
	if (animal == nullptr || animal->CanConsume == nullptr) return false;
	return animal->CanConsume(foodID);
}

// 接口实现文件

struct Cat {
	CanConsumePtr CanConsume;
}

bool Cat_CanConsume(int foodID) {
	return foodID == FOOD_ID_CAT_FOOD;
}

void Cat_Construct(Cat* cat) {
	cat->CanConsume = Cat_CanConsume;
}

```

上面的 c 代码因为语言的糖不够多，所以看着很复杂。用 c++ 就会简单很多。

```c++

// 接口定义文件

enum class FoodID {
	CatFood = 10,
	DogFood = 20,
};

struct AnimalInterface {
	virtual bool CanConsume(FoodID food) = 0;
};

// 接口实现文件

struct Cat : AnimalInterface {
	bool CanConsume(FoodID food) override {
		return food == FoodID::CatFood;
	}
};

```

尽管接口与实现的样子不同，c与c++中生成的结构体有所差异，但是可以认为，他们都在做同一件事：  
在接口的成员中加入指向子类实现功能的指针，子类将在构造时将自己的实现函数指针赋值给接口的成员。

也就是说，这两份代码，都要求修改“接口-实现”设计模式中对象的成员。  
如果我的结构体不希望有一个这样的指针呢？换句话说，有没有一种能在编译期就获取子类这个实现函数的方法呢？

毕竟，我们在编译期就知道了子类的类型，子类的成员和成员函数已经能够确定，所以我们有理由相信，这个问题是可以解决的。

### CRTP 实现

```c++

// 接口定义文件

enum class FoodID {
	CatFood = 10,
	DogFood = 20,
};

template<typename TAnimal>
struct AnimalInterface {
	bool CanConsume(FoodID food) {
		return static_cast<TAnimal*>(this)->CanConsumeImplementation(food);
		// 注意这里，我们将 this 指针转换为了 TAnimal* 类型，然后调用了子类的实现函数
	}
};

// 接口实现文件

struct Cat : AnimalInterface<Cat> { // 注意这里，子类继承了模板类，模板类的模板参数是子类自己
	bool CanConsumeImplementation(FoodID food) {
		return food == FoodID::CatFood;
	}
};

```

利用模板，我们在编译期就把子类的实现函数指针传递给了接口。  
接口通过将自己转换成子类的指针，调用子类的实现函数，实现了接口与实现的分离。

## 总结

CRTP 是一种在编译期就能获取子类实现函数指针的方法，它通过模板的特性，将子类的类型传递给了接口，实现了接口与实现的分离。  
它克服了 vtable （或者说接口中的虚函数表，包含了接口的虚函数，子类的实现函数可能覆盖这个表中的某个位置）带来的对接口和子类中成员的影响。  
CRTP 的缺点是，可读性较差，大部分 ide 都无法正确处理其中接口和实现的关系，导致批量操作麻烦无比。  
另外 CRTP 也具有一些继承固有的缺点，比如说对于接口的改动往往会造成代码结构的灾难性变化。
