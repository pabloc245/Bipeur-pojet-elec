#include "stockage.hpp"
#include <unity.h>
#include <EEPROM.h>


extern int nbMessage;
extern int nbContact;

void setUp_EEPROM(void) {
    clearEeprom();
    nbMessage = 0;
    nbContact = 0;
}

void tearDown_EEPROM(void) {
}

void test_writeMessage_increments_counter(void) {
    Message msg = {"user1", "test content"};
    writeMessage(&msg);
    TEST_ASSERT_EQUAL(1, nbMessage);
}

void test_readMessage_retrieves_correct_data(void) {
    Message write_msg = {"user2", "hello world"};
    Message read_msg;
    
    writeMessage(&write_msg);
    readMessage(&read_msg, 0);
    
    TEST_ASSERT_EQUAL_STRING(write_msg.pseudo, read_msg.pseudo);
    TEST_ASSERT_EQUAL_STRING(write_msg.message, read_msg.message);
}

void test_writeMessage_wraps_at_MAX_MESSAGES(void) {
    Message msg;
    for(int i = 0; i < MAX_MESSAGES + 2; i++) {
        writeMessage(&msg);
    }
    TEST_ASSERT_EQUAL(MAX_MESSAGES + 2, nbMessage);
}

void test_newContact_adds_contact(void) {
    Contact contact = { "Alice", 0x1234};
    uint8_t result = newContact(&contact);
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1, nbContact);
}

void test_newContact_rejects_duplicate(void) {
    Contact contact = { "Bob", 0x1234};
    newContact(&contact);
    uint8_t result = newContact(&contact);
    
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(1, nbContact);
}

void test_newContact_full_storage(void) {
    Contact contact;
    for(int i = 0; i < MAX_CONTACT; i++) {
        contact.adresse = 0x1000 + i;
        newContact(&contact);
    }
    
    Contact extra = { "Extra", 0xFFFF};
    uint8_t result = newContact(&extra);
    
    TEST_ASSERT_EQUAL(1, result);
}

void test_lireContacts_reads_correct_contact(void) {
    Contact write_contact = {"Charli", 0x567};
    Contact read_contact;
    Message msg1 = {"alice", "msg1", 0xAB};
    
    TEST_MESSAGE("=== DEBUT TEST ===");
    
    char buffer[50];
    sprintf(buffer, "msg1.adresse avant: 0x%lX", (unsigned long)msg1.pseudo);
    TEST_MESSAGE(buffer);
    
    writeMessage(&msg1);
    
    sprintf(buffer, "Adresse calculée: %d", START_CONTACT + 0 * sizeof(Contact));
    TEST_MESSAGE(buffer);
    
    lireContacts(&read_contact, 0);
    
    sprintf(buffer, "read_contact.adresse après: 0x%lX", (unsigned long)read_contact.adresse);
    TEST_MESSAGE(buffer);
    
    sprintf(buffer, "read_contact.pseudo après: %s", read_contact.pseudo);
    TEST_MESSAGE(buffer);
    
    TEST_ASSERT_EQUAL(msg1.pseudo, read_contact.adresse);
    TEST_ASSERT_EQUAL_STRING(msg1.pseudo, read_contact.pseudo);
    
    TEST_MESSAGE("=== FIN TEST (SUCCESS) ===");
}

void test_filtreMessage_finds_matching_messages(void) {
    Message msg1 = {"alice", "msg1", 0xAB};
    Message msg2 = {"bob", "msg2", 0xFFCD};
    Message msg3 = {"alice", "msg3", 0xFF};
    
    writeMessage(&msg1);
    writeMessage(&msg2);
    writeMessage(&msg3);
    
    uint8_t liste[10];
    uint8_t nb = filtreMessage(liste, (uint32_t)0xAB);
    
    TEST_ASSERT_EQUAL(2, nb);
}

