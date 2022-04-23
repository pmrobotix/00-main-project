#include "Utils.h"
#include "../odometrie/Odometrie.h"

//Limitation d'une valeur à un intervalle [valeurMin , valeurMax]
int64_t Utils::constrain(int64_t value, int64_t valeurMin, int64_t valeurMax)
{
    if (value < valeurMin)
        return valeurMin;

    if (value > valeurMax)
        return valeurMax;

    return value;
}

// Conversion mm vers UO
int64_t Utils::mmToUO(Odometrie *odo, int32_t valeur)
{
    int64_t r = ((valeur * (int)(odo->getFrontParMetre()) * (Config::uOParFront / 1000.0)) );
    //printf("\nmmToUO %d %d %d %ld\n",valeur, odo->getFrontParMetre(), Config::uOParFront, r);
    return r;
    //return (int64_t)((valeur * (odo->getFrontParMetre()) * Config::uOParFront * 1.0) / 1000.0);
}

// Conversion U0 vers mm
int32_t Utils::UOTomm(Odometrie *odo, int64_t valeur)
{
    int32_t mm = (((valeur * (1000.0 / Config::uOParFront)) / (odo->getFrontParMetre() * 1.0)));
    //printf("\nUOTomm %ld %d %d %d\n",valeur, odo->getFrontParMetre(), Config::uOParFront, mm);
    return mm;
    //return (int32_t) (((valeur * 1000.0) / (odo->getFrontParMetre() * Config::uOParFront * 1.0)));
}

// Conversion degres en UO
int64_t Utils::degToUO(Odometrie *odo, int32_t valeur)
{
    return (PI * valeur * odo->getDistanceRouesUO()) / 180.0;
}

// Conversion  UO en deg
float Utils::UOToDeg(Odometrie *odo, int64_t valeur)
{
    return (float) (valeur * 180.0 / (PI * odo->getDistanceRouesUO()));
}

// Conversion radians en UO
int64_t Utils::radToUO(Odometrie *odo, float valeur)
{
    return valeur * odo->getDistanceRouesUO();
}
