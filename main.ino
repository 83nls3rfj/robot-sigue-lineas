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
uint8_t rotateVelocity = 5;
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
 Serial.print("ahead(");
 Serial.print(vel);
 Serial.println(")");					   
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "ahead";
} 

void backwards(int vel){
 Serial.print("backwards(");	
 Serial.print(vel);
 Serial.println(")");	
 rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER);
 delay(100);
 lastMovement = "backwards";
} 

void rotateLeft(int vel, int degrees){ 
  Serial.print("rotateLeft(");
  Serial.print(vel);
  Serial.print(",");
  Serial.print(degrees);
  Serial.print(") -> ");
  /*if (degrees < 15){
    vel = vel * 0.7;
  }*/
  // Calcular el tiempo necesario para girar los grados deseados
  float circunferencia = 2 * PI * 3.2;
  float distancia_por_grado = circunferencia / 360.0;
  float distancia_total = distancia_por_grado * degrees;
  int tiempo_ms = (int) (1000.0 * distancia_total / 2.7);	// (1000mm * distancia_total / vel / corrección)				 
  rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ); 
  rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
  delay(tiempo_ms);
  Serial.println(tiempo_ms);
  lastMovement = "rotateLeft";
} 

void rotateRight(int vel, int degrees){ 
  Serial.print("rotateRight(");
  Serial.print(vel);
  Serial.print(",");
  Serial.print(degrees);
  Serial.print(") -> ");	
  /*if (degrees < 15){
    vel = vel * 0.7;
  }*/
  // Calcular el tiempo necesario para girar los grados deseados
  float circunferencia = 2 * PI * 3.2;
  float distancia_por_grado = circunferencia / 360.0;
  float distancia_total = distancia_por_grado * degrees;
  int tiempo_ms = (int) (1000.0 * distancia_total / 2.7); // (1000mm * distancia_total / vel / corrección)			 
  rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
  rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER); 
  delay(tiempo_ms);
  Serial.println(tiempo_ms);
  lastMovement = "rotateRight";
} 

void turnLeft(int vel, int rapidez) { 
 Serial.print("turnLeft(");
 Serial.print(vel);
 Serial.print(",");
 Serial.print(rapidez);
 Serial.println(")");							   
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "giroIzq";
}

void turnRight(int vel, int rapidez) { 
 Serial.print("turnRight(");
 Serial.print(vel);
 Serial.print(",");
 Serial.print(rapidez);
 Serial.println(")");						
 rueda_izq.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_IZQ); 
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
  Serial.print("readLineSensors(");
  int result = (!digitalRead(sensor_Izq) || !digitalRead(sensor_Der));
  Serial.print(result);
  Serial.println(")");
  return result;
}

// Función para buscar la línea en espiral
void searchForLineInSpiral() {
  Serial.println("searchForLineInSpiral()");

  const int initialDelay = 100;  // tiempo inicial en milisegundos para el primer movimiento en espiral
  int currentDelay = initialDelay;
  const int delayIncrement = 20;  // incremento de tiempo para expandir la espiral
  int timeout = 5;
  
  if(lastMovement == "giroDer" || lastMovement == "rotateRight") {
    while (!readLineSensors() && timeout > 0) {  // Continuar hasta que la línea sea detectada
      turnLeft(velocity, 5);
      delay(100);
      currentDelay =  currentDelay + delayIncrement;
      timeout--;
    }
  } else {
    while (!readLineSensors() && timeout > 0) {  // Continuar hasta que la línea sea detectada
      turnRight(velocity, 5);
      delay(100);
      currentDelay =  currentDelay + delayIncrement;
      timeout--;
    }
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
      Serial.println("Negro|Blanco -> Izquierda");
      turnLeft(velocity, 10);
  } else if (Izq < Der) { // Detecta blanco a la izquierda -> Giro derecha
      Serial.println("Blanco|Negro -> Derecha");
      turnRight(velocity, 10);
  } else if (Izq && Der) { // Si ambos sensores detectan blanco
    Serial.println("Blanco|Blanco -> Buscar");
    //if(lastMovement == "ahead") {
      Serial.println("posible bifurcación");
      /* Esta parte de la función debería comprobar si hay realmente una bifurcación o una discontinuidad del camino.
        Para esto, dado que el último movimiento tras la perdida de la líneael robot deberá retroceder */
      int leftWay = 0; // 0 -> no hay camino; 1 -> hay camino
      int rightWay = 0; // 0 -> no hay camino; 1 -> hay camino
      /*while(!readLineSensors() || !timeout){
        backwards(velocity);
        timeout--;
      }*/
      //backwards(velocity);
      rotateLeft(velocity, 40);
      if(readLineSensors()){
        Serial.println("Hay camino por la izquierda");
        leftWay = 1;
      }
      rotateRight(velocity, 80);
      if(readLineSensors()){
        Serial.println("Hay camino por la derecha");
        rightWay = 1;
      }
      rotateLeft(velocity, 40);

      if(leftWay && rightWay) { // Hay bifurcación?
        Serial.println("Hay bifurcación");
        if(obstruction == 2) {
          Serial.println("Hay pared -> Derecha");
          do{
            turnRight(velocity, 10);
            delay(1000);
            timeout--;
          } while(!readLineSensors() || !timeout);
        }else{
          Serial.println("No hay pared -> Izquierda");
          do{
            turnLeft(velocity, 10);
            delay(1000);
            timeout--;
          } while(!readLineSensors() || !timeout);
        }
      } else {  // Si no hay bifurcación 
        if(leftWay){ // Hay camino por la izquierda?
          Serial.println("Sigue el camino de la izquierda");
          do{
            turnLeft(velocity, 10);
            delay(1000);
            timeout--;
          } while(!readLineSensors() || !timeout);
        }else if(rightWay) { // Hay camino por la derecha?
          Serial.println("Sigue el camino de la derecha");
          do{
            turnRight(velocity, 10);
            delay(1000);
            timeout--;
          } while(!readLineSensors() || !timeout);
        } else { // No hay camino
          Serial.println("Callejon sin salida");
          searchForLineInSpiral();
        }
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
        // TODO Falta la lógica para resolver el laberinto
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
  if (button) {
    if (estado == 0){
      estado = 1;
    } else {
      estado = 0;
    }
  }

  if (estado == 1) {
    //detectarYEsquivarobstructions(); //Añadir esta línea para detectar y esquivar obstáculos
    followLine();
    //readLineSensors();
    //backwards(velocity);
    //movementTestSuit();
    //searchForLineInSpiral();
    //ahead(velocity);
    //turnLeft(velocity, 5);
    //turnRight(velocity, 5);
    //rotateLeft(velocity, 90);
    //rotateRight(velocity, 90);
    //estado = 0;
  } else if (estado == 0) {
    stop();
  }
}
