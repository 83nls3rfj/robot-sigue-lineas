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
    if (estado == 1) {
        ruedaIzq.write(0);
        ruedaDer.write(180);
    }
    else if (estado == 0) {
        ruedaIzq.write(90);
        ruedaDer.write(90);
    }
    if (digitalRead(boton) == true) {
        estado = 1;
    }
}

