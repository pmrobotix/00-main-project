#ifndef ASSERV_E_FILTRE
#define ASSERV_E_FILTRE


#include "../config/config.h"

// "interface" représentant les filtres

class Filtre
{
public:
    int64_t filtre(int64_t value1 , int64_t value2 = 0 , int64_t value3 = 0);
};

#endif
