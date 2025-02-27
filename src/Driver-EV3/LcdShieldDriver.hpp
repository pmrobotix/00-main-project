#ifndef EV3_LCDSHIELDDRIVER_HPP_
#define EV3_LCDSHIELDDRIVER_HPP_

#include <stddef.h>
#include <cstdint>
#include <string>

#include "../Common/Interface.Driver/ALcdShieldDriver.hpp"
#include "../Log/LoggerFactory.hpp"

using namespace std;

class LcdShieldDriver: public ALcdShieldDriver
{
private:

    /*!
     * \brief Retourne le \ref Logger associé à la classe \ref LcdShieldDriver(EV3).
     */
    static inline const logs::Logger & logger()
    {
        static const logs::Logger & instance = logs::LoggerFactory::logger("LcdShieldDriver.EV3");
        return instance;
    }

public:

    /*!
     * \brief Constructor.
     */
    LcdShieldDriver();

    /*!
     * \brief Destructor.
     */
    ~LcdShieldDriver();

    bool is_connected();

    void clear();

    void home();

    void setBacklightOn();

    void setBacklightOff();

    void setCursor(uint8_t, uint8_t);

    size_t write(uint8_t value);

    void print_content_string(std::string str, int row, int col);

    void print_content_integer(int value, int row, int col);

};

#endif
