import sys, pygame
import datetime
import _thread
import serial
import time

#************************ Funciones ****************************

def drawMap():
	for square in celdasVisitadas:
		pygame.draw.rect(screen, (255, 255, 255), (widthMap+(square[0]*50), heightMap+(-square[1]*50), 48, 48))
	anterior = celdasRecorridas[0]
	for square in celdasRecorridas:
		pygame.draw.line(screen, (255,0,0), [24+widthMap+(anterior[0]*50),24+heightMap+(-anterior[1]*50)],[24+widthMap+(square[0]*50),24+heightMap+(-square[1]*50)],5)
		anterior = square
	pygame.draw.circle(screen,(0,0,255), (widthMap+24+(posActual[0]*50), heightMap+24+(-posActual[1]*50)), 18)
	

def communication(a,b):
	global bateria
	global velocidad
	global started
	global firstTime
	global final
	global numCeldasVisitadas
	global direccion
	global posActual
	global celdasVisitadas
	global celdasRecorridas
	while 1:
		cadena = arduino.readline()
		cadena = cadena.decode("utf-8")
		#print(cadena)
		#cadena = input("Introduce comando")

		if cadena[0] == "b":
			bateria = cadena[1:len(cadena)-1]
		if cadena[0] == "m":
			velocidad = cadena[1:len(cadena)-1]
		if cadena[0] == "s" and not started:
			started = True
			firstTime = datetime.datetime.now()
		if cadena[0] == "f":
			final = True
		if cadena[0] == "c":
			if direccion==0:
				posActual = (posActual[0],posActual[1]+1)
				celdasRecorridas.append(posActual)
				if posActual not in celdasVisitadas:
					celdasVisitadas.append(posActual)
					numCeldasVisitadas += 1
			if direccion==1:
				posActual = (posActual[0]+1,posActual[1])
				celdasRecorridas.append(posActual)
				if posActual not in celdasVisitadas:
					celdasVisitadas.append(posActual)
					numCeldasVisitadas += 1
			if direccion==2:
				posActual = (posActual[0]-1,posActual[1])
				celdasRecorridas.append(posActual)
				if posActual not in celdasVisitadas:
					celdasVisitadas.append(posActual)
					numCeldasVisitadas += 1
			if direccion==3:
				posActual = (posActual[0],posActual[1]-1)
				celdasRecorridas.append(posActual)
				if posActual not in celdasVisitadas:
					celdasVisitadas.append(posActual)
					numCeldasVisitadas += 1
		if cadena[0] == "g":
			aux = cadena[1]
			if direccion==0 and aux=="0":
				direccion=1
			elif direccion==0 and aux=="1":
				direccion=2
			elif direccion==0 and aux=="2":
				direccion=3
			elif direccion==1 and aux=="0":
				direccion=3
			elif direccion==1 and aux=="1":
				direccion=0
			elif direccion==1 and aux=="2":
				direccion=2
			elif direccion==2 and aux=="0":
				direccion=0
			elif direccion==2 and aux=="1":
				direccion=3
			elif direccion==2 and aux=="2":
				direccion=1
			elif direccion==3 and aux=="0":
				direccion=2
			elif direccion==3 and aux=="1":
				direccion=1
			elif direccion==3 and aux=="2":
				direccion=0

		print(celdasVisitadas)



#*********************** Variables *****************************

direccion = 0 #0 Arriba, 1 Drc, 2 Izq, 3 Abajo
started = False
final = False
size = width, height = 800, 600
widthMap = 400
heightMap = 300
rectangleSize = (50,50)
black = 0, 0, 0
bateria = "0"
celdasVisitadas = list() #una lista de celdas visitadas, su posicion es relativa a la primera (0,0)
celdasVisitadas.append((0,0)) #primera celda
celdasRecorridas = list()
celdasRecorridas.append((0,0))
posActual = (0,0)
numCeldasVisitadas = 1
velocidad = "0"
distanciaRecorrida = 0

#************************ Configuracion ************************

arduino=serial.Serial('COM10',9600)
time.sleep(1)

pygame.init()
pygame.font.init()
pygame.display.set_caption('Interfaz robot arduino')
myfont = pygame.font.SysFont('arial', 15)
myfont2 = pygame.font.SysFont('arial', 30)
screen = pygame.display.set_mode(size)

#*********************** Bucle principal ***********************

_thread.start_new_thread (communication,(0,0))
msg = "a"
arduino.write(msg.encode(encoding='UTF-8'));
while 1:
    for event in pygame.event.get():
        if event.type == pygame.QUIT: 
        	arduino.close()
        	sys.exit()

    screen.fill(black) #limpia la pantalla

    #Para obtener el tiempo transcurrido
    if started:
    	auxTime = datetime.datetime.now()
    	tiempoTranscurrido = str((auxTime-firstTime).seconds)
    	horaText = myfont.render("Tiempo transcurrido: "+tiempoTranscurrido+" segundos", False, (255, 255, 0))
    else:
    	horaText = myfont.render("Tiempo transcurrido: aun no ha empezado", False, (255,255,0))


    #Para obtener el porcentaje de bateria restante
    bateriaText = myfont.render("Nivel de bateria: "+bateria+"%",False,(255,255,0))

    # Para obtener el numero de celdas recorridas
    numVisitadasText = myfont.render("Numero de celdas visitadas: "+str(numCeldasVisitadas), False, (255, 255, 0))

    # Para obtener la velocidad del robot
    velocidadText = myfont.render("Velocidad: "+velocidad+" cm/s", False, (255, 255, 0))

    # Para obtener la distancia recorrida
    distanciaRecorrida = numCeldasVisitadas*18.2 - 18.2;
    distanciaText = myfont.render("Distancia recorrida: " + str(distanciaRecorrida) + " cm", False, (255, 255, 0))

    # Si se ha llegado al final
    if final: 
    	finalText = myfont2.render("LABERINTO RESUELTO", False, (255,0,0))

	
    drawMap()

    #Preparar los elementos para dibujar
    screen.blit(horaText, (2, 2))
    screen.blit(bateriaText, (2, 20))
    screen.blit(numVisitadasText, (2, 38))
    screen.blit(velocidadText, (2, 56))
    screen.blit(distanciaText, (2, 74))
    if final:
    	screen.blit(finalText, (200, 100))

    #Dibujar todos los elementos
    pygame.display.flip()