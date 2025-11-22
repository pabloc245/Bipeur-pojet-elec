#include <unity.h>
#include <SPI.h>
#include <Arduino.h>

#define LED_ROUGE 5
#define BOUTON_PIN A6

#define TEST_ASSERT_HIGH_WITHIN(var_func, timeout_ms)                \
    do {                                                              \
        unsigned long start = millis();                               \
        int state = 0;                                                \
        while ((millis() - start) < (timeout_ms)) {                   \
            state = (var_func);                                       \
            if (state > 200) break;                                 \
        }                                                             \
        TEST_ASSERT_EQUAL_INT_MESSAGE(200, state,                   \
            "Condition HIGH non atteinte dans le temps imparti");    \
    } while(0)

void setUp(void) {
  
}

void tearDown(void) {
}

void test_serial_begin(void) {
  Serial.println("Hello world!");
  // Vérifier que Serial est bien initialisé
  TEST_ASSERT_EQUAL(1,1);
}

void test_led_RGB() {
  digitalWrite(LED_ROUGE, HIGH);
  TEST_ASSERT_HIGH_WITHIN(analogRead(BOUTON_PIN), 2000);
}

void test_messege() {
  #if UNO1
  //iljzrfj
  #endif
  
  digitalWrite(LED_ROUGE, HIGH);
  TEST_ASSERT_HIGH_WITHIN(analogRead(BOUTON_PIN), 2000);
}


void setup() {
  delay(2000);

  UNITY_BEGIN();
  UnityPrint("quelque chose s'est passé !");
  RUN_TEST(test_serial_begin);
  RUN_TEST(test_led_RGB);
  UNITY_END();
}

void loop() {}