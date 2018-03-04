import serial
import serial.tools.list_ports as listPorts
from itertools import chain
import logging
import time

class Scale():
    def __init__(self):
        self.weight = 0
        self.srl = None

    def portNames(self):
        pnames = []
        for port in listPorts.comports():
            badports = ['ttyAMA', 'COM1']

            if any(badport in port[0] for badport in badports):
                continue
            else:
                pnames.append(port[0])
        return pnames

    def initScale(self, id):
        scale = serial.Serial(self.portNames()[id], 250000)
        scale.timeout = 0.1
       #  scale.timeout = 2
        if scale.port is None:
            scale.open()
        self.srl = scale
        # self.clearingBuffer()

    def clearingBuffer(self):
        aux = self.srl.timeout
        self.srl.timeout = 3.0
        line = self.srl.readline()
        while self.srl.inWaiting() > 0:
            line = self.srl.readline()
        self.srl.timeout = aux
        if (self.srl.inWaiting() == 0):
            return True
        else:
            return False

    def readlastline(self):
        last_data = ''
        while True:
            data = self.srl.readline().decode('iso-8859-1')
            if data != '':
                last_data = data
            else:
                return last_data

    def getData(self):
        ''' Get weight in grams '''
        data = ''
        while(self.srl.inWaiting() > 0):
            data = self.srl.readline().decode('iso-8859-1')
        data = self.srl.readline().decode('iso-8859-1')
        #print(data)
        data = data.split(' ')[2].split('=')[1]
        return data

    def getKilos(self):
        ''' Convert raw data from grams into kg '''
        try:
            weight = float(self.getData())
            return weight
        except KeyboardInterrupt:
            # handle Ctrl-C
            print("Cancelado por el usuario")
            exit(0)
        except Exception as ex:
            # handle unexpected script errors
            #print(ex)
            pass
    
    def getNext(self):
        while True:
            try:
                kg = self.getKilos()
                if kg is not None:
                    return kg
            except:
                pass #retry endlessly

    def getRaw(self):
        raw = 0
        try:
            raw = int(self.getData(2))
        except ValueError as e:
            logger.error(e.strerror)
            raw = None
        return raw

    def calKilos(self, tara=True):
        self.getRaw()  # parche
        m = float(self.cfg['m'])
        b = float(self.cfg['b'])
        y = (m * self.getRaw()) + b
        if tara:
            val_tara = float(self.cfg['tara'])
            y = y - val_tara
        return y

global s
def main():
    global s
    s = Scale()
    s.initScale(0)
    

if __name__ == '__main__':
    main()