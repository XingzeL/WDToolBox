// ConfigReader.cpp: 统一配置读取接口实现
//
#include "ConfigReader.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>
#include <QDebug>

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

    // 检测文件编码（通过BOM）
    QByteArray bom = file.read(3);
    file.seek(0);  // 重置文件指针到开头

    QTextStream stream(&file);
    QString strContent;

    // 检测UTF-8 BOM (0xEF 0xBB 0xBF)
    if (bom.size() >= 3 &&
        static_cast<unsigned char>(bom[0]) == 0xEF &&
        static_cast<unsigned char>(bom[1]) == 0xBB &&
        static_cast<unsigned char>(bom[2]) == 0xBF)
    {
        // 跳过BOM（QTextStream会自动处理，但我们已经读取了，需要跳过）
        qDebug() << "Is UTF-8 with Bom";
        file.seek(3);
        stream.setCodec("UTF-8");
        strContent = stream.readAll();
    }
    // 检测UTF-16 LE BOM (0xFF 0xFE)
    else if (bom.size() >= 2 &&
             static_cast<unsigned char>(bom[0]) == 0xFF &&
             static_cast<unsigned char>(bom[1]) == 0xFE)
    {
        stream.setCodec("UTF-16LE");
        strContent = stream.readAll();
    }
    // 检测UTF-16 BE BOM (0xFE 0xFF)
    else if (bom.size() >= 2 &&
             static_cast<unsigned char>(bom[0]) == 0xFE &&
             static_cast<unsigned char>(bom[1]) == 0xFF)
    {
        stream.setCodec("UTF-16BE");
        strContent = stream.readAll();
    }
    // 没有BOM，尝试自动检测
    else
    {
#ifdef Q_OS_WIN
        qDebug() << "Not UTF-8 with Bom";
        // Windows 上先尝试UTF-8（优先，因为新文件都是UTF-8）
        // 然后如果UTF-8读取结果可疑，再尝试GB2312（兼容原有Windows配置文件）
        stream.setCodec("UTF-8");
        QString strContentUtf8 = stream.readAll();

        // 验证UTF-8读取结果是否有效
        bool bUtf8Valid = true;
        // 检查是否包含替换字符（表示编码错误）
        if (strContentUtf8.contains(QChar(0xFFFD)))
        {
            bUtf8Valid = false;
        }
        // 检查是否包含明显的乱码模式（可选：检查是否包含无效的UTF-8序列）
        // 这里简单检查：如果文件很小（<10字节）或包含明显的GB2312特征，可能需要尝试GB2312

        if (bUtf8Valid)
        {
            // UTF-8读取结果看起来有效，使用UTF-8
            strContent = strContentUtf8;
        }
        else
        {
            // UTF-8读取结果可疑，尝试GB2312（兼容旧文件）
            QTextCodec* gb2312Codec = QTextCodec::codecForName("GB2312");
            if (gb2312Codec)
            {
                file.seek(0);
                stream.setCodec(gb2312Codec);
                QString strContentGb2312 = stream.readAll();

                // 验证GB2312读取结果
                bool bGb2312Valid = true;
                if (strContentGb2312.contains(QChar(0xFFFD)))
                {
                    bGb2312Valid = false;
                }

                if (bGb2312Valid && !strContentGb2312.contains(QChar(0xFFFD)))
                {
                    // GB2312读取结果有效，使用GB2312
                    strContent = strContentGb2312;
                }
                else
                {
                    // GB2312也失败，回退到UTF-8（即使有替换字符）
                    strContent = strContentUtf8;
                }
            }
            else
            {
                // GB2312不可用，使用UTF-8（即使有替换字符）
                strContent = strContentUtf8;
            }
        }
#else
        // Linux 上直接使用UTF-8编码
        stream.setCodec("UTF-8");
        strContent = stream.readAll();
#endif
    }

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

bool CIniConfigReader::SetValue(const QString& strSection, const QString& strKey, const QString& strValue)
{
    if (strSection.isEmpty() || strKey.isEmpty())
        return false;

    // 如果是新节，记录到顺序列表中
    if (m_mapConfig.find(strSection) == m_mapConfig.end())
    {
        m_vecSectionOrder.push_back(strSection);
    }

    // 设置值
    m_mapConfig[strSection][strKey] = strValue;
    return true;
}

bool CIniConfigReader::SaveToFile(const QString& strFilePath)
{
    QString strSavePath = strFilePath;
    if (strSavePath.isEmpty())
    {
        strSavePath = m_strConfigPath;
    }

    if (strSavePath.isEmpty())
    {
        return false;
    }

    // 打开文件进行写入
    QFile file(strSavePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    // 写入 UTF-8 BOM (0xEF 0xBB 0xBF)
    //file.write("\xEF\xBB\xBF"); //似乎这个应用需要with bom才能正确识别tool.ini

    QTextStream stream(&file);
#ifdef Q_OS_WIN
    // Windows 上使用UTF-8编码（带BOM以兼容某些编辑器）
    stream.setCodec("UTF-8");
#else
    // Linux 上使用UTF-8编码
    stream.setCodec("UTF-8");
#endif

    // 按照节的顺序写入
    for (const QString& strSection : m_vecSectionOrder)
    {
        auto itSection = m_mapConfig.find(strSection);
        if (itSection == m_mapConfig.end())
            continue;

        // 写入节名
        stream << "[" << strSection << "]" << "\n";

        // 写入该节下的所有键值对
        for (const auto& pair : itSection->second)
        {
            QString strValue = pair.second;
            // 如果值包含空格或特殊字符，用引号括起来
            if (strValue.contains(' ') || strValue.contains('=') || strValue.contains(';'))
            {
                stream << pair.first << "=\"" << strValue << "\"\n";
            }
            else
            {
                stream << pair.first << "=" << strValue << "\n";
            }
        }

        // 节之间添加空行
        stream << "\n";
    }

    file.close();
    return true;
}
