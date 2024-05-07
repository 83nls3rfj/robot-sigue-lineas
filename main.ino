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
uint8_t VELOCIDAD_ERROR_IZQ = 0;
uint8_t VELOCIDAD_ERROR_DER = 5;
uint8_t velocity = 5;
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
 delay(100);
 lastMovement = "ahead";
} 

void backwards(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER);
 delay(100);
 lastMovement = "backwards"; 
} 

void rotateLeft(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "rotateLeft";
} 

void rotateRight(int vel){ 
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "rotateRight";
} 

void turnLeft(int vel, int rapidez) { 
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_DER); 
 //delay(200);
 lastMovement = "giroIzq";
}

void turnRight(int vel, int rapidez) { 
 rueda_izq.write(VELOCIDAD_PARO - vel - rapidez - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 //delay(200);
 lastMovement = "giroDer";
}

void stop() { 
 rueda_izq.write(90); 
 rueda_der.write(90); 
}

/***   Setup  ***/
void setup() {
    Serial.begin(9600);
    pinMode(led, OUTPUT);
    pinMode(boton, INPUT);
    rueda_der.attach(6);
    rueda_izq.attach(9);
    estado = 0;

}

bool readLineSensors() {
    // Código para leer los sensores de línea
    return (!digitalRead(sensor_Izq) || !digitalRead(sensor_Der));  
}

// Función para buscar la línea en espiral
void searchForLineInSpiral() {
    const int initialDelay = 100;  // tiempo inicial en milisegundos para el primer movimiento en espiral
    int currentDelay = initialDelay;
    const int delayIncrement = 20;  // incremento de tiempo para expandir la espiral

    while (!readLineSensors()) {  // Continuar hasta que la línea sea detectada
        ahead(velocity);
        delay(currentDelay);  // Moverse hacia adelante por un tiempo determinado
        stop();

        turnLeft(velocity, 5);
        delay(100);  // Tiempo para un pequeño giro a la izquierda

        // Incrementa el tiempo de avance para expandir la espiral
        currentDelay += delayIncrement;
    }
    stop();  // Detener el robot cuando la línea se encuentra
}

void followLine (){
    Der = digitalRead(sensor_Der);
    Izq = digitalRead(sensor_Izq);
    
    if (!Izq && !Der) { // Si ambos sensores detectan negro
      Serial.println("Negro|Negro -> Recto");
      ahead(velocity);
    } else if (Izq > Der) { // Detecta blanco a la izquierda -> Giro Der.
        Serial.println("Blanco|Negro -> Izquierda");
        turnRight(velocity, 5);
    } else if (Izq < Der) { // Sensor derecho detecta blanco e izquierdo detecta negro -> Giro Izq.
        Serial.println("Negro|Blanco -> Derecha");
        turnLeft(velocity, 5);
    } else if (Izq && Der) { // Si ambos sensores detectan blanco
      Serial.println("Blanco|Blanco -> Buscar");
      if(lastMovement == "giroDer") {
        Serial.println("por la Derecha");
        while(!readLineSensors()){
          rotateLeft(velocity);
          delay(100);
        }
      } else if (lastMovement == "giroIzq") {
        Serial.println("por la Izquierda");
        while(!readLineSensors()){
          rotateRight(velocity);
          delay(100);
        }
      } else if(lastMovement == "ahead") {
        backwards(velocity);
        delay(100);
      } else {
        searchForLineInSpiral();
      }
      
  }
}
/*** FUNCIONES DE DETECCIÓN Y ESQUIVE DE OBSTÁCULOS ***/
void detectarYEsquivarObstaculos() {
  int distancia = ultrasonidos.read(); // Obtener la distancia del objeto más cercano
  
  if (distancia < 10) { // Si la distancia es menor a 10 cm, consideramos que hay un obstáculo
    digitalWrite(led, HIGH); // Encender el LED
    stop(); // Detener el robot
    backwards(10); // Retroceder un poco
    delay(500); // Esperar medio segundo
    rotateLeft(15);
    /*if (random(2) == 0) { // Elegir una dirección aleatoria para girar
      rotateLeft(10);
    } else {
      rotateRight(10);
    }*/
    delay(500); // Esperar medio segundo antes de seguir avanzando
    digitalWrite(led, LOW); // Apagar el LED
  }
}

/**
  Función para probar todas las funciones de movimiento
**/
void movementTestSuit(){
  ahead(velocity);
  delay(1200);
  turnLeft(velocity, 5);
  delay(1200);
  turnRight(velocity, 5);
  delay(1200);
  rotateLeft(velocity);
  delay(1200);
  rotateRight(velocity);
  delay(1200);
  backwards(velocity);
  delay(1200);
}

/***   Loop  ***/
void loop() {
  bool button = digitalRead(boton);
  delay(100);  // delay in between reads for stability
  //Serial.println(button);
  if (button) {
    if (estado == 0){
      estado = 1;
    } else {
      estado = 0;
    }
  }
  if (estado == 1) {
      //detectarYEsquivarObstaculos(); //Añadir esta línea para detectar y esquivar obstáculos
      followLine();
      //movementTestSuit();
      //searchForLineInSpiral();
  } else if (estado == 0) {
      stop();
  }
  //Serial.println(estado);
}
