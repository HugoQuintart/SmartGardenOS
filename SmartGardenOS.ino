
/*
         __     __   __      __   __________       _______________
        /  /   /  / /  /    /  / /  _______/      /   ________    /
       /  /   /  / /  /    /  / /  /             /  /         /  /
      /  /___/  / /  /    /  / /  /     ______  /  /         /  /
     /  ____   / /  /    /  / /  /     /   __/ /  /         /  /
    /  /   /  / /  /    /  / /  /      /  /   /  /         /  /
   /  /   /  / /  /____/  / /  /______/  /   /  /_________/  /
  /__/   /__/ /__________/ /____________/   /_______________/

  Système d'exploitation du projet de l'exposcience.
*/
// préparer l'ecran lcd
#include <LiquidCrystal.h>
LiquidCrystal lcd(11,9,6,5,4,3);
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

// le capteur de niveau d'eau 0 sur la pin 46
int waterLevel;
//étalonnage de la lecture hygrométrique:
const int wet = 0;// on étalonne les capteurs d'hygrométrie avec une valeur "humide"
const int dry = 1023; // on étalonne les capteurs d'hygrométrie avec une valeur "sèche"

//préparation des variables nécessaires à la lecture hygrométrique:
bool reading = true; // système en mode automatique (par défaut)
int moisturePin1 = A0; // Brancher le capteur d'hygrométrie sur la pin A0
int moisturePin2 = A1; // Brancher le capteur d'hygrométrie sur la pin A1
int moisturePin3 = A2; // Brancher le capteur d'hygrométrie sur la pin A2
int moisturePin4 = A3; // Brancher le capteur d'hygrométrie sur la pin A3
int moisturePin5 = A4; // Brancher le capteur d'hygrométrie sur la pin A4
int moistureValue; // initialiser la variable moistureValue qui correspond à l'humidité du sol
int moistureUsable; // initaliser la variable qui va stocker la valeur de la moyenne des valeurs des capteurs.
int hygroVal; // initialiser la variable qui va socker le pourcentage d'humidité du sol.


//préparation des variables pour l'arrosage:
int dryingPin = 42; // Définir la pin de la pompe de reprise sur la pin digitale 41
int wateringPin = 41; // Brancher la pompe d'arrosage sur la pin digitale 44
int wateringState; //initialiser la variable qui va stocker l'état de la pompe d'arrosage.
int dryingState; // initialiser la variable qui va stocker l'état de la pompe de reprise.

//préparation des variables pour l'analyse de la température du sol

//et le reste des variables:
int statePin = 13; // la led d'état de la carte électronique sera la led sur la pin 13

void setup() {
  moistureValue = 0; // Définir la valeur moistureValue à 0

  pinMode(moisturePin1, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(moisturePin2, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(moisturePin3, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(moisturePin4, INPUT); // Définir la pin du capteur d'hygrométrie comme entrée
  pinMode(wateringPin, OUTPUT); // Définir la pin de la pompe d'arrosage comme sortie
  pinMode(dryingPin, OUTPUT); // Définir la pin de la pompe de reprise en tant que sortie
  pinMode(statePin, OUTPUT); // La led d'état correspond à la pin 13, en sortie de courant

  digitalWrite(statePin, HIGH);

  Serial.begin(9600); // Initialiser le moniteur série en 9600 BAUD.
  lcd.begin(20, 4); // Initialiser l'écran en lcd 20x4 (20 caractères sur 4 lignes)
  dht.begin(); //démarrer la dht (pour mesures de l'humidité et de la température)
  sensors.begin();
  // Séquence de démarrage (nom SE, infos, Créateur)
  lcd.begin(20,4);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("SMART GARDEN");
  lcd.setCursor(0,1);
  lcd.print("BOOTING, PLEASE WAIT");
  lcd.setCursor(0, 3);
  lcd.print("by: HQ Developpement");
  delay(3000);
  lcd.clear();


}

void loop() {

  // ***********************************************partie DHT***********************************************
lcd.clear(); // effacer l'écran lcd
  
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
    lcd.setCursor(0,0);
    lcd.print("Erreur Capteur DHT");
    lcd.setCursor(0,1);
    lcd.print("Verifier la");
    lcd.setCursor(0,2);
    lcd.print("connexion");
    delay(1000);
    lcd.clear();
    return;
  }

  // Calcul la température ressentie. Il calcul est effectué à partir de la température en Fahrenheit
  // On fait la conversion en Celcius dans la foulée
  float hi = dht.computeHeatIndex(f, h);
  // afficher dans le moniteur série
  Serial.print("Humidite exterieure: ");
  Serial.print(h);
  Serial.print(" %, ");
  Serial.print("Temperature exterieure: ");
  Serial.print(t);
  Serial.print(" *C,  ");
  // afficher sur l'écran
  lcd.setCursor(0, 0);
  lcd.print("Humidite exterieure:");
  lcd.setCursor(0,1);
  lcd.print(h);
  lcd.print(" %");
  lcd.setCursor(0, 2);
  lcd.print("Temperature ext: ");
  lcd.setCursor(0,3);
  lcd.print(t);
  lcd.print(" *C");
  delay(2000); // définir un arrêt de 2 secondes pour faciliter la lecture sur l'écran
  

 
  // ***********************************************partie arrosage**********************************************
  moistureValue = (analogRead(moisturePin1) + analogRead(moisturePin2) + analogRead(moisturePin3) + analogRead(moisturePin4) + analogRead(moisturePin5));
  moistureUsable = moistureValue / 5;
  hygroVal = map(moistureUsable, dry, wet, 0, 100); // on défini les différents seuils initialisés au départ comme les seuils 0 et 100%
  
       // enclencher la pompe d'arrosage si l'humidité du sol est inférieure à 65%
      if(hygroVal <= 65){
 Serial.println("wateringPin, HIGH");
        digitalWrite(wateringPin, HIGH);
        wateringState = 1;
      }
      // enclencher la pompe de reprise si l'humidité du sol dépasse les 90%
      if(hygroVal >= 90){
 Serial.println("dryingPin, HIGH");
        digitalWrite(dryingPin, HIGH);
        dryingState = 1;
      }
      // arrêter les pompes quand on se situe à nouveau dans la fourchette adaptée à la plante.
      if (hygroVal  >= 80) {
        if (hygroVal <= 74) {
 Serial.println("wateringPin, LOW");
          digitalWrite(wateringPin, LOW);
          wateringState = 0;
 Serial.println("dryingPin, LOW");
          digitalWrite(dryingPin, LOW);
          dryingState = 0;
        }
      }
      sensors.requestTemperatures(); // Envoyer l'ordre au capteur de donner la température un peu avant de l'afficher pour éviter les latences potentielles
  // On décode les données du capteur de température pour les avoir sous forme de données que nous pourrions interpréter
  float tempC = sensors.getTempCByIndex(0);

   // ***********************************************partie hygrométrie***********************************************

  moistureValue = (analogRead(moisturePin1) + analogRead(moisturePin2) + analogRead(moisturePin3) + analogRead(moisturePin4) + analogRead(moisturePin5));
  moistureUsable = moistureValue / 5;
  hygroVal = map(moistureUsable, dry, wet, 0, 100); // on défini les différents seuils initialisés au départ comme les seuils 0 et 100%
  Serial.print("Humidite du sol: ");
  Serial.print(hygroVal);
  Serial.print(" %, ");
  
  lcd.clear();
  // On s'occupe de l'humidité
  lcd.setCursor(0, 0);
  lcd.print("Humidite du sol: ");
  lcd.setCursor(0,1);
  lcd.print(hygroVal);
  lcd.print("%");
  
  // ***********************************************partie mesure de la température***********************************************

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature du sol:  ");
    Serial.print(tempC);
    Serial.println(" C, ");
    lcd.setCursor(0, 2);
    lcd.print("Temperature du sol: ");
    lcd.setCursor(0,3);
    lcd.print(tempC);
    lcd.print(" *C");
    delay(2000); // définir un arrêt de 2 seconde pour faciliter la lecture sur l'écran
  }
  else
  {
    lcd.setCursor(0,2);
    lcd.print("Pas de sonde de t*.");
    Serial.println("Error: Could not read temperature data");
  }
  
}
