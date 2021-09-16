import subprocess
import pandas as pd
import numpy as np
import io
import csv

step_width = 1

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

    for holder in data:
        for sipm in [0, 1]:
            df = pd.DataFrame(data[holder][sipm])
            df.to_csv(f"summary_{holder}_{sipm}.csv", header=False, index=False)

            buf = io.StringIO()
            df.to_csv(buf, header=False, index=False)
            buf.seek(0)
            p = subprocess.Popen([
                "root", 
                'gain_q_pde.C(\"'+holder+'\", \"'+str(sipm)+'\", \"'+str(version)+'\")'
                ], stdin=subprocess.PIPE
            )
            p.communicate(bytes(buf.read(), encoding='utf-8'))


check_linearity(1, 76)

