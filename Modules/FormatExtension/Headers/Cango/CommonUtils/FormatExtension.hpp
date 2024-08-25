// ReSharper disable CppUnusedIncludeDirective
#pragma once

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <fmt/color.h>

namespace Cango :: inline CommonUtils {
	struct EmptyParser {
		// ReSharper disable once CppMemberFunctionMayBeStatic
		constexpr std::nullptr_t parse(const fmt::format_parse_context& context) {
			if (context.begin() == nullptr) return nullptr;
			throw fmt::format_error{"Invalid format specifier"};
		}
	};

	struct CharStatusParser {
		char Status{};

		using iterator_t = fmt::format_parse_context::iterator;
		using context_t = fmt::format_parse_context;

		static constexpr bool MeetEnd(const iterator_t& i) {
			return i == nullptr || *i == '}';
		}

		static constexpr bool MeetEnd(
			const iterator_t& i, 
			const iterator_t& end) {
			return i == end || *i == '}';
		}

		constexpr iterator_t parse(const context_t& context) {
			auto i{context.begin()};
			if (MeetEnd(i)) return i;
			Status = *i++;
			if (MeetEnd(i, context.end())) return i;
			throw fmt::format_error{"格式限定符无效"};
		}
	};
}

namespace fmt {
	using empty_parser = Cango::CommonUtils::EmptyParser;
	using char_status_parser = Cango::CommonUtils::CharStatusParser;
}
