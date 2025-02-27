#include "O_AsservLineRotateTest.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "../Common/Action/Sensors.hpp"
#include "../Common/Arguments.hpp"
#include "../Common/Asserv/EncoderControl.hpp"
#include "../Common/Asserv/MovingBase.hpp"
#include "../Common/Interface.Driver/AAsservDriver.hpp"
#include "../Common/IA/IAbyPath.hpp"
#include "../Common/Robot.hpp"
#include "../Common/Utils/Chronometer.hpp"
#include "../Log/Logger.hpp"
#include "OPOS6UL_ActionsExtended.hpp"
#include "OPOS6UL_AsservExtended.hpp"
#include "OPOS6UL_IAExtended.hpp"
#include "OPOS6UL_RobotExtended.hpp"

using namespace std;

void O_AsservLineRotateTest::configureConsoleArgs(int argc, char **argv) //surcharge
{
    OPOS6UL_RobotExtended &robot = OPOS6UL_RobotExtended::instance();
    //speed
    robot.getArgs().addArgument("s", "speed en %");
    //distance
    robot.getArgs().addArgument("d", "distance mm");
    //coord
    robot.getArgs().addArgument("x", "x destination mm", "0");
    robot.getArgs().addArgument("y", "y destination mm", "0");
    //angle
    robot.getArgs().addArgument("a", "angle degre", "0");

    robot.getArgs().addArgument("back", "back:0,1", "0");

    robot.getArgs().addArgument("nb", "nb of time", "1");

    Arguments::Option cOpt('+', "Coordinates x,y,a");
    cOpt.addArgument("coordx", "coord x mm", "300.0");
    cOpt.addArgument("coordy", "coord y mm", "1800.0");
    cOpt.addArgument("coorda", "coord teta deg", "0.0"); //TODO ATTENTION ERROR SI different de ZERO !!!
    robot.getArgs().addOption(cOpt);



    //reparse arguments
    robot.parseConsoleArgs(argc, argv);
}

void O_AsservLineRotateTest::run(int argc, char **argv)
{
    logger().info() << "N° " << this->position() << " - Executing - " << this->desc() << logs::end;
    configureConsoleArgs(argc, argv);

    int left;
    int right;
    int s = 0;
    float d = 0.0;
    float x = 0.0;
    float y = 0.0;
    int nb = 0.0;
    bool back = false;
    float a = 0.0;
    float coordx = 0.0;
    float coordy = 0.0;
    float coorda_deg = 0.0;

    OPOS6UL_RobotExtended &robot = OPOS6UL_RobotExtended::instance();

    Arguments args = robot.getArgs();
    if (args["s"] != "0") {
        s = atoi(args["s"].c_str());
        logger().debug() << "Arg s set " << args["s"] << ", s = " << s << logs::end;
    }
    if (args["d"] != "0") {
        d = atof(args["d"].c_str());
        logger().info() << "Arg d set " << args["d"] << ", d = " << d << logs::end;
    }

    if (args["x"] != "0") {
        x = atof(args["x"].c_str());
        logger().info() << "Arg x set " << args["x"] << ", x = " << x << logs::end;
    }

    if (args["y"] != "0") {
        y = atof(args["y"].c_str());
        logger().info() << "Arg y set " << args["y"] << ", y = " << y << logs::end;
    }

    if (args["a"] != "0") {
        a = atof(args["a"].c_str());
        logger().info() << "Arg a set " << args["a"] << ", a = " << a << logs::end;
    }

    if (args["nb"] != "0") {
        nb = atoi(args["nb"].c_str());
        logger().info() << "Arg nb set " << args["nb"] << ", nb = " << nb << logs::end;
    }
    coordx = atof(args['+']["coordx"].c_str());
    coordy = atof(args['+']["coordy"].c_str());
    coorda_deg = atof(args['+']["coorda"].c_str());

    if (args["back"] != "0") {
        back = atoi(args["back"].c_str());
        logger().debug() << "Arg back set " << args["back"] << ", back = " << back << logs::end;
    }

    logger().info() << "COORD avec x=" << coordx << " y=" << coordy << " coorda=" << coorda_deg << logs::end;

    robot.asserv().startMotionTimerAndOdo(false);
    robot.asserv().setPositionAndColor(coordx, coordy, coorda_deg, (bool)(robot.getMyColor() != PMXBLUE));
    robot.asserv().assistedHandling();

    robot.asserv().getEncodersCounts(&right, &left); //accumulated encoders
    ROBOTPOSITION p = robot.asserv().pos_getPosition();
    logger().info() << "time= " << robot.chrono().getElapsedTimeInMilliSec() << "ms ; " << " x=" << p.x << " y=" << p.y << " deg=" << p.theta * 180.0 / M_PI << logs::end;

    robot.svgPrintPosition();

    //detection adverse
    robot.actions().start();
    robot.actions().sensors().addTimerSensors(65);
    robot.chrono().start();

    robot.actions().sensors().setIgnoreFrontNearObstacle(true, false, true);
    robot.actions().sensors().setIgnoreBackNearObstacle(true, true, true);

    //vitesse reduite
    robot.asserv().setLowSpeedForward(true, s);
    robot.asserv().setLowSpeedBackward(true, s);

    //Definition du path
    bool byPathfinding = false;

    TRAJ_STATE ts = TRAJ_OK;

    for (int num = 1; num <= nb; num++) {
        ts = TRAJ_OK;
        if (d != 0) {
            logger().info() << "go...d=" << d << "mm" << logs::end;
            //calcul de coord
            float x_dest = robot.asserv().pos_getX_mm() + cos(robot.asserv().pos_getTheta()) * d;
            float y_dest = robot.asserv().pos_getY_mm() + sin(robot.asserv().pos_getTheta()) * d;
            //if (d >= 0) {
//                robot.actions().sensors().setIgnoreFrontNearObstacle(true, false, true);
//                robot.actions().sensors().setIgnoreBackNearObstacle(true, true, true);
                while (ts != TRAJ_FINISHED) {
                    ts = robot.ia().iAbyPath().whileMoveForwardTo(x_dest, y_dest, false, 100, 0, 0, false);
                    if (ts == TRAJ_NEAR_OBSTACLE) {
                        logger().info() << "===== TRAJ_NEAR_OBSTACLE CONFIRMED" << logs::end;
                        robot.asserv().resetEmergencyOnTraj("===== TRAJ_NEAR_OBSTACLE CONFIRMED");
                    }
                    if (ts == TRAJ_COLLISION) {
                        logger().info() << "===== COLLISION ASSERV CONFIRMED" << logs::end;
                        robot.asserv().resetEmergencyOnTraj("===== COLLISION ASSERV CONFIRMED");
                    }

                    //sleep(3);
                    robot.asserv().freeMotion();
                    break;
                }
//            } else if (d < 0) {
//                robot.actions().sensors().setIgnoreFrontNearObstacle(true, true, true);
//                robot.actions().sensors().setIgnoreBackNearObstacle(true, false, true);
//                while (ts != TRAJ_FINISHED) {
//                    ts = robot.ia().iAbyPath().whileMoveBackwardTo(robot.asserv().pos_getX_mm() + d,
//                            robot.asserv().pos_getY_mm(), false, 1000000, 10, 10, false);
//                    if (ts == TRAJ_NEAR_OBSTACLE) {
//                        logger().info() << "===== TRAJ_NEAR_OBSTACLE CONFIRMED" << logs::end;
//                        robot.asserv().resetEmergencyOnTraj("==== TRAJ_NEAR_OBSTACLE CONFIRMED");
//                    }
//                    if (ts == TRAJ_COLLISION) {
//                        logger().info() << "===== COLLISION ASSERV CONFIRMED" << logs::end;
//                        robot.asserv().resetEmergencyOnTraj("===== COLLISION ASSERV CONFIRMED");
//                    }
//                    //sleep(3);
//		                robot.asserv().freeMotion();
//                    break;
//                }
//            }
        }

        if (a != 0) {
            ts = TRAJ_OK;
            logger().info() << "go Rotate..." << a << " deg" << logs::end;
            while (ts != TRAJ_FINISHED) {
                ts = TRAJ_OK;
                ts = robot.ia().iAbyPath().whileMoveRotateTo(a, 1000000, 2);

                if (ts == TRAJ_NEAR_OBSTACLE) {
                    logger().error() << "===== TRAJ_NEAR_OBSTACLE FINAL" << logs::end;
                    robot.asserv().resetEmergencyOnTraj("rotate ===== TRAJ_NEAR_OBSTACLE FINAL");
                }
                if (ts == TRAJ_COLLISION) {
                    logger().error() << "===== COLLISION ASSERV FINAL" << logs::end;
                    robot.asserv().resetEmergencyOnTraj("rotate ===== COLLISION ASSERV FINAL");
                }

                //sleep(2);
                robot.asserv().freeMotion();
                break;
            }
            robot.svgPrintPosition();
        }

        if (!(x == 0 && y == 0)) {
            if (!back) {
                ts = TRAJ_OK;
                logger().info() << "go Forward... x=" << x << ", y=" << y << logs::end;
                robot.actions().sensors().setIgnoreFrontNearObstacle(true, false, true);
                robot.actions().sensors().setIgnoreBackNearObstacle(true, true, true);
                while (ts != TRAJ_FINISHED) {
                    ts = TRAJ_OK;
                    ts = robot.ia().iAbyPath().whileMoveForwardTo(x, y, false, 2000000, 3, 3);

                    if (ts == TRAJ_NEAR_OBSTACLE) {
                        logger().error() << "===== TRAJ_NEAR_OBSTACLE CONFIRMED" << logs::end;
                        robot.asserv().resetEmergencyOnTraj(
                                "robot.ia().iAbyPath().whileMoveForwardTo FINAL TRAJ_NEAR_OBSTACLE");
                    }
                    if (ts == TRAJ_COLLISION) {
                        logger().error() << "===== COLLISION ASSERV CONFIRMED" << logs::end;
                        robot.asserv().resetEmergencyOnTraj(
                                "robot.ia().iAbyPath().whileMoveForwardTo FINAL TRAJ_COLLISION");
                    }

                    //sleep(2);
                    robot.asserv().freeMotion();
                    break;
                }
                robot.svgPrintPosition();
            } else {
                ts = TRAJ_OK;
                logger().info() << "go Backward... x=" << x << ", y=" << y << logs::end;
                robot.actions().sensors().setIgnoreFrontNearObstacle(true, true, true);
                robot.actions().sensors().setIgnoreBackNearObstacle(true, false, true);
                while (ts != TRAJ_FINISHED) {
                    ts = TRAJ_OK;
                    ts = robot.ia().iAbyPath().whileMoveBackwardTo(x, y, false, 2000000, 3, 3);

                    //TODO TRAJ_NEAR_OBSTACLE or TRAJ_NEAR_OBSTACLE_REAR ???
                    if (ts == TRAJ_NEAR_OBSTACLE) {
                        logger().error() << "===== TRAJ_NEAR_OBSTACLE CONFIRMED" << logs::end;
                        robot.asserv().resetEmergencyOnTraj(
                                "robot.ia().iAbyPath().whileMoveForwardTo FINAL TRAJ_NEAR_OBSTACLE");
                    }
                    if (ts == TRAJ_COLLISION) {
                        logger().error() << "===== COLLISION ASSERV CONFIRMED" << logs::end;
                        robot.asserv().resetEmergencyOnTraj(
                                "robot.ia().iAbyPath().whileMoveForwardTo FINAL TRAJ_COLLISION");
                    }
                    //sleep(2);
                    robot.asserv().freeMotion();
                    break;
                }
                robot.svgPrintPosition();
            }
        }

        if (a != 0) {
            ts = TRAJ_OK;
            while (ts != TRAJ_FINISHED) {

                ts = robot.ia().iAbyPath().whileMoveRotateTo(a, 1000000, 2);

                if (ts == TRAJ_NEAR_OBSTACLE) {
                    logger().error() << "===== TRAJ_NEAR_OBSTACLE CONFIRMED" << logs::end;
                    robot.asserv().resetEmergencyOnTraj();
                }
                if (ts == TRAJ_COLLISION) {
                    logger().error() << "===== COLLISION ASSERV CONFIRMED" << logs::end;
                    robot.asserv().resetEmergencyOnTraj();
                }
                //sleep(3);
                robot.asserv().freeMotion();
                break;
            }
        }
        //pour la repetition
        d += d;
    }
    //sleep(1);
    robot.svgPrintPosition();
//    robot.asserv().doLineAbs(200);
//    robot.svgPrintPosition();

    robot.asserv().freeMotion();
//    robot.asserv().getEncodersCounts(&right, &left); //accumulated encoders
    p = robot.asserv().pos_getPosition();
    logger().info() << "time= " << robot.chrono().getElapsedTimeInMilliSec() << "ms ; " << " x=" << p.x << " y=" << p.y << " deg=" << p.theta * 180.0 / M_PI << logs::end;

    robot.svgPrintPosition();
    robot.svgPrintEndOfFile();
    logger().info() << "Happy End." << logs::end;
}

