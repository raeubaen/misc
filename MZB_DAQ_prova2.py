# usr/bin/env python3

import tkinter as tk
from tkinter import *
from tkinter import messagebox
from PIL import ImageTk, Image
import os
import subprocess
import serial
import serial.tools.list_ports
import io
import numpy as np
import sys
import time
from datetime import datetime
from datetime import date
import pathlib
import csv
import socket
import pandas as pd
from multiprocessing import Process
root = tk.Tk()
root.geometry("1250x390")
root.title("Fee GUI")
root.configure(background='light grey')
global is_on
global HV_timeDelay 
HV_timeDelay = 5 
is_on = False

global ReadVop
global ReadTemp
global NStep_DOWN
global NStep_UP
global Final_name
global version
global WheelStep
global step_width
ReadVop = [0,0,0,0] 
ReadTemp = [0,0,0,0] 
Nstep_DOWN = 1
Nstep_UP   = 1
Final_name = "fin"
version  = str(0)
WheelStep = 9
step_width = 1

Worker_name_var = tk.StringVar()
Institution_var = tk.StringVar()
Fee1_name_var   = tk.StringVar()
Fee2_name_var   = tk.StringVar()
Comments_var    = tk.StringVar()


var0HV = StringVar()
var1HV = StringVar()
var2HV = StringVar()
var3HV = StringVar()
var0T  = StringVar()
var1T  = StringVar()
var2T  = StringVar()
var3T  = StringVar()

class Holder:
    def __init__(self,name):
        self.name = name
        
    def GetSiPM(self,indice):
        f1 = open('/home/user/qa_wheel/root/cose/DBtutto.csv','r')
        c1 = csv.reader(f1,delimiter = ';')
        lista = list(c1)
        nomeinizio = self.name+"_"+str(indice)
        for item in lista:
            Ho0       = item[0]
            Vop0      = item[1]
            DACSlope0 = item[2]
            DACConst0 = item[3]
            ADCSlope0 = item[4]
            ADCConst0 = item[5]
            cal1      = item[6]
            cal2      = item[7]
            cal3      = item[8]
            cal4      = item[9]
            if( nomeinizio== Ho0):
                break
        f1.close()        
        return Vop0   
    
            
    def Getfee(self,indice):
        
         f1 = open('/home/user/qa_wheel/root/cose/DBtutto.csv','r')
         c1 = csv.reader(f1,delimiter = ';')
         lista = list(c1)
         nomeinizio = self.name+"_"+str(indice)
         print("sto facendo le calibrazionni di  "+nomeinizio)
         for item in lista:
            Ho0       = item[0]
            Vop0      = item[1]
            DACSlope0 = item[2]
            DACConst0 = item[3]
            ADCSlope0 = item[4]
            ADCConst0 = item[5]
            cal1      = item[6]
            cal2      = item[7]
            cal3      = item[8]
            cal4      = item[9]
            if( nomeinizio== Ho0):
                calVal = DACSlope0 + "  " +DACConst0+"  "+ADCSlope0 + "  " +ADCConst0+ "  " +cal1+ "  " +cal2+ "  " +cal3+ "  " +cal4
                break
         f1.close()        
         return  calVal
               


def provaLettura(riga):
    riga[0:19]=[]   
    ps = riga.index("+")
    Valore = riga[0:ps-1]
    OutVal ="".join(Valore)
    return (OutVal)

initial = "pos="
fine ="\r"

def firstDAQ(Final_name, folder):
    print("we are in firstDAQ")
    global N_entries, N_chan, Nloop
    N_entries   = 9999
    N_chan = 4
    
    my_label.config(text="Starting the acquisition",fg = "black")
    messagebox.showinfo("Warning", "is the LED on? Check it please and continue")
    try:
        ser = serial.Serial('/dev/ttyUSB0',baudrate=112500,timeout=5)
        sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))
        ser.flushInput()    
        dirpath = "mkdir /home/user/qa_wheel/data/"
        final_folder = Final_name+"/"+folder
        newdir= dirpath+final_folder
        os.system(newdir)
        Nloop = int(WheelStep)+1
 #       start_command = "cd /home/user/qa_wheel/daq/; ./DAQ_nev.exe WaveDumpConfig_4chan_220121  "
        start_command = "cd /home/user/qa_wheel/daq/; ./DAQ_nev.exe WaveDumpConfig_4chan_corto  "
        command = start_command + final_folder
        #fino a qui OKAY 1 volta sola
        if ser.isOpen():
            try: # if ser is open
                sio.write("pos=1\r")
                sio.flush()
                time.sleep(2)
                ser.flushInput()

                final_com= command + " "+str(0) + " " + str(N_entries)
                time.sleep(1)
                os.system(final_com)
                # era dentro al for - FINE --------------------------------------------
                 
            except serial.serialutil.SerialException:
                try: # closin connection
                     ser.close()
                     messagebox.showinfo("Error", "Probably you disconnected the USB cable, turn off the wheel and check it out!")
                     print("Connection Closed")
                except:
                    print("Connection Lost")
    except serial.SerialTimeoutException:
        try:
            ser = serial.Serial('/dev/ttyUSB0',baudrate=112500,timeout=5)
            messagebox.showinfo("Error", "Probably the RS232 is not properly connect to the wheel")
        except:
              print("Connection Lost")
    return (command, ser, sio)


def notFirstDAQ(command, ser, sio, loopval):
     print("we are in notFirstDAQ")
     if ser.isOpen():
          try: # if ser is open
               
               #for loopval in range(1, Nloop):
               # era dentro al for - FINE --------------------------------------------
               stringa = initial+str(loopval)+fine
               print(stringa)
               sio.write(stringa)
               sio.flush()
               sio.write('pos?\r')
               sio.flush()
               time.sleep(3)
               msg1= ser.read(9)
               msg2= msg1.decode('utf-8')
               rimuovi = msg2.replace("\r","")
               uscita= rimuovi.split("=")[1]
               finale = uscita.split(">")[0]
               print("finale")
               ser.flushInput()

               final_com= command + " "+str(loopval-1) + " " + str(N_entries)
               time.sleep(1)
               os.system(final_com)
               # era dentro al for - FINE --------------------------------------------

          except serial.serialutil.SerialException:
               try: # closin connection
                    ser.close()
                    messagebox.showinfo("Error", "Probably you disconnected the USB cable, turn off the wheel and check it out!")
                    print("Connection Closed")
               except:
                    print("Connection Lost")

          except serial.SerialTimeoutException:
               try:
                    ser = serial.Serial('/dev/ttyUSB0',baudrate=112500,timeout=5)
                    messagebox.showinfo("Error", "Probably the RS232 is not properly connect to the wheel")
               except:
                    print("Connection Lost")




        
def MZBCOM_Vbias(channel,vop):
     print("-->looking at channel "+str(channel) + " and setting the Vbias at " + str(vop))
     try:
        ser = serial.Serial('/dev/ttyUSB1',baudrate= 112500,timeout=0.5)
        sio = io.TextIOWrapper(io.BufferedRWPair(ser,ser))
        commandVop  = "dacset "+str(channel) + " "+ str(vop)+"\r"
        ser.flushInput()
        ser.flushOutput()

        if ser.isOpen():
            print("-->Connection established")
            try:
                #sio.write('RES\r')
                #sio.flush()
                 sio.write('BUSYF 0 \r')
                 sio.flush()
                 msg1 = ser.read(100)
                 msg2 = msg1.decode('utf-8')
                 sio.write('HVONOFF 1 \r')
                 sio.flush()
                 msg1 = ser.read(100)
                 msg2 = msg1.decode('utf-8')
                 sio.write('skip 1 \r')
                 sio.flush()
                 msg1 = ser.read(100)
                 msg2 = msg1.decode('utf-8')
                 sio.write(commandVop)
                 sio.flush()
                 msg1 = ser.read(100)
                 msg2 = msg1.decode('utf-8')
                 ser.flushOutput()
                 time.sleep(1)
                 sio.write('DACST \r')
                 sio.flush()
                 msg3 = ser.read(1000)
                 msg4 = msg3.decode('utf-8')
                 prova = msg4.splitlines()                     
            except serial.serialutil.SerialException:
                 try:
                    ser.close()
                    print(" Connection Closed")
                 except:
                    print ("Connection Lost")             
     except serial.SerialTimeoutException:   
        try:
            ser.remove()#remove(ser)
            print("COM not properly inserted?")
        except:
            print("Connection Lost")  

def MZB_HVOFF():
    try:
         ser = serial.Serial('/dev/ttyUSB1',baudrate= 112500,timeout=0.9)
         sio = io.TextIOWrapper(io.BufferedRWPair(ser,ser))         
         ser.flushInput()
         ser.flushOutput()
     
         if ser.isOpen():
             print("-->Connection established")
             try:
                 sio.write('HVO 0\r')
                 sio.flush()
             except serial.serialutil.SerialException:
                 try:
                     ser.close()
                     print(" Connection Closed")
                 except:
                     print ("Connection Lost")
              
    except serial.SerialTimeoutException:   
         try:
             ser.remove()#remove(ser)
             print("COM not properly inserted?")
         except:
             print("Connection Lost") 


def MZB_ReadValues():
    try:
         ser = serial.Serial('/dev/ttyUSB1',baudrate= 112500,timeout=0.9)
         sio = io.TextIOWrapper(io.BufferedRWPair(ser,ser))         
         ser.flushInput()
         ser.flushOutput()

         if ser.isOpen():
             print("-->Connection established")
             try:
                   
                 sio.write('ADSTS \r')
                 sio.flush()
                 msg3 = ser.read(10000)
                 msg4 = msg3.decode('utf-8')
                 riga = msg4.splitlines()
                 #print(msg4)
                 for i in range(0,4,1):
                     #str1 = prova[7+6*i]
                     HVrowInd = 7+26*i 
                     TrowInd  = 13+26*i
                     Hrow = list(riga[HVrowInd])
                     Trow = list(riga[TrowInd])
                     ReadVop[i] = provaLettura(Hrow)
                     ReadTemp[i]= provaLettura(Trow)
                     print(ReadVop[i]+"  "+ReadTemp[i])
                     
                 var0HV.set(str(ReadVop[0]))
                 var1HV.set(str(ReadVop[1]))
                 var2HV.set(str(ReadVop[2]))
                 var3HV.set(str(ReadVop[3]))
                 
                 var0T.set(str(ReadTemp[0]))
                 var1T.set(str(ReadTemp[1]))
                 var2T.set(str(ReadTemp[2]))
                 var3T.set(str(ReadTemp[3]))
                 root.update_idletasks()
                    
 
             except serial.serialutil.SerialException:
                 try:
                     ser.close()
                     print(" Connection Closed")
                 except:
                     print ("Connection Lost")
              
    except serial.SerialTimeoutException:   
         try:
             ser.remove()#remove(ser)
             print("COM not properly inserted?")
         except:
             print("Connection Lost") 
    
def MZBCOM_Calibrations(channel,calibration):
    print("-->looking at channel "+str(channel))
    try:
        ser = serial.Serial('/dev/ttyUSB1',baudrate= 112500,timeout=0.5)
        sio = io.TextIOWrapper(io.BufferedRWPair(ser,ser))
        commandCali = " calcard " + str(channel)+" "+calibration + "\r"
        ser.flushInput()
        ser.flushOutput()

        if ser.isOpen():
            print("-->Connection established")
            try:
                #sio.write('RES\r')
                #sio.flush()
                 sio.write('BUSYF 0 \r')
                 sio.flush()
                 msg1 = ser.read(100)
                 msg2 = msg1.decode('utf-8')
                 sio.write(commandCali)
                 sio.flush()
                 time.sleep(1)
            except serial.serialutil.SerialException:
                try:
                    ser.close()
                    print(" Connection Closed")
                except:
                    print ("Connection Lost")
              
    except serial.SerialTimeoutException:   
        try:
            ser.remove()#remove(ser)
            print("COM not properly inserted?")
        except:
            print("Connection Lost")  
        

def single_volt_routine(Final_name, folder, version):
    vars = firstDAQ(Final_name, folder)

    An_Threads = []

    for n in range(1, Nloop):
        if n!= Nloop-1:
            DAQ_Thread = Process(target = notFirstDAQ, args=(*vars, n+1))
            DAQ_Thread.start()

        Analysis_Thread = Process(target = doOneAnalysis, args=(n-1, Final_name, folder)) 
        An_Threads.append(Analysis_Thread)

        Analysis_Thread.start()
        DAQ_Thread.join()

    for n, thread in enumerate(An_Threads):
        if thread.is_alive():
            print(f"joining analysis thread for pos{n}")
            thread.join()
            print(f"terminating thread for pos{n}")

    exit_code = subprocess.call(["/home/user/qa_wheel/root/final_summary.csh", holder0, holder1, Final_name, folder, str(WheelStep), str(N_chan), version])
    if exit_code == 0:
       return True #OK
    else:
       return False # not OK



def doScan():
    subprocess.Popen(["/bin/bash", "/home/user/qa_wheel/root/compile_root_file.sh"])
    N_entries   = 8000 #str(N_entries_var.get())
    global holder1, holder0
    Worker_name = str(Worker_name_var.get())
    Institution = str(Institution_var.get())
    data = date.today()
    ora = datetime.now()
    today = str(data.strftime("%m/%d/%y"))
    current_time = str(ora.strftime("%H:%M"))
    holder0 = Fee1_name_var.get()
    holder1 = Fee2_name_var.get()
    Fee_name  = holder0+"_"+holder1
    position  = "/home/user/qa_wheel/data/"
    path =position+Fee_name
    if(os.path.isdir(path)):
        print("folder already exists, creating another\n")
        for i in range(1,100):
            val =Fee_name[0:11]
            Fee_name = val+"_v"+str(i)
            version  = str(i)
            newPath =position+Fee_name
            if(not( os.path.isdir(newPath))):
                Final_name = Fee_name
                print(Final_name)
                break
            else:
                Final_name = "cia"
    else:
        Final_name = Fee_name
    print(Final_name)  
    initialFolder = "mkdir /home/user/qa_wheel/data/"+Final_name    
    os.system(initialFolder)
    holderna = [holder0,holder1]
    list=[]
    channel = [1,2,5,6]
    Step = Nstep_DOWN+Nstep_UP
     
    for x in range (0,2,1):
        list.append(Holder(holderna[x]))
     
    for j in range(0,Step+1,1): # starting the loop over the Vbias 
        inde  = 0   
        print("Vbias at step = "+str(j))
        for obj in list:
            print(obj.name)
            i = 0 # sipm Number (0,1)
            while (i < 2):
                vop =obj.GetSiPM(i)
#                print("Sipm "+str(i) +"Vop "+ vop)
                if(j==0): # apply the calibration at the first step of Vbias Loop
                    calibration = obj.Getfee(i)
                    # print("Sipm "+str(i) +"  "+ calibration)
                    MZBCOM_Calibrations(channel[inde],calibration)
                # print(vop)
                # print(str(i)+"   "+str(inde))
                
                
                vbiasMis = float(vop)-Nstep_DOWN
                vwrite   = vbiasMis + j*step_width
                print("the Vbias we are setting now is :" + str(vwrite))
                MZBCOM_Vbias(channel[inde],vwrite)
                
                if(inde ==3):
                     if(j==0):
                         time.sleep(10)
                     else:
                         time.sleep(1)
                     MZB_ReadValues()
                     folder = "GainStep_"+str(j)
                     print( str(vwrite) +"   " +str(vop))
                     if(float(vwrite) == float(vop)): 
                            folder = "Gain_vop"
                     #percorso = Final_name +"/"+folder
                     #print(percorso)
                     print(f"vwrite: {vwrite}, vop: {vop}, Voltage Folder: {folder}")
                     single_volt_routine(Final_name, folder, version)

                inde = inde+1
                i = i+1

    check_result = check_linearity(Nstep_DOWN, int(version)) #returns a bool True if OK
    print("Fine Scan")


def check_linearity(Nstep_DOWN, version):
    def vstep2dv(vstep):
        if vstep=="Gain_vop":
            return 0
        else:
            n = int(vstep.split("_")[1])
            return n - Nstep_DOWN

    db = pd.read_csv("/home/user/qa_wheel/database/Gain.dat", sep="  ",  header=None)
    db.columns = ["vstep", "date", "holder", "vers", "0", "Q0", "sQ0", "Gain0", "sGain0", "1", "Q1", "sQ1", "Gain1", "sGain1"]

    current_db_slice = db[db.vers==version]
    current_db_slice["dv"] = current_db_slice.vstep.apply(vstep2dv)

    data = {}
    for i in range(len(current_db_slice)):
        df = current_db_slice.iloc[i]
        if int(df.dv) == 0:
            data[df.holder] = {
                "df_op": df,
                0: {"Voltage": [], "Gain": [], "sGain": [], "Q": [], "sQ": [], "PDE": [], "sPDE": []},
                1: {"Voltage": [], "Gain": [], "sGain": [], "Q": [], "sQ": [], "PDE": [], "sPDE": []},
            }

    for i in range(len(current_db_slice)):
        df = current_db_slice.iloc[i]
        obj = Holder(df.holder)
        h_data = data[df.holder]
        df_op = h_data["df_op"]

        for j in [0, 1]:
            h_data[j]["Gain"].append(df[f"Gain{j}"]/df_op[f"Gain{j}"])
            h_data[j]["sGain"].append(df[f"sGain{j}"]/df_op[f"Gain{j}"])
            h_data[j]["Q"].append(df[f"Q{j}"]/df_op[f"Q{j}"])
            h_data[j]["sQ"].append(df[f"sQ{j}"]/df_op[f"Q{j}"])

            pde = (df[f"Q{j}"]/df[f"Gain{j}"]) / (df_op[f"Q{j}"]/df_op[f"Gain{j}"])
            h_data[j]["PDE"].append(pde)
            h_data[j]["sPDE"].append(pde * np.sqrt((df[f"sQ{j}"]/df[f"Q{j}"])**2 + (df[f"sGain{j}"]/df[f"Gain{j}"])**2) )

            #now you get overvoltage - to get the absolute tension remove the 0*
            h_data[j]["Voltage"].append(int(df.dv)*step_width + 0*float(obj.GetSiPM(j))) 

    for holder in ["pippo"]:
        for sipm in [0, 1]:
            df = pd.DataFrame(data[holder][sipm])
            df.to_csv(f"summary_{holder}_{sipm}.csv", header=False, index=False)

            buf = io.StringIO()
            df.to_csv(buf, header=False, index=False)
            buf.seek(0)
            os.system("g++ gain_q_pde.C `root-config --cflags --glibs` -o gain_q_pde.x")

            plot = 1 # if 0 root does not plot anything
            p = subprocess.Popen([
                "/home/user/qa_wheel/root/gain_q_pde.x", 
                holder,
                str(sipm),
                str(version),
                str(plot),
                ], stdin=subprocess.PIPE
            )
            p.communicate(bytes(buf.read(), encoding='utf-8'))
            exit_code = p.wait()
            if exit_code == 0:
              return True #OK
            else:
              return False # not OK

def doOneAnalysis(loopval, Final_name, folder):

     #my_label.config(text="Starting the analysis of the files",fg = "black")
     #print(f"Before csh call for pos{loopval} ------------------------------------------------------------------------")

     subprocess.call(['/home/user/qa_wheel/root/doAll_3par_Final2.csh', holder0, holder1, Final_name, folder, str(N_chan),str(loopval),version])
     #print(f"After csh call for pos{loopval}------------------------------------------------------------------------")



def VoltageON():
    global is_on
    #global HV_timeDelay = 5   
    s_LV= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s_LV.settimeout(5)
    s_LV.connect(('192.168.1.201',9221))
    s_HV= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s_HV.settimeout(5)
    s_HV.connect(('192.168.1.1',9221))
   
    if is_on:
        on_button.config(image = off)
        my_label.config(text = "The Switch is Off", 
                        fg = "grey")
        s_HV.sendall(bytes('OP1 0\n','ascii'))
        time.sleep(HV_timeDelay)
        s_LV.sendall(bytes('OP1 0\n','ascii'))
        
        
        #s_LV.sendall(bytes('OP1 0\n','ascii'))
        #s_HV.sendall(bytes('OP1 0\n','ascii'))

        is_on = False
    else:
        on_button.config(image = on)
        my_label.config(text = "The Switch is On", fg = "green")
        s_LV.sendall(bytes('OP1 1\n','ascii'))
        s_LV.sendall(bytes('V1 28\n','ascii'))
        time.sleep(HV_timeDelay)
        s_HV.sendall(bytes('OP1 1\n','ascii'))
        s_HV.sendall(bytes('V1 200\n','ascii'))
        #s_LV.sendall(bytes('*IDN?\n','ascii'))        
        is_on = True
        
        
on  = PhotoImage(file = "/home/user/qa_wheel/root/cose/on.png")
off = PhotoImage(file = "/home/user/qa_wheel/root/cose/off.png")






my_label = Label(root, text = "Welcome to the Mu2e Fee Gui",fg = "Black", font = ("Calibre",32))

Vop_h0_S0_entry =  Label(root, textvariable = var0HV,fg = "Black", font = ("calibre",13,"normal"))
Vop_h0_S1_entry =  Label(root, textvariable = var1HV,fg = "Black", font = ("calibre",13,"normal"))
Vop_h1_S0_entry =  Label(root, textvariable = var2HV,fg = "Black", font = ("calibre",13,"normal"))
Vop_h1_S1_entry =  Label(root, textvariable = var3HV,fg = "Black", font = ("calibre",13,"normal"))

var0HV.set(str(ReadVop[0]))
var1HV.set(str(ReadVop[1]))
var2HV.set(str(ReadVop[2]))
var3HV.set(str(ReadVop[3]))

Temp_h0_S0_entry =  Label(root, textvariable = var0T,fg = "Black", font = ("calibre",13,"normal"))
Temp_h0_S1_entry =  Label(root, textvariable = var1T,fg = "Black", font = ("calibre",13,"normal"))
Temp_h1_S0_entry =  Label(root, textvariable = var2T,fg = "Black", font = ("calibre",13,"normal"))
Temp_h1_S1_entry =  Label(root, textvariable = var3T,fg = "Black", font = ("calibre",13,"normal"))

var0T.set(str(ReadTemp[0]))
var1T.set(str(ReadTemp[1]))
var2T.set(str(ReadTemp[2]))
var3T.set(str(ReadTemp[3]))

       
#CREATING LABELS
Worker_label      = tk.Label(root,text = 'Worker:'        ,font=('calibre',10, 'bold'))
Institution_label = tk.Label(root,text = 'Institution:'   ,font=('calibre',10, 'bold'))
Fee1_label        = tk.Label(root,text = 'Holder 1 \n[HXXXX]',font=('calibre',10, 'bold'))
Fee2_label        = tk.Label(root,text = 'Holder 2 \n[HXXXX]',font=('calibre',10, 'bold'))  

Vop_h0_s0_label  = tk.Label(root,text =  'Vop0_s0 [V] ' ,font=('calibre',10, 'bold'))
Vop_h0_s1_label  = tk.Label(root,text =  'Vop0_s1 [V] ' ,font=('calibre',10, 'bold'))
Vop_h1_s0_label  = tk.Label(root,text =  'Vop1_s0 [V] ' ,font=('calibre',10, 'bold'))
Vop_h1_s1_label  = tk.Label(root,text =  'Vop1_s1 [V] ' ,font=('calibre',10, 'bold'))

Temp_h0_s0_label  = tk.Label(root,text =  'Temp0_s0 [°C] ' ,font=('calibre',10, 'bold'))
Temp_h0_s1_label  = tk.Label(root,text =  'Temp0_s1 [°C] ' ,font=('calibre',10, 'bold'))
Temp_h1_s0_label  = tk.Label(root,text =  'Temp1_s0 [°C] ' ,font=('calibre',10, 'bold'))
Temp_h1_s1_label  = tk.Label(root,text =  'Temp1_s1 [°C] ' ,font=('calibre',10, 'bold'))
  
#CREATING ENTRIES
Worker_entry      = tk.OptionMenu(root, Worker_name_var,"wk-ediociaiuti01","wk-sgiovannella01","wk-smiscetti01","wk-arusso01","wk-dpierluigi","wk-isarra01")
Institution_entry = tk.OptionMenu(root,Institution_var,"LNF","FNAL")
Fee1_entry        = tk.Entry(root,textvariable = Fee1_name_var,font=('calibre',10,'normal'))
Fee2_entry        = tk.Entry(root,textvariable = Fee2_name_var,font=('calibre',10,'normal'))    
    
btn_doScan= tk.Button(root,text = 'doScan', command = doScan,activeforeground='blue', activebackground='orange')
btn_MZBOFF= tk.Button(root,text = 'MZB OFF', command = MZB_HVOFF,activeforeground='blue', activebackground='orange')

on_button    = tk.Button(root, image = off, bd = 0, command = VoltageON) 
    

#btn_doScan= tk.Button(root,text = 'doScan', command = provaLettura,activeforeground='blue', activebackground='orange')
#positioning 
my_label.grid(row=0,column=2,columnspan=2)
on_button.grid(row=0,column =0)
Worker_label.grid(row=1,column=0)
#Worker_entry.grid(row=1,column=1,sticky=W)
Worker_entry.grid(row=1,column=1)
Institution_label.grid(row=1,column=4)
Institution_entry.grid(row=1,column=5,sticky=W)

Fee1_label.grid(row=2,column=0)
Fee2_label.grid(row=2,column=4)
Fee1_entry.grid(row=2,column=1,sticky=W)
Fee2_entry.grid(row=2,column=5,sticky=W)
btn_doScan.grid(row=1, column =1,columnspan=2)
btn_MZBOFF.grid(row=1, column =3,columnspan=2)

Vop_h0_s0_label.grid(row=5,column=0)
Vop_h0_S0_entry.grid(row=5,column=1,sticky=W)
Vop_h1_s0_label.grid(row=5,column=4)
Vop_h1_S0_entry.grid(row=5,column=5,sticky=W)

Vop_h0_s1_label.grid(row=6,column=0)
Vop_h0_S1_entry.grid(row=6,column=1,sticky=W)
Vop_h1_s1_label.grid(row=6,column=4)
Vop_h1_S1_entry.grid(row=6,column=5,sticky=W)

Temp_h0_s0_label.grid(row=7,column=0)
Temp_h0_S0_entry.grid(row=7,column=1,sticky=W)
Temp_h1_s0_label.grid(row=7,column=4)
Temp_h1_S0_entry.grid(row=7,column=5,sticky=W)

Temp_h0_s1_label.grid(row=8,column=0)
Temp_h0_S1_entry.grid(row=8,column=1,sticky=W)
Temp_h1_s1_label.grid(row=8,column=4)
Temp_h1_S1_entry.grid(row=8,column=5,sticky=W)


root.mainloop() 
