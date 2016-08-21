#include <fstream>
#include <sstream>

#include "inipaser.h"
using std::cout;
using std::endl;

void rejectFormatCh(std::string& rawstr)
{
	int strsize = rawstr.size();
	for (int i = 0; i < strsize; ++i)
	{
		if (rawstr[i] == ' ' ||
			rawstr[i] == '\n' ||
			rawstr[i] == '\r' ||
			rawstr[i] == '\t')
			rawstr.erase(i, 1);
	}
}

IniParser::IniParser()
{}

void IniParser::parser(const char* filename)
{
	std::string tmp;
	std::fstream inifile(filename, std::ios::in);

	while (std::getline(inifile, tmp))
	{
		cout << tmp << endl;
		int secleftPos = tmp.find(SECTION_LEFT);
		int secrightPos = tmp.find(SECTION_RIGHT);
		int kvsepPos = tmp.find(KV_SEPARATOR);

		if (secleftPos != std::string::npos &&
			secrightPos != std::string::npos)
		{
			m_vsecs.push_back(tmp.substr(secleftPos + 1, secrightPos - (secleftPos + 1)));
			Section cursec;
			continue;
		}
		if (kvsepPos != std::string::npos)
		{
			std::string key = tmp.substr(0, kvsepPos);
			std::string value = tmp.substr(kvsepPos + 1);
			rejectFormatCh(key);
			rejectFormatCh(value);
			if (m_vsecs.size() > 0 && key.size() > 0 && value.size() > 0)
			{
				iroot::iterator itrinsert = m_iniroot.find(m_vsecs.back());
				if (itrinsert != m_iniroot.end())
				{
					Section& cursec = itrinsert->second;
					cursec[key] = value;
				} else {
					Section cursec;
					cursec[key] = value;
					m_iniroot.insert(std::make_pair(m_vsecs.back(), cursec));
				}
			}
			continue;
		}
	}
}

IniParser::~IniParser()
{}

void IniParser::save(const char*filename)
{
	std::string tmp;
	std::fstream inifile(filename, std::ios::out | std::ios::trunc);

	for (iroot::iterator cursecitr = m_iniroot.begin(); cursecitr != m_iniroot.end(); ++cursecitr)
	{
		tmp = "[" + cursecitr->first + "]" + LF + CR;
		Section& cursec = cursecitr->second;
		for (Section::iterator curkvpairitr = cursec.begin(); curkvpairitr != cursec.end(); ++curkvpairitr)
		{
			tmp += curkvpairitr->first + KV_SEPARATOR + curkvpairitr->second + LF + CR;
		}
		inifile << tmp;
		tmp.clear();
	}

}

void IniParser::set(const char* sec, const char* key, const char* value)
{
	iroot::iterator cursecitr = m_iniroot.find(sec);
	if (cursecitr != m_iniroot.end())
	{
		Section& cursec = cursecitr->second;
		cursec[key] = value;
	}
	else {
		Section cursec;
		cursec[key] = value;
		m_iniroot.insert(std::make_pair(sec, cursec));
	}
}

std::string& IniParser::get(const char* sec, const char* key)
{
	iroot::iterator cursecitr = m_iniroot.find(sec);
	if (cursecitr != m_iniroot.end())
	{
		Section& cursec = cursecitr->second;
		Section::iterator kvitr = cursec.find(key);
		if (kvitr != cursec.end())
			return cursec[key];
		else
			return (std::string());
	}
	else {
		return (std::string());
	}
}

IniParser::Section& IniParser::getSection(const char* sectionName)
{
	iroot::iterator cursecitr = m_iniroot.find(sectionName);
	if (cursecitr != m_iniroot.end())
	{
		return cursecitr->second;
	}
	else {
		return (Section());
	}
}

