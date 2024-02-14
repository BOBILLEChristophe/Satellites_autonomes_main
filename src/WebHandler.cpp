// WebHandler.cpp

#include "WebHandler.h"

WebHandler::WebHandler() : _server(nullptr), _ws(nullptr) {}

void WebHandler::init(uint16_t webPort)
{
  _server = new AsyncWebServer(webPort);
  _ws = new AsyncWebSocket("/ws");
  _ws->onEvent(std::bind(&WebHandler::_WsEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

  WebHandler::route();

  _server->addHandler(_ws);
  _server->begin();
}

void WebHandler::loop()
{
  _ws->cleanupClients();
}

void WebHandler::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0)
    {
      WebHandler::notifyClients();
    }
  }
}

void WebHandler::_WsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;

  case WS_EVT_ERROR:
    //
    break;
  case WS_EVT_PONG:
    //
    break;
  case WS_EVT_DATA:
    // WebHandler::handleWebSocketMessage(arg, data, len);
    // Serial.printf("WebSocket arg %d\n", arg);
    // Serial.printf("WebSocket len %d\n", len);
    // Serial.printf("WebSocket data %s\n", data);

    StaticJsonDocument<1024> doc1; // Memory pool
    DeserializationError error = deserializeJson(doc1, data);

    if (error) // Check for errors in parsing
#ifdef DEBUG
      debug.println("Parsing failed");
#endif
    else
    {
      String message = (char *)data;

      if (message.indexOf("wifi_on") >= 0)
      {
#ifdef DEBUG
        debug.println("wifi_on");
#endif
        bool wifi_on = doc1["wifi_on"][0];
        // Settings::WIFI_ON = wifi_on;
      }

      if (message.indexOf("discovery_on") >= 0)
      {
        bool discovery_on = doc1["discovery_on"][0];
        // Settings::DISCOVERY_ON = discovery_on;

#ifdef DEBUG
        debug.printf(Settings::DISCOVERY_ON ? "Discovery : on" : "Discovery : off");
        debug.printf("\n");
#endif
      }
    }
    break;
  }
}

void WebHandler::notifyClients()
{
  _ws->textAll(String("ok"));
}

void WebHandler::route()
{
  // Route for root / web page
  _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/controller.html", "text/html"); });

  // Route to load style.css file
  _server->on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/w3.css", "text/css"); });

  // Route to load style.css file
  _server->on("css/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/style.css", "text/css"); });

  // Route to load script.js file
  _server->on("/script/controller_4.3.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/script.js", "text/javascript"); });

  // Route to load json file
  _server->on("/param.jso", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/settings.json", "text/json"); });

  // Route to load favicons file
  _server->on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/favicon.png", "image/png"); });

  _server->onNotFound([](AsyncWebServerRequest *request)
                      {
    Serial.printf("Not found: %s!\r\n", request->url().c_str());
    request->send(404); });
}
