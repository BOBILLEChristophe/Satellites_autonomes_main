/*


*/

#include "Settings.h"

// No constructor

bool Settings::WIFI_ON = true;
bool Settings::DISCOVERY_ON = true;

uint8_t Settings::idNode = NO_ID;
// uint8_t Settings::nbLoco = NB_LOCOS;
// const byte Settings::tableLoco[NB_LOCOS][6] = {
//     {10, 0, 0x04, 0xC3, 0x37, 0x22},
//     {11, 0, 0x04, 0x73, 0x38, 0x22},
//     {15, 0, 0x04, 0x93, 0x38, 0x22},
//     {16, 0, 0x04, 0x46, 0x37, 0x22},
//     {30, 0, 0x04, 0x25, 0x37, 0x22},
//     {34, 0, 0x04, 0xE4, 0x37, 0x22},
//     {65, 0, 0x04, 0x04, 0x37, 0x22}};

void Settings::begin()
{
  if (!SPIFFS.begin(true))
  {
    Serial.printf("An Error has occurred while mounting SPIFFS\n");
    return;
  }
  else
    Serial.printf("Ok mounting SPIFFS\n");
}

void Settings::readFile()
{
  File file = SPIFFS.open("/settings.json", "r");
  if (!file)
  {
    Serial.println("Failed to open settings.json");
    return;
  }
  else
  {
    Serial.printf("Informations du fichier \"settings.json\" : \n");

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error)
      Serial.printf("Failed to read file, using default configuration\n");

    Settings::idNode = doc["idNode"];
    // if (Settings::nbLoco == 0)
    //   Settings::nbLoco = doc["nbLoco"];

    DISCOVERY_ON = doc["discovery_on"];
    WIFI_ON = doc["wifi_on"];

    Serial.printf("- ID dernier satellite :   %d\n", Settings::idNode);
    //Serial.printf("- Nombre de locos      :   %d\n", Settings::nbLoco);
    Serial.printf("\n");

    file.close();
  }
}

void Settings::writeFile()
{

  File file = SPIFFS.open("/settings.json", "w");
  if (!file)
  {
    Serial.println("Failed to open settings.json");
    return;
  }
  else
  {
    file.print("{");

    file.print("\"idNode\":");
    file.print(Settings::idNode);
    file.print("\",");
    file.print("\"discovery_on\":");
    file.print(Settings::DISCOVERY_ON);
    file.print("\",");
    file.print("\"wifi_on\":");
    file.print(Settings::WIFI_ON); // file.print("\",");

    file.print("}");
    file.close();
  }
}
