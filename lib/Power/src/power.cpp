#include "Power.h"

/* 
kjøkken: 
- stekeovn, topp og mikrobølgeovn
bad: 
-dusj, Gulvvarme
stue:
- Tv, ovn
soverom:
- ovn
ekstern:
+solcelle
*/

float powerContribution(float appliances[], int length)
{
    float powerSum = 0;
    for (int i = 0; i <= 7; i++){
        powerSum += appliances[i];
    }
}


