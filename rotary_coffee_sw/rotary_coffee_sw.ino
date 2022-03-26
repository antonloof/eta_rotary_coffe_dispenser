// PB0
#define OUT0_PIN 8
// PB1
#define OUT1_PIN 9

// PC0
#define ADC0_FB_PIN A0
// PC1
#define ADC1_FB_PIN A1

#define ROTARY_PULSE_TIMEOUT_MS 200
#define ROTARY_PULSE_W_MS 10

#define ACT_PULSE_W_MS 50

#define MS_PER_CUP_WATER 2300
#define MS_OFFSET -160

#define MAX_CUPS 9
#define MIN_CUPS 1

typedef struct debounce_pin {
  int pin, constant_time;
  unsigned long debounce_time;
  int last_state, stable;
} debounce_pin;

typedef void (*callback)(int);
typedef struct rotary_ctrl {
  debounce_pin *act_pin, *pulse_pin;
  int pulse_state, count;
  callback on_full_rotation;
} rotary_ctrl;

void debounce_pin_init(debounce_pin *pin);
void debounce_pin_update(debounce_pin *pin);


debounce_pin rot0_act = {A3, ROTARY_PULSE_TIMEOUT_MS};
debounce_pin rot0_pulse = {A3, ROTARY_PULSE_W_MS};
debounce_pin rot1_act = {A4, ACT_PULSE_W_MS};
debounce_pin rot1_pulse = {A5, ROTARY_PULSE_W_MS};

// active pc2, pulse pc3
rotary_ctrl rotary_0 = {&rot0_act, &rot0_pulse};
// actice pc4, pulse pc5
rotary_ctrl rotary_1 = {&rot1_act, &rot1_pulse};

unsigned long time_ms_water, time_ms_water_target;
unsigned long last_loop_ms;
int last_count = 0;

void rotary_init(rotary_ctrl *rot, callback cb);
void rotary_update(rotary_ctrl *rot);
void rot0_rotation(int count);
void rot1_rotation(int count);

void setup() {
  DDRD = 0xFF; // all led out!
  rotary_init(&rotary_0, &rot0_rotation);
  rotary_init(&rotary_1, &rot1_rotation);
  pinMode(OUT0_PIN, OUTPUT);
  pinMode(OUT1_PIN, OUTPUT);

  digitalWrite(OUT0_PIN, 0);
  digitalWrite(OUT1_PIN, 0);

  srand(analogRead(A0));

  time_ms_water = 0;
  time_ms_water_target = 0;
  last_loop_ms = millis();
}

void loop() {
  int current_ms = millis();
  rotary_update(&rotary_0);
  rotary_update(&rotary_1);
  if (time_ms_water_target > time_ms_water) {
    digitalWrite(OUT1_PIN, 1);  
    display_number(last_count);
    time_ms_water += current_ms - last_loop_ms;
  } else {
    time_ms_water_target = 0;
    time_ms_water = 0;
    digitalWrite(OUT1_PIN, 0);
    PORTD = 0;
  }
  last_loop_ms = current_ms;
}

void rotary_init(rotary_ctrl *rot, callback cb) {
  debounce_pin_init(rot->act_pin);
  debounce_pin_init(rot->pulse_pin);
  rot->count = 0;
  rot->pulse_state = 0;
  rot->on_full_rotation = cb;
}

void rotary_update(rotary_ctrl *rot) {
  debounce_pin_update(rot->act_pin);
  debounce_pin_update(rot->pulse_pin);
  if (rot->pulse_pin->stable && rot->pulse_pin->last_state != rot->pulse_state) {
    rot->pulse_state = rot->pulse_pin->last_state;
    if (!rot->pulse_pin->last_state) {
      rot->count++;
    }
  }
  
  if (rot->act_pin->stable && rot->act_pin->last_state) {
    if (rot->count) {
      rot->on_full_rotation(rot->count);
      rot->count = 0;
      rot->pulse_state = 1;
    }
  }
}

void debounce_pin_update(debounce_pin *pin) {
  int state = digitalRead(pin->pin);
  unsigned long sample_time = millis();
  if (pin->last_state != state) {
    pin->debounce_time = sample_time;
  }
  pin->stable = 0;
  if ((sample_time - pin->debounce_time) > pin->constant_time) {
    if (sample_time >= pin->debounce_time) {
      pin->stable = 1;
    }
  }
  pin->last_state = state;
}

void debounce_pin_init(debounce_pin *pin) {
  pin->debounce_time = millis();
  pin->last_state = 0;
  pin->stable = 0;
  pinMode(pin->pin, INPUT);
}

void display_number(int number) {
  int to_write = 0;
  for (int i = 0; i < 8; i++) {
    if (number & (1 << i)) {
      to_write |= 1 << (7-i);
    }
  }
  PORTD = to_write;
}

void rot0_rotation(int count) {
  int wanted_cups = constrain(count - 1, MIN_CUPS, MAX_CUPS);
  if (count == 1) {
    wanted_cups = random(MIN_CUPS, MAX_CUPS + 1);
  }
  time_ms_water_target = wanted_cups * MS_PER_CUP_WATER + MS_OFFSET;
  last_count = wanted_cups;

}
void rot1_rotation(int count) {

}
