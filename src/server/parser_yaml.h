#ifndef TALLER_TP_PARSER_YAML_H
#define TALLER_TP_PARSER_YAML_H
#include <string>
#include <vector>

struct StaticBody {
    int id;
    float x;
    float y;
    float width;
    float height;
};

class ParserYaml {
public:
    std::vector<StaticBody> parse_map(std::string& map_name);
};


#endif  // TALLER_TP_PARSER_YAML_H
