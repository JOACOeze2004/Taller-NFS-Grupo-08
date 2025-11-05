#include "yaml_parser.h"
#include <QRegularExpression>
#include <QDebug>

QMap<QString, QString> YamlParser::parseMetadata(const QStringList& lines, int& currentLine) {
    QMap<QString, QString> metadata;
    QRegularExpression keyValuePattern("^(\\w+):\\s*\"?([^\"]*)\"?$");

    while (currentLine < lines.size()) {
        QString trimmed = lines[currentLine].trimmed();
        if (trimmed.endsWith(':') && !trimmed.contains('"')) {
            break;
        }
        if (trimmed.isEmpty() || trimmed.startsWith('#')) {
            currentLine++;
            continue;
        }
        QRegularExpressionMatch match = keyValuePattern.match(trimmed);
        if (match.hasMatch()) {
            QString key = match.captured(1);
            QString value = match.captured(2);
            metadata[key] = value;
        }
        currentLine++;
    }
    return metadata;
}

QList<YamlParser::YamlItem> YamlParser::parseList(const QStringList& lines, int& currentLine) {
    QList<YamlItem> items;
    YamlItem currentItem;
    bool hasCurrentItem = false;

    QRegularExpression listItemPattern("^\\s*-\\s+(\\w+):\\s*([\\d.\\-]+)");
    QRegularExpression propertyPattern("^\\s+(\\w+):\\s*([\\d.\\-]+)");

    while (currentLine < lines.size()) {
        QString line = lines[currentLine];
        QString trimmed = line.trimmed();

        if (trimmed.endsWith(':') && !line.startsWith(' ')) {
            break;
        }
        if (trimmed.isEmpty() || trimmed.startsWith('#') || trimmed == "[]") {
            currentLine++;
            continue;
        }
        QRegularExpressionMatch itemMatch = listItemPattern.match(line);
        if (itemMatch.hasMatch()) {
            if (hasCurrentItem) {
                items.append(currentItem);
            }
            currentItem = YamlItem();
            QString key = itemMatch.captured(1);
            double value = itemMatch.captured(2).toDouble();
            currentItem.properties[key] = value;
            hasCurrentItem = true;
            
            currentLine++;
            continue;
        }
        QRegularExpressionMatch propMatch = propertyPattern.match(line);
        if (propMatch.hasMatch() && hasCurrentItem) {
            QString key = propMatch.captured(1);
            double value = propMatch.captured(2).toDouble();
            currentItem.properties[key] = value;
            
            currentLine++;
            continue;
        }

        currentLine++;
    }
    if (hasCurrentItem) {
        items.append(currentItem);
    }
    return items;
}