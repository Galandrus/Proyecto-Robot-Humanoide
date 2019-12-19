/****************************************** Incluyo las librerias ******************************************/
#include "Adafruit_PWMServoDriver.h"

/****************************************** Declaro e inicializo el PWM ******************************************/
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

/****************************************** Defino constantes ******************************************/
#define INCR    5     // "angle-step", from current angle to the destination angle
#define INCRDEL 20    // "time-step", ms delay for each angle-step
#define DELAY_ENTRE_SERVOS 250 

#define HEAD    99  // Head  145/635/25/145/85
#define LELBOW  0   // Left elbow articulation
#define LSHLDRA 1   // Left shoulder articulation with arm
#define LSHLDRT 2   // Left shoulder articulation with torso
#define LHIP    3   // Left hip articulation
#define LTHIGH  4   // Left thigh articulation
#define LKNEE   5   // Left knee articulation
#define LANKLE  6   // Left ankle articulation
#define LFOOT   7   // Left foot articulation
#define RFOOT   8   // Right foot articulation
#define RANKLE  9   // Right ankle articulation
#define RKNEE   10  // Right knee articulation
#define RTHIGH  11  // Right thigh articulation
#define RHIP    12  // Right hip articulation
#define RSHLDRT 13  // Right shoulder articulation with torso
#define RSHLDRA 14  // Right shoulder articulation with arm
#define RELBOW  15  // Right elbow articulation


#define MAX_SERVOS 16

/****************************************** Defino variables ******************************************/
// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
// "Minimum" and "Maximum" pulse lengths for each servo. Indexes correspond to constants above
int servoMin[] =  {145, 145,  120,  145,  145,  145,  145,  145,  145,  145,  145,  145,  145,  160,  145, 145 };
int servoMax[] =  {640, 640,  640,  640,  640,  640,  640,  635,  635,  640,  640,  640,  640,  630,  640, 640 };
int anguloMin[] = { 80,  15,    5,   80,   75,   85,   35,  60 ,  60 ,   70,   50,    5,   60,   65,    5,  70 };
int anguloMax[] = { 90, 160,  110,  100,  145,  110,   85,  100,  100,  120,   75,   75,   75,  175,  150,  80 };
int posHome[] =   { 90, 160,   85,   80,  110,   85,   60,  80 ,  80 ,  100,   75,   45,   75,   75,    5,  70 };
//                 00   01     02    03    04    05    06    07    08    09    10    11    12    13    14   15

// Last values for each servo/articulation. Needed for "natural movements"
int lastVal[MAX_SERVOS] = {};

//Mods
int funcionPE;
int modo;
int pasos;

//Inputs
int servos[MAX_SERVOS] = {};
int angles[MAX_SERVOS] = {}; 
int posArray;

/****************************************** SETUP ******************************************/
void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(1000); // for parseInt()...
                           //  Serial.println("Go");
  Serial.println("Ingrese :");
  Serial.println("1 SERVO ANGULO");
  Serial.println("2 FUNCION");
  Serial.println("  1 - Estabilizar");
  Serial.println("  2 - Saludar");
  Serial.println("  3 - Decir que no");
  Serial.println("  4 - Dar la mano");
  Serial.println("  5 - Dab");
  Serial.println("  6 - Onda");
  Serial.println("  7 - Arigato");
  Serial.println("  8 - Chuchuwa");
  Serial.println("  9 - Sentadillas");
  Serial.println("3 CAMINAR");
  Serial.println("  2 - 4 pasos");
  Serial.println("  3 - 6 pasos");
  Serial.println("  4 - 8 pasos");
  Serial.println("  5 - 10 pasos");

  pwm.begin();
  pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates
  
  posArray = 0;
  pasos = 0;
  
  estabilizar();
}

/****************************************** LOOP ******************************************/
void loop()
{
  modo = readMonitorSerie();
  if (modo == 1)
  {
    if(posArray != 0 && servos[0] != -1){
      setAngleParallel();
      Serial.println("Servo: ");
      Serial.println(servos[0]);
      Serial.println("Angulo: ");
      Serial.println(angles[0]);
    }
  }
  else if (modo == 2)
  {
    switch (funcionPE){
      case 1:
        Serial.println("Bhasky se estabiliza");
        estabilizar();
        break;
      case 2:
        Serial.println("Bhasky saluda");
        estabilizar();
        saludar();
        break;
      case 3:
        Serial.println("Bhasky dice no");
        estabilizar();
        no();
        break;
      case 4:
        Serial.println("Bhasky da la mano");
        estabilizar();
        darLaMano();
        break;
      case 5:
        Serial.println("Bhasky hace el dab");
        dab();
        estabilizar();
        break;
      case 6:
        Serial.println("Bhasky hace la onda");
        estabilizar();
        onda();
        break;
      case 7:
        Serial.println("Bhasky hace arigato");
        estabilizar();
        arigato();
        break;
      case 8:
        Serial.println("Bhasky hace chuchuwa");
        estabilizar();
        chuchuwa();
        break;
      case 9:
        Serial.println("Bhasky hace sentadillas");
        estabilizar();
        sentadilla();
        break;
      default:
        Serial.println("Bhasky no hace nada");
        break;
    }
  }
  else if (modo == 3)
  {
    Serial.println("Bhasky camina");
    caminar();
  }
}

/****************************************** readMonitorSerie ******************************************/
int readMonitorSerie()
{
  int modoF;
  int servo, angle;
  if (Serial.available())
  {
    modoF = Serial.parseInt();
    if (modoF == 1)
    {
      cleanInputs();
      servo = Serial.parseInt();
      angle = Serial.parseInt();
      if ((servo >= 0) && (servo < MAX_SERVOS)){
        addInput(servo,angle);
      }
    }
    else if (modoF == 2)
    {
      funcionPE = Serial.parseInt();
    }
    else if (modoF == 3){
      pasos = Serial.parseInt();
      if (pasos < 2) pasos = 2;
      else if (pasos > 5) pasos = 5;
    }
  }
  return modoF;
}

/****************************************** isAngleValid ******************************************/
boolean isAngleValid(){
  for(int i=0;i<posArray;i++){
     if ((angles[i] < anguloMin[servos[i]]) || (angles[i] > anguloMax[servos[i]])){
       return false;
     }
  }
  return true;
}

/****************************************** setAngleParallel ******************************************/
void setAngleParallel()
{
  int i, pulselen;
  int j=0;
  int cantComplet=0;
  int speed = 3;
  int incV[posArray]={};
  boolean notFinish = true;
  

  if(isAngleValid())
  {   
    for(i=0;i<posArray;i++){
      if (lastVal[servos[i]] > angles[i])
        incV[i] = -1;
      else
        incV[i] = 1; 
      lastVal[servos[i]]+=incV[i];
    }
    while(notFinish){
      if (servos[j] != -1 && j<posArray){
        i = lastVal[servos[j]];
        if ((incV[j] == 1 && i <= angles[j] ) || (incV[j] == -1 && i >= angles[j])){
          pulselen = map(i, 0, 180, servoMin[servos[j]], servoMax[servos[j]]);
          pwm.setPWM(servos[j], 0, pulselen);
          delay(INCRDEL);
          i += incV[j]*speed;
          lastVal[servos[j]] = i;
        } else {
          lastVal[servos[j]] = angles[j];
          if (servos[j] < 3 || servos[j] > 12)
            limitarBrazos();
          servos[j] = -1;
          cantComplet++;
          if (cantComplet == posArray){
            notFinish = false;
          }
        }
      }else{
        if (j == posArray){
          j=-1;
        }
      }
      j++;
    }
  }
  delay(DELAY_ENTRE_SERVOS);
}

/****************************************** Manejo de Inputs ******************************************/
void cleanInputs(){
  int i;
  for(i=0;i<MAX_SERVOS;i++){
    servos[i]=-1;
    angles[i]=0;
  }
  posArray=0;
}

void addInput(int servo, int angle){
  servos[posArray]=servo;
  angles[posArray]=angle;
  posArray++;
}

/****************************************** Inicializar LastVal ******************************************/
void inicializarLastVal(){
  for(int i=0;i<MAX_SERVOS;i++){
    lastVal[i] = posHome[i] - 1;
  }
}

/****************************************** Limitar Brazos ******************************************/
void limitarBrazos(){
  if ( lastVal[RSHLDRT] > 109 || lastVal[RSHLDRA] > 34 ){
    anguloMin[RELBOW] = 5;
    if ( lastVal[RELBOW] < posHome[RELBOW] ){
      if ( lastVal[RSHLDRT] > 109 )
        anguloMin[RSHLDRA] = 5;
      if ( lastVal[RSHLDRA] > 34 )
        anguloMin[RSHLDRT] = 65;
    }
  } else {
    anguloMin[RELBOW] = posHome[RELBOW];
  }
  
  if (lastVal[LSHLDRT] < 46 || lastVal[LSHLDRA] < 131  ){
    anguloMax[LELBOW] = 165;
    if( lastVal[LELBOW] > posHome[LELBOW] ){
      if ( lastVal[LSHLDRT] < 46 )
        anguloMax[LSHLDRA] = 160;
      if ( lastVal[LSHLDRA] < 131 )
        anguloMax[LSHLDRT] = 110; 
    }
  } else {
    anguloMax[LELBOW] = posHome[LELBOW];
  }

  if (lastVal[RELBOW] < posHome[RELBOW]){
    if (lastVal[RSHLDRT] > 109 && lastVal[RSHLDRA] < 36 )
      anguloMin[RSHLDRT] = 110;
    if (lastVal[RSHLDRA] > 34 && lastVal[RSHLDRT] < 111 )
      anguloMin[RSHLDRA] = 35;
  } else {
    anguloMin[RSHLDRT] = 65;
    anguloMin[RSHLDRA] = 5;
  }

  if (lastVal[LELBOW] > posHome[LELBOW]){
    if (lastVal[LSHLDRT] < 46 && lastVal[LSHLDRA] > 129)
      anguloMax[LSHLDRT] = 45;
    if (lastVal[LSHLDRA] < 131 && lastVal[LSHLDRT] > 44)
      anguloMax[LSHLDRA] = 130;
  } else {
    anguloMax[LSHLDRT] = 110;
    anguloMax[LSHLDRA] = 160;
  }
}

/****************************************** Defino las funciones del robot ******************************************/
/****************************************** Saludar ******************************************/
void saludar(){
  cleanInputs();
  addInput(RSHLDRA,35);
  addInput(LSHLDRA,120);
  setAngleParallel();

  cleanInputs();
  addInput(LSHLDRA,65);
  addInput(RELBOW,5);
  setAngleParallel();
  
  for(int i=0;i<3;i++){
    cleanInputs();
    addInput(LSHLDRA,25);
    setAngleParallel();

    cleanInputs();
    addInput(LSHLDRA,55);
    setAngleParallel();
  }

  cleanInputs();
  addInput(LSHLDRA,135);
  addInput(RELBOW,posHome[RELBOW]);
  setAngleParallel();

  cleanInputs();
  addInput(LSHLDRA,posHome[LSHLDRA]);
  addInput(RSHLDRA,posHome[RSHLDRA]);
  setAngleParallel();
}

/****************************************** Decir que No ******************************************/
void no()
{
  cleanInputs();
  addInput(RSHLDRA,35);
  addInput(LSHLDRA,130);
  setAngleParallel();

  cleanInputs();
  addInput(RELBOW,5);
  addInput(LELBOW,150);
  setAngleParallel();

  delay(2000);

  cleanInputs();
  addInput(RELBOW,posHome[RELBOW]);
  addInput(LELBOW,posHome[LELBOW]);
  setAngleParallel();
  
  cleanInputs();
  addInput(RSHLDRA,posHome[RSHLDRA]);
  addInput(LSHLDRA,posHome[LSHLDRA]);
  setAngleParallel();
}

/****************************************** Dar La Mano ******************************************/
void darLaMano(){

  cleanInputs();
  addInput(RSHLDRT,150);
  setAngleParallel();

  cleanInputs();
  addInput(RSHLDRT,130);
  setAngleParallel();    


  cleanInputs();
  addInput(RSHLDRT,150);
  setAngleParallel();

  cleanInputs();
  addInput(RSHLDRT,130);
  setAngleParallel(); 
    
  cleanInputs();
  addInput(RSHLDRT,150);
  setAngleParallel();

  cleanInputs();
  addInput(RSHLDRT,posHome[RSHLDRT]);
  setAngleParallel(); 
}
/****************************************** Caminar ******************************************/
void caminar(){
  for(int i=0;i<pasos;i++){

    //Paso Izquierdo
    cleanInputs();
    addInput(RTHIGH,posHome[RTHIGH]);
    addInput(LTHIGH, 135);
    addInput(LANKLE, 85);
    addInput(RANKLE, 120);
    addInput(RFOOT, 90);
    addInput(LFOOT,posHome[LFOOT]);
    setAngleParallel();

    //Paso Derecho
    cleanInputs();
    addInput(LTHIGH,posHome[LTHIGH]);
    addInput(RTHIGH, 15);
    addInput(LANKLE, 35);
    addInput(RANKLE, 80);
    addInput(LFOOT, 75);
    addInput(RFOOT,posHome[RFOOT]);
    setAngleParallel();
  }

  cleanInputs();
  addInput(LFOOT,posHome[LFOOT]);
  addInput(RTHIGH,posHome[RTHIGH]);
  addInput(RFOOT,posHome[RFOOT]);
  addInput(LTHIGH,posHome[LTHIGH]);
  addInput(RANKLE,posHome[RANKLE]);
  addInput(LANKLE,posHome[LANKLE]);
  setAngleParallel();
}

/****************************************** Estabilizar ******************************************/
void estabilizar()
{
  if (lastVal[0] == 0)
    inicializarLastVal();

  cleanInputs();
  addInput(RELBOW,posHome[RELBOW]);
  addInput(LELBOW,posHome[LELBOW]);
  setAngleParallel(); 

  cleanInputs();
  addInput(RSHLDRA,posHome[RSHLDRA]);
  addInput(LSHLDRA,posHome[LSHLDRA]);
  setAngleParallel(); 

  cleanInputs();
  addInput(RSHLDRT,posHome[RSHLDRT]);
  addInput(LSHLDRT,posHome[LSHLDRT]);
  setAngleParallel(); 

  cleanInputs();
  addInput(RHIP,posHome[RHIP]);
  addInput(LHIP,posHome[LHIP]);
  setAngleParallel();

  cleanInputs();
  addInput(RTHIGH,posHome[RTHIGH]);
  addInput(LTHIGH,posHome[LTHIGH]);
  setAngleParallel();

  cleanInputs();
  addInput(RKNEE,posHome[RKNEE]);
  addInput(LKNEE,posHome[LKNEE]);
  setAngleParallel();

  cleanInputs();
  addInput(RANKLE,posHome[RANKLE]);
  addInput(LANKLE,posHome[LANKLE]);
  setAngleParallel(); 

  cleanInputs();
  addInput(RFOOT,posHome[RFOOT]);
  addInput(LFOOT,posHome[LFOOT]);
  setAngleParallel(); 
}

/****************************************** Dab ******************************************/
void dab(){
  
  cleanInputs();
  addInput(RSHLDRT, 175);
  addInput(LSHLDRA, 80);
  setAngleParallel();

  cleanInputs();
  addInput(RELBOW, 40);
  addInput(LSHLDRA, 35);
  setAngleParallel();

  cleanInputs();
  addInput(RTHIGH, 30);
  addInput(RELBOW, 5);
  setAngleParallel();

  delay(1000);

  cleanInputs();
  addInput(RTHIGH, posHome[RTHIGH]);
  addInput(RELBOW, 20);
  setAngleParallel();

  cleanInputs();
  addInput(RELBOW, posHome[RELBOW]);
  addInput(LSHLDRA, 80);
  setAngleParallel();

  cleanInputs();
  addInput(RSHLDRT, posHome[RSHLDRT]);
  addInput(LSHLDRA, posHome[LSHLDRA]);
  setAngleParallel();  

}

/****************************************** Onda ******************************************/
void onda(){
  
  //Sube los brazos
  cleanInputs();
  addInput(RSHLDRA, 90);
  addInput(RSHLDRT, 65);
  addInput(LSHLDRA, 75);
  addInput(LSHLDRT, 90);
  setAngleParallel();

  cleanInputs();
  addInput(RELBOW, 35);
  addInput(RSHLDRA, 110);
  addInput(LSHLDRA, 85);
  setAngleParallel();

  cleanInputs();
  addInput(RELBOW, 70);
  addInput(RSHLDRA, 65);
  addInput(LSHLDRA, 50);
  addInput(LELBOW, 125);
  setAngleParallel();

  cleanInputs();
  addInput(LELBOW, 90);
  addInput(LSHLDRA, 85);
  addInput(RSHLDRA, 110);
  addInput(RELBOW, 35);
  setAngleParallel();

  cleanInputs();
  addInput(RELBOW,posHome[RELBOW]);
  addInput(LELBOW,posHome[LELBOW]);
  setAngleParallel(); 

  cleanInputs();
  addInput(RSHLDRA,posHome[RSHLDRA]);
  addInput(LSHLDRA,posHome[LSHLDRA]);
  setAngleParallel(); 

  cleanInputs();
  addInput(RSHLDRT,posHome[RSHLDRT]);
  addInput(LSHLDRT,posHome[LSHLDRT]);
  setAngleParallel(); 
}

/****************************************** Arigato ******************************************/
void arigato(){

  cleanInputs();
  addInput(LSHLDRT, 45);
  addInput(RSHLDRT, 120);
  setAngleParallel();

  cleanInputs();
  addInput(LSHLDRT, 5);
  addInput(LELBOW, 160);
  addInput(RSHLDRT, 160);
  addInput(RELBOW, 5);
  setAngleParallel();

  cleanInputs();
  addInput(LTHIGH, 125);
  addInput(RTHIGH, 30);
  setAngleParallel();

  delay(1000);

  cleanInputs();
  addInput(LTHIGH, posHome[LTHIGH]);
  addInput(RTHIGH, posHome[RTHIGH]);
  setAngleParallel();

  cleanInputs();
  addInput(LSHLDRT, 45);
  addInput(LELBOW, posHome[LELBOW]);
  addInput(RSHLDRT, 120);
  addInput(RELBOW, posHome[RELBOW]);
  setAngleParallel();

  cleanInputs();
  addInput(LSHLDRT, posHome[LSHLDRT]);
  addInput(RSHLDRT, posHome[RSHLDRT]);
  setAngleParallel();
}

/****************************************** Chuchuwa ******************************************/
void chuchuwa(){

  cleanInputs();
  addInput(LSHLDRT, 15);
  addInput(RSHLDRT, 150);
  addInput(RELBOW, 65);
  addInput(LELBOW, 95);
  setAngleParallel();

  for(int i=0;i<3;i++){
    cleanInputs();
    addInput(LSHLDRT, 5);
    addInput(RTHIGH, 35);
    addInput(LTHIGH, posHome[LTHIGH]);
    addInput(RSHLDRT, 140);
    setAngleParallel();

    cleanInputs();
    addInput(LSHLDRT, 25);
    addInput(LTHIGH, 120);
    addInput(RTHIGH, posHome[RTHIGH]);
    addInput(RSHLDRT, 170);
    setAngleParallel();
  }

  cleanInputs();
  addInput(LTHIGH, posHome[LTHIGH]);
  addInput(RTHIGH, posHome[RTHIGH]);
  setAngleParallel();

  cleanInputs();
  addInput(LELBOW, posHome[LELBOW]);
  addInput(RELBOW, posHome[RELBOW]);
  setAngleParallel();

  cleanInputs();
  addInput(LSHLDRT, posHome[LSHLDRT]);
  addInput(RSHLDRT, posHome[RSHLDRT]);
  setAngleParallel();
}

/****************************************** Sentadilla ******************************************/
void sentadilla(){
  
  cleanInputs();
  addInput(LSHLDRT, 15);
  addInput(RSHLDRT, 155);
  setAngleParallel();

  for(int i=0;i<3;i++){
    cleanInputs();
    addInput(RTHIGH, 5);
    addInput(LTHIGH, 145);
    addInput(LKNEE, 110);
    addInput(RKNEE, 50);
    addInput(LANKLE, 45);
    addInput(RANKLE, 110);
    setAngleParallel();

    cleanInputs();
    addInput(LKNEE, posHome[LKNEE]);
    addInput(RKNEE, posHome[RKNEE]);
    addInput(LTHIGH, posHome[LTHIGH]);
    addInput(RTHIGH, posHome[RTHIGH]);
    addInput(LANKLE, posHome[LANKLE]);
    addInput(RANKLE, posHome[RANKLE]);
    setAngleParallel();
  }
  
  cleanInputs();
  addInput(LSHLDRT, posHome[LSHLDRT]);
  addInput(RSHLDRT, posHome[RSHLDRT]);
  addInput(RELBOW, posHome[RELBOW]);
  addInput(LELBOW, posHome[LELBOW]);
  setAngleParallel();
}
