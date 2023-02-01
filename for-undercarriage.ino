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

  constexpr uint8_t analog_thresholds[6] = {2, 2, 2, 2, 2, 2};
  constexpr uint8_t weights[6] = {2, 1, 0, 0, -1, -2};

  constexpr uint8_t analog_pins[6] = {
    static_cast<uint8_t>(constants::PIN::analog0), 
    static_cast<uint8_t>(constants::PIN::analog1),
    static_cast<uint8_t>(constants::PIN::analog2),
    static_cast<uint8_t>(constants::PIN::analog3),
    static_cast<uint8_t>(constants::PIN::analog4),
    static_cast<uint8_t>(constants::PIN::analog5)
  };

}

namespace globals {
  uint8_t loop_counter = 0;

  uint16_t analog_input[6] = {0, 0, 0, 0, 0, 0};

  pair<uint8_t, uint8_t> wheel_spd(200, 200);
}

void analog_reader(void);
void timer_inturrepts100(void);


void setup() {
  pinMode(static_cast<int>(constants::PIN::LED1), OUTPUT);
  pinMode(static_cast<int>(constants::PIN::LED2), OUTPUT);
  pinMode(static_cast<int>(constants::PIN::LED3), OUTPUT);

  digitalWrite(static_cast<int>(constants::PIN::LED3), HIGH);//check is moving

  analogWrite(static_cast<int>(constants::PIN::PWM1), globals::wheel_spd.first());
  analogWrite(static_cast<int>(constants::PIN::PWM3), globals::wheel_spd.second());

  MsTimer2::set(100, timer_inturrepts100);
}

void loop() {
  analog_reader();
}

void analog_reader() {
  for(int i = 0; i < 6; i++){
    globals::analog_input[i] += analogRead(constants::analog_pins[i]) << 8;
  }

  ++globals::loop_counter;
}

void timer_inturrepts100(void) {
  uint8_t analog_integraled[6] = {0, 0, 0, 0, 0, 0};
  bool analog_is_white[6];
  
  for(int i = 0; i < 6; i++){
    analog_integraled[i] = (globals::analog_input[i] / globals::loop_counter) >> 8;

    analog_is_white[i] = (analog_integraled[i] > constants::analog_thresholds[i]) ? true : false;

    globals::analog_input[i] = 0;
  }

  for(int i = 0; i < 6; i++){
    globals::wheel_spd.first() += constants::weights[i] * analog_is_white[i];
    globals::wheel_spd.second() += - constants::weights[i] * analog_is_white[i];
  }

  analogWrite(static_cast<int>(constants::PIN::PWM1), globals::wheel_spd.first());
  analogWrite(static_cast<int>(constants::PIN::PWM3), globals::wheel_spd.second());
  
  globals::loop_counter = 0;
  
}