#include "VerticalScrollLCD.h"



VerticalScrollLCD::VerticalScrollLCD(int rs, int en, int d4, int d5, int d6, int d7, int cols, int rows)
{
    m_lcd = LiquidCrystal(rs, en, d4,d5,d6,d7);
    m_lcd.begin(cols,rows)
}