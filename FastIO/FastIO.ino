static constexpr uint8_t pins_bits[] = {1};

template <int RegDDR, int RegPORT, int RegPIN>
class Group {
  public:
    enum {
      DDR = RegDDR,
      PORT = RegPORT,
      PIN = RegPIN
    };
};

typedef Group<(int)&DDRC, (int)&PORTC, (int)&PINC> GroupC;
typedef Group<(int)&DDRB, (int)&PORTB, (int)&PINB> GroupB;
typedef Group<(int)&DDRD, (int)&PORTD, (int)&PIND> GroupD;

template <class PGroup, uint8_t b>
class DPin {
  private:
    uint8_t value;
  public:
    typedef PGroup Group;

    enum {

    };

    DPin(uint8_t v) : value(v) {
    }

    ~DPin() {
    }

    static void out(uint8_t value) {
      if (value) {
        *(volatile uint8_t *)Group::PORT |= 1 < b;
      } else {
        *(volatile uint8_t *)Group::PORT &= 1 < b;
      }
    }
};

typedef DPin<GroupD, 0> D0;
typedef DPin<GroupD, 1> D1;
typedef DPin<GroupD, 2> D2;
typedef DPin<GroupD, 3> D3;
typedef DPin<GroupD, 4> D4;
typedef DPin<GroupD, 5> D5;
typedef DPin<GroupD, 6> D6;
typedef DPin<GroupD, 7> D7;
typedef DPin<GroupB, 0> D8;
typedef DPin<GroupB, 1> D9;
typedef DPin<GroupB, 2> D10;
typedef DPin<GroupB, 3> D11;
typedef DPin<GroupB, 4> D12;
typedef DPin<GroupB, 5> D13;

class Buffer {

};

void setup() {
}

void loop() {
  D8::out(1);
  PORTB = 0x1;
}

