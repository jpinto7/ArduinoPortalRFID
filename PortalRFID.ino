#include <SPI.h>                                    // RC522 Module uses SPI protocol
#include <MFRC522.h>                                // Library for Mifare RC522 Device
#include <ArduinoJson.h>                            // Library to serialize/deserialize data as JSON

#define MFRC_SS_PIN 10                              // Nominate a pin to slave select the MFRC522
#define MFRC_RST_PIN  9                             // Nominate a pin to reset the MFRC522

boolean successfulRead = false;                     // Flag to tell whether or not a card has been read

MFRC522 mfrc522(MFRC_SS_PIN, MFRC_RST_PIN);         // define the MFRC522

void setup() {
  Serial.begin(9600);                               // start serial port at 9600 baud
  SPI.begin();                                      // start SPI bus
  mfrc522.PCD_Init();                               // start the MFRC522
}

void loop() {
  do {
    successfulRead = checkAsset();                  // Check if a card has been read successfully  
  } while (!successfulRead);                        // go no further until a card is read successfully
}

void sendPortalData(String assetId) {
  DynamicJsonBuffer jb(JSON_OBJECT_SIZE(1));
  JsonObject& portalData = jb.createObject();
  portalData["assetId"] = assetId;
  portalData.printTo(Serial);
  Serial.println();
}

String getAssetId() {
  String assetId = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    assetId += String(mfrc522.uid.uidByte[i], HEX); // Cast card UID bytes as string
  }
  assetId.toUpperCase();
  return assetId;                                   // Return card UID as string with hex values
}

boolean checkAsset() {
  if (!mfrc522.PICC_IsNewCardPresent()) {           // Look for a card being presented every 500ms until one is found
    delay(500);
    return false;
  }
  
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  
  String assetId = getAssetId();
  sendPortalData(assetId);

  mfrc522.PICC_HaltA();                             // Stop reading the card and tell it to switch off
  return true;
}
