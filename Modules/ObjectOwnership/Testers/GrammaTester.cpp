#include <iostream>
#include <Cango/CommonUtils/ObjectOwnership.hpp>

namespace {
	struct A {
		int V1;
		float V2;

		A() noexcept = default;

		A(int v1, float v2) noexcept : V1(v1), V2(v2) {}

		friend std::ostream& operator<<(std::ostream& stream, const A& a) {
			return stream << a.V1 << ',' << a.V2;
		}
	};
}

using namespace Cango;

int main()
{
	auto bts = [](const auto& b) { return b ? "true" : "false";};
	auto out = [](const std::string_view message) {
		std::cout << message << '\n';
	};
	auto print = [&bts, &out](const auto& obj) {
		out(bts(obj));
	};

	Owner<A> a{};
	print(a);

	auto b {Owner<A>::CreateEmpty()};
	print(b);

	return 0;
}
