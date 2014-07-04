#include "DHT.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    uint8_t data [4];
    
    int8_t i;
    
    initDHT();
    
    if(fetchData(data))
    {
        for(i = data[2]; i >= 0; --i)
        {
            SET_BIT(PORTB,LED);
            _delay_ms(100);
            
            CLEAR_BIT(PORTB,LED);
            _delay_ms(500);
        }
    }

    return 0;
}

void initDHT(void)
{
    /* Set LED as output */
    SET_BIT(DDRB,LED);
    
    /* According to the DHT11's datasheet, the sensor needs about
       1-2 seconds to get ready when first getting power, so we
       wait
     */
    _delay_ms(2000);
}


uint8_t fetchData(uint8_t* arr)
{
    uint8_t data [5];
    uint8_t cnt, check;
    int8_t i,j;
    
    /******************* Sensor communication start *******************/
    
    /* Set data pin as output first */
    SET_BIT(DDRB,DHT_PIN);
    
    /* First we need milliseconds delay, so set clk/1024 prescaler */
    TCCR0B = 0x05;
    
    TCNT0 = 0;
    
    /* Clear bit for 20 ms */
    CLEAR_BIT(DHT_PORT_OUT,DHT_PIN);
    
    /* Wait about 20 ms */
    while(TCNT0 < 160);
    
    /* Now set Timer0 with clk/8 prescaling.
     Gives 1µs per cycle @8Mhz */
    TCCR0B = 0x02;
    
    TCNT0 = 0;
    
    /* Pull high again */
    SET_BIT(DHT_PORT_OUT,DHT_PIN);
    
    /* And set data pin as input */
    CLEAR_BIT(DDRB,DHT_PIN);
    
    /* Wait for response from sensor, 20-40µs according to datasheet */
    while(IS_SET(DHT_PORT_IN,DHT_PIN))
    { if (TCNT0 >= 60) return 0; }
    
    /************************* Sensor preamble *************************/
    
    TCNT0 = 0;
    
    /* Now wait for the first response to finish, low ~80µs */
    while(!IS_SET(DHT_PORT_IN,DHT_PIN))
    { if (TCNT0 >= 100) return 0; }
    
    TCNT0 = 0;
    
    /* Then wait for the second response to finish, high ~80µs */
    while(IS_SET(DHT_PORT_IN,DHT_PIN))
    { if (TCNT0 >= 100) return 0; }
    
    /********************* Data transmission start **********************/
    
    for (i = 0; i < 5; ++i)
    {
        for(j = 7; j >= 0; --j)
        {
            TCNT0 = 0;
            
            /* First there is always a 50µs low period */
            while(!IS_SET(DHT_PORT_IN,DHT_PIN))
            { if (TCNT0 >= 70) return 0; }
            
            TCNT0 = 0;
            
            /* Then the data signal is sent. 26 to 28µs (ideally)
             indicate a low bit, and around 70µs a high bit */
            while(IS_SET(DHT_PORT_IN,DHT_PIN))
            { if (TCNT0 >= 90) return 0; }
            
            /* Store the value now so that the whole checking doesn't
             move the TCNT0 forward by too much to make the data look
             bad */
            cnt = TCNT0;
            
            if (cnt >= 20 && cnt <= 35)
            { CLEAR_BIT(data[i],j); }
            
            else if (cnt >= 60 && cnt <= 80)
            { SET_BIT(data[i],j); }
            
            else return 0;
        }
    }
    
    /********************* Sensor communication end *********************/
    
    check = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    
    if (check != data[4]) return 0;
    
    for(i = 0; i < 4; ++i)
    { arr[i] = data[i]; }
    
    return 1;
}