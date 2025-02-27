#ifndef EV3_BUTTONDRIVER_HPP_
#define EV3_BUTTONDRIVER_HPP_

#include "../Common/Interface.Driver/AButtonDriver.hpp"
#include "../Log/LoggerFactory.hpp"

class ButtonDriver: public AButtonDriver
{
private:

    /*!
     * \brief Retourne le \ref Logger associé à la classe \ref ButtonDriver(EV3).
     */
    static inline const logs::Logger & logger()
    {
        static const logs::Logger & instance = logs::LoggerFactory::logger("ButtonDriver.EV3");
        return instance;
    }

public:

    /*!
     * \brief Constructor.
     */
    ButtonDriver();

    /*!
     * \brief Destructor.
     */
    ~ButtonDriver();

    bool pressed(ButtonTouch button);

    //bool process(ButtonTouch button);

};

#endif
