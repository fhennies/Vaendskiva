#include <Arduino.h>

int fadePin[] = {3, 5, 6, 9, 10, 11};                 // LED connected to digital pin 13
static const uint8_t stopPin[] = {A1,A3,A5};
static const uint8_t  goPin[] = {A0,A2,A4};
boolean isStop[] = {0, 0, 0}, isGo[] = {0, 0, 0};
unsigned long nextFade[6];
int fadeBrightness[] = {150, 150, 150, 150, 150, 150};
char fadeDir[] = {1, 1, 1, 1, 1, 1};



void fade(boolean isOn, boolean isBlink, int ledNr) {
    boolean action = true;
    unsigned long now;
    now = millis();
    if ((fadeBrightness[ledNr] > 254) && !isOn)  {action = false;} 
    if ((fadeBrightness[ledNr] < 1) && isOn)  {action = false;}
    if (isBlink)  {action = true;}
    if ((now >= nextFade[ledNr]) && action)  {
        nextFade[ledNr] = now + 1;
        if ((fadeDir[ledNr] > 0) && (fadeBrightness[ledNr] > 254)){
                fadeDir[ledNr] = - fadeDir[ledNr];
                nextFade[ledNr] += 1000;
        } 
        else if ((fadeDir[ledNr] < 0) && (fadeBrightness[ledNr] < 1)) {
                fadeDir[ledNr] = - fadeDir[ledNr];
                nextFade[ledNr] += 1000;
        }
        else {  fadeBrightness[ledNr] += fadeDir[ledNr];
        }
        analogWrite(fadePin[ledNr], fadeBrightness[ledNr]);
    }
}



void setup() {
    // put your setup code here, to run once:
    for (int i = 0; i < 6; i++) {
        pinMode(fadePin[i], OUTPUT);      // sets the digital pin as output
        digitalWrite(fadePin[i], LOW);
        nextFade[i] = millis() + 100 * i;
    };
    for (int i = 0; i < 3; i++) {
        pinMode(goPin[i], INPUT_PULLUP);
        pinMode(stopPin[i], INPUT_PULLUP);
    };
        
}

void loop() {
    //Eingägne einlesen, Ausgänge setzen
    for (int i = 0; i < 3; i++) {
        isStop[i] = digitalRead(stopPin[i]);
        isGo[i] = digitalRead(goPin[i]);
        fade(isGo[i], (isStop[i] && isGo[i]), i*2);
        fade(isStop[i], false, i*2+1);
    };
}