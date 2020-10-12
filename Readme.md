# Practica 1

*En este repositorio se encuentran los códigos realizados para la práctica 1.*

Esta práctica consta de **5 ejercicios**:  
1. **ADC**  :   Con una periodicidad de un segundo, lee el valor del ADC y muéstralo por consola.
2. **timer**: Utilizando los timer hardware genera una interrupción cada 10s que lea el ADC y lo muestre por pantalla. Se ha intentado de dos formas (Interrupción con *pooling* y con modo *Deep Sleep*)
3. **PWM-ADC**: Genera una salida pwm a 5kHz proporcional a la lectura del ADC. Comprueba en el osciloscopio tanto la medida analógica como la salida PWM.
4. **ADC-TIMER-PWM**: Mediante UART (puerto serie), se comandan los periféricos: PWM y ADC (activado según interrupción de un timer).
5. **I2C-LED**: Comunicación mediante I2C entre el acelerómetro del **GY-91** (esclavo) y el **ESP32** (máster). Además de comunicación UART cada cierto tiempo con los datos del acelerómetro. Todo coordinado con el encendido-apagado de un LED.