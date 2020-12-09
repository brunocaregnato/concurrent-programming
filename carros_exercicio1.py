from threading import Thread
from threading import Semaphore
from time import sleep
from random import randint

import sys

ncarros = 0
    
#---------------------------------------------------
def cruzamentoCarros(carro, carroCruzando, carroTerminou, semaforoAberto, semaforoFechado, ruas):

    origem = ruas[carro % 2]
    destino = ruas[0] if origem == ruas[1] else ruas[1]
    
    print(f'carro {carro} quer ir da {origem} para {destino}')

    while True:
        sleep(2)

        semaforoFechado.acquire()
        carroCruzando.release()

        semaforoAberto.acquire()
        sleep(5)
        print(f'carro {carro} terminou de cruzar')

        sleep(3)
        print(f'Sinal fechado...')

        carroTerminou.release()
        semaforoFechado.release()

#---------------------------------------------------
def cruzamentoSemaforos(carroCruzando, carroTerminou, semaforoAberto, semaforoFechado, ruas):
    rua = 0
    while True:
        carroCruzando.acquire()

        if not semaforoFechado:
            continue

        sleep(2)

        print(f'Sinal aberto...')

        semaforoAberto.release()
        carroTerminou.acquire()

        if rua == 0:
            rua = 1
        else:
            rua = 0

#---------------------------------------------------
if (len(sys.argv) != 2):
	print ('%s <numero_de_carros>' %sys.argv[0])
	sys.exit(0)

ncarros = int(sys.argv[1])
print(f'--- Numero total de carros no cruzamento: {ncarros} ---')

ruas = ['esquerda', 'direita']
carros = []

semaforoAberto = Semaphore(0)
semaforoFechado = Semaphore(1)

carroCruzando = Semaphore(0)
carroTerminou = Semaphore(0)

tp = Thread(target=cruzamentoSemaforos, args=[carroCruzando, carroTerminou, semaforoAberto, semaforoFechado, ruas])
tp.start()

for carro in range(ncarros):
   tc = Thread(target=cruzamentoCarros, args=[carro, carroCruzando, carroTerminou, semaforoAberto, semaforoFechado, ruas])
   carros.append(tp)
   tc.start()

tp.join()

for i in carros:
    tc.join()