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
uint8_t VELOCIDAD_ERROR_DER = 1;
uint8_t sensor_Izq = 2;
uint8_t sensor_Der = 3;
int estado = 0;
String lastMovement = "init";
uint8_t velocity = 10;
uint8_t rotateVelocity = 5;
uint8_t distanceMin = 23;
int obstruction = 0; // 0 - No hay obstaculo, 1 - Obstaculo, 2 - Wall
int degreesLimit = 110; // Máximo número de grados que se buscará un desvio

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
 rueda_der.write(VELOCIDAD_PARO - vel );
 delay(100);
 lastMovement = "backwards";
} 

void rotateLeft(int vel, int degrees){ 
  Serial.print("rotateLeft(");
  Serial.print(vel);
  Serial.print(",");
  Serial.print(degrees);
  Serial.print(") -> ");
  // Calcular el tiempo necesario para girar los grados deseados
  float circunferencia = 2 * PI * 3.2;
  float distancia_por_grado = circunferencia / 360.0;
  float distancia_total = distancia_por_grado * degrees;
  int tiempo_ms = (int) (1000.0 * distancia_total / 4.4);	// (1000mm * distancia_total / vel / corrección)				 
  rueda_izq.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_IZQ - 2); 
  rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER + 5); 
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
  // Calcular el tiempo necesario para girar los grados deseados
  float circunferencia = 2 * PI * 3.2;
  float distancia_por_grado = circunferencia / 360.0;
  float distancia_total = distancia_por_grado * degrees;
  int tiempo_ms = (int) (1000.0 * distancia_total / 4.5); // (1000mm * distancia_total / vel / corrección)			 
  rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ + 7); 
  rueda_der.write(VELOCIDAD_PARO - vel - VELOCIDAD_ERROR_DER - 2); 
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
 rueda_izq.write(VELOCIDAD_PARO); 
 rueda_der.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "turnLeft";
}

void turnRight(int vel, int rapidez) { 
 Serial.print("turnRight(");
 Serial.print(vel);
 Serial.print(",");
 Serial.print(rapidez);
 Serial.println(")");						
 rueda_izq.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO); 
 delay(100);
 lastMovement = "turnRight";
}

void goLeft(int vel, int rapidez) { 
 Serial.print("turnLeft(");
 Serial.print(vel);
 Serial.print(",");
 Serial.print(rapidez);
 Serial.println(")");							   
 rueda_izq.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "turnLeft";
}

void goRight(int vel, int rapidez) { 
 Serial.print("turnRight(");
 Serial.print(vel);
 Serial.print(",");
 Serial.print(rapidez);
 Serial.println(")");						
 rueda_izq.write(VELOCIDAD_PARO + vel + rapidez + VELOCIDAD_ERROR_IZQ); 
 rueda_der.write(VELOCIDAD_PARO + vel + VELOCIDAD_ERROR_DER); 
 delay(100);
 lastMovement = "turnRight";
}

void stop() { 
 rueda_izq.write(VELOCIDAD_PARO); 
 rueda_der.write(VELOCIDAD_PARO);
}

// Función para leer los sensores de línea
int readLineSensors() {
  Serial.print("readLineSensors(");
  int result = (!((int) digitalRead(sensor_Izq)) || !((int) digitalRead(sensor_Der)));
  Serial.print(result);
  Serial.println(")");
  return result;
}

/*** Función para buscar la línea en espiral ***/
void searchForLineInSpiral(String compass) {
  Serial.println("searchForLineInSpiral()");
  if(compass == "right") {
    while (!readLineSensors()) {  // Continuar hasta que la línea sea detectada
      rueda_izq.write(VELOCIDAD_PARO + velocity + VELOCIDAD_ERROR_IZQ); 
      rueda_der.write(VELOCIDAD_PARO + VELOCIDAD_ERROR_DER);
    }
  } else {
    while (!readLineSensors()) {  // Continuar hasta que la línea sea detectada
      rueda_izq.write(VELOCIDAD_PARO + VELOCIDAD_ERROR_IZQ); 
      rueda_der.write(VELOCIDAD_PARO + velocity + VELOCIDAD_ERROR_DER);
    }
  }
  stop();  // Detener el robot cuando la línea se encuentra
}

/*** Busca la línea buscando a derecha (direction = "right") o izquierda (direction = "left") ***/
int searchWay(String direction){
  Serial.print("seachWay(");
  Serial.print(direction);
  Serial.println(")");
  int degrees = 0;
  int limit = degreesLimit;
  bool initialLeftSensor = !digitalRead(sensor_Der);
  bool initialRightSensor = !digitalRead(sensor_Der);
  if(direction == "left"){ // Busca hasta encontrar la linea hasta un máximo de degreesLimitº
    do{ 
      rotateLeft(velocity, 5);
      limit -= 5;
    //} while((initialLeftSensor != !digitalRead(sensor_Izq) || initialRightSensor != !digitalRead(sensor_Der)) && limit > 0);
    } while(!readLineSensors() && limit > 0);
  } else if ("right"){
    do{ 
      rotateRight(velocity, 5);
      limit -= 5;
    //} while((initialLeftSensor != !digitalRead(sensor_Izq) || initialRightSensor != !digitalRead(sensor_Der)) && limit > 0);
    } while(!readLineSensors() && limit > 0);
  }
  degrees = (degreesLimit - limit); // Calcula los grados hasta encontrar la línea
  if(direction == "left"){ // Deshace la rotación para volver a la posición inicial
    rotateRight(velocity, degrees);
  } else if(direction == "right"){
    rotateLeft(velocity, degrees);
  }
  Serial.print("seachWay(");
  Serial.print(direction);
  Serial.print(") -> ");
  Serial.println(degrees);
  return degrees;
}

// Desace el último movimiento y se intenta el contrario, si tras esto, el robot recupera la línea,
void undoLastMovement(){
    if(lastMovement == "turnRight" || lastMovement == "rotateRight") {
        turnLeft(velocity, 5); // Se deshace el movimiento x2
        backwards(velocity); // Retrocede un poco para compensar el giro
        delay(200);
    } else if(lastMovement == "turnLeft" || lastMovement == "rotateLeft") {
        turnRight(velocity, 5); // Se deshace el movimiento x2
        backwards(velocity); // Retrocede un poco para compensar el giro
        delay(200);
    }else if(lastMovement == "ahead"){
      backwards(velocity); // Se deshace el movimiento x2
      delay(300);
    }
}

/*** Función para seguir la linea del circuito ***/
void followLine (){
  float der = (int) digitalRead(sensor_Der);
  float izq = (int) digitalRead(sensor_Izq);
  
  if (!izq && !der) { // Si ambos sensores detectan negro
    Serial.println("Negro|Negro -> Recto");
    ahead(velocity);
  } else if(izq > der) { // Detecta blanco a la derecha -> Giro izquierda.
      Serial.println("Negro|Blanco -> Izquierda");
      turnLeft(velocity, 5);
  } else if(izq < der) { // Detecta blanco a la izquierda -> Giro derecha
      Serial.println("Blanco|Negro -> Derecha");
      turnRight(velocity, 5);
  } else if(izq && der) { // Si ambos sensores detectan blanco
    Serial.println("Blanco|Blanco -> Buscar");
    Serial.println("Posible bifurcación!");

    //undoLastMovement();
    ahead(velocity);
    delay(200);

    // Leemos el estado de los sensores
    izq = (int) digitalRead(sensor_Izq);
    der = (int) digitalRead(sensor_Der);
    Serial.print("Sensor Izquierdo: ");
    Serial.println(izq);
    Serial.print("Sensor Derecho: ");
    Serial.println(der);
    if(izq == der && izq){ // Si tras desacer el último movimiento ambos sensores están en la línea, se busca una posible bifurcación
      Serial.println("Fuera de la línea");
      /* Esta parte de la función debería comprobar si hay realmente una bifurcación o una discontinuidad del camino.
        Para esto, dado que el último movimiento tras la perdida de la líneael robot deberá retroceder */
      int leftDegrees = searchWay("left");
      int rightDegrees = searchWay("right");   
      int errorRange = 20;
      ahead(velocity);
      delay(200);
      /*rotateLeft(velocity, 45);
      int maxiLeft = readLineSensors();
      rotateRight(velocity, 90);
      int maxiRight = readLineSensors();
      rotateLeft (velocity, 45);*/
      
      if(leftDegrees > 5 && leftDegrees < degreesLimit && rightDegrees > 5 && rightDegrees < degreesLimit) { // Hay bifurcación? camino a izquierda y derecha a menos de degreesLimit
        izq = (int) digitalRead(sensor_Izq);
        der = (int) digitalRead(sensor_Der); 
        if(abs(leftDegrees - rightDegrees) > errorRange) { 
          Serial.println("Curva peligrosa!");
          // Guarda el estado actual de los sensores 
          if(leftDegrees > rightDegrees){ // Si el recorrido mas largo está a la izquierda lo seguirá
            Serial.println("Hacia la izquierda");
            do{
              ahead(velocity);
            } while(readLineSensors());
            searchForLineInSpiral("left");
            /*do{
              turnLeft(velocity, 5);
            } while(digitalRead(sensor_Izq) == digitalRead(sensor_Der) && digitalRead(sensor_Izq) == 0);*/
          } else { // Si el recorrido mas largo está a la derecha lo seguirá
            Serial.println("Hacia la derecha");
            do{
              ahead(velocity);
            } while(readLineSensors());
            searchForLineInSpiral("right");
            /*do{
              goRight(velocity, 5);
            } while(digitalRead(sensor_Izq) == digitalRead(sensor_Der) && digitalRead(sensor_Izq) == 0);*/
          }
        } else {
          Serial.println("Hay bifurcación!");
          if(obstruction == 2) { // Hay pared?
            Serial.println("Hay pared -> Derecha");
            turnLeft(velocity, 5);
            /*do{
              turnRight(velocity, 5);
            } while((izq != digitalRead(sensor_Izq) || der != digitalRead(sensor_Der)));*/
          } else { // Si no hay pared, girará a la izquierda segun normas del concurso
            Serial.println("No hay pared -> Izquierda");
            turnLeft(velocity, 5);
            /*do{
              turnLeft(velocity, 5);
            } while((izq != digitalRead(sensor_Izq) || der != digitalRead(sensor_Der)));*/
          }
        }
      } else {  // Si no hay bifurcación 
        Serial.println("No hay bifurcación");
        if(leftDegrees < degreesLimit){ // Hay camino por la izquierda?
          Serial.println("Sigue el camino por la izquierda");
          //turnLeft(velocity, 5);
          /*do{
            turnLeft(velocity, 5);
          } while(!readLineSensors());*/
          do{
            ahead(velocity);
          } while(readLineSensors());
          searchForLineInSpiral("left");
        }else if(rightDegrees < degreesLimit) { // Hay camino por la derecha?
          Serial.println("Sigue el camino por la derecha");
          //turnLeft(velocity, 5);
          /*do{
            turnRight(velocity, rightDegrees);
          } while(!readLineSensors());*/
          do{
            ahead(velocity);
          } while(readLineSensors());
          searchForLineInSpiral("right");
        } else { // No hay camino! rota 180º y media vuelta
          Serial.println("Callejon sin salida! media vuelta");
          rotateRight(velocity, 45);
          do {
            rotateRight(velocity, 10);
          } while(!readLineSensors());
        }
      }
    } else if(izq == der && izq){ // Ambos sensores están fuera de linea, buscar en espiral
      Serial.println("Callejon sin salida! media vuelta");
      rotateRight(velocity, 45);
      do {
        turnRight(velocity, 10);
      } while(!readLineSensors());
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
      rotateLeft(velocity, 30); 
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

/***  Main Loop  ***/
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

  if (estado == 1) { // Estado que permite controlar el arranque y parado del robot mediante su boton físico
    //detectarYEsquivarobstructions(); //Añadir esta línea para detectar y esquivar obstáculos
    followLine();
    //readLineSensors();
    //searchForLineInSpiral("right");
    //searchWay("left");
    //movementTestSuit();
    //ahead(velocity);
    //backwards(velocity);
    //turnRight(velocity, 5);
    //turnLeft(velocity, 5);
    //turnRight(velocity, 5);
    //rotateLeft(velocity, 180);
    //rotateRight(velocity, 180);
    //estado = 0; /* Descomentar para hacer debug */
  } else if (estado == 0) {
    stop();
  }
}
