# Planta robotizada (proyecto XR+Inst)

Proyecto en uVision5 utilizando arm-mbed v5, compilado para:

- NUCLEO_F103RB
- BLUEPILL_F103C8

El continuum robot es funcional a falta de solventar el calentamiento de los motores. Se puede seguir parte del proceso
de montaje [aquí](https://photos.app.goo.gl/LPMHRP82iHefyZrRA)
  
## Changelog

----------------------------------------------------------------------------------------------
##### 26.08.2017 ->commit:"Replanteamiento general"
- [x] El robot es funcional en cuanto a motores y detección de presencia
- [ ] Hay que mejorar y/o corregir los siguientes aspectos:

	. Los motores sufren mucho calentamiento si están alimentados constantemente. Es necesario 
	  incluir un regulador por driver con una línea de activación de forma que se puedan des-
	  activar cuando se quiera (todos a la vez). BUSCAR REGULADOR 5V@2A con EN.
	  
	. Los sensores de presencia no pueden estar en la base, ya que el propio movimiento del robot
	  genera eventos de presencia. Deberian situarse en el borde exterior de la maceta. REPLANTEAR.
	  
	. Los movimientos de cambio de dirección, inicio y parada son bruscos y generan oscilaciones 
	  en la estructura. Habría que modificar el modelo IK para que se inicien y finalicen de forma
	  suave. REDISEÑAR. 
	  
	. Buscar un led RGB para la copa de la flor y redifinir su formato. Quizás múltiples led colocados
	  en las flores y controlados por una salida PWM y otro para el cuerpo de la copa. BUSCAR LED.
	  
	. Reutilizar RasPi con micrófono y altavoz para implementar servicios y funcionalidades avanzadas:
		- Librería Node para implementar un medidor de decibelios y detectar chillidos y/o gritos.
		- Librería Node/C++ para implementar protocolo serie mediante bluetooth y comunicar con el robot.
	

----------------------------------------------------------------------------------------------
##### 11.08.2017 ->commit:"Modo 3PIR con terminal remoto"
- [x] Incluyo terminal remoto con comandos T para recalibrar
- [ ] Definir otros módulos y juegos.

----------------------------------------------------------------------------------------------
##### 11.08.2017 ->commit:"Terminado modo seguimiento 3PIR"
- [x] Incluyo función de posicionamiento
- [x] Dejo activado simulación de presencia con 3PIR
- [ ] Definir otros módulos y juegos.

----------------------------------------------------------------------------------------------
##### 06.08.2017 ->commit:"Incluyo tests J con movimientos acumulados"
- [x] Incluyo tests con varios movimientos enlazados
- [ ] En los movimientos enlazados alguna vez he tenido HardFault. Hay que revisar a que se debe
- [ ] Definir cómo será el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 06.08.2017 ->commit:"Validados modelos IK9 e IK3"
- [x] Corrijo bug en movimiento oi2section en IKModel
- [x] Movimiento de rotación verificado	  
- [ ] Definir test de movimientos encadenados
- [ ] Definir cómo será el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 05.08.2017 ->commit:"Nuevos modelos IK9 e IK3"
- [x] Modifico IKModel para que sea un interfaz común e implemento IK9 e IK3 como clases derivadas
	  de IKModel para realizar diferentes implementaciones.
- [ ] Definiendo movimiento de rotación	  
- [ ] Definir cómo será el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 04.08.2017 ->commit:"Mejorar rotación"
- [x] Durante el giro sobre su eje (movimientos L,R) veo que es necesario iniciar el movimiento del
	  segundo motor sin descontar el primero, hasta que la proporción sea 2:3. En ese momento desharé
	  los movimientos del motor 1 para que el giro sea más fiel. De todas formas hay que hacer pruebas
- [ ] Definir cómo será el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 01.08.2017 ->commit:"Pruebas de movimiento IK"
- [x] Corrijo bug en isOOL.
- [x] Realizando TEST_I
	* Aumento límites a 90º
- [ ] Definir cómo será el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 25.07.2017 ->commit:"Completado IKModel"
- [x] Completo y verifico el modelo IK para realizar los movimientos:
		Izq, Dcha, Arriba, Abajo, Cabeza arriba, Cabeza abajo, En pie
- [ ] Verificar con robot real y terminal TEST_I
- [ ] Definir cómo será el juego y la cabeza a colocar. Ya tengo sensores de presencia.

----------------------------------------------------------------------------------------------
##### 24.07.2017 ->commit:"Desarrollando IKModel"
- [x] Comienzo la codificación del módulo IKModel para definir los movimientos que puede realizar el robot.
- [ ] Realizar fotos de inclinación para facilitar la codificación IK
- [ ] Comprar sensores de presencia, led de colores, cámara¿?	 

----------------------------------------------------------------------------------------------
##### 14.07.2017 ->commit:"Revisar tests avanzados"
- [x] Después de probar los tests. He corregido algún bug, pero me falta revisar los movimientos combinados 
	  porque debe haber algún errror. De todas formas, los movimientos son buenos para unos ángulos máximos 
	  de unos 50 grados.
- [ ] Definir un algoritmo de trayectorias IK incluyendo zonas límite para evitar tensar demasiado
	  los tendones. Estudiar ROS. O definir un conjunto de posibles estados|posiciones y su transición
	  a otra cualquiera.
- [ ] Comprar sensores de presencia, led de colores, cámara¿?	 

----------------------------------------------------------------------------------------------
##### 14.07.2017 ->commit:"Incluyo tests avanzados"
- [x] Incluyo tests para realizar movimientos combinados por sección, por segmento y de rotación.
- [x] Funciona bien con fuente de alimentación de 5V@5A (cada motor 85mA)
- [ ] Definir un algoritmo de trayectorias IK incluyendo zonas límite para evitar tensar demasiado
	  los tendones. Estudiar ROS. O definir un conjunto de posibles estados|posiciones y su transición
	  a otra cualquiera.
- [ ] Comprar sensores de presencia, led de colores, cámara¿?	 

----------------------------------------------------------------------------------------------
##### 05.07.2017 ->commit:"Corrijo callback en timer"
- [x] Corrijo callback del timer ya que no se puede destruir el timer y luego hacer un signal_set.
- [ ] Realizar prueba con fuente de alimentación Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 04.07.2017 ->commit:"Corrijo bug en cálculo de grados"
- [x] Corrijo error en cálculo de grados en test_trunkcontroller.cpp
- [ ] Realizar prueba con fuente de alimentación Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 04.07.2017 ->commit:"Cambio terminal a PA_9,PA_10"
- [x] Cambio terminal a PA_9 y PA_10 para hacer pruebas con el módulo HC05 via bluetooth
- [ ] Realizar prueba con fuente de alimentación Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 29.06.2017 ->commit:"Incluyo modo procesamiento dedicado en SerialTerminal"
- [x] Actualizo SerialTerminal pero mantengo versión (versión 1.0.0-03.06.2017)
- [ ] Realizar prueba con fuente de alimentación Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 29.06.2017 ->commit:"Modifico todo para funcionar con el SerialTerminal"
- [x] Implemento y verifico el módulo SerialTerminal (versión 1.0.0-03.06.2017)
- [ ] Realizar prueba con fuente de alimentación Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 29.06.2017 ->commit:"Defino terminal de test"
- [x] Defino terminal de test "T,seccion,motor,grados"+$00 
- [x] Montados resto de cables
- [ ] Realizar prueba con fuente de alimentación Rpi o de 5V@2A (cada motor 85mA)

----------------------------------------------------------------------------------------------
##### 22.06.2017 ->commit:"Modifico Logger e incluyo terminal de comandos"
- [x] Modifico el objeto Logger para que derive de RawSerial pero definiendo un mutex para que diferentes
	  módulos puedan compartirlo.
- [ ] Definir el terminal de comandos, para ver de qué forma puedo probarlo fácilmente y que no sea una historia	  
- [ ] Montar resto de cables y hacer una prueba con los 9 motores en vacío.
----------------------------------------------------------------------------------------------
##### 22.06.2017 ->commit:"Incluyo Logger en lugar de RawSerial"
- [x] Incluyo el objeto Logger para sustituir a RawSerial y tener la capacidad de acceder al puerto
	  serie de forma exclusiva (mutex) de forma que no se solapen las trazas de depuración de distintas
	  tareas.
- [x] Modifico TrunkController para que tenga su propio hilo de control mediante la api 'actionRequested'	  
      y que notifique al módulo llamante mediante la callback instalada por medio de 'notifyReady', cuando
	  no queden más acciones a ejecutar.
- [x] Añado a TrunkController un servicio para consultar el número de acciones completadas y otro que permita
      borrar ese contador.
- [x] Añado a TrunkController un servicio que permite detener la ejecución de acciones en curso.	  
- [ ] Montar resto de cables y hacer una prueba con los 9 motores en vacío.
- [ ] Montar el componente esp8266
----------------------------------------------------------------------------------------------
##### 21.06.2017 ->commit:"Funcionamiento del motor verificado"
- [x] Corrijo error en el cálculo de los patrones del shifter.
- [x] Motor montado y verificado, OK!
- [ ] Montar resto de cables y hacer una prueba con los 9 motores en vacío.
- [ ] Implementar módulo RemoteController que procese solicitudes externas y las transmita a 
      TrunkController. Este módulo podrá recibir las solicitudes vía serie o vía MQTT por medio
	  del esp8266
- [ ] (opcional) Cambiar el RawSerial por un buffer de datos al igual que hacía con SerialMon
----------------------------------------------------------------------------------------------
##### 12.06.2017 ->commit :/"TrunkController verificado"
- [x] Termino implementación de TrunkController. Utilizo RawSerial en lugar de SerialMon para las trazas serie.
- [x] Añado esquema de conexión en la subcarpeta /TrunkController/docs/
- [ ] Montar hardware y verificar con motor
- [ ] Estudiar la comunicación entre ROS y mbed:
		> RawSerial
		> esp8266 + MQTT
		> [esp32 + MQTT] + RawSerial

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 11.06.2017 ->commit :/"Añado código TrunkController para verificarlo"
- [x] Añado TrunkController.
- [ ] Falta validar los cambios para ver si funciona bien.
- [!] No he probado mi código basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y éste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver cómo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 05.01.2017 ->commit :/"Modifico send y sendComplete"
- [x] Modifico SerialMon cambiando send y sendComplete para que sea más intuitivo su uso con datos o con streams.
- [ ] Falta validar los cambios para ver si funciona bien.
- [!] No he probado mi código basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y éste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver cómo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 03.01.2017 ->commit :/"Incluyo callback y Ticker para timeout de recepción"
- [x] Modifico SerialMon añadiendo callback de timeout en recepción, así como su flag y su Ticker
- [ ] Falta validar los cambios para ver si funciona bien.
- [!] No he probado mi código basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y éste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver cómo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 22.12.2016 ->commit :/"Refactorizo SerialMon"
- [x] Refactorizo SerialMon para que quede un código más claro y sencillo.
- [!] No he probado mi código basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y éste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver cómo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 21.12.2016 ->commit :/"SerialMon con isr callbacks y configurable"
- [x] Modifico SerialMon para que pueda utilizarse sin MsgBox y sin RTOS, únicamente con callbacks tx y 
	  rx complete.
- [!] No he probado mi código basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y éste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver cómo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 16.12.2016 ->commit :/"MsgBox con FPointerDummy"
- [x] Modifico MsgBox para que se puedan registrar listeneres clase::metodo
- [x] Actualizo todas las tareas para adaptarse a la nueva funcionalidad de MsgBox.
- [!] No he probado mi código basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y éste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver cómo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 14.12.2016 ->commit :/"ESP8266 funcionando OK"
- [x] Verifico que el módulo se conecta a la wifi y escribe y recibe datos de un socket TCP.
- [!] No he probado mi código basado en MsgBroker porque acabo de tener un dilema sobre la conveniencia
	  del modelo pub-sub vs req-res en cierto tipo de casos. Y éste es uno de ellos, o sea que lo tengo
	  que pensar un poco mejor para ver cómo lo dejo finalmente.

- [ ] Continuar con la appnote de mbed.
----------------------------------------------------------------------------------------------
##### 09.12.2016 ->commit :/"Añado ESP8266 para mbed-os"
- [x] Incluyo driver esp8266 desde ARMmbed/mbed-os-example-wifi
- [x] Incluyo código de ejemplo de uso desde https://docs.mbed.com/docs/mbed-os-api-reference/en/5.2/APIs/communication/network_sockets/
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

