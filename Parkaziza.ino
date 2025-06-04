#include <rgb_lcd.h>
#include <SeeedRFID.h>
#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>

rgb_lcd lcd;
SeeedRFID rfid(10, 11);
Servo entryServo;
Servo aServo;
Servo bServo;
Servo exitServo;

int NOMBRE_PLACES_DISPONIBLES_PARKING = 12; // Définir le nombres de places disponibles du parking

long IDENTIFIANT_CARTE_VOITURE_ELECTRIQUE = 9039565; // Associer un numéro de carte ou badge RFID à la voiture électrique
long IDENTIFIANT_CARTE_VOITURE_THERMIQUE = 9053446; // Associer un numéro de carte ou badge RFID à la voiture thermique

int lastButtonState = 0;

void setup() 
{
    Serial.begin(9600);
    
    pinMode(2, INPUT);
    pinMode(6, INPUT);
    pinMode(4, OUTPUT);
    pinMode(8, OUTPUT);
    
    digitalWrite(4, HIGH);
    
    lcd.begin(16, 2);
    entryServo.attach(7);
    entryServo.write(0);
    aServo.attach(6);
    aServo.write(0);
    bServo.attach(5);
    bServo.write(0);
    exitServo.attach(4);
    exitServo.write(0);
    
    delay(1000);
}

void loop()
{
    lcd.home();
    lcd.print(NOMBRE_PLACES_DISPONIBLES_PARKING);
    lcd.print(" places         "); // Afficher le nombre de places disponibles
    
    if (NOMBRE_PLACES_DISPONIBLES_PARKING > 0) // S'il reste des places
    {
        if (rfid.isAvailable()) // Si une carte ou badge RFID est détecté
        {
            lcd.setCursor(0, 1);
            lcd.print("Lecture vehicule"); // Afficher "Lecture véhicule"
            
            delay(1000); // Attendre 1 seconde
            
            if (rfid.cardNumber() == IDENTIFIANT_CARTE_VOITURE_ELECTRIQUE || rfid.cardNumber() == IDENTIFIANT_CARTE_VOITURE_THERMIQUE)
            {
                digitalWrite(4, LOW); // Éteindre le feu rouge
                digitalWrite(8, HIGH); // Allumer le feu vert
                
                entryServo.write(90); // Ouvrir la barrière principale (position à 90°)

                if (rfid.cardNumber() == IDENTIFIANT_CARTE_VOITURE_ELECTRIQUE) // Si c'est une voiture électrique
                {
                    aServo.write(90); // Ouvrir la barrière de l'étage 0 (position à 90°)
                    lcd.setCursor(0, 1);
                    lcd.print("Etage 0         "); // Afficher "Étage 0"
                } else
                {
                    bServo.write(90); // Ouvrir la barrière de l'étage -1 (position à 90°)
                    lcd.setCursor(0, 1);
                    lcd.print("Etage -1        "); // Afficher "Étage -1"
                }
                
                delay(5000); // Attendre 5 secondes (temps pour laisser passer la voiture)
            
                digitalWrite(8, LOW);
                digitalWrite(4, HIGH);
                
                NOMBRE_PLACES_DISPONIBLES_PARKING = NOMBRE_PLACES_DISPONIBLES_PARKING - 1; // Enlever une place disponible
                
                delay(1000); // Attendre 1 seconde
                
                entryServo.write(0); // Fermer la barrière (position à 0°)
                aServo.write(0);     // ""
                bServo.write(0);     // ""

                while (rfid.isAvailable())
                {
                    rfid.data(); // Nettoyer les restes de données non-lues
                }
            }
        } else
        {
            lcd.setCursor(0, 1);
            lcd.print("Attente vehicule"); // Afficher "Attente véhicule"
        }
    } else
    {
        lcd.setCursor(0, 1);
        lcd.print("Parking complet"); // Afficher "Parking complet"
    }
    
    if (digitalRead(2) == 1 && lastButtonState == 0) // Si le botuon de sortie est pressé
    {
        aServo.write(90); // Ouvrir la barrière de sortie (position à 90°)
        
        delay(5000); // Attendre 5 secondes (temps pour laisser passer la voiture)

        aServo.write(0); // Fermer la barrière de sortie (position à 0°)

        NOMBRE_PLACES_DISPONIBLES_PARKING = NOMBRE_PLACES_DISPONIBLES_PARKING + 1; // Ajouter une place disponible
    }
    
    lastButtonState = digitalRead(2);
}
