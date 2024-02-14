/*


*/

#ifndef __SATELLITE__
#define __SATELLITE__

#include <Arduino.h>

class Satellite
{
private:
    uint8_t m_idNode;
    uint8_t m_ip[4];

public:
    Satellite();
    void begin();
    uint8_t id();
    void id(uint8_t);
    static void watchDog(void *);
};

#endif
