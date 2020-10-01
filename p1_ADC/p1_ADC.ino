/*------------
 * Enunciado:
 * -----------
 *Con una periodicidad de un segundo, lee el valor del ADC y muéstralo por consola. Utiliza la salida de
  3V3 de la placa con un potenciómetro para generar la tensión analógica. Antes de conectarla al
  módulo comprueba que no superas 3V3 con un multímetro --. 
 */

 const int ADC_pin = 36;

 int ADC_val;
 float valor_V;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  ADC_val = analogRead(ADC_pin);

  valor_V = 3.3 * ADC_val/4095;

  Serial.print("El valor del ADC es : ");
  Serial.println(ADC_val);

  Serial.print("En Voltios : ");
  Serial.println(valor_V);

  Serial.println("");
  delay(1000);

}
