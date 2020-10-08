
//Creamos la variable que nos permitirá saber cuando se ha producido la interrupción
//Se declara como volatile ya que es compartida entre la interrupción y el main y así le decimos al compilador que no la optimice
volatile int interruptCounter;
//int totalInterruptCounter;

//Creamos un puntero que represente a un timer 
hw_timer_t * timer = NULL;

//Creamos una variable de tipo portMUX_type que nos permita sincronizar la variable volatile
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//Variables asociadas a la lectura analógica
const int ADC_pin = 36;
int ADC_val;
float valor_v;

//Interrupción:
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

  // Aquí "unimos" nuestro timer a la función "onTimer"
  //true -> la interrupción será de tipo flanco (la interrupción solo se generará cuando el timer cambie de nivel)
  //false -> la " " " nivel (la interrupción se genera  siempre que la señal esté en alto)
  timerAttachInterrupt(timer, &onTimer, true);

  // Aquí definimos que la interrupción se cree cada 10 segundos (al contar hasta 10.000.000 -> con cada conteo realizado en 1 us -> 10 seg.)
  // true/false ->
  timerAlarmWrite(timer, 10000000, true);

  timerAlarmEnable(timer);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    ADC_val = analogRead(ADC_pin);
    valor_v = 3.3 * ADC_val/4095;
 
    Serial.print("Valor del ADC: ");
    Serial.print(valor_v);
    Serial.println(" Voltios");
  }

}
