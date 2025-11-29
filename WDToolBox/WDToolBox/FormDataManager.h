#pragma once
#include <vector>
#include <map>
#include <afx.h>

// 表单数据结构
struct FormData
{
    CString strFormName;  // 表单名称
    std::vector<CString> vecHeaders;  // 表头（列名）
    std::vector<std::vector<CString>> vecRows;  // 数据行
};

class CFormDataManager
{
public:
    CFormDataManager();
    ~CFormDataManager();

    // 从文件加载多个表单
    bool LoadFromFile(const CString& strFilePath);
    
    // 获取所有表单名称
    void GetAllFormNames(std::vector<CString>& vecFormNames);
    
    // 根据表单名称获取表单数据
    bool GetFormData(const CString& strFormName, FormData& formData);
    
    // 清空所有数据
    void Clear();

private:
    std::map<CString, FormData> m_mapForms;  // 存储所有表单
    
    // 解析CSV行
    std::vector<CString> ParseCSVLine(const CString& strLine);
    
    // 判断是否为表单分隔符（例如：以 [表单名] 格式）
    bool IsFormSeparator(const CString& strLine, CString& strFormName);
};

