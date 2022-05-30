 #include <Servo.h>
Servo servo;
#define PIN_servo_mot 2 // PIN PWM

// pin moteur
#define mot_pwm_A 6
#define mot_pwm_D 8
#define mot_pwm_G 3

#define mot_sens_A 7
#define mot_sens_D 9
#define mot_sens_G 4

// couleur d equipe
#define pin_side 53
int side;//coté sélectionné (1=Violet, -1= Jaune)
int side_value;//état de l'interrupteur de selection (LOW ou HIGH)

// PINs sur lesquelles les encodeurs sont branchés
#define interruptA 18
#define interruptG 19
#define interruptD 20

// PIN reliée à l'INT ultra sonore
#define interrupt_US 21
#define US_AV 32
#define US_AR 33

// LED
#define PIN_LED_R 12
#define PIN_LED_G 11
#define PIN_LED_B 10

// tirette
#define Pin_tirette 42

// compteur de tic
#define tic_metre_AV 1300
#define tic_metre_G 1480

volatile int comptD = 0; // necessaire de les mettre en 'volatile' sinon ça empêche le bon fonctionnement
volatile int comptA = 0;

int Use_US = 1;
int Use_US_AV = 1;

// Vitesses des moteurs -> NE DOIT PAS DEPASSER 255/2 (moteurs 6V ; batterie 12V)
int VA;
int VD;
int VG;

volatile unsigned long temps;

// ------------------------------------------------------------------------------------------------------------------------------------------------------------- SETUP

void setup() {
  // PINs des moteurs
  pinMode(mot_pwm_A, OUTPUT);
  pinMode(mot_pwm_G, OUTPUT);
  pinMode(mot_pwm_D, OUTPUT);

  pinMode(mot_sens_A, OUTPUT);
  pinMode(mot_sens_G, OUTPUT);
  pinMode(mot_sens_D, OUTPUT);

  analogWrite(mot_pwm_G, 0);
  analogWrite(mot_pwm_A, 0);
  analogWrite(mot_pwm_D, 0);

  digitalWrite(mot_sens_A, LOW);
  digitalWrite(mot_sens_D, LOW);
  digitalWrite(mot_sens_G, LOW);

  // PINs des encodeurs
  
  pinMode(interruptD, INPUT_PULLUP);
  pinMode(interruptA, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptD), augmenter_comptD, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptA), augmenter_comptA, CHANGE);


  // PIN d'interruption des US
  //pinMode(interrupt_US, INPUT);
  pinMode(US_AV, INPUT);
  pinMode(US_AR, INPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_US), obstacle_proche, RISING); // front montant

  // Initialise les broches
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);

  // tirette
  pinMode(Pin_tirette, INPUT_PULLUP);

  // couleur de l'equipe
  pinMode(pin_side, INPUT);

  temps = millis();

  Use_US_AV = 1;
  Use_US = 1;

  // PIN du servo-mot
  servo.attach(PIN_servo_mot);
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------- LOOP

void loop() {
  servo.write(30);
  while (digitalRead(Pin_tirette) == LOW)
  { // en attendant la tirette
    side=SideSelect();
  }

  digitalWrite(PIN_LED_R, 0);//BLUE BUSY
  digitalWrite(PIN_LED_G, 0);
  digitalWrite(PIN_LED_B, 100);


  //*********************************mouvements***************************************
  deplacement(1, 0, (int)(0.3 * tic_metre_AV)); // avant
  temps = millis();
  while ((millis() - temps) < 200)
  {
  }

servo.write(110);//sortie du bras
  temps = millis();
    while ((millis() - temps) < 200)
  {
  }
  
  deplacement(side, 90, (int)(0.3 * tic_metre_G)); // coté
  temps = millis();
  while ((millis() - temps) < 200)
  {
    
  }
  
  servo.write(30);//rentrée bras
  temps = millis();
    while ((millis() - temps) < 200)
  {
    
  }
  deplacement(-1, 0, (int)(0.3 * tic_metre_AV)); // avant
  temps = millis();
  while ((millis() - temps) < 200)
  {
  }
  
  deplacement(-side, 90, (int)(0.3 * tic_metre_G)); // coté
  temps = millis();
  while ((millis() - temps) < 200)
  {
    
  }
  
  //****************************fin mouvements***************************************

  /*analogWrite(mot_pwm_G, 0);
  analogWrite(mot_pwm_A, 0);
  analogWrite(mot_pwm_D, 0);

  digitalWrite(PIN_LED_R, 250);
  digitalWrite(PIN_LED_G, 100);
  digitalWrite(PIN_LED_B, 100);

  while (1) {} // on s arrete*/
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ fonctions
// INCREMENTATION DES COMPTEURS
void augmenter_comptD() { // incrémente le compteur droit
  comptD++;
}

void augmenter_comptA() { // incrémente le compteur arrière
  comptA++;
}

void deplacement(int sens, int dir, int tick) {
  //sens définit en avant (1) ou en arrière (-1)
  //dir indique la direction du mouvement (0 tout droit, 90 sur gauche)
  //tick le nombre d'impulsion des encodeurs
  comptD = 0;
  comptA = 0;
  if (dir == 90) {
    VA = 110;
    VD = 55; //45
    VG = 56;
    if (sens == 1) {
      digitalWrite(mot_sens_A, LOW);
      digitalWrite(mot_sens_D, LOW);
      digitalWrite(mot_sens_G, HIGH);
    }
    if (sens == (-1)) {
      digitalWrite(mot_sens_A, HIGH);
      digitalWrite(mot_sens_D, HIGH);
      digitalWrite(mot_sens_G, LOW);
    }
  }
  //avant
  else if (dir == 0) {

    VA =  0;
    VD = 75;
    VG = 76;
    if (sens == 1) {
      digitalWrite(mot_sens_A, LOW);
      digitalWrite(mot_sens_D, LOW);
      digitalWrite(mot_sens_G, LOW);
    }
    if (sens == (-1)) {
      digitalWrite(mot_sens_A, LOW);
      digitalWrite(mot_sens_D, HIGH);
      digitalWrite(mot_sens_G, HIGH);
    }

  }

  analogWrite(mot_pwm_A, VA);
  analogWrite(mot_pwm_D, VD);
  analogWrite(mot_pwm_G, VG);

  if (dir == 0) {
    while (comptD <= tick) {
      analogWrite(mot_pwm_A, VA);
      analogWrite(mot_pwm_D, VD);
      analogWrite(mot_pwm_G, VG);
    }
  }
  else if (dir == 90) {
    while (comptA <= tick) {
      analogWrite(mot_pwm_A, VA);
      analogWrite(mot_pwm_D, VD);
      analogWrite(mot_pwm_G, VG);
    }
  }
  analogWrite(mot_pwm_A, 0);
  analogWrite(mot_pwm_D, 0);
  analogWrite(mot_pwm_G, 0);
  return;
}

void obstacle_proche() {
  while ((((digitalRead(US_AV) == HIGH) && Use_US_AV == 1) || (digitalRead(US_AR) == HIGH)) && (Use_US == 1)) {
    digitalWrite(PIN_LED_R, 100);
    digitalWrite(PIN_LED_G, 0);
    digitalWrite(PIN_LED_B, 0);
    analogWrite(mot_pwm_G, 0);
    analogWrite(mot_pwm_A, 0);
    analogWrite(mot_pwm_D, 0);
  }
  if ((Use_US == 1) || (Use_US_AV == 0)) {
    digitalWrite(PIN_LED_R, 0);
    digitalWrite(PIN_LED_G, 0);
    digitalWrite(PIN_LED_B, 100);
  }
}

int SideSelect(){
  side_value = digitalRead(pin_side);
  if (side_value == LOW){
    //equipe jaune
    
    analogWrite(PIN_LED_R, 200);
    analogWrite(PIN_LED_G, 70);
    analogWrite(PIN_LED_B, 00);
    return -1;
  }
  else{
    //equipe violette
//    digitalWrite(Rpin, 138);
//    digitalWrite(Gpin, 43);
//    digitalWrite(Bpin, 226);
    digitalWrite(PIN_LED_R, 70);
    digitalWrite(PIN_LED_G, 0);
    digitalWrite(PIN_LED_B, 70);
    return 1;
  }
  
}
