#ifndef ARM_SERVODRIVER_HPP_
#define ARM_SERVODRIVER_HPP_

#include "../Common/Action.Driver/AServoDriver.hpp"
#include "../Log/LoggerFactory.hpp"
#include "ServoMotorStd.hpp"

using namespace std;

class ServoDriver: public AServoDriver
{
private:

    /*!
     * \brief Retourne le \ref Logger associé à la classe \ref ServoDriver(ARM).
     */
    static inline const logs::Logger & logger()
    {
        static const logs::Logger & instance = logs::LoggerFactory::logger("ServoDriver.ARM");
        return instance;
    }

    int connected_;

    ServoMotorStd left_;
    ServoMotorStd right_;
    ServoMotorStd centre_;

protected:

public:
    /*!
     * \brief Constructor.
     */
    ServoDriver();

    /*!
     * \brief Destructor.
     */
    inline ~ServoDriver()
    {
    }

    void hold(int servo);

    //percentage pour EV3 , 0->4096 pour APF
    void setPosition(int servo, int percent);

    void release(int servo);

    void setRate(int servo, int millisec);

    void turn(int servo, int speed);

    int getMoving(int servo);

    int getPos(int servo);

    int ping(int);

    void setMinPulse(int servo, int value = 600); //default 600 [300 to 700]

    void setMidPulse(int servo, int value = 1500); //default 1500 [1300 to 1700]

    void setMaxPulse(int servo, int value = 2400); //default 2400 [2300 to 2700]

    void setPolarity(int servo, bool inversed);
};

#endif
