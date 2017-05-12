#include <stdint.h>
#include <stm32f4xx.h>
#include <carme.h>
#include <carme_io1.h>

int main(void)
{
        uint8_t switchezzz;
        CARME_IO1_Init();
        for(;;)
        {
                CARME_IO1_SWITCH_Get(&switchezzz);
                CARME_IO1_LED_Set(switchezzz,0xFF);
        }
        return 0;
}
