// ConfigReader.h: 统一配置读取接口
//
#pragma once
#include <vector>
#include <map>
#include <QString>

// 配置读取器接口
// 提供统一的配置读取接口，支持不同的配置格式（INI、JSON等）
class IConfigReader
{
public:
    virtual ~IConfigReader() {}

    // 从文件加载配置
    virtual bool LoadFromFile(const QString& strFilePath) = 0;

    // 获取配置值（键值对格式）
    virtual bool GetValue(const QString& strSection, const QString& strKey, QString& strValue) = 0;

    // 获取配置值（带默认值）
    virtual QString GetValue(const QString& strSection, const QString& strKey, const QString& strDefault = "") = 0;

    // 获取节下的所有键
    virtual bool GetKeys(const QString& strSection, std::vector<QString>& vecKeys) = 0;

    // 获取所有节名称
    virtual bool GetSections(std::vector<QString>& vecSections) = 0;

    // 检查配置是否已加载
    virtual bool IsLoaded() const = 0;

    // 清空配置数据
    virtual void Clear() = 0;

    // 设置配置值
    virtual bool SetValue(const QString& strSection, const QString& strKey, const QString& strValue) = 0;

    // 保存配置到文件
    virtual bool SaveToFile(const QString& strFilePath = "") = 0;
};

// INI文件配置读取器实现
class CIniConfigReader : public IConfigReader
{
public:
    CIniConfigReader();
    virtual ~CIniConfigReader();

    // IConfigReader 接口实现
    virtual bool LoadFromFile(const QString& strFilePath) override;
    virtual bool GetValue(const QString& strSection, const QString& strKey, QString& strValue) override;
    virtual QString GetValue(const QString& strSection, const QString& strKey, const QString& strDefault = "") override;
    virtual bool GetKeys(const QString& strSection, std::vector<QString>& vecKeys) override;
    virtual bool GetSections(std::vector<QString>& vecSections) override;
    virtual bool IsLoaded() const override { return m_bLoaded; }
    virtual void Clear() override;
    virtual bool SetValue(const QString& strSection, const QString& strKey, const QString& strValue) override;
    virtual bool SaveToFile(const QString& strFilePath = "") override;

private:
    bool m_bLoaded;
    QString m_strConfigPath;

    // 解析INI文件内容
    bool ParseIniContent(const QString& strContent);

    // 存储结构：节名 -> (键名 -> 值)
    std::map<QString, std::map<QString, QString>> m_mapConfig;
    std::vector<QString> m_vecSectionOrder;  // 保持节的顺序
};
