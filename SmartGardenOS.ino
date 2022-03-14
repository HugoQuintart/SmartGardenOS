
/*
         __     __   __      __   __________       _______________
        /  /   /  / /  /    /  / /  _______/      /   ________    /
       /  /   /  / /  /    /  / /  /             /  /         /  /
      /  /___/  / /  /    /  / /  /     ______  /  /         /  /
     /  ____   / /  /    /  / /  /     /   __/ /  /         /  /
    /  /   /  / /  /    /  / /  /      /  /   /  /         /  /
   /  /   /  / /  /____/  / /  /______/  /   /  /_________/  /
  /__/   /__/ /__________/ /____________/   /_______________/

  
  Ceci est le système d'exploitation du projet.
  Pour ceux qui le désirent, envoyez-moi en e-mail si vous voulez collaborer.
*/
// préparer l'ecran lcd
#include <LiquidCrystal.h>
LiquidCrystal lcd(11, 9, 6, 5, 4, 3);
// inclure les bibliothèque qui permettra l'utilisation de la sonde
#include <DallasTemperature.h>
#include <OneWire.h>

// preparer le sonde:
#define ONE_WIRE_BUS 39
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// préparation de la DHT
#include "DHT.h"   // Librairie des capteurs DHT

#define DHTPIN 8    // le pin sur lequel est branché le DHT

#define DHTTYPE DHT22      // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// Préparation du nivelage de l'eau:
int lowLed = 48;
int fullPin = 47; // le capteur de niveau d'eau plein sur la pin 47
int stopPin = 46; // le capteur de niveau d'eau 0 sur la pin 46
int waterLevel;
//étalonnage de la lecture hygrométrique:
const int wet = 230;// on étalonne les capteurs d'hygrométrie avec une valeur "humide"
const int dry = 591; // on étalonne les capteurs d'hygrométrie avec une valeur "sèche"

//préparation des variables nécessaires à la lecture hygrométrique:
bool reading = true; // système en mode automatique (par défaut)
int moisturePin1 = A0; // Brancher le capteur d'hygrométrie sur la pin A0
int moisturePin2 = A1; // Brancher le capteur d'hygrométrie sur la pin A1
int moisturePin3 = A2; // Brancher le capteur d'hygrométrie sur la pin A2
int moisturePin4 = A3; // Brancher le capteur d'hygrométrie sur la pin A3
int moistureValue; // initialiser la variable moistureValue qui correspond à l'humidité du sol
int moistureUsable; // initaliser la variable qui va stocker la valeur de la moyenne des valeurs des capteurs.
int hygroVal; // initialiser la variable qui va socker le pourcentage d'humidité du sol.


//préparation des variables pour l'arrosage:
int dryingPin = 41; // Définir la pin de la pompe de reprise sur la pin digitale 12
int wateringPin = 43; // Brancher la pompe d'arrosage sur la pin digitale 11
int wateringState; //initialiser la variable qui va stocker l'état de la pompe d'arrosage.
int dryingState; // initialiser la variable qui va stocker l'état de la pompe de reprise.

//préparation des variables pour l'analyse de la température du sol

//et le reste des variables:
int statePin = 13; // la led d'état de la carte électronique sera la led sur la pin 13

void setup() {
  moistureValue = 0; // Définir la valeur moistureValue à 0
  pinMode(lowLed, OUTPUT); // définir l'indicateur de niveau vide comme sortie
  pinMode(moisturePin1, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(moisturePin2, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(moisturePin3, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(moisturePin4, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(wateringPin, OUTPUT); // Définir la pin de la pompe d'arrosage comme sortie
  pinMode(dryingPin, OUTPUT); // Définir la pin de la pompe de reprise en tant que sortie
  pinMode(statePin, OUTPUT); // La led d'état correspond à la pin 13, en sortie de courant
  pinMode(stopPin, INPUT_PULLUP); // le capteur de niveau d'eau 0 sur en entrée
  pinMode(fullPin, INPUT_PULLUP); // le capteur de niveau plein en entrée
  digitalWrite(statePin, HIGH);

  Serial.begin(9600); // Initialiser le moniteur série en 9600 BAUD.
  lcd.begin(20, 4); // Initialiser l'écran en lcd 20x4
  dht.begin(); //démarrer la dht
  sensors.begin();
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("SMART GARDEN");
  lcd.setCursor(4, 1);
  lcd.print("BOOTING, PLEASE WAIT");
  delay(3000);
  lcd.clear();


}

void loop() {

  // ***********************************************partie DHT***********************************************
  lcd.clear();

  // Lecture du taux d'humidité
  float h = dht.readHumidity();
  // Lecture de la température en Celcius
  float t = dht.readTemperature();
  // Pour lire la température en Fahrenheit
  float f = dht.readTemperature(true);

  // Stop le programme et renvoie un message d'erreur si le capteur ne renvoie aucune mesure
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from the DHT sensor !");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erreur Capteur DHT");
    lcd.setCursor(0, 1);
    lcd.print("Vérifier la");
    lcd.setCursor(0, 2);
    lcd.print("connexion");
    delay(1000);
    lcd.clear();
    return;
  }

  // Calcul la température ressentie. Il calcul est effectué à partir de la température en Fahrenheit
  // On fait la conversion en Celcius dans la foulée
  float hi = dht.computeHeatIndex(f, h);
  Serial.print("Humidite exterieure: ");
  Serial.print(h);
  Serial.print(" %, ");
  Serial.print("Temperature exterieure: ");
  Serial.print(t);
  Serial.print(" *C,  ");
  lcd.setCursor(0, 0);
  lcd.print("Humidite exterieure:");
  lcd.setCursor(0, 1);
  lcd.print(h);
  lcd.print(" %");
  lcd.setCursor(0, 2);
  lcd.print("Temp. ext: ");
  lcd.setCursor(0, 3);
  lcd.print(t);
  lcd.print("*C");
  delay(2000); // définir un arrêt de 2 secondes pour faciliter la lecture sur l'écran

  // ***********************************************niveau sur écran lcd et led d'avertissement***********************************************

  lcd.clear();
  // affichage du niveau de l'eau
  lcd.setCursor(0, 0);
  lcd.print("Niveau d'eau: ");
  lcd.setCursor(0, 2);
  // on interroge pour savoir si les deux interrupteurs sont fermés
  // si oui, on affiche que le réservoir est plein.
  if (digitalRead(stopPin) == LOW) { // on interroge pour savoir si les deux interrupteurs sont fermés
    if (digitalRead(fullPin) == LOW) {
      lcd.print("PLEIN");
      waterLevel = 3;
    }
  }
  // on interroge pour savoir si l'interrupteur  de niveau vide est fermé et si celui pour le trop plein est ouvert
  // si oui, on affiche que le réservoir a un niveau correct.
  if (digitalRead(stopPin) == LOW) {
    if (digitalRead(fullPin) == HIGH) {
      lcd.print("OK");
      waterLevel = 2;
    }
  }
  // Enfin, interroge pour savoir si les deux interrupteurs sont ouverts
  // si oui, on affiche que le réservoir est vide.
  if (digitalRead(stopPin) == HIGH) {
    if (digitalRead(fullPin) == HIGH) {
      lcd.print("VIDE");
      digitalWrite(lowLed, HIGH);
      waterLevel = 1;
    }
  }



  // ***********************************************partie arrosage**********************************************
  // enclencher la pompe d'arrosage si l'humidité du sol est inférieure à 25
  if (hygroVal < 25) {
    digitalWrite(wateringPin, HIGH);
    wateringState = 1;
  }
  // enclencher la pompe de reprise si l'humidité du sol dépasse la valeur "60"
  if (hygroVal > 60) {
    digitalWrite(dryingPin, HIGH);
    dryingState = 1;
  }
  // arrêter les pompes quand on se situe à nouveau dans la fourchette adaptée à la plante.
  if (hygroVal  > 30) {
    if (hygroVal < 50) {
      digitalWrite(wateringPin, LOW);
      wateringState = 0;
      digitalWrite(dryingPin, LOW);
      dryingState = 0;
    }
  }
  sensors.requestTemperatures(); // Envoyer l'ordre au capteur de donner la température un peu avant de l'afficher pour éviter les latences potentielles
  // On décode les données du capteur de température pour les avoir sous forme de données que nous pourrions interpréter
  float tempC = sensors.getTempCByIndex(0);

  // ***********************************************partie hygrométrie***********************************************

  moistureValue = (analogRead(moisturePin1) + analogRead(moisturePin2) + analogRead(moisturePin3) + analogRead(moisturePin4));
  moistureUsable = moistureValue / 4;
  hygroVal = map(moistureUsable, dry, wet, 0, 100); // on défini les différents seuils initialisés au départ comme les seuils 0 et 100%
  Serial.print("Humidite du sol: ");
  Serial.print(hygroVal);
  Serial.print(" %, ");

  lcd.clear();
  // On s'occupe de l'humidité
  lcd.setCursor(0, 0);
  lcd.print("Humidite du sol: ");
  lcd.setCursor(0, 1);
  lcd.print(hygroVal);
  lcd.print("%");

  // ***********************************************partie mesure de la température***********************************************

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature du sol:  ");
    Serial.print(tempC);
    Serial.print(" C, ");
    lcd.setCursor(0, 2);
    lcd.print("Temperature du sol: ");
    lcd.setCursor(0, 3);
    lcd.print(tempC);
    lcd.println("*C");
    delay(2000); // définir un arrêt de 2 seconde pour faciliter la lecture sur l'écran
  }
  else
  {
    lcd.setCursor(0, 2);
    lcd.print("Pas de sonde de t*.");
    Serial.println("Error: Could not read temperature data");
  }



  delay(2000); // définir un arrêt de 2 seconde pour faciliter la lecture sur l'écran

}
