#pragma once

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <spdlog/logger.h>

namespace Cango :: inline CommonUtils :: inline Configurations {
	using VariableTable = boost::property_tree::ptree;
	using FilePathType = boost::filesystem::path;

	bool LoadVariableTableFromFile(spdlog::logger& logger, VariableTable& table, const FilePathType& file) noexcept;

	bool SaveVariableTableToFile(spdlog::logger& logger, const VariableTable& table, const FilePathType& file) noexcept;

	template<typename TValue>
	bool ReadVariable(spdlog::logger& logger, const VariableTable& table, const std::string_view& key, TValue& value) {
		const auto value_opt = table.get_optional<TValue>(key.data());
		if (!value_opt) {
			logger.error("DefaultConfigure> key not found: {}", key);
			return false;
		}
		value = *value_opt;
		return true;
	}


}
