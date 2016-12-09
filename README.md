# BluePill_mbedOS

Proyecto en uVision5 utilizando mbed-os 5, compilado para 2 targets muy similares:

- NUCLEO_F103RB
- BLUEPILL_F103C8

Usan un procesador de la familia STM32F103xB. El primero con 64 pines y 128KB de flash y el segundo con 48 pines
y 64KB de flash.

La diferencia radica en los PinName.h ya que el segundo carece de los puertos C y D.

  
## Changelog

----------------------------------------------------------------------------------------------
##### 09.12.2016 ->commit :/"Añado ESP8266 para mbed-os"
- [x] Incluyo driver esp8266 desde ARMmbed/mbed-os-example-wifi
- [X] Incluyo código de ejemplo de uso desde https://docs.mbed.com/docs/mbed-os-api-reference/en/5.2/APIs/communication/network_sockets/
- [ ] Probar módulo ESP8266 y probar funcionamiento.
- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 01.12.2016 ->commit :/"Añado pinout con esp8266"
- [x] Incluyo el envío del caracter\0 al final de enviar la cadena de texto.
- [X] Modifico el comportamiento del envío y la recepción para no perder bytes en lectura.
- [ ] Añadir el topic /stream para poder enviar bytes en bruto a través de un SerialMon y por
      lo tanto no enviar un caracter \0 al final.
- [ ] Integrar módulo ESP8266 y probar funcionamiento.
- [ ] Continuar con la appnote de mbed.

----------------------------------------------------------------------------------------------
##### 01.12.2016 ->commit :/"SerialMon envía y recibe cadenas de texto correctamente"
- [x] Incluyo el envío del caracter\0 al final de enviar la cadena de texto.
- [X] Modifico el comportamiento del envío y la recepción para no perder bytes en lectura.
- [ ] Añadir el topic /stream para poder enviar bytes en bruto a través de un SerialMon y por
      lo tanto no enviar un caracter \0 al final.
- [ ] Integrar módulo ESP8266 y probar funcionamiento.
- [ ] Continuar con la appnote de mbed.

----------------------------------------------------------------------------------------------
##### 23.11.2016 ->commit :/"Incluyo MsgBroker"
- [x] Incluyo MsgBroker y dejo funcionando.
- [ ] Veriricar recepción de tramas en SerialMon y publicar mensajes "/cmd"

----------------------------------------------------------------------------------------------
##### 23.11.2016 ->commit :/"Commit inicial"
- [x] Dejo proyecto listo y funcionando en BluePill.

