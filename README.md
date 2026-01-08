# Tarea5_Karla

Sistemas Embebidos – ESP32
UART, FreeRTOS y Gestión de Energía

Este repositorio alberga el desarrollo técnico de la Tarea #5 para la cátedra de Sistemas Embebidos. El proyecto se centra en la implementación de soluciones avanzadas utilizando el microcontrolador ESP32, abarcando desde la comunicación serial y el procesamiento multitarea con FreeRTOS, hasta la optimización del consumo energético. Todo el desarrollo fue realizado bajo el ecosistema PlatformIO en Visual Studio Code.
Estructura del Proyecto

El repositorio se organiza en cuatro módulos independientes, cada uno con su propia configuración y código fuente:

text

├── Ejercicio1_UART2          # Gestión de comandos vía UART2  
├── Ejercicio2_FreeRTOS       # Planificación de tareas concurrentes  
├── Ejercicio3_Energia        # Implementación de modos de bajo consumo  
└── Ejercicio4_UART_FreeRTOS  # Integración de sistemas complejos  

Requisitos y Herramientas

    IDE: Visual Studio Code con la extensión PlatformIO.
    Hardware: ESP32 Dev Module (requerido físicamente para el Ejercicio 3).
    Simulación: Extensión Wokwi para validación lógica.
    Drivers: Controladores USB actualizados (CP210x / CH340).

Descripción de los Ejercicios

1. Comunicación Serial Avanzada (UART2)
Implementación de un intérprete de comandos que utiliza el puerto UART2. El sistema procesa cadenas de texto de forma no bloqueante, permitiendo el control remoto del dispositivo mediante instrucciones como status, led on/off, info y reset.

2. Arquitectura Multitarea con FreeRTOS
Diseño de un entorno concurrente donde coexisten tres procesos con prioridades específicas:

    Sensor Virtual: Simulación de adquisición de datos.
    Control de LED: Gestión de señales visuales.
    Monitoreo: Reporte de métricas al terminal serial.
    Se utiliza el scheduler de FreeRTOS para garantizar una ejecución fluida y sin bloqueos.

3. Optimización Energética (Deep Sleep)
Desarrollo enfocado en la eficiencia del hardware. El ESP32 alterna entre un estado activo y el modo Deep Sleep, utilizando un temporizador RTC para despertar automáticamente. Nota: Este ejercicio fue validado exclusivamente en hardware real para medir su comportamiento térmico y eléctrico.

4. Sistema Integrado: UART + FreeRTOS
Fusión de los conceptos anteriores en una aplicación robusta y escalable. El sistema integra una tarea de escucha UART para comandos externos, una tarea de control de periféricos y una tarea de supervisión de estado, todas comunicadas mediante variables globales compartidas bajo el control de FreeRTOS.
