
//Variable para controlar si se ha producido interrupcion:
volatile int interruptCounter;

//Creamos un puntero que represente a un timer 
hw_timer_t * timer = NULL;

//Creamos una variable de tipo portMUX_type que nos permita sincronizar la variable volatile
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//Variables asociadas a la lectura analógica
const int ADC_pin = 36;
int ADC_val;
float valor_v;
bool alarmBool = false; //para decidir si usar el timer o no para sacar la lectura analógica

//Variables asociadas al PWM:
const int PWM_channel = 0;
const int PWM_pin = 22;
const int freq = 5000; //Hz
const int duty_bits = 16;
const int duty_max = int(pow(2.0,float(duty_bits))-1);
const int duty_step = duty_max/9; //hay 10 posibles valores del duty: este es el salto entre ellos.
int duty = 0;

//Variable que usamos para guardarnos el comando de la consola:
String comando;

//Interrupcion:
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux); 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Nuestro timer por defecto tiene una freq. de 80 Mhz
  //timerBegin(a,b,c):
  //a-> timer que elegimos (p.ej. timer 0), b-> el prescaler que elegimos (al elegir 80 -> nuestro timer pasa a ser de 1 MHz)
  //c-> true/false -> si elegimos true entonces el conteo empieza en 0, sino en 2^64-1 (FS del timer)
  timer = timerBegin(0, 80, true);

  // Aqui "unimos" nuestro timer a la función "onTimer"
  //true -> la interrupción será de tipo flanco (la interrupción solo se generará cuando el timer cambie de nivel)
  //false -> la " " " nivel (la interrupción se genera  siempre que la señal esté en alto)
  timerAttachInterrupt(timer, &onTimer, true);

  // Aqui definimos que INICIALMENTE la interrupcion se cree cada segundo (que cuente hasta 10^6 a 1 MHz -> 1 seg.)
  // true/false -> si queremos que el conteo se renueve (true) o que continue hasta el maximo valor (false)
  timerAlarmWrite(timer, 1000000, true);

  timerAlarmEnable(timer);

  //Designamos las funcionalidades del PWM:
  //la última entrada determina la resolución del duty, siendo como máximo de 16 bits,
  ledcSetup(PWM_channel, freq, duty_bits);

  // Asociamos el Pin al canal:
  ledcAttachPin(PWM_pin, PWM_channel);

  //Creamos inicialmente la salida PWM en función del valor inicial del duty
  ledcWrite(PWM_channel, duty);

}

void loop() {
  // Si se recibe un string por la consola nos lo guardamos: 
  if (Serial.available()>0) {
     comando = Serial.readString();   
     //En funcion del comando elegimos que hacer:
     ejecutarComando(comando);  
  }
  
  if (interruptCounter > 0) {

    //Inicializamos de nuevo la variable, para detectar de nuevo la itnerrupción en el if
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    
    if (alarmBool) {
      ADC_val = analogRead(ADC_pin);
      valor_v = 3.3 * ADC_val/4095;
   
      Serial.print("Valor del ADC: ");
      Serial.print(valor_v);
      Serial.println(" Voltios");
    }
  }

}

void ejecutarComando(String orden){
  if (orden == "ADC") {
    ADC_val = analogRead(ADC_pin);
    valor_v = 3.3 * ADC_val/4095;
    Serial.print("Valor del ADC: ");
    Serial.print(valor_v);
    Serial.println(" Voltios");    
  }

  if (orden.substring(0,4) == "ADC(") {
    int tiempo = orden.substring(4,5).toInt(); // en segundos, solo toma valores enteros
    if (tiempo > 0) {
      timerAlarmWrite(timer, 1000000 * tiempo, true); //cambiamos cuando se produce la interrupción
      alarmBool = true;
    }
    else {
      alarmBool = false;
    }   
  }

  if (orden.substring(0,4) == "PWM(") {
    duty = orden.substring(4,5).toInt(); // tomamos el valor que va del 0 al 9 y que modifica al PWM
    //Creamos la salida PWM en función del valor del comando
    ledcWrite(PWM_channel, duty_step * duty);
  }
}



/*
#define s_to_us 1000000 // Conversión de useg a seg

// Aspectos importantes:
// 1.  Las variables que necesitamos guardar en memoria, les asignamos el atributo RTC_DATA_ATTR
// 2.  Como solo tenemos una fuente que genera la interrupción (un timer cada 10 seg.) no necesitamos una función
//     que nos diga cuál ha sido la fuente de interrupción.


//Variables asociadas a la lectura analógica
RTC_DATA_ATTR const int ADC_pin = 36;
int ADC_val;
float valor_v;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000); //Para que le de tiempo a abrir el puerto serie

  //Wake-up source que despierta al ESP32:
  esp_sleep_enable_timer_wakeup(10 * s_to_us); //queremos despertarlo cada 10 segundos, pero el valor lo introducimos en useg.
  
  //Al despertarse cada 10seg, entonces es cuando sacamos por el puerto serie el valor del ADC:
  ADC_val = analogRead(ADC_pin);
  valor_v = 3.3 * ADC_val/4095;
  
  Serial.print("Valor en Voltios: ");
  Serial.println(valor_v);
  Serial.flush(); //Esperamos a que la transmisión por puerto serie haya terminado

  //Una vez terminada la tarea, mandamos de nuevo al ESP32 a dormir:
  esp_deep_sleep_start();

}

//El loop nunca se ejecuta (antes de entrar a él, mandamos el ESP32 a dormir)
void loop() {  
}

*/
