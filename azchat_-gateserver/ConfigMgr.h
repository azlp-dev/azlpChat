#pragma once
#include <map>
#include "single.h"
#include <string>
struct SectionInfo {
	SectionInfo() {}
	SectionInfo(const SectionInfo& si) {
		this->_sec_data = si._sec_data;
	}
	SectionInfo& operator = (const SectionInfo& si) {
		if (this == &si) {
			return *this;
		}
		this->_sec_data = si._sec_data;
		return *this;
	}

	~SectionInfo() { _sec_data.clear(); }

	std::string operator[] (const std::string key) {
		if (_sec_data.find(key) == _sec_data.end()) {
			return "";
		}
		else {
			return _sec_data[key];
		}
	}

	std::map<std::string, std::string> _sec_data;
};

class ConfigMgr:public Singleton<ConfigMgr>
{
	friend class Singleton<ConfigMgr>;
public:
	~ConfigMgr() { _secs_map.clear(); }

	ConfigMgr();
	ConfigMgr(const ConfigMgr& cm) {
		this->_secs_map = cm._secs_map;
	}
	ConfigMgr& operator=(const ConfigMgr& cm) {
		if (this == &cm) {
			return *this;
		}
		else {
			this->_secs_map = cm._secs_map;
			return *this;
		}
	}
	SectionInfo operator[](std::string key) {
		if (_secs_map.find(key) == _secs_map.end()) {
			return SectionInfo();
		}
		return _secs_map[key];
	}

private:
	std::map<std::string, SectionInfo> _secs_map;
};

