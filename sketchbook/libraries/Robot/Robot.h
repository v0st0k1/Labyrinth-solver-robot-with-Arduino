#ifndef Robot_h
#define Robot_h

#include "Arduino.h"

/*
	Ecuacion de la recta para la velocidad segun la tension
	y = 5.9*x - 5.6
	y => distancia
	x => tension leida de la bateria (max 4.15)
*/


class Robot{
	public:
		Robot(int a1,int a2,int b1,int b2, int bateria, int arriba, int abajoizq, int abajodrc);

		/*
		Funciones para controlar el movimiento del robot
		*/
		void move_forward();
		void move_backward();
		void stop();
		void move_right();
		void move_left();
		void move_right_motor();
		void move_left_motor();


		/*
		Funciones para controlar el algoritmo para resolver
		el laberinto.
		*/
		bool algorithm();
		void alinear_robot();
		int check_wall();
		void change_direction(int dir);
		bool check_final_cell();

		float read_ultrasound();

		/*
		Mover servo mediante un angulo
		*/
		void move_servo(int ang);

		/*
		Elegir cny70:
		arriba = 0
		abajo_izq = 1
		abajo_drc = 2
		si es blanco devuelve true
		si es negro devuelve false
		*/
		bool read_cny70(int opt); 

		/*Tenemos 9V de bateria, con una caida de 0.7V, por lo que
		tenemos 8.3V que es dividido en 2, asi que maximo = 4.15V
		*/
		float read_battery(); 

		/*Funciones para enviar datos mediante bluetooth */
		void send_bluetooth(char* str); //envia cualquier string
		void send_battery();
		void send_velocity();
		void wait_communication();
		
		~Robot(){}

	private:
		/*Pareja de pines motor A (IZQ, pines 9-10)*/
		int A1pin;
		int A2pin;
		/*Pareja de pines motor B (DRC, pines */
		int B1pin;
		int B2pin;

		//Pin bateria
		int pinBateria;

		//CNY70
		int cnyArriba;
		int cnyAbajoIzq;
		int cnyAbajoDrc;


};

#endif