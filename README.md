
# 🌱 IOTAGRO — Sistema de Monitoreo Agrícola Inteligente

> Un sistema IoT Full-Stack para la supervisión de cultivos en tiempo real
> usando ESP32, AWS IoT Core y arquitectura Serverless.

![Estado del Proyecto](https://img.shields.io/badge/Estado-Finalizado-success)
![Versión](https://img.shields.io/badge/Versión-1.0.0-blue)
![Tecnología](https://img.shields.io/badge/IoT-ESP32%20%7C%20AWS-orange)

## Contenido
- **Descripción**: ¿Qué hace el proyecto.
- **Arquitectura**: Componentes y flujo de datos.
- **Requisitos**: Hardware y software necesarios.
- **Instalación**: Pasos para compilar y desplegar firmware y servicios.
- **Uso**: Cómo probar y comprobar el sistema en funcionamiento.
- **Futuras mejoras** y **Autores**.

## 📖 Descripción

IOTAGRO es una solución integral para modernizar la agricultura mediante
Internet de las Cosas. El sistema recolecta variables críticas del entorno
(humedad del suelo, luminosidad y precipitación) y las transmite de forma
segura a AWS. Los datos se almacenan históricamente y se exponen a través de
un dashboard web en tiempo real para facilitar la toma de decisiones.

## 🏗 Arquitectura del Sistema

Arquitectura IoT Serverless, escalable:

1. Capa física (Edge): ESP32-C6 leyendo sensores analógicos/digitales.
2. Comunicación: MQTT sobre WiFi con TLS (certificados X.509).
3. Nube: AWS IoT Core como broker MQTT.
4. Almacenamiento: DynamoDB para registros históricos.
5. Backend: AWS Lambda (Python) que expone API REST/Function URL.
6. Frontend: Dashboard HTML5 + Chart.js para visualización en tiempo real.

## 🛠 Requisitos de Hardware

- Microcontrolador: `ESP32-C6` (compatible con ESP32 estándar).
- Sensor de humedad de suelo: sensor capacitivo (recomendado, resistente).
- Sensor de luz: fotorresistencia (LDR).
- Sensor de lluvia: placa detectora (ej. MH-RD).
- Conectividad: cable USB-C y jumpers.

## 💻 Tecnologías y Software

- Firmware: C / C++ (Espressif IDF v5.5).
- Cloud: AWS IoT Core, DynamoDB, Lambda.
- Lenguajes: C, Python 3.12, HTML5, CSS3, JavaScript.
- Entorno de desarrollo recomendado: Visual Studio Code (con ESP-IDF).

## 📂 Estructura del Proyecto

```
IOTAGRO/
├── main/                   # Código fuente del firmware (ESP32)
│   ├── certs/              # Certificados AWS (root.pem, client.crt, client.key)
│   ├── iotagro_main.c      # Lógica principal (sensores + MQTT)
│   └── CMakeLists.txt      # Configuración de compilación
├── web/                    # Frontend del dashboard
│   └── index.html          # Interfaz de usuario (reemplazar API_URL)
├── CMakeLists.txt          # Configuración global del proyecto
└── README.md               # Documentación
```

## 🚀 Instalación y Configuración

Antes de comenzar, asegúrate de tener instalado y configurado ESP-IDF
en tu entorno de desarrollo (VS Code o terminal). También necesitarás una
cuenta de AWS con permisos para crear recursos en IoT Core, DynamoDB y Lambda.

### 1) Configuración del Firmware (ESP32)

1. Clona el repositorio:

```bash
git clone <url-del-repositorio>
cd campo_iot-projecto
```

2. Copia los certificados de AWS a `main/certs/`:

- `root.pem` (CA raíz)
- `client.crt` (certificado del dispositivo)
- `client.key` (clave privada)

3. Configura credenciales WiFi y el endpoint de AWS en `main/iotagro_main.c`
(busca las variables `WIFI_SSID`, `WIFI_PASS`, y `AWS_ENDPOINT`).

4. Compila y sube el firmware a la placa ESP32:

```bash
idf.py build
idf.py flash monitor
```

### 2) Configuración en AWS

1. IoT Core:
	 - Crear un "Thing" en AWS IoT.
	 - Generar o subir certificados X.509.
	 - Crear una policy que permita la conexión MQTT y publicación en tópicos
		 (ej. `iotagro/data`).

2. DynamoDB:
	 - Crear la tabla `IOTAGRO_Historial` con:
		 - Partition Key: `id_dispositivo`
		 - Sort Key: `fecha_hora`

3. Reglas de IoT:
	 - Crear una regla que capture mensajes del tópico `iotagro/data` y los
		 inserte en DynamoDB.

4. Lambda (opcional / Backend):
	 - Crear una función Python que consulte DynamoDB y exponga un Function URL
		 (o API Gateway) para que el frontend pueda solicitar datos.
	 - Habilitar CORS en la Function URL si se usa desde un `index.html` local.

## 3) Ejecución del Dashboard

1. Ve a la carpeta `web/` y abre `index.html`.
2. Reemplaza la variable `API_URL` en `index.html` por la Function URL de tu
	 Lambda o por la URL de tu API Gateway.
3. Abre `index.html` en un navegador moderno (Chrome, Firefox, Edge).

> Nota: Para desplegar el frontend públicamente, puedes usar AWS S3 + CloudFront
o GitHub Pages.

## 📊 Uso

- Al encender el ESP32, el LED de estado indicará la conexión WiFi.
- Los sensores envían datos automáticamente cada ~30 segundos al tópico MQTT
	configurado.
- Los datos se almacenan en DynamoDB y el dashboard se actualiza en tiempo real.

## 🔮 Futuras Mejoras

- Alertas por correo electrónico con AWS SNS para niveles críticos de sequía.
- Control remoto de actuadores (bomba de agua) desde el dashboard.
- Despliegue del frontend en hosting público (AWS S3 o GitHub Pages).

## 👥 Autores

- Omar Andrés Rodriguez Quiceno
- Juan Esteban Agreda Gutierrez

© 2025 IOTAGRO Project. Todos los derechos reservados.
