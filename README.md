# BluePill_mbedOS

Proyecto en uVision5 utilizando mbed-os 5, compilado para 2 targets muy similares:

- NUCLEO_F103RB
- BLUEPILL_F103C8

Usan un procesador de la familia STM32F103xB. El primero con 64 pines y 128KB de flash y el segundo con 48 pines
y 64KB de flash.

La diferencia radica en los PinName.h ya que el segundo carece de los puertos C y D.

  
## Changelog

----------------------------------------------------------------------------------------------
##### 12.06.2017 ->commit :/"TrunkController verificado"
- [x] Termino implementaci�n de TrunkController. Utilizo RawSerial en lugar de SerialMon para las trazas serie.
- [x] A�ado esquema de conexi�n en la subcarpeta /TrunkController/docs/
- [ ] Montar hardware y verificar con motor
- [ ] Estudiar la comunicaci�n entre ROS y mbed:
		> RawSerial
		> esp8266 + MQTT
		> [esp32 + MQTT] + RawSerial

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 11.06.2017 ->commit :/"A�ado c�digo TrunkController para verificarlo"
- [x] A�ado TrunkController.
- [ ] Falta validar los cambios para ver si funciona bien.
- [!] No he probado mi c�digo basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y �ste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver c�mo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 05.01.2017 ->commit :/"Modifico send y sendComplete"
- [x] Modifico SerialMon cambiando send y sendComplete para que sea m�s intuitivo su uso con datos o con streams.
- [ ] Falta validar los cambios para ver si funciona bien.
- [!] No he probado mi c�digo basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y �ste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver c�mo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 03.01.2017 ->commit :/"Incluyo callback y Ticker para timeout de recepci�n"
- [x] Modifico SerialMon a�adiendo callback de timeout en recepci�n, as� como su flag y su Ticker
- [ ] Falta validar los cambios para ver si funciona bien.
- [!] No he probado mi c�digo basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y �ste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver c�mo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 22.12.2016 ->commit :/"Refactorizo SerialMon"
- [x] Refactorizo SerialMon para que quede un c�digo m�s claro y sencillo.
- [!] No he probado mi c�digo basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y �ste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver c�mo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 21.12.2016 ->commit :/"SerialMon con isr callbacks y configurable"
- [x] Modifico SerialMon para que pueda utilizarse sin MsgBox y sin RTOS, �nicamente con callbacks tx y 
	  rx complete.
- [!] No he probado mi c�digo basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y �ste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver c�mo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 16.12.2016 ->commit :/"MsgBox con FPointerDummy"
- [x] Modifico MsgBox para que se puedan registrar listeneres clase::metodo
- [x] Actualizo todas las tareas para adaptarse a la nueva funcionalidad de MsgBox.
- [!] No he probado mi c�digo basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y �ste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver c�mo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 14.12.2016 ->commit :/"ESP8266 funcionando OK"
- [x] Verifico que el m�dulo se conecta a la wifi y escribe y recibe datos de un socket TCP.
- [!] No he probado mi c�digo basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y �ste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver c�mo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 09.12.2016 ->commit :/"A�ado ESP8266 para mbed-os"
- [x] Incluyo driver esp8266 desde ARMmbed/mbed-os-example-wifi
- [x] Incluyo c�digo de ejemplo de uso desde https://docs.mbed.com/docs/mbed-os-api-reference/en/5.2/APIs/communication/network_sockets/
- [ ] Probar m�dulo ESP8266 y probar funcionamiento.
- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 01.12.2016 ->commit :/"A�ado pinout con esp8266"
- [x] Incluyo el env�o del caracter\0 al final de enviar la cadena de texto.
- [X] Modifico el comportamiento del env�o y la recepci�n para no perder bytes en lectura.
- [ ] A�adir el topic /stream para poder enviar bytes en bruto a trav�s de un SerialMon y por
      lo tanto no enviar un caracter \0 al final.
- [ ] Integrar m�dulo ESP8266 y probar funcionamiento.
- [ ] Continuar con la appnote de mbed.

----------------------------------------------------------------------------------------------
##### 01.12.2016 ->commit :/"SerialMon env�a y recibe cadenas de texto correctamente"
- [x] Incluyo el env�o del caracter\0 al final de enviar la cadena de texto.
- [X] Modifico el comportamiento del env�o y la recepci�n para no perder bytes en lectura.
- [ ] A�adir el topic /stream para poder enviar bytes en bruto a trav�s de un SerialMon y por
      lo tanto no enviar un caracter \0 al final.
- [ ] Integrar m�dulo ESP8266 y probar funcionamiento.
- [ ] Continuar con la appnote de mbed.

----------------------------------------------------------------------------------------------
##### 23.11.2016 ->commit :/"Incluyo MsgBroker"
- [x] Incluyo MsgBroker y dejo funcionando.
- [ ] Veriricar recepci�n de tramas en SerialMon y publicar mensajes "/cmd"

----------------------------------------------------------------------------------------------
##### 23.11.2016 ->commit :/"Commit inicial"
- [x] Dejo proyecto listo y funcionando en BluePill.

