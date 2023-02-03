#define DEBUG
#include <MsTimer2.h>

template<typename first_t, typename second_t> 
class pair{
  first_t first_element;
  second_t second_element;
public:
  pair(first_t _first, second_t _second): first_element(_first), second_element(_second) { }

  void operator = (pair<first_t, second_t> p) {
    first_element = p.first();
    second_element = p.second();
  }

  first_t& first() {return first_element; }
  second_t& second() {return second_element; }
};


class output{
public:
  template <typename T>
  output operator << (T out) const{
    Serial.print(out);
  }
};

namespace constants {
enum class PIN {
  PWM1 = 5,
  PWM2 = 6,
  PWM3 = 9,
  PWM4 = 10,

  LED1 = 11,
  LED2 = 12,
  LED3 = 13,

  analog0 = A0,
  analog1 = A1,
  analog2 = A2,
  analog3 = A3,
  analog4 = A4,
  analog5 = A5,
};

  constexpr uint16_t analog_thresholds[4] = {1023, 9000, 6000, 10000};
  constexpr uint8_t weights[4] = {2, 1, -1, -2};

  constexpr uint8_t analog_pins[4] = {
    static_cast<uint8_t>(constants::PIN::analog0), 
    static_cast<uint8_t>(constants::PIN::analog1),
    static_cast<uint8_t>(constants::PIN::analog2),
    static_cast<uint8_t>(constants::PIN::analog3),
  };

}

namespace globals {
  uint16_t loop_counter = 0;

  uint16_t analog_input[4] = {0, 0, 0, 0};

  pair<int16_t, int16_t> wheel_spd(0, 0);
}

void analog_reader(void);
void timer_inturrepts100(void);


void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif //DEBUG

  pinMode(static_cast<int>(constants::PIN::LED1), OUTPUT);
  pinMode(static_cast<int>(constants::PIN::LED2), OUTPUT);
  pinMode(static_cast<int>(constants::PIN::LED3), OUTPUT);

  digitalWrite(static_cast<int>(constants::PIN::LED3), HIGH);//check is moving

  analogWrite(static_cast<int>(constants::PIN::PWM1), globals::wheel_spd.first());
  analogWrite(static_cast<int>(constants::PIN::PWM3), globals::wheel_spd.second());

  MsTimer2::set(10, timer_inturrepts10);
  MsTimer2::start();
}

void loop() {
  analog_reader();
}

void analog_reader() {
  for(int i = 0; i < 4; i++){
    globals::analog_input[i] += analogRead(constants::analog_pins[i]) << 8;
  }

  ++globals::loop_counter;
}

void timer_inturrepts10(void) {
#ifdef DEBUG
  output() << globals::wheel_spd.first() << ", " << globals::wheel_spd.second() << '\n';
#endif //DEBUG

  uint8_t analog_integraled[4] = {0, 0, 0, 0};
  uint8_t analog_is_white[4];
  
  for(int i = 0; i < 4; i++){
    analog_is_white[i] = (globals::analog_input[i] > constants::analog_thresholds[i]) ? 1 : 0;

    globals::analog_input[i] = 0;
  }

  for(int i = 0; i < 4; i++){
    globals::wheel_spd.first() += constants::weights[i] * analog_is_white[i];
    globals::wheel_spd.second() += - constants::weights[i] * analog_is_white[i];
  }
  if(abs(globals::wheel_spd.first()) > 2750) globals::wheel_spd.first() = (globals::wheel_spd.first() > 0) ? 2750: -2750;
  if(abs(globals::wheel_spd.second()) > 2750) globals::wheel_spd.second() = (globals::wheel_spd.second() > 0) ? 2750: -2750;

  analogWrite(static_cast<int>(constants::PIN::PWM1), 200 + globals::wheel_spd.first()/50);
  analogWrite(static_cast<int>(constants::PIN::PWM3), 200 + globals::wheel_spd.second()/50);
  
  globals::loop_counter = 0;


}
