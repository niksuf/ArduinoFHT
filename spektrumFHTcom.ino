// ---------------- НАСТРОЙКИ ----------------
#define FHT_N 256          // ширина спектра х2
#define samplingFrequency 38400
// ---------------- НАСТРОЙКИ ----------------

// ---------------------- ПИНЫ ----------------------
#define AUDIO_IN 0          // пин, куда подключен звук
// ---------------------- ПИНЫ ----------------------

// --------------- БИБЛИОТЕКИ ---------------
#define LIN_OUT 1
#include <FHT.h>          // преобразование Фурье
#include <Wire.h>
#define printByte(args) write(args);
double prevVolts = 100.0;
// --------------- БИБЛИОТЕКИ ---------------
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

int Freq = 0;

void setup() {
  // поднимаем частоту опроса аналогового порта до 38.4 кГц, по теореме
  // Котельникова (Найквиста) частота дискретизации будет 19 кГц
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);

  // для увеличения точности уменьшаем опорное напряжение,
  // выставив EXTERNAL и подключив Aref к выходу 3.3V на плате через делитель
  // GND ---[2х10 кОм] --- REF --- [10 кОм] --- 3V3
  analogReference(EXTERNAL);

  Serial.begin(9600);
  //delay(2000);
}

void loop() {
  analyzeAudio();   // функция FHT, забивает массив fht_log_out[] величинами по спектру

  Serial.println("Computed magnitudes:");
  for (int pos = 0; pos < FHT_N / 2; pos++) {
    Freq = (pos) * (samplingFrequency) / (FHT_N);
    Serial.print(Freq);
    Serial.print(" ");
    Serial.println(fht_lin_out[pos]);
  }
  Serial.println();
}

void analyzeAudio() {
  for (int i = 0 ; i < FHT_N ; i++) {
    int sample = analogRead(AUDIO_IN);
    fht_input[i] = sample; // put real data into bins
  }
  fht_window();  // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run();     // process the data in the fht
  fht_mag_lin(); // take the output of the fht
}
