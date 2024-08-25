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
		constexpr fmt::format_parse_context::iterator parse(const fmt::format_parse_context& context) {
			auto i{context.begin()};
			const auto end{context.end()};

			if (i == end || *i == '}') {
				Status = 0;
				return i;
			}

			Status = *i++;
			if (i == end || *i == '}') return i;
			throw fmt::format_error{"Invalid kv format specifier"};
		}

		char Status;
	};
}

namespace fmt {
	using empty_parser = Cango::CommonUtils::EmptyParser;
	using char_status_parser = Cango::CommonUtils::CharStatusParser;
}
