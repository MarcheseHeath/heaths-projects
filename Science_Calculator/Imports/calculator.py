# -*- coding: utf-8 -*-
"""
Created on Fri Nov 20 18:32:44 2020

@author: Heath
"""

from tkinter import *
import tkinter
import math
import cmath
import turtle
from turtle import*
import pyromat as pm
import matplotlib.pyplot as plt
import numpy as np
from conversions import Conversions

"""    Thermodynamic Data     """
global ThermoMolecG  #Gas Phase Data MORGOD
global ThermoEntropyG  #Entropy is in J/molK
global ThermoEnthalpyG  #Enthalpy is in KJ/mol
global ThermoSHG  #Specific Heat is in J/mol K

ThermoMolecG = ['Ag',]

ThermoEnthalpyG = [284.9,]

ThermoEntropyG = [173,]

ThermoSHG = [20.8,]

global ThermoMolecL  #Liquid Phase Data
global ThermoEntropyL
global ThermoEnthalpyL
global ThermoSHL

ThermoMolecL = ['Br2','CCl4','CS2','H2O','H2O2','H2SO4','H3PO4','HCN','HNO3','Hg','NH4OH',
                'N2H4','N2O4','PCl3','POCl3','SiCl4','SnCl4','TiCl4','VCl4']

ThermoEnthalpyL = [0,-128.2,89,-285.8,-187.8,-814.0,-1271.7,108.9,-174.1,0,-361.2,50.6,-19.5,
                   -319.7,-597.1,-687.0,-511.3,-804.2,-569.4]

ThermoEntropyL = [152.2,216.2,151.3,70.0,109.6,156.9,150.8,112.8,155.6,75.9,165.6,121.2,
                  209.2,217.1,222.5,239.7,258.6,252.3,255.0]

ThermoSHL = [75.7,0,76.4,75.3,89.1,138.9,145.0,70.6,109.9,28.0,154.9,0,142.7,0,0,0,165.3,
             145.2,0]

global ThermoMolecS  #Solid Phase Data
global ThermoEntropyS
global ThermoEnthalpyS
global ThermoSHS

ThermoMolecS = ['Ag','AgCN','Ag2CO3','AgNO3','Ag2O','Ag2S','AgBr','AgCl','AgF','AgI','Al',
                'Al2O3','AlF3','AlI3','AlBr3','AlCl3','Al(OH)3','AlPO4','B','B2S3','Ba',
                'BaCO3','BaH2','BaBr2','BaCl2','BaF2','BaI2','BaO','BaSO4','Be','BeBr2',
                'BeCl2','BeF2','BeI2','BeO','Be(OH)2','BeSO4','Bi','Bi2O3','BiCl3','CBr4',
                'Ca','Ca(OH)2','CaBr2','CaCl2','CaCN','CaF2','CaH2','CaI2','CaO','CaSO4',
                'Cd','CdBr2','CdCl2','CdCO3','CdF2','CdS','CdSO4','Co','CoCl2','Cr',
                'Cr2O3','CrCl2','CrCl3','Cs','CsCl','Cu','Cu2O','CuO','Cu2S','CuS','CuSO4',
                'CuBr','CuBr2','CuCl','CuCl2','CuCN','Fe','FeO','Fe2O3','Fe3O4','FeCO3',
                'FeS2','FeCl2','FeCl3','FeBr2','FeBr3','Fe3C','H3PO4','HgO','Hg2SO4',
                'HgSO4','Hg2Cl2','HgCl2','Hg2Br2','HgBr2','Hg2I2','I2','HIO3','K','K2CO3',
                'K2O','K2O2','K2SO4','KBr','KCl','KF','KI','KClO3','KMnO4','KNO2','KNO3',
                'KSCN','Li','Li2O','LiOH','LiNO3','LiBr','LiCl','LiF','LiI','Mg','MgO',
                'Mg(OH)2','MgS','MgSO4','MgBr2','MgCl2','MgF2','Mn','MnO','MnO2','MnBr2',
                'MnCl2','Mo','MoO2','MoO3','MoS2','MoS3','NH4Cl','NH4NO3','(NH4)2SO4','Na',
                'Na2CO3','Na2O','Na2O2','Na2SO4','NaBr','NaCl','NaCN','NaF','NaN3','NaNO3',
                'NaO2','NaOH','NaH','Ni','Ni2O3','Ni(OH)2','NiBr2','NiCl2','NiF2','Os',
                'OsO4','Pb','PbCl2','PbCO3','PbO2','Pb(NO3)2','PbS','PbSO4','Rb','RbCl',
                'Si','SnO2','Ti','TiCl2','TiCl3','TiO2','U','UF4','UF6','UO2','V','V2O5',
                'VCl3','Zn','ZnBr2','ZnCl2','ZnF2','ZnI2','Zn(NO3)2','ZnSO4','Zr','ZrCl2',
                'ZrCl4']

ThermoEnthalpyS = [0,146,-505.8,-124.4,-31.1,-32.6,-100.4,-127.0,-204.6,-61.8,0,-1675.7,
                   -1510.4,-302.9,-527.2,-704.5,-1277,-1733.8,0,-240.6,0,-1213.0,-177,
                   -757.3,-855,-1207.1,-602.1,-548.0,-1473.2,0,-353.5,-490.4,-1026.8,
                   -192.5,-609.4,-902.5,-1205.2,0,-573.9,-379.1,29.4,0,-985.2,-682.8,
                   -795.4,-184.5,-1228.0,-181.5,-533.5,-634.9,-1434.5,0,-316.2,-391.5,
                   -750.6,-700.4,-161.9,-933.3,0,-312.5,0,-1139.7,-395.4,-556.5,0,-443.0,
                   0,-168.6,-157.3,-79.5,-53.1,-771.4,-104.6,-141.8,-137.2,-220.1,96.2,
                   0,-272.0,-824.2,-1118.4,-740.6,-178.2,-341.8,-399.5,-249.8,-268.2,
                   25.1,-1284.4,-90.8,-743.1,-707.5,-265.4,-224.3,-206.9,-170.7,-121.3,
                   -105.4,0,-230.1,0,-1151.0,-361.5,-494.1,-1437.8,-393.8,-436.5,-567.3,
                   -327.9,-397.7,-837.2,-369.8,-494.6,-200.2,0,-597.9,-487.5,-483.1,
                   -351.2,-408.6,-616,-270.4,0,-601.6,-924.5,-346.0,-1284.9,-524.3,
                   -641.3,-1124.2,0,-385.2,-520.0,-384.9,-481.3,0,-588.9,-745.1,-235.1,
                   -364,-314.4,-365.6,-1180.9,0,-1130.7,-414.2,-510.9,-1387.1,-361.1,
                   -411.2,-87.5,-576.6,21.7,-467.9,-260.2,-425.8,-56.3,0,-489.5,-529.7,
                   -212.1,-305.3,-651.4,0,-394.1,0,-359.4,-699.1,-277.4,-451.9,-100.4,
                   -920.0,0,-435.4,0,-557.6,0,-513.8,-720.9,-944.0,0,-1914.2,-2197.0,
                   -1085.0,0,-1550.6,-580.7,0,-328.7,-415.1,-764.4,-208.0,-483.7,-982.8,
                   0,-502.0,-980.5]

ThermoEntropyS = [42.6,107.2,167.4,140.9,121.3,144,107.1,96.3,84,115.5,28.3,50.9,66.5,
                  195.9,180.2,112.3,0,90.8,5.9,100.0,62.5,112.1,63.0,146.0,123.7,96.4,
                  167.0,72.1,132.2,9.5,108,75.8,53.4,121,13.8,45.5,77.9,56.7,151.5,177.0,
                  212.5,41.6,83.4,130,108.4,0,68.5,41.4,142,38.1,106.5,51.8,137.2,115.3,
                  92.5,77.4,64.9,123.0,30,109.2,23.8,81.2,115.3,123,85.2,101.2,33.2,93.1,
                  42.6,120.9,66.5,109.2,96.1,0,86.2,108.1,84.5,27.3,60.7,87.4,146.4,92.9,
                  52.9,118,142.3,140.6,0,104.6,110.5,70.3,200.7,0,191.6,146.0,218.0,
                  172.0,233.5,180.0,116.1,0,64.7,155.5,94.1,102.1,175.6,95.9,82.6,66.6,
                  106.3,143.1,171.7,152.1,133.1,124.3,29.1,37.6,42.8,90.0,74.3,59.3,35.7,
                  86.8,32.7,27.0,63.2,50.3,91.6,117.2,89.6,57.2,32,59.7,53.1,0,118.2,
                  28.7,46.3,77.7,62.6,119,94.6,151.1,220.1,51.3,135,75.1,95,149.6,86.8,
                  72.1,115.6,51.1,96.9,116.5,115.9,64.4,40,29.9,0,88,0,97.7,73.6,32.6,
                  143.9,64.8,136,131,68.6,0,91.2,148.5,76.8,95.9,18.8,49,30.7,87.4,139.7,
                  50.6,50.2,151.7,227.6,77.0,28.9,131.0,131.0,41.6,138.5,111.5,73.7,161.1,
                  0,110.5,39,110,181.6]

ThermoSHS = [25.4,66.7,112.3,93.1,65.9,76.5,52.4,50.8,0,56.8,24.2,79.0,75.1,0,100.6,91.1,
             0,93.2,11.1,111.7,28.1,86.0,0,0,75.1,71.2,0,47.3,101.8,13.4,69.4,62.4,51.8,
             71.1,25.6,62.1,85.7,25.5,113.5,105.0,0,25.9,87.5,0,72.9,0,67.0,41.0,0,42.0,
             99.7,26.0,76.7,74.7,0,0,0,99.6,24.8,78.5,23.4,118.7,71.2,91.8,32.2,52.5,24.2,
             63.6,0,76.3,47.8,0,54.7,0,48.5,71.9,0,25.1,0,103.9,143.4,82.1,62.2,75.7,96.7,
             0,0,105.9,106.1,44.1,132.0,0,191.6,146.0,218.0,172.0,233.5,180.0,54.4,0,29.6,
             114.4,0,0,131.5,52.3,51.3,49.0,52.9,100.3,117.6,107.4,96.4,88.5,24.9,54.1,
             49.6,0,0,48.0,41.6,51.0,24.9,37.2,77.0,45.6,96.5,0,71.4,61.6,26.3,45.4,54.1,
             0,72.9,24.1,56.0,75.0,63.6,0,84.1,139.3,187.5,28.2,112.3,69.1,89.2,128.2,51.4,
             50.5,70.4,46.9,76.6,92.9,72.1,59.5,36.4,26.1,0,0,0,71.7,64.1,24.7,0,26.8,0,
             87.4,64.6,0,49.5,103.2,31.1,52.4,20.0,52.6,25.1,69.8,97.2,55.0,27.7,116.0,
             166.8,63.6,24.9,127.7,93.2,25.4,0,71.3,65.7,0,0,99.2,25.4,0,119.8]

global ThermoMolecAQ  #Aqueous Phase Data MORGOD
global ThermoEntropyAQ
global ThermoEnthalpyAQ
global ThermoSHAQ

ThermoMolecAQ = ['Ag+','Al(OH)4-','Br−','Cl−','F−','Fe2+','Fe3+','H+','H2SO4','HBr','HCl',
                 'HF','HI','HNO3','I−','Li+','MnO4−','NH4+','Na+','NaBr','NaCl','NaF',
                 'NaNO3','OH−','Pb(NO3)2','SO42−']

ThermoEnthalpyAQ = [105.8,-1490,-121.4,-167.1,-335.4,-89.1,-48.5,0,-909.3,-121.6,-167.2,
                    -332.6,-55.2,-207.4,-56.8,-278.5,-541.4,-133.3,-240.2,-361.7,-407.3,
                    -572.8,-447.5,-230.0,-416.3,-909.3]

ThermoEntropyAQ = [73.5,117,82.6,56.6,-13.8,-137.7,-315.9,0,20.1,82.4,56.5,-13.8,111.3,
                   146.4,106.5,12.4,191.2,111.2,58.5,141.4,115.5,45.2,205.4,-10.9,303.3,
                   18.5]

#ThermoSHAQ = []

"""    Output Functions     """
def Output(out):  #Output window
    master = Tk()
    master.title('Science Calculator © 2020 Heath Marchese All Rights Reserved')
    Label(master, text='Solution:', font='Helvetica 24 bold').pack(pady=(15,0))
    Label(master, text=out, font ='Helvetica 14').pack(ipadx=(25),ipady=(25))
        
    Button(master, text="Close", font ='Helvetica 12 bold', command=master.destroy).pack(pady=(25))
    
def ErrorOutput(out):  #Output window
    master = Tk()
    master.title('Science Calculator © 2020 Heath Marchese All Rights Reserved')
    Label(master, text=out, font ='Helvetica 14').grid(row=0,column=0,columnspan=4,padx=(30),ipady=(20))
        
    Button(master, text="Close", font ='Helvetica 12 bold', command=master.destroy).grid(row=1,column=1,columnspan=2,pady=(50,0))

def ChooseUnit():  #Output window when units not selected
    master = Tk()
    master.title('Science Calculator © 2020 Heath Marchese All Rights Reserved')
    Label(master, text='You must choose units to continue', font ='Helvetica 14').grid(row=0,column=0,columnspan=4,padx=(30),ipady=(20))
        
    Button(master, text="Close", font ='Helvetica 12 bold', command=master.destroy).grid(row=1,column=1,columnspan=2,pady=(50,0))

def EnterNumber():  #Output window when variable values not entered
    master = Tk()
    master.title('Science Calculator © 2020 Heath Marchese All Rights Reserved')
    Label(master, text='You are missing necessary values', font ='Helvetica 14').grid(row=0,column=0,columnspan=4,padx=(30),ipady=(20))
        
    Button(master, text="Close", font ='Helvetica 12 bold', command=master.destroy).grid(row=1,column=1,columnspan=2,pady=(50,0))

def ChooseTarget():  #Output window when target variable not selected
    master = Tk()
    master.title('Science Calculator © 2020 Heath Marchese All Rights Reserved')
    Label(master, text='You must choose target variable to continue', font ='Helvetica 14').grid(row=0,column=0,columnspan=4,padx=(30),ipady=(20))
        
    Button(master, text="Close", font ='Helvetica 12 bold', command=master.destroy).grid(row=1,column=1,columnspan=2,pady=(50,0))
    
def ChooseState():  #Output window when molecule's phase not selected
    master = Tk()
    master.title('Science Calculator © 2020 Heath Marchese All Rights Reserved')
    Label(master, text='Please make sure each molecule is assigned a state', font ='Helvetica 14').grid(row=0,column=0,columnspan=4,padx=(30),ipady=(20))
        
    Button(master, text="Close", font ='Helvetica 12 bold', command=master.destroy).grid(row=1,column=1,columnspan=2,pady=(50,0))

class Calculator:
    """    Mathematics Equations     """
    def QuadraticFormula(master,a,b,c):  #(-b +(-) sqrt(b^2 - 4ac))/2a
        d = (b ** 2) - 4 * a * c 

        if d < 0:
            x1 = (- (b) + cmath.sqrt(d)) / (2 * a)
            x2 = (- (b) - cmath.sqrt(d)) / (2 * a)
            out=("This equation has two complex solutions: {a.real:0.04f}+{a.imag:0.04f}j or {b.real:0.04f}+{b.imag:0.04f}j").format(a=x1,b=x2)
            Output(out)
        
        else:
            x1 = float((- (b) + math.sqrt(d)) / (2 * a))
            x2 = float((- (b) - math.sqrt(d)) / (2 * a))
    
            if x1 == x2:
                out=("This equation has one solution: %.5f" % x1)
                Output(out)
            else:
                out=("This equation has two solutions: %.5f or %.5f" % (x1,x2))
                Output(out)
            
    """    Science Equations     """
    def IdealGas(master,p,v,n,t,targ,pU,vU,tU): #PV = nRT
        Convert = Conversions()
        if (pU == 1):  #Checks to see if units match constant for all variables
            p = Convert.kPa_ATM(p)  #Converts varibles to correct units if needed
        elif (pU == 2):
            p = Convert.torr_ATM(p)
        elif (pU == 4):
            p = Convert.mmHg_ATM(p)
        if (vU == 2):
           v = Convert.cm_L(v)
        if (tU == 1):
           t = Convert.C_K(t)
        elif (tU == 2):
           t = Convert.F_K(t)
           
        if targ == 0:
            ChooseTarget()  
            return None
        if (pU == 0) or (vU == 0) or (tU == 0):  #Checks for unit selection
            if (targ == 1) and (vU != 0) and (tU != 0):  #Checks if non selected unit and selected target match
                pres = (n * 0.0821 * t) / v  #Performs calculation
                out = ("Answer: %.5f ATM" % pres)  #Formats output for Output() function
                Output(out)
            elif (targ == 2) and (pU != 0) and (tU != 0):
                vol = (n * 0.0821 * t) / p
                out = ("Answer: %.5f L" % vol)
                Output(out)
            elif (targ == 4) and (pU != 0) and (vU != 0):
                temp = (p * v) / (n * 0.0821)
                out = ("Answer: %.5f K" % temp)
                Output(out)
            else:
                ChooseUnit()            
        elif (targ == 3) and (pU != 0) and (vU != 0) and (tU != 0):
                mol = (p * v) / (0.0821 * t) 
                out = ("Answer: %.5f mol" % mol)
                Output(out)
        else:
            ChooseUnit()
                
    def ReactionProp(master,cT,t,p,rE1,rE2,rE3,rE4,rE5,pE1,pE2,pE3,pE4,pE5,rC1,rC2,rC3,rC4,rC5,pC1,pC2,pC3,pC4,pC5,rs1,rs2,rs3,rs4,rs5,ps1,ps2,ps3,ps4,ps5):  #dG, dH, dSo and what they mean
        Convert = Conversions()
        
        x = 0
        
        speciesR = []
        statesR = []
        speciesP = []
        statesP = []
        coefR = []
        coefP = []
        enthalpyR = []
        entropyR = []
        enthalpyP = []
        entropyP = []
        
        speciesR[:] = [rE1,rE2,rE3,rE4,rE5]
        statesR[:] = [rs1,rs2,rs3,rs4,rs5]
        coefR[:] =  [rC1,rC2,rC3,rC4,rC5]
        speciesP[:] = [pE1,pE2,pE3,pE4,pE5]
        statesP[:] = [ps1,ps2,ps3,ps4,ps5]
        coefP[:] =  [pC1,pC2,pC3,pC4,pC5]
        
        T = 298.15 
        if cT == 1:
            if t <= 0:
                out = "You must enter a valid temperature"
                ErrorOutput(out)
                return 0
            else:
                T = t
        
        
        
        for A in range(len(speciesR)):
            if statesR[A] == 1:
                if speciesR[A] in ThermoMolecG:
                    i = ThermoMolecG.index(speciesR[A])
                    enthalpyR.append(ThermoEnthalpyG[i])
                    entropyR.append(ThermoEntropyG[i])
                else:
                    out = "Sorry we do not have enough information about your current molecules"
                    ErrorOutput(out)
                    x += 1
                    break
            if statesR[A] == 2:
                if speciesR[A] in ThermoMolecL:
                    i = ThermoMolecL.index(speciesR[A])
                    enthalpyR.append(ThermoEnthalpyL[i])
                    entropyR.append(ThermoEntropyL[i])
                else:
                    out = "Sorry we do not have enough information about your current molecules"
                    ErrorOutput(out)
                    x += 1
                    break
            if statesR[A] == 3:
                if speciesR[A] in ThermoMolecS:
                    i = ThermoMolecS.index(speciesR[A])
                    enthalpyR.append(ThermoEnthalpyS[i])
                    entropyR.append(ThermoEntropyS[i])
                else:
                    out = "Sorry we do not have enough information about your current molecules"
                    ErrorOutput(out)
                    x += 1
                    break
            if statesR[A] == 4:
                if speciesR[A] in ThermoMolecAQ:
                    i = ThermoMolecAQ.index(speciesR[A])
                    enthalpyR.append(ThermoEnthalpyAQ[i])
                    entropyR.append(ThermoEntropyAQ[i])
                else:
                    out = "Sorry we do not have enough information about your current molecules"
                    ErrorOutput(out)
                    x += 1
                    break
            if statesR[A] == 0:
                pass
            
        for A in range(len(speciesP)):
            if x == 0:
                if statesP[A] == 1:
                    if speciesP[A] in ThermoMolecG:
                        i = ThermoMolecG.index(speciesP[A])
                        enthalpyP.append(ThermoEnthalpyG[i])
                        entropyP.append(ThermoEntropyG[i])
                    else:
                        out = "Sorry we do not have enough information about your current molecules"
                        ErrorOutput(out)
                        break
                if statesP[A] == 2:
                    if speciesP[A] in ThermoMolecL:
                        i = ThermoMolecL.index(speciesP[A])
                        enthalpyP.append(ThermoEnthalpyL[i])
                        entropyP.append(ThermoEntropyL[i])
                    else:
                        out = "Sorry we do not have enough information about your current molecules"
                        ErrorOutput(out)
                        break
                if statesP[A] == 3:
                    if speciesP[A] in ThermoMolecS:
                        i = ThermoMolecS.index(speciesP[A])
                        enthalpyP.append(ThermoEnthalpyS[i])
                        entropyP.append(ThermoEntropyS[i])
                    else:
                        out = "Sorry we do not have enough information about your current molecules"
                        ErrorOutput(out)
                        break
                if statesP[A] == 4:
                    if speciesP[A] in ThermoMolecAQ:
                        i = ThermoMolecAQ.index(speciesP[A])
                        enthalpyP.append(ThermoEnthalpyAQ[i])
                        entropyP.append(ThermoEntropyAQ[i])
                    else:
                        out = "Sorry we do not have enough information about your current molecules"
                        ErrorOutput(out)
                        break
                if statesP[A] == 0:
                    pass
        
        out = ''
        enthR = 0.0
        enthP = 0.0
        enthSum = 0.0
        entroR = 0.0
        entroP = 0.0
        entroSum = 0.0
        
        if len(entropyR) == 0:
            out = "Sorry we do not have enough information about your current molecules"
            ErrorOutput(out)
            x += 1
        elif len(entropyP) == 0:
            out = "Sorry we do not have enough information about your current molecules"
            ErrorOutput(out)
            x += 1
        elif x == 0:
            for A in range(len(enthalpyR)):
                enthR += coefR[A] * enthalpyR[A]
                enthP += coefP[A] * enthalpyP[A]
                entroR += coefR[A] * entropyR[A]
                entroP += coefP[A] * entropyP[A]
            
            enthSum = enthP - enthR
            entroSum = entroP - entroR 
            G = enthSum - T * (entroSum / 1000)
            
            if enthSum > 0:
                out += ('Your reaction is endothermic because its Enthalpy, %.5f kJ/mol, is above zero' % enthSum)
            if enthSum < 0:
                out += ('Your reaction is exothermic because its Enthalpy, %.5f kJ/mol, is below zero' % enthSum)
            if G > 0:
                out += ("\n\nYour reaction is not spontaneous because its Gibb's Free Energy, %.5f kJ/mol, is above zero" % G)
                Output(out)
            if G < 0:
                out += ("\n\nYour reaction is spontaneous because its Gibb's Free Energy, %.5f kJ/mol, is below zero" % G)
                Output(out)