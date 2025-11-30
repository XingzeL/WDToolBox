// ConfigReader.h: 统一配置读取接口
//
#pragma once
#include <vector>
#include <map>
// 配置读取器接口
// 提供统一的配置读取接口，支持不同的配置格式（INI、JSON等）
class IConfigReader
{
public:
	virtual ~IConfigReader() {}

	// 从文件加载配置
	virtual bool LoadFromFile(const CString& strFilePath) = 0;

	// 获取配置值（键值对格式）
	virtual bool GetValue(const CString& strSection, const CString& strKey, CString& strValue) = 0;

	// 获取配置值（带默认值）
	virtual CString GetValue(const CString& strSection, const CString& strKey, const CString& strDefault = _T("")) = 0;

	// 获取节下的所有键
	virtual bool GetKeys(const CString& strSection, std::vector<CString>& vecKeys) = 0;

	// 获取所有节名称
	virtual bool GetSections(std::vector<CString>& vecSections) = 0;

	// 检查配置是否已加载
	virtual bool IsLoaded() const = 0;

	// 清空配置数据
	virtual void Clear() = 0;
};

// INI文件配置读取器实现
class CIniConfigReader : public IConfigReader
{
public:
	CIniConfigReader();
	virtual ~CIniConfigReader();

	// IConfigReader 接口实现
	virtual bool LoadFromFile(const CString& strFilePath) override;
	virtual bool GetValue(const CString& strSection, const CString& strKey, CString& strValue) override;
	virtual CString GetValue(const CString& strSection, const CString& strKey, const CString& strDefault = _T("")) override;
	virtual bool GetKeys(const CString& strSection, std::vector<CString>& vecKeys) override;
	virtual bool GetSections(std::vector<CString>& vecSections) override;
	virtual bool IsLoaded() const override { return m_bLoaded; }
	virtual void Clear() override;

private:
	bool m_bLoaded;
	CString m_strConfigPath;

	// 解析INI文件内容
	bool ParseIniContent(const CString& strContent);

	// 存储结构：节名 -> (键名 -> 值)
	std::map<CString, std::map<CString, CString>> m_mapConfig;
	std::vector<CString> m_vecSectionOrder;  // 保持节的顺序
};

