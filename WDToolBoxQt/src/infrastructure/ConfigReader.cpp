// ConfigReader.cpp: 统一配置读取接口实现
//
#include "ConfigReader.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>

CIniConfigReader::CIniConfigReader()
    : m_bLoaded(false)
{
}

CIniConfigReader::~CIniConfigReader()
{
    Clear();
}

bool CIniConfigReader::LoadFromFile(const QString& strFilePath)
{
    Clear();

    // 检查文件是否存在
    QFileInfo fileInfo(strFilePath);
    if (!fileInfo.exists())
    {
        return false;
    }

    // 读取文件内容
    QFile file(strFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    // 尝试使用合适的编码读取配置文件
    QTextStream stream(&file);
#ifdef Q_OS_WIN
    // Windows 上尝试使用GB2312编码（兼容原有Windows配置文件）
    QTextCodec* codec = QTextCodec::codecForName("GB2312");
    if (codec)
    {
        stream.setCodec(codec);
    }
    else
    {
        // 如果GB2312不可用，使用UTF-8
        stream.setCodec("UTF-8");
    }
#else
    // Linux 上直接使用UTF-8编码
    stream.setCodec("UTF-8");
#endif

    QString strContent = stream.readAll();
    file.close();

    m_strConfigPath = strFilePath;
    m_bLoaded = ParseIniContent(strContent);
    return m_bLoaded;
}

bool CIniConfigReader::ParseIniContent(const QString& strContent)
{
    QString strCurrentSection;
    QStringList lines = strContent.split('\n');

    // 逐行解析
    for (const QString& line : lines)
    {
        QString strLine = line.trimmed();
        if (strLine.isEmpty())
            continue;

        // 判断是否是节 [节名]
        if (strLine.length() > 2 && strLine[0] == '[' &&
            strLine[strLine.length() - 1] == ']')
        {
            strCurrentSection = strLine.mid(1, strLine.length() - 2).trimmed();

            // 如果是新节，记录到顺序列表中
            if (m_mapConfig.find(strCurrentSection) == m_mapConfig.end())
            {
                m_vecSectionOrder.push_back(strCurrentSection);
            }
        }
        // 判断是否是键值对 键名=值
        else if (!strCurrentSection.isEmpty())
        {
            int nEqualPos = strLine.indexOf('=');
            if (nEqualPos > 0)
            {
                QString strKey = strLine.left(nEqualPos).trimmed();
                QString strValue = strLine.mid(nEqualPos + 1).trimmed();

                // 去除引号
                if (strValue.length() > 0 && strValue[0] == '"')
                {
                    strValue = strValue.mid(1);
                }
                if (strValue.length() > 0 && strValue[strValue.length() - 1] == '"')
                {
                    strValue = strValue.left(strValue.length() - 1);
                }

                // 存储键值对
                if (!strKey.isEmpty())
                {
                    m_mapConfig[strCurrentSection][strKey] = strValue;
                }
            }
        }
    }

    return true;
}

bool CIniConfigReader::GetValue(const QString& strSection, const QString& strKey, QString& strValue)
{
    if (!m_bLoaded)
        return false;

    auto itSection = m_mapConfig.find(strSection);
    if (itSection == m_mapConfig.end())
        return false;

    auto itKey = itSection->second.find(strKey);
    if (itKey == itSection->second.end())
        return false;

    strValue = itKey->second;
    return true;
}

QString CIniConfigReader::GetValue(const QString& strSection, const QString& strKey, const QString& strDefault)
{
    QString strValue;
    if (GetValue(strSection, strKey, strValue))
        return strValue;
    return strDefault;
}

bool CIniConfigReader::GetKeys(const QString& strSection, std::vector<QString>& vecKeys)
{
    vecKeys.clear();
    if (!m_bLoaded)
        return false;

    auto itSection = m_mapConfig.find(strSection);
    if (itSection == m_mapConfig.end())
        return false;

    for (const auto& pair : itSection->second)
    {
        vecKeys.push_back(pair.first);
    }

    return true;
}

bool CIniConfigReader::GetSections(std::vector<QString>& vecSections)
{
    vecSections.clear();
    if (!m_bLoaded)
        return false;

    // 按照插入顺序返回节
    for (const auto& section : m_vecSectionOrder)
    {
        if (m_mapConfig.find(section) != m_mapConfig.end())
        {
            vecSections.push_back(section);
        }
    }

    return true;
}

void CIniConfigReader::Clear()
{
    m_bLoaded = false;
    m_strConfigPath.clear();
    m_mapConfig.clear();
    m_vecSectionOrder.clear();
}
