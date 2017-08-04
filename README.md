# BluePill_mbedOS

Proyecto en uVision5 utilizando mbed-os 5, compilado para 2 targets muy similares:

- NUCLEO_F103RB
- BLUEPILL_F103C8

Usan un procesador de la familia STM32F103xB. El primero con 64 pines y 128KB de flash y el segundo con 48 pines
y 64KB de flash.

La diferencia radica en los PinName.h ya que el segundo carece de los puertos C y D.

  
## Changelog

----------------------------------------------------------------------------------------------
##### 04.08.2017 ->commit:"Mejorar rotaci�n"
- [x] Durante el giro sobre su eje (movimientos L,R) veo que es necesario iniciar el movimiento del
	  segundo motor sin descontar el primero, hasta que la proporci�n sea 2:3. En ese momento deshar�
	  los movimientos del motor 1 para que el giro sea m�s fiel. De todas formas hay que hacer pruebas
- [ ] Definir c�mo ser� el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 01.08.2017 ->commit:"Pruebas de movimiento IK"
- [x] Corrijo bug en isOOL.
- [x] Realizando TEST_I
	* Aumento l�mites a 90�
- [ ] Definir c�mo ser� el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 25.07.2017 ->commit:"Completado IKModel"
- [x] Completo y verifico el modelo IK para realizar los movimientos:
		Izq, Dcha, Arriba, Abajo, Cabeza arriba, Cabeza abajo, En pie
- [ ] Verificar con robot real y terminal TEST_I
- [ ] Definir c�mo ser� el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 24.07.2017 ->commit:"Desarrollando IKModel"
- [x] Comienzo la codificaci�n del m�dulo IKModel para definir los movimientos que puede realizar el robot.
- [ ] Realizar fotos de inclinaci�n para facilitar la codificaci�n IK
- [ ] Comprar sensores de presencia, led de colores, c�mara�?	 

----------------------------------------------------------------------------------------------
##### 14.07.2017 ->commit:"Revisar tests avanzados"
- [x] Despu�s de probar los tests. He corregido alg�n bug, pero me falta revisar los movimientos combinados 
	  porque debe haber alg�n errror. De todas formas, los movimientos son buenos para unos �ngulos m�ximos 
	  de unos 50 grados.
- [ ] Definir un algoritmo de trayectorias IK incluyendo zonas l�mite para evitar tensar demasiado
	  los tendones. Estudiar ROS. O definir un conjunto de posibles estados|posiciones y su transici�n
	  a otra cualquiera.
- [ ] Comprar sensores de presencia, led de colores, c�mara�?	 

----------------------------------------------------------------------------------------------
##### 14.07.2017 ->commit:"Incluyo tests avanzados"
- [x] Incluyo tests para realizar movimientos combinados por secci�n, por segmento y de rotaci�n.
- [x] Funciona bien con fuente de alimentaci�n de 5V@5A (cada motor 85mA)
- [ ] Definir un algoritmo de trayectorias IK incluyendo zonas l�mite para evitar tensar demasiado
	  los tendones. Estudiar ROS. O definir un conjunto de posibles estados|posiciones y su transici�n
	  a otra cualquiera.
- [ ] Comprar sensores de presencia, led de colores, c�mara�?	 

----------------------------------------------------------------------------------------------
##### 05.07.2017 ->commit:"Corrijo callback en timer"
- [x] Corrijo callback del timer ya que no se puede destruir el timer y luego hacer un signal_set.
- [ ] Realizar prueba con fuente de alimentaci�n Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 04.07.2017 ->commit:"Corrijo bug en c�lculo de grados"
- [x] Corrijo error en c�lculo de grados en test_trunkcontroller.cpp
- [ ] Realizar prueba con fuente de alimentaci�n Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 04.07.2017 ->commit:"Cambio terminal a PA_9,PA_10"
- [x] Cambio terminal a PA_9 y PA_10 para hacer pruebas con el m�dulo HC05 via bluetooth
- [ ] Realizar prueba con fuente de alimentaci�n Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 29.06.2017 ->commit:"Incluyo modo procesamiento dedicado en SerialTerminal"
- [x] Actualizo SerialTerminal pero mantengo versi�n (versi�n 1.0.0-03.06.2017)
- [ ] Realizar prueba con fuente de alimentaci�n Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 29.06.2017 ->commit:"Modifico todo para funcionar con el SerialTerminal"
- [x] Implemento y verifico el m�dulo SerialTerminal (versi�n 1.0.0-03.06.2017)
- [ ] Realizar prueba con fuente de alimentaci�n Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 29.06.2017 ->commit:"Defino terminal de test"
- [x] Defino terminal de test "T,seccion,motor,grados"+$00 
- [x] Montados resto de cables
- [ ] Realizar prueba con fuente de alimentaci�n Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 22.06.2017 ->commit:"Modifico Logger e incluyo terminal de comandos"
- [x] Modifico el objeto Logger para que derive de RawSerial pero definiendo un mutex para que diferentes
	  m�dulos puedan compartirlo.
- [ ] Definir el terminal de comandos, para ver de qu� forma puedo probarlo f�cilmente y que no sea una historia	  
- [ ] Montar resto de cables y hacer una prueba con los 9 motores en vac�o.
----------------------------------------------------------------------------------------------
##### 22.06.2017 ->commit:"Incluyo Logger en lugar de RawSerial"
- [x] Incluyo el objeto Logger para sustituir a RawSerial y tener la capacidad de acceder al puerto
	  serie de forma exclusiva (mutex) de forma que no se solapen las trazas de depuraci�n de distintas
	  tareas.
- [x] Modifico TrunkController para que tenga su propio hilo de control mediante la api 'actionRequested'	  
      y que notifique al m�dulo llamante mediante la callback instalada por medio de 'notifyReady', cuando
	  no queden m�s acciones a ejecutar.
- [x] A�ado a TrunkController un servicio para consultar el n�mero de acciones completadas y otro que permita
      borrar ese contador.
- [x] A�ado a TrunkController un servicio que permite detener la ejecuci�n de acciones en curso.	  
- [ ] Montar resto de cables y hacer una prueba con los 9 motores en vac�o.
- [ ] Montar el componente esp8266
----------------------------------------------------------------------------------------------
##### 21.06.2017 ->commit:"Funcionamiento del motor verificado"
- [x] Corrijo error en el c�lculo de los patrones del shifter.
- [x] Motor montado y verificado, OK!
- [ ] Montar resto de cables y hacer una prueba con los 9 motores en vac�o.
- [ ] Implementar m�dulo RemoteController que procese solicitudes externas y las transmita a 
      TrunkController. Este m�dulo podr� recibir las solicitudes v�a serie o v�a MQTT por medio
	  del esp8266
- [ ] (opcional) Cambiar el RawSerial por un buffer de datos al igual que hac�a con SerialMon
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

