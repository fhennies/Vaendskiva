// Channel A: Direction D12; PWM D3; Brake D9 (Brake when HIGH); Current sensing A0
// Channel B: Direction D13; PWM D11; Brake D8 (Brake when HIGH); Current sensing A1




#include <Arduino.h>

int fadePin[] = {3, 11};                 // Pins used for PWM
int dirPin[] = {12, 13, 9, 8};
static const uint8_t inPin[] = {A2, A3, A4, A5};
boolean isDirOld[] ={1, 1}, isDir[] = {1, 1}, isGo[] = {0, 0}; //isDir: 0-left 1-right
unsigned long nextFade[6];
int fadeBrightness[] = {0, 0};
char fadeDir[] = {1, 1};

//debounce 
int counter[] = {0, 0, 0, 0};       // how many times we have seen new value
boolean reading[] = {0, 0, 0, 0};   // the current value read from the input pin
long time = 0;                       // the last time the output pin was sampled
boolean current_state[] = {1, 1, 1, 1};            // the debounced input value
int debounce_count = 10;            // number of millis/samples to consider before declaring a debounced input

void fade(boolean isOn, boolean isBlink, int ledNr) {
    boolean action = true;
    unsigned long now;
    now = millis();
    if ((fadeBrightness[ledNr] > 254) && isOn)  {action = false;} 
    if ((fadeBrightness[ledNr] < 1) && !isOn)  {action = false;}
    if (isBlink)  {action = true;}
    if ((now >= nextFade[ledNr]) && action)  {
        nextFade[ledNr] = now + 1;
        if ((fadeDir[ledNr] > 0) && (fadeBrightness[ledNr] > 254)){
                fadeDir[ledNr] = - fadeDir[ledNr];
//                nextFade[ledNr] += 1000;
        } 
        else if ((fadeDir[ledNr] < 0) && (fadeBrightness[ledNr] < 1)) {
                fadeDir[ledNr] = - fadeDir[ledNr];
//                nextFade[ledNr] += 1000;
        }
        else {  fadeBrightness[ledNr] += fadeDir[ledNr];
        }
        analogWrite(fadePin[ledNr], fadeBrightness[ledNr]);
    }
}



void setup() {
    // put your setup code here, to run once:
    for (int i = 0; i < 2; i++) {
        pinMode(fadePin[i], OUTPUT);      // sets the digital pin as output
        digitalWrite(fadePin[i], LOW);
        nextFade[i] = millis() + 100 * i;
    };
    for (int i = 0; i < 4; i++) {
        pinMode(dirPin[i], OUTPUT);      // sets the digital pin as output
        digitalWrite(dirPin[i], LOW);
        nextFade[i] = millis() + 100 * i;
    };
    for (int i = 0; i < 4; i++) {
        pinMode(inPin[i], INPUT_PULLUP);
    };
        
}

void loop() {

    //Eingänge debouncen
    for (int i = 0; i < 4; i++) {
        if(millis() != time){  // If we have gone on to the next millisecond
            reading[i] = digitalRead(inPin[i]);
            if(reading[i] == current_state[i] && counter[i] > 0) {
                counter[i]--;
            }
            if(reading[i] != current_state[i]) {
                counter[i]++; 
            }
            // If the Input has shown the same value for long enough let's switch it
            if(counter[i] >= debounce_count) {
                counter[i] = 0;
                current_state[i] = reading[i];
            }
            time = millis();
        }
    }    
    //Ausgänge setzen
    for (int i = 0; i < 2; i++) {
        isGo[i] = !(current_state[2*i] && current_state[i+1]);
        isDir[i] = current_state[2*i];
        if (isDirOld[i] == isDir[i]) {
            fade(isGo[i], false, i);
        }
        else if (fadeBrightness[i] < 1) {
            isDirOld[i]=isDir[i];
            digitalWrite(dirPin[i], isDirOld[i]);
        }
        else {
            fade(false, false, i);
        };
    };
}