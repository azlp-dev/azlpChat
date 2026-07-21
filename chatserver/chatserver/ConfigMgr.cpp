#include "ConfigMgr.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

ConfigMgr::ConfigMgr()
{
	boost::filesystem::path config_path = boost::filesystem::current_path() / "config.ini";
	std::cout << "config.ini: " << config_path << std::endl;

	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	for (auto& section_pair : pt) {
		const std::string& section_name = section_pair.first;
		boost::property_tree::ptree& section_tree = section_pair.second;

		std::map<std::string, std::string> key_value;
		for (auto& key_value_pair : section_tree) {
			const std::string key = key_value_pair.first;
			const std::string value = key_value_pair.second.get_value<std::string>();
			key_value[key] = value;
		}
		SectionInfo si;
		si._sec_data = key_value;
		this->_secs_map[section_name] = si;
	}
	for (const auto& section_pair : this->_secs_map) {
		SectionInfo section_config = section_pair.second;
		std::cout << "[" << section_pair.first << "]" << std::endl;
		for (const auto& key_value_pair : section_config._sec_data) {
			std::cout << key_value_pair.first << " = " << key_value_pair.second << std::endl;
		}
	}
}
