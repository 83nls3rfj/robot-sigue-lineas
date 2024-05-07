/***   Included libraries  ***/
#include <Servo.h>
#include <BitbloqUS.h>
#include <BitbloqLineFollower.h>

/***   Global variables and function definition  ***/
uint8_t led = 8;
uint8_t boton = 4;
Servo rueda_der;
Servo rueda_izq;
US ultrasonidos(5, 7);
LineFollower siguelineas(2, 3);
uint8_t VELOCIDAD_PARO = 90;
uint8_t VELOCIDAD_ERROR_IZQ = 90;
uint8_t VELOCIDAD_ERROR_DER = 90;
uint8_t vel = 10;
uint8_t sensor_Izq = 2;
uint8_t sensor_Der = 3;
float Der = 0;
float Izq = 0;
float estado = 0;
String lastMovement = "stop";

/*** FUNCIONES DE MOVIMIENTO ***/
void ahead(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 lastMovement = "ahead";
} 

void backwards(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER);
 lastMovement = "backwards"; 
} 

void rotateLeft(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 lastMovement = "rotateLeft";
} 

void rotateRight(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER); 
 lastMovement = "rotateRight";
} 

void turnLeft(int vel, int rapidez) { 
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_DER); 
 lastMovement = "giroIzq";
}

void turnRight(int vel, int rapidez) { 
 rueda_izq.write(VELOCIDAD_PARO - vel - rapidez - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 lastMovement = "giroDer";
}

void stop() { 
 rueda_izq.write(90); 
 rueda_der.write(90); 
}

/***   Setup  ***/
void setup() {
    pinMode(led, OUTPUT);
    pinMode(boton, INPUT);
    rueda_der.attach(6);
    rueda_izq.attach(9);
    estado = 0;
}

void detectLine (){
    Der = digitalRead(sensor_Der);
    Izq = digitalRead(sensor_Izq);
    
    if ((Der == true) && (Izq == true)) { // Si ambos sensores detectan negro
      ahead(10);
    } else if (Izq > Der) { // giro izq.
        turnLeft(10, 5);
    } else if (Izq < Der) { // Sensor derecho detecta blanco e izquierdo detecta negro
        turnRight(10, 5);
    } else if ((Der == false) && (Izq == false)) { // Si ambos sensores detectan blanco
      if(lastMovement == "giroDer") {
        rotateLeft(10);
      } else if (lastMovement == "giroIzq") {
        rotateRight(10);
      } else if (lastMovement == "ahead") {
        rotateRight(10);
    }
}

/***   Loop  ***/
void loop (){
    if (estado == 1) { 
        detectLine();
    }
    else if (estado == 0) {
        stop();
    }
    if (digitalRead(boton) == true) {
        if (estado == 0){
          estado = 1;
        } else {
          estado = 0;
        }
    }
 }
