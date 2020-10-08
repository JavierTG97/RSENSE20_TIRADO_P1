// EJERCICIO 3


//Variables asociadas a la lectura analógica
const int ADC_pin = 36;
int ADC_val;

//Variables asociada al PWM:
const int PWM_channel = 0;
const int PWM_pin = 22;
const int freq = 5000; //Hz
const int duty_bits = 16;
const int duty_max = int(pow(2.0,float(duty_bits))-1);
int duty = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  

  //Designamos las funcionalidades del PWM:
  //la última entrada determina la resolución del duty, siendo como máximo de 16 bits,
  ledcSetup(PWM_channel, freq, duty_bits);

  // Asociamos el Pin al canal:
  ledcAttachPin(PWM_pin, PWM_channel);
}

void loop() {
  // Leemos la entrada del ADC (su FS = 2^16 -1):
  ADC_val = analogRead(ADC_pin);

  //El duty es de duty_bits, por tanto su máximo valor se corresponde con 2^(duty_bits)-1;
  duty = round(ADC_val/4095.0*duty_max); 

  //Creamos la salida PWM en función del valor del duty
  ledcWrite(PWM_channel, duty);

  Serial.println(duty);
  
}
