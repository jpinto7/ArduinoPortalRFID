#include <SPI.h>                 // RC522 Module uses SPI protocol
#include <MFRC522.h>            // Library for Mifare RC522 Device
#include <ArduinoJson.h>

#define MFRC_SS_PIN          10 // Nominate a pin to slave select the MFRC522
#define MFRC_RST_PIN         9  // Nominate a pin to reset the MFRC522

// ************* Stuff for reading cards with MFRC522 *******************
boolean successfulRead = false;    // Whether or not a card has been read

MFRC522 mfrc522(MFRC_SS_PIN, MFRC_RST_PIN);  // define the MFRC522

void setup() {
  Serial.begin(9600);
  SPI.begin();                                // start SPI bus
  mfrc522.PCD_Init();                         // start the MFRC522
}

void loop() {
  do {
    successfulRead = checkAsset();        // Check if a card has been read successfully  
  } while (!successfulRead);          // go no further until a card is read successfully
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
    assetId += String(mfrc522.uid.uidByte[i], HEX);  // Print the UID to serial as hex values
  }
  return assetId;
}

boolean checkAsset() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(500);                       // Look for a card being presented every 500ms until one is found
    return false;
  }
  
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  
  String assetId = getAssetId();
  sendPortalData(assetId);

  mfrc522.PICC_HaltA();              // Stop reading the card and tell it to switch off
  return true;
}
