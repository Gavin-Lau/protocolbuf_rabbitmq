#ifndef _INIPARSER_H_
#define _INIPARSER_H_

#include <iostream>
#include <vector>
#include <map>

#define SECTION_LEFT	'['
#define SECTION_RIGHT	']'
#define KV_SEPARATOR	'='
#define CR				'\n'
#define LF				'\r'

class IniParser {

public:

	typedef std::map<std::string, std::string>		Section;


	IniParser();
	~IniParser();

	void parser(const char* filename);
	void save(const char*filename);
	void set(const char* sec, const char* key, const char* value);
	std::string& get(const char* sec, const char* key);
	Section& getSection(const char* sectionName);

private:

	
	typedef std::map<std::string, Section>		iroot;
	iroot	m_iniroot;
	std::vector<std::string> m_vsecs;
};

#endif //_INIPARSER_H_