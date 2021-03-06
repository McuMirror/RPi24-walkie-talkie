#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "compatibility.h"
#include "rf24.h"
#include "nRF24L01.h"

uint8_t tx_address[5] = {0xF0, 0xF0, 0xF0, 0xF0, 0xE1};
uint8_t rx_address_0[5] = {0xB7, 0xB7, 0xB7, 0xB7, 0xD5};
uint8_t rx_address_1[5] = {0xA1, 0xA1, 0xA1, 0xA1, 0xC2};
uint8_t receiveAddr[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
/* 32 byte character array is max payload */
char receivePayload[32];

uint8_t len = 5;

typedef struct result {
    uint8_t pass;
    uint8_t fail;
} Result;


uint8_t assert(int check, int value, Result *r) {
    if (check == value) {
        r->pass++;
        printf("\tPASS\n");
        return 1;
    } else {
        r->fail++;
        printf("\tFAIL\n");
        return 0;
    }
}

void test_data_rate(Result *r) {
    printf("[TEST SUITE] Performing Data rate tests...\n");
    printf("[TEST]\tSetting data rate to 250Kbps...");
    rf24_setDataRate(RF24_250KBPS);
    printf("\t\t> Model: %s\n", 
        (assert(RF24_250KBPS, rf24_getDataRate(), r) ? "nRF24L01+" : "nRF24L01"));
    printf("[TEST]\tSetting data rate to 1Mbps...");
    rf24_setDataRate(RF24_1MBPS);
    printf("Result is: %d", rf24_getDataRate() == RF24_1MBPS);
    assert(RF24_1MBPS, rf24_getDataRate(), r);
    printf("[TEST]\tSetting data rate to 2Mbps...");
    rf24_setDataRate(RF24_2MBPS);
    printf("Result is: %d", rf24_getDataRate() == RF24_2MBPS);
    assert(RF24_2MBPS, rf24_getDataRate(), r);
}

void test_power_level(Result *r) {
    printf("[TEST SUITE] Performing power level tests...\n");
    printf("[TEST]\tSetting power level to min...");
    rf24_setPALevel(RF24_PA_MIN);
    assert(RF24_PA_MIN, rf24_getPALevel(), r);
    printf("[TEST]\tSetting power level to low...");
    rf24_setPALevel(RF24_PA_LOW);
    assert(RF24_PA_LOW, rf24_getPALevel(), r);
    printf("[TEST]\tSetting power level to high...");
    rf24_setPALevel(RF24_PA_HIGH);
    assert(RF24_PA_HIGH, rf24_getPALevel(), r);
    printf("[TEST]\tSetting power level to max...");
    rf24_setPALevel(RF24_PA_MAX);
    assert(RF24_PA_MAX, rf24_getPALevel(), r);
}

void test_crc_length(Result *r) {
    printf("[TEST SUITE] Performing crc tests...\n");
    printf("[TEST]\tDisabling CRC...");
    rf24_setCRCLength(RF24_CRC_DISABLED);
    assert(RF24_CRC_DISABLED, rf24_getCRCLength(), r);
    printf("[TEST]\tSetting CRC length to 8bits...");
    rf24_setCRCLength(RF24_CRC_8);
    assert(RF24_CRC_8, rf24_getCRCLength(), r);
    printf("[TEST]\tSetting CRC length to 16bits...");
    rf24_setCRCLength(RF24_CRC_16);
    assert(RF24_CRC_16, rf24_getCRCLength(), r);
}

void run_test_suite(Result *r) {
    printf("\n***********************************\n");
    printf("      nRF24L01(+) test suite\n\n");
    test_data_rate(r);
    test_power_level(r);
    printf("***********************************\n");
    printf("           Test summary\n\n");
    printf("   > Tests passed: %d\n", r->pass);
    printf("   > Tests failed: %d\n\n", r->fail);
    printf("***********************************\n");
}

void setup(void) {
    Result r = {.pass = 0, .fail = 0};
    uint8_t status = rf24_init_radio("/dev/spidev0.0", 6000000, 25);
    if (status == 0)
    {
	exit(-1);
	printf("We have failure!!!");
    }
    run_test_suite(&r);
    rf24_resetcfg();
    rf24_setAddressWidth(5);
    rf24_setRXAddressOnPipe(rx_address_1, 1);
    rf24_setChannel(60);
    rf24_setPayloadSize(10);
    rf24_setAutoAckOnAll(0);
}

void loop(void) {
    while(rf24_packetAvailable()) {
        memset(receivePayload, 0, 32);
        len = rf24_recvfrom(receivePayload, len, receiveAddr, 1); /* Blocking recv */
        printf("Recvd pkt - len: %d : %d\n", len, receivePayload[0]);
        //rf24_startWrite(receiveAddr, receivePayload, len);
    }
}

int main() {
    setup();

    rf24_stopListening(); // enter a talking mode
    setTXAddress(tx_address);
    system("date");
    printf("\nDETAILS AGAIN\n");
    rf24_printDetails();
    int SIZE = 10;
    char a[10] = {'a','b','c','d','r','a','e','i','o','u'};
    void * voidie = &a;

    system("date +\"%s %3N\"");
    rf24_write(voidie, SIZE);
    system("date +\"%s %3N\"");
//    while(1) {
//        loop();
//    }

    return 0;
}
