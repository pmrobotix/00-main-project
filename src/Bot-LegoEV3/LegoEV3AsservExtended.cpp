#include "LegoEV3AsservExtended.hpp"

#include <stdio.h>
#include <cmath>
#include <string>

#include "../Asserv.Esial/AsservEsialR.hpp"
#include "../Asserv.Esial/config/config.h"
#include "../Log/Logger.hpp"
#include "LegoEV3RobotExtended.hpp"

LegoEV3AsservExtended::LegoEV3AsservExtended(std::string botId, LegoEV3RobotExtended *robot) :
        Asserv(botId, robot) //on appelle le constructeur pere
{
    botId_ = botId;
    useAsservType_ = ASSERV_INT_ESIALR;
    pRobotExt_ = robot;

    //set the value setLowSpeedForward for asserv
    setLowSpeedvalue(32);

    B3_is_taken = 0;
    B4_is_taken = 0;
    A5_is_taken = 0;
    D5_is_taken = 0;
    taken_max = 3;
}

void LegoEV3AsservExtended::startMotionTimerAndOdo(bool assistedHandlingEnabled)
{

    //Real EV3 for ESIAL internal asserv config
    if (useAsservType_ == ASSERV_INT_ESIALR) {

        std::string filename = "config_";
        filename.append(botId_);
        filename.append(".txt");
        Config::loadFile(filename.c_str());
        logger().debug() << "Version configuration : " << Config::configVersion << logs::end;
        //logger().debug() << Config::dumpConfig() << logs::end;

        float periodSec = Config::asservPeriod;

        //on active le thread de check de position et les drivers //TODO a faire dans esialR et non dans asservdriver
        //asservdriver_->motion_ActivateManager(true);

        //start asserv thread et timer
        pAsservEsialR_->startAsserv(1.0f / periodSec); //f=20 Hz => every 50ms

        pAsservEsialR_->motion_ActivateManager(true); //init and start
        if (assistedHandlingEnabled) {
            pAsservEsialR_->motion_AssistedHandling();
        } else {
            pAsservEsialR_->motion_FreeMotion();
        }

    } else if (useAsservType_ == ASSERV_EXT) {

        asservdriver_->motion_ActivateManager(true); //on active la carte d'asserv externe et le thread de position
        if (assistedHandlingEnabled)
            asservdriver_->motion_AssistedHandling();
        else
            asservdriver_->motion_FreeMotion();
    }
}
bool LegoEV3AsservExtended::filtre_IsInsideTableXY(int x_botpos, int y_botpos)
{
    //table verticale
    int table_x = 2000;
    int table_y = 3000;

    //filtre de prise des elements de jeux par l'adversaire
    if (getRobot()->getMyColor() == PMXGREEN) {
        if (x_botpos > 600 && x_botpos < 850 && y_botpos < 1250 && y_botpos > 1000) {

//TODO afficher en SVG et faire un ensure sur plusieurs fois
            B3_is_taken++;
            if (B3_is_taken > taken_max)
                getRobot()->B3_is_taken = true;
        }

        if (x_botpos > 500 && x_botpos < 850 && y_botpos < 2000 && y_botpos > 1775) {
            B4_is_taken++;
            if (B4_is_taken > taken_max)
                getRobot()->B4_is_taken = true;
        }

        if (x_botpos > 0 && x_botpos < 400 && y_botpos < 2800 && y_botpos > 2100) {
            A5_is_taken++;
            if (A5_is_taken > taken_max)
                getRobot()->A5_is_taken = true;
        }

        if (x_botpos > 2000 - 400 && x_botpos < 2000 && y_botpos < 2800 && y_botpos > 2100) {
            D5_is_taken++;
            if (D5_is_taken > taken_max)
                getRobot()->D5_is_taken = true;
        }
    } else {
        if (x_botpos > (2000 - 850) && x_botpos < (2000 - 600) && y_botpos < 1250 && y_botpos > 1000) {
            B3_is_taken++;
            if (B3_is_taken > taken_max)
                getRobot()->B3_is_taken = true;
        }

        if (x_botpos > (2000 - 850) && x_botpos < (2000 - 500) && y_botpos < 2000 && y_botpos > 1775) {
            B4_is_taken++;
            if (B4_is_taken > taken_max)
                getRobot()->B4_is_taken = true;
        }

        if (x_botpos > (2000 - 450) && x_botpos < 2000 && y_botpos < 2800 && y_botpos > 2100) {
            A5_is_taken++;
            if (A5_is_taken > taken_max)
                getRobot()->A5_is_taken = true;
        }
        if (x_botpos > 0 && x_botpos < 400 && y_botpos < 2800 && y_botpos > 2100) {
            D5_is_taken++;
            if (D5_is_taken > taken_max)
                getRobot()->D5_is_taken = true;
        }

    }
    //getRobot()->B4_is_taken = true;

    //on filtre si c'est en dehors de la table verticale! avec 10cm de marge
    if ((x_botpos > 90 && x_botpos < table_x - 90) && (y_botpos > 90 && y_botpos < table_y - 90)) {
        logger().debug() << "INSIDE filtre_IsInsideTableXY xy_botpos=" << x_botpos << " " << y_botpos << logs::end;
        return true;
    } else
        return false;

}

/*
 bool LegoEV3AsservExtended::filtre_IsInsideTableXY(int d_mm, int x_mm, int y_mm, float theta_deg, int *x_botpos,
 int *y_botpos)
 {
 //return true; //PATCH

 //table verticale
 int table_x = 2000;
 int table_y = 3000;
 RobotPosition p = pos_getPosition();

 //coordonnées de l'objet detecté sur la table// M_P/2
 //    *x_botpos = p.x + (d_mm * cos(p.theta - M_PI_2 + (theta_deg * M_PI / 180.0f)));
 //    *y_botpos = p.y + (d_mm * sin(p.theta - M_PI_2 + (theta_deg * M_PI / 180.0f)));
 //float a = (p.theta - M_PI_2 + (theta_deg * M_PI / 180.0f));
 float a = (p.theta + (theta_deg * M_PI / 180.0f));

 a= std::fmod(a, 2 * M_PI);
 if (a < -M_PI)
 a += 2*M_PI;
 if (a > M_PI)
 a -= 2*M_PI;

 //ADV coord repere table
 *x_botpos = p.x + (d_mm * cos(a));
 *y_botpos = p.y + (d_mm * sin(a));

 logger().error() << __FUNCTION__ << "DEBUG --xy_botpos= " << *x_botpos << " " << *y_botpos
 << " pos: " << p.x << " " << p.y << " p_rad:" << p.theta << " --balise: " << d_mm << " " << x_mm << " "
 << y_mm << " t_deg:" << theta_deg << logs::end;

 //filtre de prise des elements de jeux par l'adversaire
 if (getRobot()->getMyColor() == PMXGREEN) {
 if (*x_botpos > 650 && *x_botpos < 850 && *y_botpos < 1250 && *y_botpos > 1000) {

 getRobot()->B3_is_taken = true;
 }

 if (*x_botpos > 650 && *x_botpos < 850 && *y_botpos < 2000 && *y_botpos > 1775) {

 getRobot()->B4_is_taken = true;
 }

 if (*x_botpos > 0 && *x_botpos < 650 && *y_botpos < 2800 && *y_botpos > 2100) {

 getRobot()->A5_is_taken = true;
 }

 if (*x_botpos > 2000 - 650 && *x_botpos < 2000 && *y_botpos < 2800 && *y_botpos > 2100) {

 getRobot()->D5_is_taken = true;
 }
 } else {
 if (*x_botpos > (2000 - 850) && *x_botpos < (2000 - 650) && *y_botpos < 1250 && *y_botpos > 1000) {

 getRobot()->B3_is_taken = true;
 }

 if (*x_botpos > (2000 - 850) && *x_botpos < (2000 - 650) && *y_botpos < 2000 && *y_botpos > 1775) {

 getRobot()->B4_is_taken = true;
 }

 if (*x_botpos > (2000 - 650) && *x_botpos < 2000 && *y_botpos < 2800 && *y_botpos > 2100) {

 getRobot()->A5_is_taken = true;
 }
 if (*x_botpos > 0 && *x_botpos < 650 && *y_botpos < 2800 && *y_botpos > 2100) {

 getRobot()->D5_is_taken = true;
 }

 }
 //getRobot()->B4_is_taken = true;

 //on filtre si c'est en dehors de la table verticale! avec 10cm de marge
 if ((*x_botpos > 100 && *x_botpos < table_x - 100) && (*y_botpos > 100 && *y_botpos < table_y - 100)) {
 logger().debug() << "INSIDE filtre_IsInsideTableXY xy_botpos=" << *x_botpos << " " << *y_botpos << "pos: "
 << p.x << " " << p.y << " p_rad:" << p.theta << " balise: " << d_mm << " " << x_mm << " " << y_mm
 << " t_deg:" << theta_deg << logs::end;
 return true;
 } else
 return false;

 }
 */
//TODO fonction pour les capteurs de proximité ...a rendre generique pour tous les robots
//dist_detect_mm : distance detecté de l'objet
//lateral_pos_sensor_mm : position du capteur : gauche -1 ; droite +1
bool LegoEV3AsservExtended::filtre_IsInsideTable(int dist_detect_mm, int lateral_pos_sensor_mm, std::string desc)
{

    float dist_sensor_from_axis_robot_mm = 110.0;
    //logger().error() << "==== filtreInsideTable" << logs::end;
    float distmm = dist_detect_mm;
    //On filtre si c'est pas à l'exterieur du terrain
    float x = 0.0;
    float y = 0.0;
    bool result = false;
    ROBOTPOSITION p = pos_getPosition();
    x = p.x + ((lateral_pos_sensor_mm * dist_sensor_from_axis_robot_mm) * cos(p.theta - M_PI_2))
            + (distmm * cos(p.theta));
    y = p.y + ((lateral_pos_sensor_mm * dist_sensor_from_axis_robot_mm) * sin(p.theta - M_PI_2))
            + (distmm * sin(p.theta));

    //TODO utiliser les zones de l'ia ???

    //filtre table Horizontale
//    if ((x > 150 && x < 2850) && (y > 150 && y < 1850)) //en mm
//        result = true;
//    else
//        result = false;

    //filtre table verticale
    if ((x > 150 && x < 1850) && (y > 150 && y < 2850)) //en mm
        result = true;
    else
        result = false;

//    //2022 filtre triangle yellow //todo violet !!!!!!!!!!!!!!!!!!!!!!!!
//    if(y <= (700 - (x)))
//        result = false;
//
//    if(y <= (-2300 + (x)))
//        result = false;

//    logger().debug() << desc << " filtreInsideTable : dist=" << dist_detect_mm
//            << " capteur:" << lateral_pos_sensor_mm
//            << " p.x=" << p.x << " p.y=" << p.y << " p.T=" << p.theta << " x=" << x
//            << " y=" << y << " result = " << result << logs::end;

    if (result) {
        logger().debug() << desc << " filtreInsideTable : dist=" << dist_detect_mm << " capteur:"
                << lateral_pos_sensor_mm << " p.x=" << p.x << " p.y=" << p.y << " p.T=" << p.theta << " x=" << x
                << " y=" << y << " result = " << result << logs::end;
        return true; //si ok
    } else
        return false; //si en dehors de la table*/

    /*
     //logger().error() << "==== filtreInsideTable" << logs::end;
     float distmetre = dist_detect_mm / 1000.0;
     //On filtre si c'est pas à l'exterieur du terrain
     float x = 0.0;
     float y = 0.0;
     bool result = false;
     RobotPosition p = pos_getPosition();
     x = p.x + ((lateral_pos_sensor_mm * 100.0 / 1000.0) * cos(p.theta - M_PI_2)) + (distmetre * cos(p.theta));
     y = p.y + ((lateral_pos_sensor_mm * 100.0 / 1000.0) * sin(p.theta - M_PI_2)) + (distmetre * sin(p.theta));
     if ((x > 0.150 && x < 2.850) && (y > 0.150 && y < 1.500)) //en mètre
     result = true;
     else
     result = false;
     logger().error() << "filtreInsideTable" << " p.x=" << p.x << " p.y=" << p.y << " p.T=" << p.theta << " x=" << x
     << " y=" << y << " result = " << result << logs::end;

     if (result) {
     return true; //si ok
     } else
     return false; //si en dehors de la table*/
}

//void LegoEV3AsservExtended::setLowSpeedForward(bool enable, int percent)
//{
//    //logger().error() << "LegoEV3AsservExtended::setLowSpeedForward 40!" << logs::end;
//    pAsservEsialR_->motion_setLowSpeedForward(enable, percent);
//
//}
//void LegoEV3AsservExtended::setLowSpeedBackward(bool enable, int percent)
//{
//    //logger().error() << "LegoEV3AsservExtended::setLowSpeedBackward 40!" << logs::end;
//    pAsservEsialR_->motion_setLowSpeedBackward(enable, percent);
//}
