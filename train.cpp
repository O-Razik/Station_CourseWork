#include "train.h"

//--------------------------------------------------------

Train::Train() : train_id(0) {};

Train::Train(unsigned int id) : train_id(id) {};

Train::Train(unsigned int id,
             std::vector<std::pair<QString, QTime>> station_list,
             unsigned int distance) :
    train_id(id),
    stations(station_list),
    path_distance(distance)
{}

Train::~Train(){};

//--------------------------------------------------------

unsigned int Train::get_id() const
{
    return train_id;
}

unsigned int Train::get_distance() const{
    return path_distance;
}

double Train::get_speed() const {
    if (stations.size() < 2) {
        return 0.0; // Speed cannot be calculated with less than 2 stations.
    }

    double time_sum = 0.0;

    for (size_t i = 1; i < stations.size(); ++i) {
        const QTime time_this = stations[i - 1].second;
        const QTime time_next = stations[i].second;

        // Calculate the time difference between this station and the next one
        int timeInSeconds = time_this.secsTo(time_next);

        // Handle cases where the time crosses midnight
        if (timeInSeconds < 0) {
            timeInSeconds += 24 * 3600; // Add 24 hours to handle midnight crossings
        }

        time_sum += timeInSeconds;
    }

    if (time_sum == 0.0) {
        return 0.0; // Prevent division by zero.
    }

    double speed = get_distance() / (time_sum / 3600.0); // Speed in km/h
    return speed;
}


std::pair<QString, QTime> Train::get_begin() const{
    return stations.front();
}

std::pair<QString, QTime> Train::get_end() const{
    return stations.back();
}

std::vector<std::pair<QString, QTime>> Train::get_stations() const{
    return stations;
}

//--------------------------------------------------------

void Train::set_id(unsigned int id){
    train_id = id;
}

void Train::set_stations(std::vector<std::pair<QString, QTime>> station_list){
    if(!stations.empty()) stations.clear();
    stations = station_list;
}

void Train::set_distance(unsigned int distance){
    path_distance = distance;
}

bool Train::operator==(Train &two){
    bool is_same = 0;
    if (this->train_id == two.train_id && this->path_distance == two.path_distance){
        if(this->stations.size() == two.stations.size()) {
            for (int i = 0; i < this->stations.size(); ++i) {
                if(this->stations[i].first == two.stations[i].first && this->stations[i].second == two.stations[i].second){
                    is_same = 1;
                }
                else {
                    is_same = 0;
                    break;
                }
            }
        }
    }
    return is_same;
}

//--------------------------------------------------------

