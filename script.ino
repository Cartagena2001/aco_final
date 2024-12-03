#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const int pinLDR = A1;
const int pinReleLuz = 3;
const int pinTemp = A0;
const int pinReleTemp = 4;
const int pinMovimiento = 8;
const int pinBuzzer = 5;
const int pinHumedad = A3;
const int pinReleHumedad = 2;

const int pinBotonArriba = 6;
const int pinBotonAbajo = 7;
const int pinBotonEnter = 9;

const int umbralLuz = 500;
const float umbralTemp = 25.0;
const int umbralHumedad = 700;

int opcionActual = 0;
int estadoLuces = 2;
int estadoAlarma = 2;
int estadoRiego = 2;
int estadoVentilacion = 2;

String opcionesMenu[] = {
  "Luces",
  "Alarma",
  "Riego",
  "Ventilacion"
};

// Son normalmente abiertos
bool arribaPresionado = true;
bool abajoPresionado = true;
bool enterPresionado = true;

bool lucesEncendidas = false;
bool alarmaActivada = false;
bool riegoActivo = false;
bool ventilacionActiva = false;

void setup() {
  pinMode(pinLDR, INPUT);
  pinMode(pinTemp, INPUT);
  pinMode(pinReleLuz, OUTPUT);
  pinMode(pinReleTemp, OUTPUT);
  pinMode(pinMovimiento, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinHumedad, INPUT);
  pinMode(pinReleHumedad, OUTPUT);
  pinMode(pinBotonArriba, INPUT_PULLUP);
  pinMode(pinBotonAbajo, INPUT_PULLUP);
  pinMode(pinBotonEnter, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido a ACO");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);

  mostrarMenu();
}

void loop() {
  if (digitalRead(pinBotonArriba) == LOW && !arribaPresionado) {
    arribaPresionado = true;
    opcionActual = (opcionActual - 1 + 4) % 4;
    mostrarMenu();
  } else if (digitalRead(pinBotonArriba) == HIGH) {
    arribaPresionado = false;
  }

  if (digitalRead(pinBotonAbajo) == LOW && !abajoPresionado) {
    abajoPresionado = true;
    opcionActual = (opcionActual + 1) % 4;
    mostrarMenu();
  } else if (digitalRead(pinBotonAbajo) == HIGH) {
    abajoPresionado = false;
  }

  if (digitalRead(pinBotonEnter) == LOW && !enterPresionado) {
    enterPresionado = true;
    cambiarEstado(opcionActual);
    mostrarMenu();
  } else if (digitalRead(pinBotonEnter) == HIGH) {
    enterPresionado = false;
  }

  if (estadoLuces == 1) {
    digitalWrite(pinReleLuz, HIGH);
    lucesEncendidas = true;
  } else if (estadoLuces == 0) {
    digitalWrite(pinReleLuz, LOW);
    lucesEncendidas = false;
  } else if (estadoLuces == 2) {
    int nivelLuz = analogRead(pinLDR);
    if (nivelLuz > umbralLuz) {
      digitalWrite(pinReleLuz, HIGH);
      if (!lucesEncendidas) {
        lucesEncendidas = true;
        mostrarMensaje("Sistema Luces", "activado");
      }
    } else {
      digitalWrite(pinReleLuz, LOW);
      if (lucesEncendidas) {
        lucesEncendidas = false;
        mostrarMensaje("Sistema Luces", "desactivado");
      }
    }
  }

  if (estadoAlarma == 1) {
    tone(pinBuzzer, 1000);
    alarmaActivada = true;
  } else if (estadoAlarma == 0) {
    noTone(pinBuzzer);
    alarmaActivada = false;
  } else if (estadoAlarma == 2) {
    int movimiento = digitalRead(pinMovimiento);
    if (movimiento == HIGH) {
      tone(pinBuzzer, 1000);
      if (!alarmaActivada) {
        alarmaActivada = true;
        mostrarMensaje("Sistema Alarma", "activado");
      }
    } else {
      noTone(pinBuzzer);
      if (alarmaActivada) {
        alarmaActivada = false;
        mostrarMensaje("Sistema Alarma", "desactivado");
      }
    }
  }

  if (estadoRiego == 1) {
    digitalWrite(pinReleHumedad, HIGH);
    riegoActivo = true;
  } else if (estadoRiego == 0) {
    digitalWrite(pinReleHumedad, LOW);
    riegoActivo = false;
  } else if (estadoRiego == 2) {
    int nivelHumedad = analogRead(pinHumedad);
    if (nivelHumedad < umbralHumedad) {
      digitalWrite(pinReleHumedad, HIGH);
      if (!riegoActivo) {
        riegoActivo = true;
        mostrarMensaje("Sistema Riego", "activado");
      }
    } else {
      digitalWrite(pinReleHumedad, LOW);
      if (riegoActivo) {
        riegoActivo = false;
        mostrarMensaje("Sistema Riego", "desactivado");
      }
    }
  }

  if (estadoVentilacion == 1) {
    digitalWrite(pinReleTemp, HIGH);
    ventilacionActiva = true;
  } else if (estadoVentilacion == 0) {
    digitalWrite(pinReleTemp, LOW);
    ventilacionActiva = false;
  } else if (estadoVentilacion == 2) {
    int temperatura = analogRead(pinTemp);
    float temperaturaC = (temperatura * 5.0 / 1023.0) * 100;
    if (temperaturaC > umbralTemp) {
      digitalWrite(pinReleTemp, HIGH);
      if (!ventilacionActiva) {
        ventilacionActiva = true;
        mostrarMensaje("Sistema Vent.", "activado");
      }
    } else {
      digitalWrite(pinReleTemp, LOW);
      if (ventilacionActiva) {
        ventilacionActiva = false;
        mostrarMensaje("Sistema Vent.", "desactivado");
      }
    }
  }
}

void mostrarMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(opcionesMenu[opcionActual]);
  lcd.setCursor(0, 1);
  switch (opcionActual) {
    case 0:
      lcd.print(estadoTexto(estadoLuces));
      break;
    case 1:
      lcd.print(estadoTexto(estadoAlarma));
      break;
    case 2:
      lcd.print(estadoTexto(estadoRiego));
      break;
    case 3:
      lcd.print(estadoTexto(estadoVentilacion));
      break;
  }
}

void cambiarEstado(int opcion) {
  switch (opcion) {
    case 0:
      estadoLuces = (estadoLuces + 1) % 3;
      break;
    case 1:
      estadoAlarma = (estadoAlarma + 1) % 3;
      break;
    case 2:
      estadoRiego = (estadoRiego + 1) % 3;
      break;
    case 3:
      estadoVentilacion = (estadoVentilacion + 1) % 3;
      break;
  }
}

String estadoTexto(int estado) {
  switch (estado) {
    case 0: return "Desactivado";
    case 1: return "Activado";
    case 2: return "Auto";
  }
  return "";
}

void mostrarMensaje(String linea1, String linea2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linea1);
  lcd.setCursor(0, 1);
  lcd.print(linea2);
  delay(2000);
  mostrarMenu();
}