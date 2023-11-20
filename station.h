#ifndef STATION_H
#define STATION_H

#include "classes.h"

class Station
{
public:

    // Constuctors
    Station();
    Station(std::string name);
    Station
        (std::string name, std::vector<std::pair<Train*, std::pair<QTime, QTime>>> trains);
    Station(Station& other);

    ~Station();

    // Getters
    std::string get_name() const;
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> get_trains() const;

    // Setters
    void set_name(std::string name);
    void set_trains( std::vector<std::pair<Train*, std::pair<QTime, QTime>>> train_list);
    void add_train( std::pair<Train*, std::pair<QTime, QTime>> train);
    void set_arrive_time(int index, QTime &time);

    void sort_dict_trains(); // 1) +
    int trains_number(); // 2) +
    double aver_train_speed(); // 3a) +
    void sort_speed_trains(); // 3b) +

    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> depart_trains_period
        (QTime begin_t, QTime end_t); // 4) +
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> arrive_trains_period
        (QTime begin_t, QTime end_t); // 5) +

    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> to_same_st_period
        (QString station_name); // 6a) +
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> from_same_st_period
        (QString station_name); // 6b) +
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> stops_same_st_period
        (QString station_name); // 6c) +

    friend std::ifstream& operator>>(std::ifstream& file, Station& st); // +
    friend std::ofstream& operator<<(std::ofstream& file, const Station& st); // +

private:

    std::string station_name;
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> trains;
};


#endif // STATION_H
