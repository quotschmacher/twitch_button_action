#include <Arduino.h>
//#include <WiFi.h>
#include <ESP8266WiFi.h>
#include <IRCClient.h>
#include "security.h"

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667

char ssid[] = WLAN_SSID;
char password[] = WLAN_PASS;

unsigned long bandit_last_sent = 0;
unsigned long bandit_interval = 155000;

//The name of the channel that you want the bot to join
const String twitchChannelName = "projektiontv";

//The name that you want the bot to have
#define TWITCH_BOT_NAME "quotschmacher"

String ircChannel = "";

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);



void callback(IRCMessage ircMessage) {
  //Serial.println("In CallBack");

  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    //Serial.println("Passed private message.");
    ircMessage.nick.toUpperCase();

    String message("<" + ircMessage.nick + "> " + ircMessage.text);

    //prints chat to serial
    Serial.println(message);



    return;
  }
}

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

  client.setCallback(callback);
}

void sendTwitchMessage(String message) {
    client.sendMessage(ircChannel, message);
}

void sendBanditCommand(unsigned long current_millis) 
{
    Serial.println("Sende Bandit-Command");
    sendTwitchMessage("!bandit");
    bandit_last_sent = current_millis;
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
            sendBanditCommand(current_millis);
        } else {
            Serial.println("failed... try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
        return;
    }
    else
    {
        if ((current_millis - bandit_last_sent) > bandit_interval)
        {
            sendBanditCommand(current_millis);
        }
    }
    // nur zum reagieren auf neue Nachrichten
    client.loop();
}