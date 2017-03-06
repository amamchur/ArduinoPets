
template <int DDRx, int PORTx, int PINx>
class AVRPort {
  public:
    enum : int {
      DDR = DDRx,
      PORT = PORTx,
      PIN = PINx
    };

    uint8_t data;
    uint8_t usage;

    AVRPort() : data(0), usage(0) {
    }
};

class Noop {};
Noop noop;

template <class AvrPort, uint8_t b>
class DigitalPin {
  public:
    typedef AvrPort Port;
    uint8_t value;
    //uint8_t *ptr;

    DigitalPin(uint8_t v) : value(v) {}
    ~DigitalPin() { }

    static inline __attribute__((always_inline)) void mode(uint8_t md) {
      switch (md) {
        case INPUT:
          *(volatile uint8_t *)Port::DDR &= ~(1 << b);
          *(volatile uint8_t *)Port::PORT &= ~(1 << b);
          break;
        case OUTPUT:
          *(volatile uint8_t *)Port::DDR |= 1 << b;
          break;
        case INPUT_PULLUP:
          *(volatile uint8_t *)Port::DDR &= ~(1 << b);
          *(volatile uint8_t *)Port::PORT |= 1 << b;
          break;
      }
    }

    static inline __attribute__((always_inline)) uint8_t read() {
      return (*(volatile uint8_t *)Port::PIN & (1 << b)) >> b;
    }

    static inline __attribute__((always_inline)) void write(uint8_t value) {
      if (value) {
        *(volatile uint8_t *)Port::PORT |= (1 << b);
      } else {
        *(volatile uint8_t *)Port::PORT &= ~(1 << b);
      }
    }

    static inline __attribute__((always_inline)) void low() {
      *(volatile uint8_t *)Port::PORT &= ~(1 << b);
    }

    static inline __attribute__((always_inline)) void high() {
      *(volatile uint8_t *)Port::PORT |= (1 << b);
    }
};

template <class AvrPort, uint8_t b>
class AnalogPin : public DigitalPin<AvrPort, b> {
  public:
    static inline __attribute__((always_inline)) uint16_t read(uint8_t ref = DEFAULT) {
      ADMUX = (ref << 6) | b;
      while (ADCSRA & (1 << ADSC));     // Thanks T, this line waits for the ADC to finish
      uint8_t low  = ADCL;
      uint8_t high = ADCH;
      return (high << 8) + low;
    }
};

template <class DataPin, class ClockPin, uint8_t bitOrder> void avrShitfOut(uint8_t value) {
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t v;
    if (bitOrder) {
      v = value & (1 << i);
    } else {
      v= value & (1 << (7 - i));
    }
    DataPin::write(!!v);
    ClockPin::high();
    ClockPin::low();
  }
}

typedef AVRPort<(int)&DDRC, (int)&PORTC, (int)&PINC> PortC;
typedef AVRPort<(int)&DDRB, (int)&PORTB, (int)&PINB> PortB;
typedef AVRPort<(int)&DDRD, (int)&PORTD, (int)&PIND> PortD;

typedef DigitalPin<PortD, 0> D00;
typedef DigitalPin<PortD, 1> D01;
typedef DigitalPin<PortD, 2> D02;
typedef DigitalPin<PortD, 3> D03;
typedef DigitalPin<PortD, 4> D04;
typedef DigitalPin<PortD, 5> D05;
typedef DigitalPin<PortD, 6> D06;
typedef DigitalPin<PortD, 7> D07;
typedef AnalogPin<PortC, 0> A00;
typedef AnalogPin<PortC, 1> A01;
typedef AnalogPin<PortC, 2> A02;
typedef AnalogPin<PortC, 3> A03;
typedef AnalogPin<PortC, 4> A04;
typedef AnalogPin<PortC, 5> A05;
typedef AnalogPin<PortC, 6> A06;
typedef AnalogPin<PortC, 7> A07;

//typedef ADCPin<PortC, 8> ATS;

typedef DigitalPin<PortB, 0> D08;
typedef DigitalPin<PortB, 1> D09;
typedef DigitalPin<PortB, 2> D10;
typedef DigitalPin<PortB, 3> D11;
typedef DigitalPin<PortB, 4> D12;
typedef DigitalPin<PortB, 5> D13;

class BufferedOutput {
  private:
    int noops;
    PortB portB;
    PortC portC;
    PortD portD;

    template<int DDRx, int PORTx, int PINx>
    static inline __attribute__((always_inline))
    void flushPortData(AVRPort<DDRx, PORTx, PINx> port) {
      uint8_t sb = port.data & port.usage;
      uint8_t rb = port.usage & ~port.data;
      if (sb) {
        *(volatile uint8_t *)PORTx |= sb;
      }

      if (rb) {
        *(volatile uint8_t *)PORTx &= ~rb;
      }
    }
  public:
    inline __attribute__((always_inline)) BufferedOutput() : noops(0) {
    }

    inline __attribute__((always_inline)) ~BufferedOutput() {
      flushPortData(portB);
      flushPortData(portC);
      flushPortData(portD);

      switch (noops) {
        case 0: return;
        case 1:
          __asm__("nop\n\t");
          break;
        case 2:
          __asm__("nop\n\tnop\n\t");
          break;
        case 3:
          __asm__("nop\n\tnop\n\tnop\n\t");
          break;
        case 4:
          __asm__("nop\n\tnop\n\tnop\n\tnop\n\t");
          break;
        case 5:
          __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
          break;
        case 6:
          __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
          break;
        case 7:
          __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
          break;
        default:
          __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
          break;
      }
    }

    inline __attribute__((always_inline)) BufferedOutput& noop() {
      noops++;
      return *this;
    }

    template<class Port, uint8_t b>
    inline BufferedOutput& operator <<(DigitalPin<Port, b> pintValue) {
      switch ((int)Port::PORT) {
        case (int)PortB::PORT:
          portB.usage |= 1 << b;
          portB.data = (portB.data & ~(1 << b)) | (pintValue.value << b);
          break;
        case (int)PortC::PORT:
          portC.usage |= 1 << b;
          portC.data = (portC.data & ~(1 << b)) | (pintValue.value << b);
          break;
        case (int)PortD::PORT:
          portD.usage |= 1 << b;
          portD.data = (portD.data & ~(1 << b)) | (pintValue.value << b);
          break;
      }

      return *this;
    }

    inline BufferedOutput& operator <<(const Noop &) {
      noops++;
      return *this;
    }
};

void setup() {
  Serial.begin(9600);
  D13::mode(INPUT);
  D12::mode(INPUT);
  D11::mode(INPUT);
}

void loop() {
  uint8_t a = D13::read();
  uint8_t b = D12::read();
  uint8_t c = D11::read();
  Serial.println(a);
  Serial.println(b);
  Serial.println(c);
  Serial.println("End");
  delay(1000);
}

