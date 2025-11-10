#ifndef TALLER_NFS_GRUPO_08_RACE_H
#define TALLER_NFS_GRUPO_08_RACE_H
#include <map>
#include <string>

#include "car.h"
#include "parser_yaml.h"


class Race {
    ParserYaml parser;
    Track track;
    std::map<int, Car> *cars;
    std::map<int, int> car_progress;
    std::map<int, bool> finished;
public:
    explicit Race(const std::string& track_name, std::map<int, Car> *_cars);
	bool car_finished(const int& id);
	bool car_dead(const int& id);
	void update_checkpoints();
};


#endif  // TALLER_NFS_GRUPO_08_RACE_H
