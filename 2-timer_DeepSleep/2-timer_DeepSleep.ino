/* En este sketch haremos lo mismo que en el anterior peo usando
 *  la opción DeepSleep del ESP32 -> nos permite ahorrar energía,
 *  algo muy importante en IoT
 */

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
  Serial.begin(115200);
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
  esp_set_deep_sleep_wake_stub(NULL);
  esp_deep_sleep_start();

}

//El loop nunca se ejecuta (antes de entrar a él, mandamos el ESP32 a dormir)
void loop() {  
}
