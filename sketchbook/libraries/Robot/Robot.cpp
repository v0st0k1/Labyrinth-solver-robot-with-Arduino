#include "Robot.h"
#include <stdlib.h>
#include <string.h>

/*
  pinMode(9,OUTPUT); //MOTOR IZQ
  pinMode(10,OUTPUT); //MOTOR IZQ
  pinMode(5,OUTPUT); //MOTOR DRC
  pinMode(6,OUTPUT); //MOTOR DRC
*/

Robot::Robot(int a1,int a2, int b1, int b2, int bateria, int arriba, int abajoizq, int abajodrc){
	A1pin = a1;
	A2pin = a2;
	B1pin = b1;
	B2pin = b2;
	pinBateria = bateria;
	cnyArriba = arriba;
	cnyAbajoIzq = abajoizq;
	cnyAbajoDrc = abajodrc;



	Serial.begin(9600);
	Serial1.begin(9600); //bluetooth

	pinMode(A1pin, OUTPUT);
	pinMode(A2pin, OUTPUT);
	pinMode(B1pin, OUTPUT);
	pinMode(B2pin, OUTPUT);
	pinMode(11, OUTPUT);

}

void Robot::move_forward(){
	digitalWrite(A1pin,LOW);
    digitalWrite(A2pin,HIGH);
    digitalWrite(B1pin,LOW);
    digitalWrite(B2pin,HIGH);

}
void Robot::move_backward(){
	digitalWrite(A1pin,HIGH);
    digitalWrite(A2pin,LOW);
    digitalWrite(B1pin,HIGH);
    digitalWrite(B2pin,LOW);
	
}
void Robot::stop(){
	digitalWrite(A1pin,LOW);
    digitalWrite(A2pin,LOW);
	digitalWrite(B1pin,LOW);
    digitalWrite(B2pin,LOW);
}
void Robot::move_right(){
	digitalWrite(A1pin,HIGH);
    digitalWrite(A2pin,LOW);

	digitalWrite(B1pin,LOW);
    digitalWrite(B2pin,HIGH);
	
}
void Robot::move_left(){
	digitalWrite(A1pin,LOW);
    digitalWrite(A2pin,HIGH);

	digitalWrite(B1pin,HIGH);
    digitalWrite(B2pin,LOW);
}

void Robot::move_right_motor(){
	digitalWrite(A1pin,LOW);
    digitalWrite(A2pin,HIGH);

	digitalWrite(B1pin,LOW);
    digitalWrite(B2pin,LOW);
}

void Robot::move_left_motor(){
	digitalWrite(A1pin,LOW);
    digitalWrite(A2pin,LOW);

	digitalWrite(B1pin,LOW);
    digitalWrite(B2pin,HIGH);
}

void Robot::move_servo(int ang){
  digitalWrite(11,LOW); //ponemos en intensidad baja
  digitalWrite(11,HIGH); //comenzamos el pulso
  delayMicroseconds(map(ang,0,180,544,2400));
  digitalWrite(11,LOW);
  delay(20);
}

float Robot::read_battery(){
	int val_tension = analogRead(pinBateria);
	return ((val_tension*0.00488)/4.15)*100;
}

/*
		Elegir cny70:
		arriba = 0
		abajo_izq = 1
		abajo_drc = 2
		si es blanco devuelve true
		si es negro devuelve false
*/
bool Robot::read_cny70(int opt){
	if(opt==0){
		float valor = analogRead(cnyArriba)*0.00488;
		if(valor <= 2.5){ //umbral: blanco o negro?
			return true;
		}else if(valor > 2.5){
			return false;
		}
	}else if(opt==1){
		float valor = analogRead(cnyAbajoIzq)*0.00488;
		if(valor <= 3.5){ //umbral: blanco o negro?
			return true;
		}else if(valor > 3.5){
			return false;
		}
	}else if(opt==2){
		float valor = analogRead(cnyAbajoDrc)*0.00488;
		if(valor <= 3){ //umbral
			return true;
		}else if(valor > 3){
			return false;
		}
	}else{
		Serial.println("ERROR: No se ha elegido el cny70 correcto");
	}
}

float get_bateria(int pinBateria){
	return analogRead(pinBateria)*0.00488;
}

/*
	Ecuacion de la recta para la velocidad segun la tension
	y = 5.9*x - 5.6
	y => velocidad en cm/seg
	x => tension leida de la bateria (max 4.15)
*/
float get_tiempo(float distance, int pinBateria){
	float cm_por_segundo = 5.9*get_bateria(pinBateria) - 5.6;

	return distance / cm_por_segundo;
}

void Robot::alinear_robot(){
	bool alineado = false;
	bool drc_alineado = false;
	bool izq_alineado = false;
	bool linea_encontrada = false;
	while(!alineado){
		bool es_blanco_izq = read_cny70(1);
		bool es_blanco_drc = read_cny70(2);

		if(!linea_encontrada) {
			if(es_blanco_izq && es_blanco_drc) move_forward();
			else linea_encontrada = true; 

		}else{
			if(!drc_alineado){

				while(es_blanco_drc){
					es_blanco_drc = read_cny70(2);
					move_right();
				}
				drc_alineado = true;

			}

			if(!izq_alineado){

				while(es_blanco_izq){
					es_blanco_izq = read_cny70(1);
					move_left();
				}
				izq_alineado = true;
			}
		}

		if(izq_alineado && drc_alineado) alineado = true;		  
	}
}

/*
0 -> no pared derecha
1 -> no pared frente
2 -> no pared izquierda
3 -> rodeado de paredes
*/
int Robot::check_wall(){
	int dir = -1;

	delay(500);
	move_servo(180);
	float d = read_ultrasound();
	if(d < 30){
		delay(500);
		move_servo(90);
		d = read_ultrasound();
		if(d < 30){
			delay(500);
			move_servo(0);
			d = read_ultrasound();
			if(d < 30){
				dir = 3;
			}else{
				dir = 2;
			}
		}else{
			dir = 1;
		}
	}else{
		dir = 0;
	}

	return dir;
}

void Robot::change_direction(int dir){
	if(dir==0){
		float time = get_tiempo(7.2, pinBateria);
		move_right();
		delay(time*1000);
		send_bluetooth("g0");
	}else if(dir==1){
		//SIGUE RECTO
	}else if(dir==2){
		float time = get_tiempo(7.2, pinBateria);
		move_left();
		delay(time*1000);
		send_bluetooth("g1");
	}else if(dir==3){
		float time = get_tiempo(14, pinBateria);
		move_right();
		delay(time*1000);
		send_bluetooth("g2");
	}
}

bool Robot::check_final_cell(){
	bool es_blanco_izq = read_cny70(1);
	bool es_blanco_drc = read_cny70(2);
	bool es_blanco_abajo = read_cny70(0);

	if(!es_blanco_izq && !es_blanco_abajo && !es_blanco_drc) return true; else return false;
}

float Robot::read_ultrasound(){
  pinMode(A3,OUTPUT);
  digitalWrite(A3,LOW);
  delayMicroseconds(5);

  digitalWrite(A3,HIGH);
  delayMicroseconds(10);
  digitalWrite(A3,LOW);

  pinMode(A3, INPUT);
  unsigned long time_bounce = pulseIn(A3,HIGH);

  float distance = 0.017*time_bounce;

  return distance;
  //Serial.print("Distancia: ");
  //Serial.print(distance);
  //Serial.println("cm");

  //delay(1000);
}

void Robot::send_bluetooth(char* str){
	
	Serial1.write(str);
}

void Robot::send_battery(){
	float aux = read_battery();
	char res[10];
  	dtostrf(aux,4,2, res);
  	send_bluetooth("b");
    send_bluetooth(res);
    send_bluetooth("\n");
}

void Robot::send_velocity(){
	float velocity = 5.9*get_bateria(pinBateria) - 5.6;
	char res[10];
  	dtostrf(velocity,4,2, res);
  	send_bluetooth("m");
    send_bluetooth(res);
    send_bluetooth("\n");
}

bool Robot::algorithm(){
	// *** Empieza en el centro de la celda ***
	send_battery(); //estado de la bateria

	//checkear paredes
	int direccion = check_wall();

	//cambiar direccion
	change_direction(direccion);

	//moverse hasta alinearse con la linea negra
	alinear_robot();
	
	//moverse hasta el centro
	float time = get_tiempo(9, pinBateria);
	send_velocity();
	move_forward();
	delay(time*1000);
	stop();
	send_bluetooth("m0\n"); //SE PUEDE QUITAR Y USAR SOLO EL SEND(C)
	send_bluetooth("c\n");

	//checkear si se ha llegado al final
	bool algoritmo_terminado = check_final_cell();

	return algoritmo_terminado;

}