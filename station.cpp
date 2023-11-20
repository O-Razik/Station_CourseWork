#include "station.h"
#include "add_func.h"
#include "errorwindow.h"

//-----------------------------------------------------------

Station::Station() : station_name("-") {};

Station::Station(std::string name) : station_name(name) {};

Station::Station(std::string name,
                 std::vector<std::pair<Train*, std::pair<QTime, QTime>>> _trains) :
    station_name(name), trains(_trains) {};

Station::Station(Station &other) :
    station_name(other.get_name()),
    trains(other.get_trains())
{}

Station::~Station(){
    for (int i = 0; i < trains.size(); ++i) {
        delete trains[i].first;
    }
    trains.clear();
};

//-----------------------------------------------------------

std::string Station::get_name() const
{
    return station_name;
}

std::vector<std::pair<Train *, std::pair<QTime, QTime> > > Station::get_trains() const
{
    return trains;
}

//-----------------------------------------------------------

void Station::set_name(std::string name){
    station_name = name;
}

void Station::set_trains( std::vector<std::pair<Train*, std::pair<QTime, QTime>>> train_list){
    trains = train_list;
}

void Station::add_train( std::pair<Train*, std::pair<QTime, QTime>> train){
    trains.push_back(train);
}

void Station::set_arrive_time(int index, QTime& time)
{
    trains[index].second.first = time;
}

//-----------------------------------------------------------

int Station::trains_number()
{
    return static_cast<int>(trains.size());
}

void Station::sort_dict_trains()
{
    trains = merge_sort_dist(trains, 0, trains.size()-1);
}

double Station::aver_train_speed() {
    double totalSpeed = 0.0;
    int numTrains = static_cast<int>(trains.size());

    for (int i = 0; i < numTrains; ++i) {
        totalSpeed += trains[i].first->get_speed();
    }

    if (numTrains > 0) {
        return totalSpeed / numTrains;
    }

    return 0.0;
}


void Station::sort_speed_trains()
{
    std::sort(trains.begin(), trains.end(),
        [](const auto& trainInfo1, const auto& trainInfo2) {
            double speed1 = (trainInfo1.first)->get_speed();
            double speed2 = (trainInfo2.first)->get_speed();
            return speed1 > speed2;
        }
    );
}


std::vector<std::pair<Train*, std::pair<QTime, QTime>>>
    Station::depart_trains_period(QTime begin_t, QTime end_t)
{
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> trains_period;
    for (int i = 0; i < trains.size(); ++i) {
        if(begin_t <= trains[i].second.second && trains[i].second.second <= end_t){
            std::pair<Train*, std::pair<QTime, QTime>> one_train;
            one_train.second = trains[i].second;
            one_train.first = new Train(*trains[i].first);
            trains_period.push_back(one_train);
        }
    }

    return trains_period;
}

std::vector<std::pair<Train*, std::pair<QTime, QTime>>>
    Station::arrive_trains_period(QTime begin_t, QTime end_t)
{
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> trains_period;
    for (int i = 0; i < trains.size(); ++i) {
        if(begin_t <= trains[i].second.first && trains[i].second.first <= end_t){
            std::pair<Train*, std::pair<QTime, QTime>> one_train;
            one_train.second = trains[i].second;
            one_train.first = new Train(*trains[i].first);
            trains_period.push_back(one_train);
        }
    }

    return trains_period;
}

std::vector<std::pair<Train*, std::pair<QTime, QTime>>>
Station::to_same_st_period(QString station_name)
{
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> trains_period;
    for (int i = 0; i < trains.size(); ++i) {
        if(station_name == trains[i].first->get_stations().back().first){
            std::pair<Train*, std::pair<QTime, QTime>> one_train;
            one_train.second = trains[i].second;
            one_train.first = new Train(*trains[i].first);
            trains_period.push_back(one_train);
        }
    }

    return trains_period;
}

std::vector<std::pair<Train*, std::pair<QTime, QTime>>>
Station::from_same_st_period(QString station_name)
{
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> trains_period;
    for (int i = 0; i < trains.size(); ++i) {
        if(station_name == trains[i].first->get_stations()[0].first){
            std::pair<Train*, std::pair<QTime, QTime>> one_train;
            one_train.second = trains[i].second;
            one_train.first = new Train(*trains[i].first);
            trains_period.push_back(one_train);
        }
    }

    return trains_period;
}

std::vector<std::pair<Train*, std::pair<QTime, QTime>>>
Station::stops_same_st_period(QString station_name)
{
    std::vector<std::pair<Train*, std::pair<QTime, QTime>>> trains_period;
    for (int i = 0; i < trains.size(); ++i) {
        auto t = trains[i].first->get_stations();
        for (int j = 0; j < t.size(); ++j) {
            if(station_name == t[j].first){
                std::pair<Train*, std::pair<QTime, QTime>> one_train;
                one_train.second = trains[i].second;
                one_train.first = new Train(*trains[i].first);
                trains_period.push_back(one_train);
                break;
            }
        }
    }

    return trains_period;
}

//-----------------------------------------------------------

std::ifstream& operator>>(std::ifstream& file, Station& st) {
    try
    {
        file.seekg(0, std::ios::beg);

        std::vector<QString> error_message;

        std::string name;
        if (std::getline(file, name)) {
            st.set_name(name);
        } else {
            error_message.push_back("Error №1: bad train name");
            error_message.push_back("Error reading the name from the file.");
            error_message.push_back("\"" + QString::fromStdString(name) + "\"");
            throw error_message;
        }

        if(name.empty() || std::islower(name[0]) || name.length() < 3){
            error_message.push_back("Error №1: bad train name");
            error_message.push_back("Error reading the name from the file.");
            error_message.push_back("\"" + QString::fromStdString(name) + "\"");
            throw error_message;
        }

        std::string str;
        while (!file.eof()) {
            std::getline(file, str);
            if(str == "") continue;
            std::string str_id, str_sts, str_path;
            std::istringstream line(str);
            std::getline(line, str_id, '|');
            std::getline(line, str_sts, '|');
            std::getline(line, str_path);
            int id, distance;

            try {
                id = std::stoi(str_id);
                if(id == 0) throw id;
            } catch (const std::invalid_argument& e) {
                error_message.clear();
                error_message.push_back("Error №2a: bad train ID");
                error_message.push_back(e.what());
                error_message.push_back("\"" + QString::fromStdString(str_id) + "\"");
                error_message.push_back("Train will be skipped");
                errorwindow* error = new errorwindow(error_message);
                error->show();
                error_message.clear();
                continue;
            } catch (int& id){
                error_message.clear();
                error_message.push_back("Error №2b: bad train ID");
                error_message.push_back("\"" + QString::fromStdString(str_id) + "\"");
                error_message.push_back("Train will be skipped");
                errorwindow* error = new errorwindow(error_message);
                error->show();
                error_message.clear();
                continue;
            }

            try {

                distance = std::stoi(str_path);
                if(distance == 0) throw distance;
                for (int i = 1; i < str_path.length(); ++i) {
                    if(!std::isdigit(str_path[i]))  throw distance;
                }
            } catch (const std::invalid_argument& e) {
                error_message.clear();
                error_message.push_back("Error №3a: bad train distanse");
                error_message.push_back(e.what());
                error_message.push_back("\"" + QString::fromStdString(str_path) + "\"");
                error_message.push_back("Train will be skipped");
                errorwindow* error = new errorwindow(error_message);
                error->show();
                error_message.clear();
                continue;
            }
            catch (int& distance){
                error_message.clear();
                error_message.push_back("Error №3b: bad train distanse");
                error_message.push_back("\"" + QString::fromStdString(str_path) + "\"");
                error_message.push_back("Train will be skipped");
                errorwindow* error = new errorwindow(error_message);
                error->show();
                error_message.clear();
                continue;
            }

            Train* train = new Train;
            train->set_id(id);
            train->set_distance(distance);

            std::istringstream sts_stream(str_sts);
            std::vector<std::pair<QString, QTime>> station_list;

            while (sts_stream) {
                std::string station_name;
                char bracket;
                std::string time_str;

                try{
                    sts_stream >> station_name;
                    if(station_name == "") break;
                    if(std::islower(station_name[0]) || station_name.length() < 3) throw station_name;
                }
                catch(std::string& station_name){
                    error_message.clear();
                    error_message.push_back("Error №4: Bad stop name");
                    error_message.push_back("\"" + QString::fromStdString(station_name) + "\"");
                    error_message.push_back("Stop will be skipped");
                    errorwindow* error = new errorwindow(error_message);
                    error->show();
                    error_message.clear();
                    sts_stream >> bracket >> time_str;
                    continue;
                }

                sts_stream >> bracket;
                try {
                    if (bracket != '[') {
                        error_message.clear();
                        error_message.push_back("Error №5a: Missing opening bracket '[' for time.");
                        error_message.push_back("\"" + QString::fromStdString(str_sts) + "\"");
                        throw error_message;
                    }
                    sts_stream >> time_str;
                    if (time_str[time_str.size()-1] != ']') {
                        error_message.clear();
                        error_message.push_back("Error №5b: Missing opening bracket ']' for time.");
                        error_message.push_back("\"" + QString::fromStdString(str_sts) + "\"");
                        throw error_message;
                    }
                } catch (std::vector<QString>& message) {
                    message.push_back("Stop will be skipped");
                    errorwindow* error = new errorwindow(message);
                    error->show();
                    message.clear();
                    continue;
                }
                time_str.pop_back();
                QTime time = QTime::fromString(QString::fromStdString(time_str), "hh:mm");
                station_list.push_back(std::make_pair(QString::fromStdString(station_name), time));
            }

            try {
                if(station_list.size() < 2){
                    error_message.clear();
                    error_message.push_back("Error №6: Train has less then 2 stops.");
                    error_message.push_back("\"" + QString::fromStdString(str_sts) + "\"");
                    throw error_message;
                }
            }
            catch (std::vector<QString>& message) {
                errorwindow* error = new errorwindow(message);
                error->show();
                message.clear();
                delete train;
                continue;
            }
            train->set_stations(station_list);

            st.add_train(
                std::make_pair(train, std::make_pair(station_list.front().second, station_list.back().second))
            );
        }
        if(st.get_trains().size() == 0){
            error_message.clear();
            error_message.push_back("Error №7: Station has 0 trains.");
            throw error_message;
        }
    }
    catch (std::vector<QString>& message) {
        message.push_back("Error №8: Station creation failed.");
        throw message;
    }


    for (int i = 0; i < st.trains_number(); ++i) {
        auto t = st.get_trains()[i].first;
        for (int j = 0; j < t->get_stations().size(); ++j) {
            if (t->get_stations()[j].first == QString::fromStdString(st.station_name)) {
                st.set_arrive_time(i,t->get_stations()[j].second);
                break;
            }
        }
    }


    return file;
}

std::ofstream& operator<<(std::ofstream &file, const Station &st) {
    file << st.get_name().c_str();

    for (const auto& trainInfo : st.get_trains()) {
        const Train* train = trainInfo.first;
        file  << "\n"<< train->get_id() << " | ";

        const std::vector<std::pair<QString, QTime>>& stations = train->get_stations();
        for (const auto& stationInfo : stations) {
            file << stationInfo.first.toStdString().c_str() <<
                " [" << stationInfo.second.toString("hh:mm").toStdString() << "] ";
        }

        file << "| " << train->get_distance();
    }

    return file;
}

//-----------------------------------------------------------
