/***   Included libraries  ***/
#include <Servo.h>

/***   Global variables and function definition  ***/
uint8_t led = 8;
uint8_t boton = 4;
Servo ruedaDer;
Servo ruedaIzq;

/***   Setup  ***/
void setup() {
    pinMode(led, OUTPUT);
    pinMode(boton, INPUT);
    ruedaDer.attach(6);
    ruedaIzq.attach(9);
    digitalWrite(led, HIGH);
}

/***   Loop  ***/
void loop() {
    ruedaIzq.write(0);
    ruedaDer.write(180);
    if (digitalRead(boton) == 1) {
        ruedaIzq.write(90);
    }
    if (digitalRead(boton) == 1) {
        ruedaDer.write(90);
    }
}

