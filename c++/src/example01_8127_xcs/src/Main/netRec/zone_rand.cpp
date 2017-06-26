

#include <stdlib.h>
#include <time.h>

int zone_rand(int lower,int upper)
{
    static int seedReset = 0;
    
    if(!seedReset)
    {
        time_t tt = time(NULL);
        
        srand((unsigned long)tt);   
        seedReset = 1;
    }
    
    if((upper-lower)<=0)
        return upper;
        
    return (lower+rand()%(upper-lower));
}
