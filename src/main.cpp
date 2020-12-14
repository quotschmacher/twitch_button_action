#include <Arduino.h>
//#include <WiFi.h>
#include <ESP8266WiFi.h>
#include <IRCClient.h>
// is ne ganz gute lib fuer buttons. entprellt und hat sonst auch nette features
#include <OneButton.h>
// hier sind meine security-einstellungen drin (wlan und twitch)
#include "security.h"

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667

#ifndef WLAN_SSID
#define WLAN_SSID "ssid"
#endif

#ifndef WLAN_PASS
#define WLAN_PASS "passwort"
#endif

#ifndef TWITCH_OAUTH_TOKEN
#define TWITCH_OAUTH_TOKEN "" // https://twitchapps.com/tmi/
#endif

// Funktionsprototypen
void step_click();
void sendTwitchMessage(String);
void sendStepCounter();

char ssid[] = WLAN_SSID;
char password[] = WLAN_PASS;

unsigned long step_counter_last_sent = 0;
unsigned long step_couunter_last_sent_interval = 31000; // millisekunden
uint8_t step_counter_cnt = 0;

//The name of the channel that you want the bot to join
const String twitchChannelName = "projektiontv";

// he name that you want the bot to have 
// der twitch username
#define TWITCH_BOT_NAME "addyourusernamehere"

String ircChannel = "";

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

uint8 stepcounter_button_pin = 1;

OneButton button_step_counter(stepcounter_button_pin, true);

void setup() {
    Serial.begin(115200);
    Serial.println();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    ircChannel = "#" + twitchChannelName;

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);

    button_step_counter.attachClick(step_click);
}

void loop() {
    unsigned long current_millis = millis();
    // Try to connect to chat. If it loses connection try again
    if (!client.connected())
    {
        Serial.println("Attempting to connect to " + ircChannel );
        // Attempt to connect
        // Second param is not needed by Twtich
        if (client.connect(TWITCH_BOT_NAME, "", TWITCH_OAUTH_TOKEN))
        {
            client.sendRaw("JOIN " + ircChannel);
            Serial.println("connected and ready to rock");
            //sendTwitchMessage("Ready to go Boss!");
        } else {
            Serial.println("failed... try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
        return;
    }
    else
    {
        if (step_counter_cnt > 0)
        {
            if ((current_millis- step_counter_last_sent) > step_couunter_last_sent_interval)
            {
                sendStepCounter();
            }
        }
    }

    // ist mMn ueberfluessig
    // nur zum reagieren auf neue Nachrichten
    //client.loop();
    
    button_step_counter.tick();
}

void sendTwitchMessage(String message) {
    client.sendMessage(ircChannel, message);
}

void sendStepCounter()
{
    sendTwitchMessage("!stepounter+");
    step_counter_last_sent = millis();
    step_counter_cnt--;
}

void step_click()
{
    step_counter_cnt++;
}
