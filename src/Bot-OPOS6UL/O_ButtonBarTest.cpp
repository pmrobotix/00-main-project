/*!
 * \file
 * \brief Implémentation de la classe O_ButtonBarTest.
 */

#include "O_ButtonBarTest.hpp"

#include <string>

#include "../Common/Action/ButtonBar.hpp"
#include "../Common/Interface.Driver/AButtonDriver.hpp"
#include "../Log/Logger.hpp"
#include "OPOS6UL_ActionsExtended.hpp"
#include "OPOS6UL_RobotExtended.hpp"

using namespace std;

void O_ButtonBarTest::run(int argc, char** argv)
{
    logger().info() << "N° " << this->position() << " - Executing - " << this->desc() << logs::end;

    OPOS6UL_RobotExtended &robot = OPOS6UL_RobotExtended::instance();

    logger().info() << "Please press UP" << logs::end;
    robot.actions().buttonBar().waitPressed(BUTTON_UP_KEY);

    logger().info() << "Please press DOWN" << logs::end;
    robot.actions().buttonBar().waitPressed(BUTTON_DOWN_KEY);

    logger().info() << "Please press LEFT" << logs::end;
    robot.actions().buttonBar().waitPressed(BUTTON_LEFT_KEY);

    logger().info() << "Please press RIGHT" << logs::end;
    robot.actions().buttonBar().waitPressed(BUTTON_RIGHT_KEY);

    logger().info() << "Please press BACK" << logs::end;
    robot.actions().buttonBar().waitPressed(BUTTON_BACK_KEY);

    //ICI -- Pas de bouton ENTER !!
    //logger().info() << "Please press ENTER" << logs::end;
    //robot.actions().buttonBar().waitPressed(BUTTON_ENTER_KEY);

    logger().info() << "Happy End." << logs::end;
}

