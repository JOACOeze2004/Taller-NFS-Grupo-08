#ifndef TALLER_TP_YAML_PARSER_H
#define TALLER_TP_YAML_PARSER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>

class YamlParser {
public:
    struct YamlItem {
        QMap<QString, double> properties;
    };
    static QMap<QString, QString> parseMetadata(const QStringList& lines, int& currentLine);
    static QList<YamlItem> parseList(const QStringList& lines, int& currentLine);
};

#endif // TALLER_TP_YAML_PARSER_H