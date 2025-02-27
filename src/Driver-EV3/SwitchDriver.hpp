#ifndef EV3_SWITCHDRIVER_HPP_
#define EV3_SWITCHDRIVER_HPP_

#include "../Common/Interface.Driver/ASwitchDriver.hpp"
#include "../Log/LoggerFactory.hpp"
#include "LegoTouch.hpp"


using namespace std;
using namespace ev3dev;


class SwitchDriver: public ASwitchDriver, utils::Mutex {
private:

    /*!
     * \brief Retourne le \ref Logger associé à la classe \ref SwitchDriver(EV3).
     */
    static inline const logs::Logger & logger() {
        static const logs::Logger & instance = logs::LoggerFactory::logger("SwitchDriver.EV3");
        return instance;
    }

    //touch_sensor touch_;

    LegoTouch legotouchposix_;

public:

    /*!
     * \brief Constructor.
     */
    SwitchDriver();

    /*!
     * \brief Destructor.
     */
    ~SwitchDriver();

    bool is_connected();

    int tirettePressed();
    int backLeftPressed();
    int backRightPressed();

    void setGPIO(int gpio, bool activate);

    //int pressed(unsigned char pin);

};

#endif
