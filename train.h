#ifndef TRAIN_H
#define TRAIN_H

#include "classes.h"

class Train
{
public:
    Train();
    Train(unsigned int id);
    Train(unsigned int id,
          std::vector<std::pair<QString, QTime>> station_list,
          unsigned int distance);

    ~Train();

    unsigned int get_id() const;
    std::pair<QString, QTime> get_begin() const;
    std::pair<QString, QTime> get_end() const;
    std::vector<std::pair<QString, QTime>> get_stations() const;
    unsigned int get_distance() const;
    double get_speed() const;

    void set_id(unsigned int id);
    void set_stations(std::vector<std::pair<QString, QTime>> station_list);
    void set_distance(unsigned int distance);

    bool operator==(Train& two);

private:
    unsigned int train_id;
    std::vector<std::pair<QString, QTime>> stations;
    unsigned int path_distance;

};

#endif // TRAIN_H
