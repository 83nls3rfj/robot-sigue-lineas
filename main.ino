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
uint8_t VELOCIDAD_ERROR_DER = 2;
uint8_t sensor_Izq = 2;
uint8_t sensor_Der = 3;
int estado = 0;
String lastMovement = "init";
uint8_t velocity = 10;
uint8_t distanceMin = 23;
int obstruction = 0; // 0 - No hay obstaculo, 1 - Obstaculo, 2 - Wall

/***  Setup  ***/
void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(boton, INPUT);
  rueda_der.attach(6);
  rueda_izq.attach(9);
  estado = 0;
}

/*** FUNCIONES DE MOVIMIENTO ***/
void ahead(int vel){ 
 Serial.println("ahead()");						   
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "ahead";
} 

void backwards(int vel){
 Serial.println("backwards()");	
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER);
 delay(100);
 lastMovement = "backwards";
} 

void rotateLeft(int vel, int degrees){ 
  Serial.println("rotateLeft()");
  // Calcular el tiempo necesario para girar los grados deseados
  float circunferencia = 2 * PI * 3.2;
  float distancia_por_grado = circunferencia / 360.0;
  float distancia_total = distancia_por_grado * degrees;
  int tiempo_ms = (int) (1000.0 * distancia_total / 2.66);						 
  rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
  rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
  delay(tiempo_ms);
  lastMovement = "rotateLeft";
} 

void rotateRight(int vel, int degrees){ 
  Serial.println("rotateRight()");
  // Calcular el tiempo necesario para girar los grados deseados
  float circunferencia = 2 * PI * 3.2;
  float distancia_por_grado = circunferencia / 360.0;
  float distancia_total = distancia_por_grado * degrees;
  int tiempo_ms = (int) (1000.0 * distancia_total / 2.66); // (1000 * distancia_total / vel / 100)			 
  rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
  rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER); 
  delay(tiempo_ms);
  lastMovement = "rotateRight";
} 

void turnLeft(int vel, int rapidez) { 
 Serial.println("turnLeft()");							   
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "giroIzq";
}

void turnRight(int vel, int rapidez) { 
 Serial.println("turnRight()");								
 rueda_izq.write(VELOCIDAD_PARO - vel - rapidez - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "giroDer";
}

void stop() { 
 rueda_izq.write(VELOCIDAD_PARO); 
 rueda_der.write(VELOCIDAD_PARO);
}

// Función para leer los sensores de línea
bool readLineSensors() {
  Serial.println("readLineSensors()");

  return (!digitalRead(sensor_Izq) || !digitalRead(sensor_Der));
}

// Función para buscar la línea en espiral
void searchForLineInSpiral() {
  Serial.println("searchForLineInSpiral()");

  const int initialDelay = 100;  // tiempo inicial en milisegundos para el primer movimiento en espiral
  int currentDelay = initialDelay;
  const int delayIncrement = 20;  // incremento de tiempo para expandir la espiral
  int timeout = 5;

  while (!readLineSensors() || !timeout) {  // Continuar hasta que la línea sea detectada
    if(lastMovement == "giroDer" || lastMovement == "rotateRight") {
      turnLeft(velocity, 5);
    } else {
      turnRight(velocity, 5);
    }
    delay(100);  // Tiempo para un pequeño giro a la izquierda
    // Incrementa el tiempo de avance para expandir la espiral
    currentDelay =  currentDelay + delayIncrement;
    timeout--;
  }
  stop();  // Detener el robot cuando la línea se encuentra
}

// Función para seguir la linea del circuito
void followLine (){
  //Serial.println("followLine()");

  float Der = digitalRead(sensor_Der);
  float Izq = digitalRead(sensor_Izq);
  int timeout = 5;
  
  if (!Izq && !Der) { // Si ambos sensores detectan negro
    Serial.println("Negro|Negro -> Recto");
    ahead(velocity);
  } else if (Izq > Der) { // Detecta blanco a la derecha -> Giro izquierda.
      Serial.println("Blanco|Negro -> Izquierda");
      rotateLeft(velocity, 1);
  } else if (Izq < Der) { // Detecta blanco a la izquierda -> Giro derecha
      Serial.println("Negro|Blanco -> Derecha");
      rotateRight(velocity, 1);
  } else if (Izq && Der) { // Si ambos sensores detectan blanco
    Serial.println("Blanco|Blanco -> Buscar");
    if(lastMovement == "giroDer" || lastMovement == "rotateRight") {
      Serial.println("por la izquierda");
      while(!readLineSensors() || !timeout){
        rotateLeft(velocity, 1);
        timeout--;
      }
    } else if (lastMovement == "giroIzq" || lastMovement == "rotateLeft") {
      Serial.println("por la derecha");
      while(!readLineSensors() || !timeout){
        rotateRight(velocity, 1);
        timeout--;
      }
    } else if(lastMovement == "ahead") {
      Serial.println("posible bifurcación");
      /* Esta parte de la función debería comprobar si hay realmente una bifurcación o una discontinuidad del camino.
        Para esto, dado que el último movimiento tras la perdida de la líneael robot deberá retroceder */

      if(obstruction == 2){
        while(!readLineSensors() || !timeout){
          rotateRight(velocity, 1);
          timeout--;
        }
      }else{
        while(!readLineSensors() || !timeout){
          rotateLeft(velocity, 1);
          timeout--;
        }
      }
    } else {
      searchForLineInSpiral();
    }
  }
}

/*** FUNCIONES DE DETECCIÓN Y ESQUIVE DE OBSTÁCULOS ***/
void detectarYEsquivarobstructions() {
  if(obstruction != 1) {
    Serial.println("detectarYEsquivarobstructions()");
    int distancia = ultrasonidos.read(); // Obtener la distancia del objeto más cercano
    int leftDistance = -1;
    int rightDistance = -1;

    Serial.print("Distancia: ");
    Serial.println(distancia);
    if (distancia < distanceMin ) { // Si la distancia es menor a 22 cm, consideramos que hay un obstáculo
      digitalWrite(led, HIGH); // Encender el LED
      stop(); // Detener el robot
      delay(1000);
      rotateLeft(velocity, 30); 
      stop(); // Detener el robot
      delay(1000);
      leftDistance = ultrasonidos.read();
      delay(100);
      rotateRight(velocity, 60);
      stop(); // Detener el robot
      delay(1000);
      rightDistance = ultrasonidos.read();
      delay(100);
      rotateLeft(velocity, 50); 
      stop(); // Detener el robot
      delay(1000);
      Serial.print("Medición: ");
      Serial.print(leftDistance);
      Serial.print("cm izq. ");
      Serial.print(rightDistance);
      Serial.println("cm der.");
      if(leftDistance > 30 && rightDistance > 30){
        Serial.println("obstruction! sigue adelante!");// Es un obstruction
        obstruction = 1; // Seguir la línea empujando el obstaculo hasta que el obstruction se quede fuera del circuito
      } else {
        Serial.println("Pared!");// Es una pared
        obstruction = 2;
        rotateRight(velocity, 90);
        // Lógica para resolver el laberinto
        Serial.print(distancia);
        if(distancia < distanceMin){
          stop();
          delay(1000);
          rotateLeft(velocity, 180);
          delay(1000);
        }
      }
      digitalWrite(led, LOW); // Apagar el LED
    }
  } else if(ultrasonidos.read() > 30 && obstruction == 1) {
    obstruction = 0;
  }
  
}

/**
  Función para probar todas las funciones de movimiento
**/
void movementTestSuit(){
  Serial.println("movementTestSuit()");
  ahead(velocity);
  delay(3200);
  turnLeft(velocity, 5);
  delay(1200);
  turnRight(velocity, 5);
  delay(1200);
  rotateLeft(velocity, 90);
  rotateRight(velocity, 90);
  backwards(velocity);
  delay(2200);
}

/***   Loop  ***/
void loop() {
  bool button = digitalRead(boton);
  delay(100);  // delay in between reads for stability
  //Serial.print("Button: ");
  //Serial.println(button);
  if (button) {
    if (estado == 0){
      estado = 1;
    } else {
      estado = 0;
    }
  }

  if (estado == 1) {
    detectarYEsquivarobstructions(); //Añadir esta línea para detectar y esquivar obstáculos
    followLine();
    //movementTestSuit();
    //searchForLineInSpiral();
    //ahead(velocity);
  } else if (estado == 0) {
    stop();
  }
}
