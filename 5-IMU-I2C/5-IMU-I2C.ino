#include <MPU9250_asukiaaa.h>

//LED:
const int ledPIN = 17;

// IMU:
#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;
float offsetX, offsetY, offsetZ, modulo2;
float aXtotal = 0.0, aYtotal = 0.0, aZtotal = 0.0;

//INTERRUPCION:
//Variable para detectar interrupción, y contador:
volatile int interruptCounter;
int cont = 0;

//Creamos un puntero que represente a un timer 
hw_timer_t * timer = NULL;

//Creamos una variable de tipo portMUX_type que nos permita sincronizar la variable volatile
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//Interrupcion:
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux); 
}

void setup() {
  Serial.begin(115200);
  //while(!Serial);
  delay(2000); //Para que le de tiempo al puerto serie a abrirse

  // SET-UP del IMU:
  #ifdef _ESP32_HAL_I2C_H_ // For ESP32
    Wire.begin(SDA_PIN, SCL_PIN);
    mySensor.setWire(&Wire);
  #endif

  mySensor.beginAccel();

  while (!Serial.available()){
  }
  
  Serial.println("");
  Serial.println("---------------- CALIBRACION -----------------");
  Serial.flush();
  Serial.println("Para calibrar mandar algo por el Puerto Serie:");
  Serial.println("IMPORTANTE!! Debe estar en reposo");
  Serial.println("");
  Serial.flush();

  while (!Serial.available()){
  }
  

  if (mySensor.accelUpdate() == 0) {
    offsetX = mySensor.accelX();
    offsetY = mySensor.accelY();
    aZ = mySensor.accelZ();
  } else {
    Serial.println("Cannot read accel values");
  }

  //En la siguiente líne asumimos que el oofset en el eje Z NO es suficiente como para cambiar el signo en la medición:
  if (aZ > 0){
    offsetZ = aZ - 1.0;
  }
  else {
    offsetZ = aZ + 1.0;
  }
  
  Serial.println("-------------- Sensor calibrado --------------");

  // SET-UP del timer:
  timer = timerBegin(0, 80, true); //Por orden: asociamos el timer al canal 0, prescaler de 80 -> freq=80/80=1 MHz, y el conteo es en sentido ascendente.
  timerAttachInterrupt(timer, &onTimer, true); //Asignamos el timer a la interrip. onTimer, y la interrupción se genera tras producirse el flanco de timer correspondiente. 
  timerAlarmWrite(timer, 100000, true); //Interrupcion = 200.000 conteos (100 ms) e indicamos que cada vez que llegue a ese valor, el conteo se renueve.
  timerAlarmEnable(timer); //activamos la interrupción.

  // SET-UP del LED:
  pinMode(ledPIN , OUTPUT);
  
}

void loop() {

  //LEEMOS sensor si han pasado 200 ms:
  if (interruptCounter > 0) {

    //Inicializamos de nuevo la variable, para detectar de nuevo la itnerrupción en el if
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    //Actualizamos el contador (para ver si hemos llegado al segundo):
    ++cont;
    
    //Leemos address del sensor, en caso de que quisieramos lo podemos sacar tambien por la UART:
    /*
    uint8_t sensorId;
    if (mySensor.readId(&sensorId) == 0) {
      Serial.println("sensorId: " + String(sensorId));
    } 
    else {
      Serial.println("Cannot read sensorId");
    }*/

    //Leemos aceleraciones y actualizamos los sumatorios:
    if (mySensor.accelUpdate() == 0) {
      aX = mySensor.accelX(); 
      aXtotal = aXtotal + aX - offsetX;
      
      aY = mySensor.accelY(); 
      aYtotal = aYtotal + aY - offsetY;
      
      aZ = mySensor.accelZ(); 
      aZtotal = aZtotal + aZ - offsetZ;
      //aSqrt = mySensor.accelSqrt();
    } 
    else {
      Serial.println("Cannot read accel values");
    }    

    if (cont == 1){
      digitalWrite(ledPIN , LOW);
    }
    
    //Si ha pasado 1 segundo, enviamos la media de las acelarciones durante ese segundo
    if (cont==10) {
      Serial.println("acel_X: " + String(aXtotal/10));
      Serial.println("acel_Y: " + String(aYtotal/10));
      Serial.println("acel_Z: " + String(aZtotal/10));
      
      //Calculamos tambien el modulo de la aceleracion:      
      modulo2 = aXtotal*aXtotal + aYtotal*aYtotal, aZtotal*aZtotal;
      Serial.println("Modulo de la aceleracion medio: " + String(sqrt(modulo2)));

      //encendemos el LED:
      digitalWrite(ledPIN , HIGH);
      
      Serial.println("");
      aXtotal = 0.0;  aYtotal = 0.0; aZtotal = 0.0;
      cont=0;
    }
  }
  
}
