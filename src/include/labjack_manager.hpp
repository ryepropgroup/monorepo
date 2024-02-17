#include <string>
#include <vector>

namespace mach {

class Valve;
class Sensor;

class LabJack {
    public:
        LabJack(std::string labjackName);
        ~LabJack();

        int handle;

        void addValve(Valve valve);

        void addSensor(Sensor sensor);

        void setHigh(std::string name);

        void setLow(std::string name);

    private:
        std::vector<Valve> valves;
        std::vector<Sensor> sensors;
};

class Valve {
    public:
        Valve(std::string name, std::string pin);

        void addToLabjack(LabJack labjack);

        bool getOpenState();

        void open();

        void close();
    
    private:
        LabJack* labjack;
        std::string name;
        std::string pin;
        bool state;
};

class Sensor {
    public:
        std::vector<std::string> settingNames;
        std::vector<double> settingValues;

        Sensor(std::string name, std::string pin, std::vector<std::string> settingNames, std::vector<double> settingValues);

        void addToLabjack(LabJack labjack);

        double read();
        
    private:
        LabJack* labjack;
        std::string name;
        std::string pin;
};

}