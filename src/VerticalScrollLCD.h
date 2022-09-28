#include<Arduino.h>
#include<LiquidCrystal.h>

class VerticalScrollLCD
{
    LiquidCrystal m_lcd;
    unsigned long m_delayMillis = 500;
    unsigned long m_lastDelay = millis();

public:
    VerticalScrollLCD(int rs, int en, int d4, int d5, int d6, int d7, int rows, int cols);
    void SetDelayMillis(int millis);
    void Write(String text);
    void Write(String text, int columnNumber, int lineNumber);
    void UpdateScroll();
};
