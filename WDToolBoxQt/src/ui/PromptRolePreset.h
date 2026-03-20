#pragma once

#include <QString>

// Default field bundle for a prompt "role" (left list item).
struct PromptRolePreset
{
    QString displayName;
    QString role;
    QString context;
    QString task; // usually empty; user fills per run
    QString constraints;
    QString outputFormat;
    QString acceptanceCriteria;
};
