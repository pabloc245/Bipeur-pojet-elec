#include <unity.h>
#include <Arduino.h>
#include "radio.hpp"

Message messageBuffer = {
        "user1",
        "Hello world"
    };
bool R=false;

void setUp_radio(void) {
    //Parametre param = {"test", 0, 0};
    
}

void tearDown_radio(void) {}

// Tests hashPseudo
void test_hash_consistent(void) {
    uint32_t h1 = hashPseudo("alice");
    uint32_t h2 = hashPseudo("alice");
    TEST_ASSERT_EQUAL(h1, h2);
}

void test_hash_different_users(void) {
    uint32_t h1 = hashPseudo("alice");
    uint32_t h2 = hashPseudo("bob");
    TEST_ASSERT_NOT_EQUAL(h1, h2);
}

// Tests logique encodage
void test_address_position(void) {
    uint32_t addr = 0x12345;
    uint8_t pos = addr & 0b111;
    TEST_ASSERT_EQUAL(5, pos); // 0x12345 & 0b111 = 5
}

void test_payload_pair(void) {
    uint8_t data[] = {0xAA, 0xBB, 0xEE};
    uint32_t p = ((uint32_t)data[0] << 22) | 
                 ((uint32_t)data[1] << 14) | 
                 ((uint32_t)(data[2] & 0xF0) << 6);
    TEST_ASSERT_EQUAL((0xAABBE<<10), p);
}

void test_payload_impair(void) {
    uint8_t data[] = {0xCC, 0xDD, 0xEE};
    uint32_t p = ((uint32_t)(data[0] & 0x0F) << 26) | 
                 ((uint32_t)data[1] << 18) | 
                 ((uint32_t)data[2] << 10);
    TEST_ASSERT_EQUAL((0xCDDEE<<10), p);
    
}

void test_radio_communication(void){
    #ifdef TEST_TRANSMITTER
    R=0;
    setRole();
    TEST_ASSERT_EQUAL(1,1);
    //TEST_ASSERT_EQUAL_INT8(envoyerMessage(&messageBuffer, LOW_P), 1);
    #endif
    #ifdef TEST_RECEIVER
    Message vide = {0, 0};
    R=1;
    setRole();
    lireMessage(&vide);
    TEST_ASSERT_EQUAL_STRING(vide.message, messageBuffer.message);
    #endif
}

void setup() {
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_hash_consistent);
    RUN_TEST(test_hash_different_users);
    RUN_TEST(test_address_position);
    RUN_TEST(test_payload_pair);
    RUN_TEST(test_payload_impair);
    RUN_TEST(test_radio_communication);

    UNITY_END();
}

void loop() {}