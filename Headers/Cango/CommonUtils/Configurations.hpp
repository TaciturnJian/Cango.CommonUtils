#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <utility>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace Cango :: inline CommonUtils :: inline Configurations {
	using VariableTable = boost::property_tree::ptree;
	using FilePathType = boost::filesystem::path;

	bool LoadVariableTableFromFile(spdlog::logger& logger, VariableTable& table, const FilePathType& file) noexcept;

	bool SaveVariableTableToFile(spdlog::logger& logger, const VariableTable& table, const FilePathType& file) noexcept;

	template<typename TValue>
	bool ReadVariable(spdlog::logger& logger, const VariableTable& table, const std::string_view key, TValue& value) noexcept {
		const auto value_opt = table.get_optional<TValue>(key.data());
		if (value_opt) {
			value = *value_opt;
			return true;
		}
		logger.error("ReadVariable> 没有查找到键：{}", key);
		return false;
	}

	class FileConfigure {
		VariableTable Table{};
		FilePathType File;

	public:
		explicit FileConfigure(FilePathType file) noexcept : File(std::move(file)) {}

		[[nodiscard]] bool Load(spdlog::logger& logger) noexcept {
			return LoadVariableTableFromFile(logger, Table, File);
		}

		[[nodiscard]] bool Load() noexcept {
			return Load(*spdlog::default_logger());
		}

		[[nodiscard]] bool Save(spdlog::logger& logger) const noexcept {
			return SaveVariableTableToFile(logger, Table, File);
		}

		[[nodiscard]] bool Save() const noexcept {
			return Save(*spdlog::default_logger());
		}

		[[nodiscard]] bool Read(spdlog::logger& logger, const std::string_view key, auto& value) const noexcept {
			return ReadVariable(logger, Table, key, value);
		}

		[[nodiscard]] bool Read(const std::string_view key, auto& value) const noexcept {
			return Read(*spdlog::default_logger(), key, value);
		}

		VariableTable* operator->() noexcept {
			return &Table;
		}

		const VariableTable* operator->() const noexcept {
			return &Table;
		}
	};
}
