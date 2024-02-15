/*


*/

#ifndef ARDUINO_ARCH_ESP32
#error "Select an ESP32 board"
#endif

#define VERSION "v 0.2"
#define PROJECT "Main satellite"

#include "Config.h"
#include "Satellite.h"
#include "Settings.h"
#include "Task.h"
#include "WebHandler.h"
#include "Wifi_fl.h"

uint8_t idMain = 254;

#include <ACAN_ESP32.h>

static const uint32_t CAN_BITRATE = 1000UL * 1000UL; // 1 Mb/s
Fl_Wifi wifi;
WebHandler webHandler;
Satellite sat[NB_SAT];

#define debug Serial

void setup()
{

  //--- Start serial
  debug.begin(115200);
  delay(100);

  Serial.printf("\n\nProject :    %s", PROJECT);
  Serial.printf("\nVersion :      %s", VERSION);
  Serial.printf("\nFichier :      %s", __FILE__);
  Serial.printf("\nCompiled :     %s", __DATE__);
  Serial.printf(" - %s\n\n", __TIME__);

  //--- Configure ESP32 CAN
  debug.print("Configure ESP32 CAN ");
  ACAN_ESP32_Settings settings(CAN_BITRATE);
  settings.mRxPin = GPIO_NUM_22;
  settings.mTxPin = GPIO_NUM_23;
  // const ACAN_ESP32_Filter filter = ACAN_ESP32_Filter::singleExtendedFilter(
  //     ACAN_ESP32_Filter::dataAndRemote, 0xB << 7, 0x1FFFFDFF);
  // uint32_t errorCode = ACAN_ESP32::can.begin(settings, filter);

  uint32_t errorCode = ACAN_ESP32::can.begin(settings);
  if (errorCode == 0)
    debug.print("ok !\n");
  else
  {
    debug.printf("error 0x%x\n", errorCode);
  }

  Settings::begin();
  Settings::readFile();
  Task::begin();

  wifi.start();
  webHandler.init(80);
}

void loop()
{
  CANMessage frameIn;
  CANMessage frameOut;

  /*--------------------------------------
    Reception
    --------------------------------------*/

  if (ACAN_ESP32::can.receive(frameIn))
  {
    debug.println("recu");
    auto sendMsg = [](CANMessage frameOut) -> bool
    {
      bool err = ACAN_ESP32::can.tryToSend(frameOut);
      return err;
    };

    const byte priorite = 0;
    const byte cmde = (frameIn.id & 0x7F80000) >> 19;
    const byte idExp = (frameIn.id & 0x7F800) >> 11; // ID du satellite qui envoie
    const byte resp = (frameIn.id & 0x7F8) >> 3; // Reponse true = 1 false = O
    const byte *idDest = &idExp;

    frameOut.id |= priorite << 27;   // Priorite 0, 1, 2 ou 3
    frameOut.id |= idMain << 11;     // ID expediteur
    frameOut.id |= *idDest << 3; // ID destinataire
    frameOut.ext = true;
    // frameOut.rtr = true; // Remote frame

    debug.printf("Reception du sattelite : %d\n", idExp);
    debug.printf("commande : 0x%0X\n", cmde);

    switch (cmde)
    {
    case 0xB2: // Test du bus CAN
      debug.print("Req->Test du bus CAN\n");
      frameOut.len = 1;
      frameOut.id |= 0xB3 << 19; // commande chez le destinataire
      frameOut.id |= 0x01 << 2;  // reponse
      frameOut.data[0] = 1;      // message retourné (1 pour OK)

      if (sendMsg(frameOut))
        debug.printf("Send->Test du bus CAN : OK\n\n");

      // Enregistrement de l'expéditeur dans la base de données des satellites
      for (byte i = 0; i < NB_SAT; i++)
      {
        if (idExp == sat[i].id())
          break;
        else if (sat[i].id() == NO_ID)
        {
          sat[i].id(idExp);
          break;
        }
      }
      break;

    case 0xB4: // Demande d'identifiant
      if (Settings::idNode < 253)
      {
        debug.print("Req->Demande d'identifiant\n");
        frameOut.len = 1;
        frameOut.id |= 0xB5 << 19;            // commande chez le destinataire
        frameOut.id |= 0x01 << 2;  // reponse
        frameOut.data[0] = Settings::idNode; // id retourne

        if (sendMsg(frameOut))
        {
          debug.printf("Send->Identifiant satellite : %d\n\n", Settings::idNode);
          Settings::idNode++;
          Settings::writeFile();
        }
      }
      else
        debug.printf("Taille maxi des identifiants atteinte : %d\n\n", Settings::idNode);
      break;

    // case 0xB6: // Demande le nombre de locos
    //   debug.print("Req->Nombre de locos\n");
    //   frameOut.len = 1;
    //   frameOut.id |= 0xB7 << 3; // commande chez le destinataire
    //   frameOut.data[0] = Settings::nbLoco;

    //   if (sendMsg(frameOut))
    //     debug.printf("Send->Nombre de locos : %d\n\n", frameOut.data[1]);
    //   break;

      //      case 0xB8 : // Demande le tag de locos
      //        debug.print ("Demande tag de locos\n");
      //        frameOut.len = 8;
      //        frameOut.data[0] = 0xB9;                  // id requete
      //        frameOut.data[1] = frameIn.data[1];       // index du tableau
      //        frameOut.data[2] = Settings::tableLoco[frameIn.data[1]][0];
      //        frameOut.data[3] = Settings::tableLoco[frameIn.data[1]][1];
      //        frameOut.data[4] = Settings::tableLoco[frameIn.data[1]][2];
      //        frameOut.data[5] = Settings::tableLoco[frameIn.data[1]][3];
      //        frameOut.data[6] = Settings::tableLoco[frameIn.data[1]][4];
      //        frameOut.data[7] = Settings::tableLoco[frameIn.data[1]][5];
      //        if (sendMsg(frameOut))
      //          debug.printf ("Envoi du tag %d\n", frameIn.data[1]);
      //        break;
    }
  }
}