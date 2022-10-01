#include <AccelStepper.h>
#include <TMCStepper.h>
#include <SoftwareSerial.h>
class TMC2208_AccelStepper : public AccelStepper
{
    TMC2208Stepper *m_tmcDriver;
    SoftwareSerial* m_serial;
public:
    TMC2208_AccelStepper(AccelStepper::MotorInterfaceType interface, uint8_t stepPin, uint8_t dirPin, uint8_t enablePin, uint8_t tx, uint8_t rx, float rsense)
        :AccelStepper(interface,stepPin,dirPin)
        {
            m_serial = new SoftwareSerial(rx,tx);
            m_serial->begin(9600);
            m_tmcDriver = new TMC2208Stepper(m_serial, rsense);
            m_tmcDriver->begin();
            m_tmcDriver->pdn_disable(true);
            m_tmcDriver->I_scale_analog(false);
            setEnablePin(enablePin);
            disableOutputs();
        }
        

    void rms_Current(uint16_t mA) {
        m_tmcDriver->rms_current(mA);};
    const uint16_t rms_Current()const {
        return m_tmcDriver->rms_current();}
    void en_spreadcycle(bool B){
        m_tmcDriver->en_spreadCycle(B);}
    void pwm_autoscale(bool B){
        m_tmcDriver->pwm_autoscale(B);}
    void microsteps(uint16_t ms){
        m_tmcDriver->microsteps(ms);}
    const uint16_t microsteps(uint16_t ms)const{
        return m_tmcDriver->microsteps();}
}
