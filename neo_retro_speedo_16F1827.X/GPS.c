/* 
 * File:   GPS.c
 * Author: nio101
 *
 * Created on September 20, 2016, 6:04 AM
 */


#include "GPS.h"

extern t_fp    speed;

void GPS_Initialize(void)
{
    __delay_sec(1); // needed for letting the GPS start
    // filter the NMEA sentences => only the VTG (speed)@every fix
    printf("$PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    __delay_ms(100);
    //printf("$PMTK220,1000*1F\r\n"); // 1 update per sec
    printf("$PMTK220,100*2F\r\n"); // ask for a fix every 100ms (update @10hz)
    __delay_ms(100);
}

bool GPS_read_speed(void) // read & parse/check one NMEA sentence
{   
    // example of VTG sentence:
    // $GPVTG,165.48,T,,M,150.03,N,300.06,K,A*37
    unsigned char buffer[NMEA_BUFFER_SIZE];
    unsigned char i_buff = 0;
    unsigned char checksum = 0x00;
    // we are going to wait for a '$' to begin recording received chars
    unsigned char m_char = 0x00;
    while (m_char != '$')
        if(eusartRxCount!=0)
            m_char=EUSART_Read();  // read a byte from RX
    
    // were are at the beginning of a new NMEA sentence
    // let's get the NMEA header and check that it's a VTG sentence
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    // buffer should contain 'GPVTG'...
    if ((buffer[0] != 'G')||(buffer[1] != 'P')||(buffer[2] != 'V')||(buffer[3] != 'T')||(buffer[4] != 'G'))
        return false;
    i_buff = 0; // clear buffer
    
    // let's wait and then skip the T field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
        }
    // should be the T keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'T')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer

    // let's wait and then skip the M field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
        }
    // should be the M keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'M')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer
    
    // let's wait and then skip the N field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
        }
    // should be the N keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'N')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer
    
    // let's wait and then read the K field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    // buffer should contain the current speed reading in km/h
    // read it now and convert it to speed/FP
    unsigned char i_buff2 = 0;
    unsigned short read_speed_int=0;
    unsigned short read_speed_fra=0;
    bool before_dot = true;
    while (i_buff2 < i_buff)
        if (buffer[i_buff2]=='.')
        {
            before_dot = false;
            i_buff2++;  // step over the '.'
        }
        else
        {
            if ((buffer[i_buff2]<0x30)||(buffer[i_buff2]>0x39))
                return false;   // not a digit, abort!
            else
            {
                if (before_dot)
                    read_speed_int = read_speed_int*10 + (buffer[i_buff2]-0x30);
                else
                    read_speed_fra = read_speed_fra*10 + (buffer[i_buff2]-0x30);
                i_buff2++;
            }
        }
    // this type of GPS always outputs 2 digits avec dot
    // so multiply the fractional part by 100
    // to comply with the FP definition/conversion
    read_speed_fra *= 100;
    // read_speed_int/fra now contain the read speed
    // if checksum is ok, overwrite speed later
    i_buff = 0; // clear buffer
    
    // should be the K keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'K')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer
    
    // we should now arrive to the last field + checksum value
    bool no_more_checksum = false;
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            if (m_char == '*')
                no_more_checksum = true;
            if (!no_more_checksum)
                checksum ^= m_char;
            if (m_char == 0x0D)
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[1] != '*')||(i_buff != 4))
        return false;

    // now check the checksum against ours
    unsigned char checksum2 = 0;
    if (buffer[2] > 0x39)   // not 0-9, more A-F
        checksum2 = buffer[2] - 0x37;
    else
        checksum2 = buffer[2] - 0x30;
    checksum2 = checksum2 << 4;
    if (buffer[3] > 0x39)   // not 0-9, more A-F
        checksum2 += buffer[3] - 0x37;
    else
        checksum2 += buffer[3] - 0x30;
    if (checksum == checksum2)
    {
        speed = convert_to_fp(read_speed_int, read_speed_fra);
        return true;
    }
    else
        return false;
}

