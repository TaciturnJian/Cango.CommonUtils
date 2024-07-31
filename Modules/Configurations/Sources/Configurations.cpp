#include <functional>
#include <map>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem/fstream.hpp>
#include <Cango/CommonUtils/Configurations.hpp>

namespace {
	using namespace Cango;
	using namespace boost;
	using namespace boost::property_tree;
	using namespace boost::system;
	using namespace boost::filesystem;

	std::string ExtensionToLower(const std::string& extension) noexcept {
		static auto to_lower = [](const unsigned char c) { return std::tolower(c); };

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

	bool SelectReader(spdlog::logger& logger, const FilePathType& path, reader_t& reader) noexcept {
#define SC_READER_HELPER(boost_call) [](const std::string& filename, VariableTable& pt) { boost_call(filename, pt); }
		static const std::map<std::string, reader_t> reader_map{
			{".ini", SC_READER_HELPER(read_ini)},
			{".json", SC_READER_HELPER(read_json)},
			{".xml", SC_READER_HELPER(read_xml)},
			{".info", SC_READER_HELPER(read_info)}
		};
#undef SC_READER_HELPER

		const auto extension = path.extension();
		const auto& extension_string = extension.string();
		const auto extension_lower = ExtensionToLower(extension_string);
		if (!reader_map.contains(extension_lower)) {
			logger.error("不支持的配置文件后缀：{}", extension_string);
			return false;
		}

		reader = reader_map.at(extension_lower);
		return true;
	}

	bool TryRead(
		spdlog::logger& logger,
		const FilePathType& path,
		VariableTable& tree,
		const reader_t& reader) noexcept {
		const auto& path_string = path.string();
		try { reader(path_string, tree); }
		catch (const std::exception& ex) {
			logger.error("无法读取文件({}): {}", path_string, ex.what());
			return false;
		}
		catch (...) {
			logger.error("无法读取文件({}): 未知错误", path_string);
			return false;
		}
		return true;
	}

#ifdef _WINDOWS
#undef CreateFile
#endif

	bool CreateFile(spdlog::logger& logger, const FilePathType& path) {
		if (const ofstream stream{path, fstream::out}; stream.is_open()) return true;
		logger.error("无法创建文件({})", path.string());
		return false;
	}

	bool VerifyPathToWrite(spdlog::logger& logger, const FilePathType& path) noexcept {
		if (error_code result{}; exists(path, result) && !result.failed()) return true;
		return CreateFile(logger, path);
	}

	using writer_t = std::function<void(const std::string&, const VariableTable&)>;

	bool SelectWriter(spdlog::logger& logger, const FilePathType& path, writer_t& writer) noexcept {
#define SC_WRITER_HELPER(boost_call) [](const std::string& filename, const VariableTable& pt) { boost_call(filename, pt); }
		static const std::map<std::string, writer_t> writer_map{
			{".ini", SC_WRITER_HELPER(write_ini)},
			{".json", SC_WRITER_HELPER(write_json)},
			{".xml", SC_WRITER_HELPER(write_xml)},
			{".info", SC_WRITER_HELPER(write_info)}
		};
#undef SC_WRITER_HELPER

		const auto extension = path.extension();
		const auto& extension_string = extension.string();
		const auto extension_lower = ExtensionToLower(extension_string);
		if (!writer_map.contains(extension_lower)) {
			logger.error("不支持的配置文件后缀：{}", extension_string);
			return false;
		}

		writer = writer_map.at(extension_lower);
		return true;
	}

	bool TryWrite(
		spdlog::logger& logger,
		const FilePathType& path,
		const VariableTable& tree,
		const writer_t& writer) noexcept {
		const auto& path_string = path.string();
		try { writer(path_string, tree); }
		catch (const std::exception& ex) {
			logger.error("无法写入配置文件({}): {}", path_string, ex.what());
			return false;
		}
		catch (...) {
			logger.error("无法写入配置文件({}): 未知错误", path_string);
			return false;
		}

		return true;
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

		reader_t reader;
		if (!SelectReader(logger, file, reader)) {
			logger.error("找不到合适读取器读取配置文件({})", path_string);
			return false;
		}

		if (!TryRead(logger, file, table, reader)) {
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

		writer_t writer;
		if (!SelectWriter(logger, file, writer)) {
			logger.error("找不到合适的写入器写入配置文件({})", path_string);
			return false;
		}

		if (!TryWrite(logger, file, table, writer)) {
			logger.error("写入内容到配置文件({})时出现异常", path_string);
			return false;
		}

		return true;
	}
}
