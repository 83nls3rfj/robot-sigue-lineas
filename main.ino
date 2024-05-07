/***   Included libraries  ***/
#include <Servo.h>
#include <BitbloqUS.h>
#include <BitbloqLineFollower.h>

/***   Global variables and function definition  ***/
uint8_t led = 8;
uint8_t boton = 4;
Servo servocontinuo de;
Servo servocontinuo izq;
US ultrasonidos(5, 7);
LineFollower siguelineas(2, 3);
uint8_t VELOCIDAD_PARO = 90;
uint8_t VELOCIDAD_ERROR_IZQ = 90;
uint8_t VELOCIDAD_ERROR_DER = 90;
uint8_t vel = 10
uint8_t sensor_Izq = 2;
uint8_t sensor_Der = 3;
float Der = 0;
float Izq = 0;

/*** FUNCIONES DE MOVIMIENTO ***/
void adelante(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
} 

void atras(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER); 
} 

void rota_izquierda(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
} 

void rota_derecha(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER); 
} 


/***   Setup  ***/
void setup() {
    pinMode(led, OUTPUT);
    pinMode(boton, INPUT);
    servocontinuo der.attach(6);
    servocontinuo izq.attach(9);
    float estado = 0;
}

void detectarLinea (){
Der = digitalRead(sensor_Der);
    Izq = digitalRead(boton);
    if (Der > Izq) {
        servo_Izq.write(0);
        servo_Der.write(90);
    }
    if (Izq > Der) {
        servo_Der.write(180);
        servo_Izq.write(90);
    }
    else {
        servo_Der.write(180);
        servo_Izq.write(0);
    }
}

/***   Loop  ***/
void loop() {
    if (estado == 1) {
        servocontinuo izq.write(0);
        servocontinuo der.write(180 + 50);
    }
    else if (estado == 0) {
        servocontinuo izq.write(90);
        servocontinuo der.write(90);
    }
    if (digitalRead(boton) == true) {
        if (estado == 0){
          estado = 1;
        } else {
          estado = 0;
        }
    }
}
