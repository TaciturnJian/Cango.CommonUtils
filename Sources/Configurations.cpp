#include <functional>
#include <optional>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <Cango/CommonUtils/Configurations.hpp>
#include <frozen/map.h>

namespace {
	using namespace Cango;
	using namespace boost;
	using namespace boost::property_tree;
	using namespace boost::system;
	using namespace boost::filesystem;

	std::string ExtensionToLower(const std::string& extension) noexcept {
		static auto to_lower = [](const unsigned char c) noexcept { return std::tolower(c); };

		std::string extension_lower{extension};
		std::ranges::transform(extension, extension_lower.begin(), to_lower);
		return extension_lower;
	}

	bool VerifyPathToRead(spdlog::logger& logger, const FilePathType& path) noexcept {
		const auto& path_string = path.string();

		if (error_code result; !exists(path, result)) {
			if (result.failed()) {
				logger.error("检查路径({})是否存在时发生错误：{}", path_string, result.what());
				return false;
			}
			logger.error("给定的路径({})不存在", path_string);
			return false;
		}

		if (error_code result; !is_regular_file(path, result)) {
			if (result.failed()) {
				logger.error("检查路径({})是否为普通文件时发生错误：{}", path_string, result.what());
				return false;
			}
			logger.error("给定的路径({})不是普通文件", path_string);
			return false;
		}

		return true;
	}

	using reader_t = std::function<void(const std::string&, VariableTable&)>;

	std::optional<reader_t> SelectReader(spdlog::logger& logger, const FilePathType& path) noexcept {
#define sc_generate_reader(boost_call) [](const std::string& filename, VariableTable& table) noexcept { boost_call(filename, table); }
		static const frozen::map<std::string, reader_t, 4> reader_map{
			{".ini", sc_generate_reader(read_ini)},
			{".json", sc_generate_reader(read_json)},
			{".xml", sc_generate_reader(read_xml)},
			{".info", sc_generate_reader(read_info)}
		};
#undef sc_generate_reader

		const auto extension = path.extension();
		const auto& extension_string = extension.string();
		const auto extension_lower = ExtensionToLower(extension_string);
		if (!reader_map.contains(extension_lower)) {
			logger.error("不支持的配置文件后缀：{}", extension_string);
			return std::nullopt;
		}

		return reader_map.at(extension_lower);
	}

	bool TryRead(
		spdlog::logger& logger,
		const FilePathType& path,
		VariableTable& table,
		const reader_t& reader) noexcept
	try {
		reader(path.string(), table);
		return true;
	}
	catch (const std::exception& ex) {
		logger.error("无法读取文件({}): {}", path.string(), ex.what());
		return false;
	}
	catch (...) {
		logger.error("无法读取文件({}): 未知错误", path.string());
		return false;
	}

	bool CreateConfigFile(spdlog::logger& logger, const FilePathType& path) {
		if (const ofstream stream{path, fstream::out}; stream.is_open()) return true;
		logger.error("无法创建文件({})", path.string());
		return false;
	}

	bool VerifyPathToWrite(spdlog::logger& logger, const FilePathType& path) noexcept {
		if (error_code result{}; exists(path, result) && !result.failed()) return true;
		return CreateConfigFile(logger, path);
	}

	using writer_t = std::function<void(const std::string&, const VariableTable&)>;

	std::optional<writer_t> SelectWriter(spdlog::logger& logger, const FilePathType& path) noexcept {
#define sc_generate_writer(boost_call) [](const std::string& filename, const VariableTable& pt) noexcept { boost_call(filename, pt); }
		static const frozen::map<std::string, writer_t, 4> writer_map{
			{".ini", sc_generate_writer(write_ini)},
			{".json", sc_generate_writer(write_json)},
			{".xml", sc_generate_writer(write_xml)},
			{".info", sc_generate_writer(write_info)}
		};
#undef sc_generate_writer

		const auto extension = path.extension();
		const auto& extension_string = extension.string();
		const auto extension_lower = ExtensionToLower(extension_string);
		if (!writer_map.contains(extension_lower)) {
			logger.error("不支持的配置文件后缀：{}", extension_string);
			return std::nullopt;
		}
		return writer_map.at(extension_lower);
	}

	bool TryWrite(
		spdlog::logger& logger,
		const FilePathType& path,
		const VariableTable& table,
		const writer_t& writer) noexcept
	try {
		writer(path.string(), table);
		return true;
	}
	catch (const std::exception& ex) {
		logger.error("无法写入配置文件({}): {}", path.string(), ex.what());
		return false;
	}
	catch (...) {
		logger.error("无法写入配置文件({}): 未知错误", path.string());
		return false;
	}
}

namespace Cango :: inline CommonUtils :: inline Configurations {
	bool LoadVariableTableFromFile(
		spdlog::logger& logger,
		VariableTable& table,
		const FilePathType& file) noexcept {
		const auto& path_string = file.string();
		if (!VerifyPathToRead(logger, file)) {
			logger.error("无法读取配置文件({})", path_string);
			return false;
		}

		const auto reader = SelectReader(logger, file);
		if (!reader) {
			logger.error("找不到合适读取器读取配置文件({})", path_string);
			return false;
		}

		if (!TryRead(logger, file, table, *reader)) {
			logger.error("读取配置文件({})的内容时出现错误", path_string);
			return false;
		}

		return true;
	}

	bool SaveVariableTableToFile(
		spdlog::logger& logger,
		const VariableTable& table,
		const FilePathType& file) noexcept {
		const auto& path_string = file.string();

		if (!VerifyPathToWrite(logger, file)) {
			logger.error("无法写入配置文件({})", path_string);
			return false;
		}

		const std::optional<writer_t> writer = SelectWriter(logger, file);
		if (!writer) {
			logger.error("找不到合适的写入器写入配置文件({})", path_string);
			return false;
		}

		if (!TryWrite(logger, file, table, *writer)) {
			logger.error("写入内容到配置文件({})时出现异常", path_string);
			return false;
		}

		return true;
	}
}
