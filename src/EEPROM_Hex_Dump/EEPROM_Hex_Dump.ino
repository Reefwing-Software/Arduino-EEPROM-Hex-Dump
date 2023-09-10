#include <EEPROM.h>

void printHex(uint8_t hexNum) {
    Serial.print("0x");
    if (hexNum <= 15)
       Serial.print("0");
    Serial.print(hexNum, HEX);
}

void printAddress(int addr) {
  char buffer[3];

  Serial.print("\n0x");
  sprintf(buffer, "%02X", addr);
  Serial.print(buffer);
  Serial.print("\t");
}

void printASCII(int idx) {
  char octet[9];

  //  Print ASCII Representations of the last 8-bytes
  Serial.print("|    ");
  EEPROM.get(idx - 8, octet);

  //  Replace non-printable characters
  for (int i = 0; i < 8; i++) {
    if ((int)octet[i] < 32 || (int)octet[i] > 127) {
      octet[i] = '.';
    }
  }

  //  Add null terminator to create a C string
  octet[8] = '\0';
  Serial.print(octet);
}

void setup() {
  char buffer[3];

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);

  //  Display Header
  Serial.println(F("=====================++++++ Memory Size ++++++====================="));
  Serial.print(F("Maximum Static RAM Address:\t0x"));
  Serial.print(RAMEND, HEX);
  Serial.print(" (");
  Serial.print(RAMEND / 1024);
  Serial.println("KB)");
  Serial.print(F("Maximum EEPROM Address:\t\t0x"));
  Serial.print(E2END, HEX);
  Serial.print(" (");
  Serial.print((float)(E2END / 1024.0), 0);
  Serial.println("KB)");
  Serial.print(F("Maximum Flash Address:\t\t0x"));
  Serial.print(FLASHEND, HEX);
  Serial.print(" (");
  Serial.print((float)(FLASHEND / 1024.0), 0);
  Serial.println("KB)");
  Serial.println(F("===================== Arduino EEPROM HEX Dump ====================="));
  Serial.println(F("Addr     0       1       2       3       4       5       6       7"));

  //  Iterate each EEPROM byte and display on serial monitor
  for (int index = 0 ; index < EEPROM.length() ; index++) {
    if (index == 0) {
      printAddress(index);
    }
    else if (index % 8 == 0) { 
      //  Print ASCII Representations of the last 8-bytes
      printASCII(index);

      //  Newline and print address every 8-bytes
      printAddress(index);
    }

    unsigned char value = EEPROM.read(index);
    
    sprintf(buffer, "%02X", value);
    Serial.print(buffer);
    Serial.print("\t");

    delay(5);
  }

  //  Print ASCII Representations of the final 8-bytes
  printASCII(EEPROM.length() - 1);

}

void loop() { }
