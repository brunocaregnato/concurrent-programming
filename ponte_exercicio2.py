from threading import Thread
from threading import Semaphore
from time import sleep
from random import randint

import sys

nviajFloresta = 0
nviajRio = 0
l = 0
    
#---------------------------------------------------
def passarViajantesFLoresta(s0, s1, l):
    while True:
        l = l + 1
        print('Viajante da floresta passou: %d' % l)
        if (l>4):
            s1.acquire()
            l = 0
            sleep(2)
            s0.release()    
            print('liberei o caminho para o Rio')
    
#---------------------------------------------------
def passaViajantesRio(s0, s1, l):
    while True:
        s0.acquire()
        print('Viajante do Rio passou: %d' % l)
        sleep(2)
        s1.release()

#---------------------------------------------------


if (len(sys.argv) != 3):
	print ('%s <numero_de_viajantesFLoresta> <numero_de_viajantesRrio>' %sys.argv[0])
	sys.exit(0)

nviajantesFloresta = int(sys.argv[1])
nviajantesRio = int(sys.argv[2])
print(f'--- Numero total de pessoas para atravessar a ponte: {nviajantesFloresta + nviajantesRio} ---')
print(f'--- Numero total de pessoas da Floresta Alta: {nviajantesFloresta} ---')
print(f'--- Numero total de pessoas do Grande Rio: {nviajantesRio} ---')

nviajFloresta = nviajantesFloresta
nviajRio = nviajantesRio

s0 = Semaphore(0)
s1 = Semaphore(1)

tp = Thread(target=passarViajantesFLoresta, args=[s0, s1, l])
tp.start()

for cnt in range(nviajantesRio):
   tc = Thread(target=passaViajantesRio, args=[s0, s1, l])
   tc.start()

tp.join()
for i in range(nviajRio):
    tc.join()