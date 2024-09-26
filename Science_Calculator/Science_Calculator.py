# -*- coding: utf-8 -*-
"""
Created on Fri Nov 20 12:50:34 2020

@author: Heath
"""

from tkinter import *
import tkinter as tk
import math
import cmath
import turtle
from turtle import*
import matplotlib.pyplot as plt
import numpy as np
from conversions import Conversions
from calculator import Calculator,EnterNumber,ChooseState

"""    Calculator     """     
class CalculatorGUI(tk.Tk):
    def __init__(self, *args, **kwargs):
        
        tk.Tk.__init__(self, *args, **kwargs)
        container = tk.Frame(self)

        container.pack(expand = True)

        container.grid_rowconfigure(0)
        container.grid_columnconfigure(0)

        self.frames = {}

        for F in ([Home,ChemTab,PhysTab,MathTab,ConvertTab,
                   ThermoTab,ReactionProp,IdealGas,
                   VectorMath,VectorAdd,VectorProjection,
                   QuadraticFormula,
                   PressureConvert,VolumeConvert,TempConvert]):

            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky=NS)

        self.show_frame(Home)

    def show_frame(self, cont):

        frame = self.frames[cont]
        frame.tkraise()
        
class Home(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)

        Label(self, text='Choose Your Needed Subject', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(275),pady=(0,30))
    
        Button(self, text="Chemistry", font = "Helvetica 18", command=lambda:[controller.show_frame(ChemTab)]).grid(row=2,column=1,columnspan=2,pady=(0,20),sticky=EW)
        Button(self, text="Physics", font = "Helvetica 18", command=lambda:[controller.show_frame(PhysTab)]).grid(row=3,column=1,columnspan=2,pady=(0,20),sticky=EW)
        Button(self, text="Mathematics", font = "Helvetica 18", command=lambda:[controller.show_frame(MathTab)]).grid(row=4,column=1,columnspan=2,pady=(0,20),sticky=EW)
        Button(self, text="Conversions", font = "Helvetica 18", command=lambda:[controller.show_frame(ConvertTab)]).grid(row=5,column=1,columnspan=2,pady=(0,20),sticky=EW)
        
        #Button(self, text="Quit", font ='Helvetica 12 bold', command=start.destroy).grid(row=6,column=1,columnspan=2,pady=(100,0))
        
"""    Chemistry Calculators     """
class ChemTab(tk.Frame):           
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        Label(self, text='Chem Tab - Select Calculator', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
        
        Button(self, text='Thermodynamics', font='Helvetica 12 bold', command=lambda:[controller.show_frame(ThermoTab)]).grid(row=1,column=1,columnspan=2,sticky=EW)
        
        Button(self, text="Return", font='Helvetica 12 bold', command=lambda:[controller.show_frame(Home)]).grid(row=6,column=1,columnspan=2,pady=(100,0))
        
"""     Thermodynamics Calculators     """ 
class ThermoTab(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        Label(self, text='Chem Tab - Thermodynamics', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(265),pady=(0,30))
        
        Button(self, text='Reaction Properties', font='Helvetica 12 bold', command=lambda:[controller.show_frame(ReactionProp)]).grid(row=1,column=1,columnspan=2,sticky=EW)
        Button(self, text='Ideal Gas Law', font='Helvetica 12 bold', command=lambda:[controller.show_frame(IdealGas)]).grid(row=2,column=1,columnspan=2,sticky=EW)
        
        Button(self, text="Return", font='Helvetica 12 bold', command=lambda:[controller.show_frame(ChemTab)]).grid(row=6,column=1,columnspan=2,pady=(100,0))            

class ReactionProp(tk.Frame):       
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        checkT = tk.IntVar(self)
        checkP = tk.IntVar(self)
        rs1 = tk.IntVar(self)
        rs2 = tk.IntVar(self)
        rs3 = tk.IntVar(self)
        rs4 = tk.IntVar(self)
        rs5 = tk.IntVar(self)
        ps1 = tk.IntVar(self)
        ps2 = tk.IntVar(self)
        ps3 = tk.IntVar(self)
        ps4 = tk.IntVar(self)
        ps5 = tk.IntVar(self)
        
        """    Activation Code     """
        def SolveReactionProp():
            Calculate = Calculator()  #Refers to Calculator Class
            
            T = 0.0
            p = 0.0
            r1 = ''
            r2 = ''
            r3 = ''
            r4 = ''
            r5 = ''
            p1 = ''
            p2 = ''
            p3 = ''
            p4 = ''
            p5 = ''
            cr1 = 1
            cr2 = 1
            cr3 = 1
            cr4 = 1
            cr5 = 1
            cp1 = 1
            cp2 = 1
            cp3 = 1
            cp4 = 1
            cp5 = 1
            
            cT = checkT.get()
            #cp = checkP.get()
            rS1 = rs1.get()
            rS2 = rs2.get()
            rS3 = rs3.get()
            rS4 = rs4.get()
            rS5 = rs5.get()
            pS1 = ps1.get()
            pS2 = ps2.get()
            pS3 = ps3.get()
            pS4 = ps4.get()
            pS5 = ps5.get()
            
            if rE1.get():
                r1 = rE1.get()
            if rE2.get():
                r2 = rE2.get()
            if rE3.get():
                r3 = rE3.get()
            if rE4.get():
                r4 = rE4.get()
            if rE5.get():
                r5 = rE5.get()
            if pE1.get():
                p1 = pE1.get()
            if pE2.get():
                p2 = pE2.get()
            if pE3.get():
                p3 = pE3.get()
            if pE4.get():
                p4 = pE4.get()
            if pE5.get():
                p5 = pE5.get()
            if rC1.get():
                cr1 = int(rC1.get())
            if rC2.get():
                cr2 = int(rC2.get())
            if rC3.get():
                cr3 = int(rC3.get())
            if rC4.get():
                cr4 = int(rC4.get())
            if rC5.get():
                cr5 = int(rC5.get())
            if pC1.get():
                cp1 = int(pC1.get())
            if pC2.get():
                cp2 = int(pC2.get())
            if pC3.get():
                cp3 = int(pC3.get())
            if pC4.get():
                cp4 = int(pC4.get())
            if pC5.get():
                cp5 = int(pC5.get())
            
            species = []
            states = []
            species[:] = [r1,r2,r3,r4,r5,p1,p2,p3,p4,p5]
            states[:] = [rS1,rS2,rS3,rS4,rS5,pS1,pS2,pS3,pS4,pS5]
            
            for A in range (len(species)):
                if (species[A] != '') and (states[A] == 0):
                    ChooseState()
                    return 0
                        
            Calculate.ReactionProp(cT,T,p,r1,r2,r3,r4,r5,p1,p2,p3,p4,p5,cr1,cr2,cr3,cr4,cr5,cp1,cp2,cp3,cp4,cp5,rS1,rS2,rS3,rS4,rS5,pS1,pS2,pS3,pS4,pS5)
        
        Label(self, text='Thermodynamics - Reaction Properties', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=8,padx=(200),pady=(0,30))
        
        Radiobutton(self,text='Temp. (K)',font='Helvetica 14',variable=checkT,value=1).grid(row=1,column=2,columnspan=2,pady=(0,25))
        Radiobutton(self,text='Standard Temp.',font='Helvetica 14',variable=checkT,value=0).grid(row=1,column=4,columnspan=2,pady=(0,25))
        tE = Entry(self,width=30)
        tE.grid(row=2,column=3,columnspan=2)
        
        #Radiobutton(self,text='Pressure (atm)',font='Helvetica 14',variable=checkP,value=1).grid(row=1,column=4,columnspan=2,pady=(0,25))
        #Radiobutton(self,text='Standard Pressure',font='Helvetica 14',variable=checkP,value=0).grid(row=2,column=4,columnspan=2,pady=(0,25))
        #pE = Entry(self,width=25)
        #pE.grid(row=1,column=6,pady=(35,0))
    
        Label(self, text='Reactants:',font='Helvetica 14').grid(row=3,column=0,columnspan=4)
        Label(self, text='Coefficient:',font='Helvetica 14').grid(row=4,column=0,columnspan=2)
        Label(self, text='Molecule:',font='Helvetica 14').grid(row=4,column=2,columnspan=2)
        
        rC1 = Entry(self,width=25)
        rC1.grid(row=5,column=0,columnspan=2)
        rE1 = Entry(self,width=25)
        rE1.grid(row=5,column=2,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=rs1,value=1).grid(row=6,column=0,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=rs1,value=2).grid(row=6,column=1)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=rs1,value=3).grid(row=6,column=2)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=rs1,value=4).grid(row=6,column=3,sticky=W)
        
        rC2 = Entry(self,width=25)
        rC2.grid(row=7,column=0,columnspan=2)
        rE2 = Entry(self,width=25)
        rE2.grid(row=7,column=2,columnspan=2,)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=rs2,value=1).grid(row=8,column=0,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=rs2,value=2).grid(row=8,column=1)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=rs2,value=3).grid(row=8,column=2)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=rs2,value=4).grid(row=8,column=3,sticky=W)
        
        rC3 = Entry(self,width=25)
        rC3.grid(row=9,column=0,columnspan=2)
        rE3 = Entry(self,width=25)
        rE3.grid(row=9,column=2,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=rs3,value=1).grid(row=10,column=0,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=rs3,value=2).grid(row=10,column=1)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=rs3,value=3).grid(row=10,column=2)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=rs3,value=4).grid(row=10,column=3,sticky=W)
        
        rC4 = Entry(self,width=25)
        rC4.grid(row=11,column=0,columnspan=2)
        rE4 = Entry(self,width=25)
        rE4.grid(row=11,column=2,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=rs4,value=1).grid(row=12,column=0,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=rs4,value=2).grid(row=12,column=1)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=rs4,value=3).grid(row=12,column=2)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=rs4,value=4).grid(row=12,column=3,sticky=W)
        
        rC5 = Entry(self,width=25)
        rC5.grid(row=13,column=0,columnspan=2)
        rE5 = Entry(self,width=25)
        rE5.grid(row=13,column=2,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=rs5,value=1).grid(row=14,column=0,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=rs5,value=2).grid(row=14,column=1)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=rs5,value=3).grid(row=14,column=2)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=rs5,value=4).grid(row=14,column=3,sticky=W)
        
        Label(self, text='Products:',font='Helvetica 14').grid(row=3,column=4,columnspan=4)
        Label(self, text='Coefficient:',font='Helvetica 14').grid(row=4,column=4,columnspan=2)
        Label(self, text='Molecule:',font='Helvetica 14').grid(row=4,column=6,columnspan=2)
        
        pC1 = Entry(self,width=25)
        pC1.grid(row=5,column=4,columnspan=2)
        pE1 = Entry(self,width=25)
        pE1.grid(row=5,column=6,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=ps1,value=1).grid(row=6,column=4,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=ps1,value=2).grid(row=6,column=5)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=ps1,value=3).grid(row=6,column=6,)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=ps1,value=4).grid(row=6,column=7,sticky=W)
        
        pC2 = Entry(self,width=25)
        pC2.grid(row=7,column=4,columnspan=2)
        pE2 = Entry(self,width=25)
        pE2.grid(row=7,column=6,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=ps2,value=1).grid(row=8,column=4,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=ps2,value=2).grid(row=8,column=5)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=ps2,value=3).grid(row=8,column=6,)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=ps2,value=4).grid(row=8,column=7,sticky=W)
        
        pC3 = Entry(self,width=25)
        pC3.grid(row=9,column=4,columnspan=2)
        pE3 = Entry(self,width=25)
        pE3.grid(row=9,column=6,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=ps3,value=1).grid(row=10,column=4,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=ps3,value=2).grid(row=10,column=5)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=ps3,value=3).grid(row=10,column=6,)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=ps3,value=4).grid(row=10,column=7,sticky=W)
        
        pC4 = Entry(self,width=25)
        pC4.grid(row=11,column=4,columnspan=2)
        pE4 = Entry(self,width=25)
        pE4.grid(row=11,column=6,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=ps4,value=1).grid(row=12,column=4,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=ps4,value=2).grid(row=12,column=5)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=ps4,value=3).grid(row=12,column=6,)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=ps4,value=4).grid(row=12,column=7,sticky=W)
        
        pC5 = Entry(self,width=25)
        pC5.grid(row=13,column=4,columnspan=2)
        pE5 = Entry(self,width=25)
        pE5.grid(row=13,column=6,columnspan=2)
        Radiobutton(self,text='(g)',font='Helvetica 10',variable=ps5,value=1).grid(row=14,column=4,sticky=W)
        Radiobutton(self,text='(l)',font='Helvetica 10',variable=ps5,value=2).grid(row=14,column=5)
        Radiobutton(self,text='(s)',font='Helvetica 10',variable=ps5,value=3).grid(row=14,column=6,)
        Radiobutton(self,text='(aq)',font='Helvetica 10',variable=ps5,value=4).grid(row=14,column=7,sticky=W)
        
        Button(self, text="Solve", font ='Helvetica 12 bold', command=SolveReactionProp).grid(row=15,column=2,columnspan=2,pady=(100,0))
        Button(self, text="Return", font='Helvetica 12 bold', command=lambda:[controller.show_frame(ThermoTab)]).grid(row=15,column=4,columnspan=2,pady=(100,0))
            
class IdealGas(tk.Frame):  #IDEAL GAS LAW
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        var1 = tk.IntVar(self)
        var2 = tk.IntVar(self)
        var3 = tk.IntVar(self)
        var4 = tk.IntVar(self)
                
        """    Activation Code     """
        def SolveIdeal():
            Calculate = Calculator()
                    
            target = var1.get()
            pU = var2.get()
            vU = var3.get()
            tU = var4.get()
            p = 0
            v = 0
            n = 0
            t = 0
            
            if pN.get():
                p = float(pN.get())
            if vN.get():
                v = float(vN.get())
            if nN.get():
                n = float(nN.get())
            if tN.get():
                t = float(tN.get())
            if not pN.get() and (target != 1):
                EnterNumber()
                return None
            elif not vN.get() and (target != 2):
                EnterNumber()
                return None
            elif not nN.get() and (target != 3):
                EnterNumber()
                return None
            elif not tN.get() and (target != 4):
                EnterNumber()
                return None
                
            Calculate.IdealGas(p,v,n,t,target,pU,vU,tU)  #Sends all the input variables and sends them to calculator to process

        Label(self, text="Thermodynamics - Ideal Gas Law", font='Helvetica 24 bold').grid(row=0,rowspan=2,column=0,columnspan=4,padx=(200),pady=(0,20),sticky=N)
        
        Label(self, text="Which variable is needed?", font ='Helvetica 14').grid(row=2,rowspan=1,column=1,columnspan=2,pady=(0,20))
    
        Radiobutton(self, text="Pressure", font = "Helvetica 12", variable=var1, value=1).grid(row=3,column=0)
        Radiobutton(self, text="Volume", font = "Helvetica 12", variable=var1, value=2).grid(row=3,column=1)
        Radiobutton(self, text="Mole", font = "Helvetica 12", variable=var1, value=3).grid(row=3,column=2)
        Radiobutton(self, text="Temperature", font = "Helvetica 12", variable=var1, value=4).grid(row=3,column=3)
    
        Label(self, text="Pressure:", font ='Helvetica 14').grid(row=4,column=0,pady=(25,5))
        pN = Entry(self,width=50)
        pN.grid(row=4,column=1,columnspan=3,sticky=W,pady=(25,0)) 
        Radiobutton(self, text="kPa", font = "Helvetica 12", variable=var2, value=1).grid(row=5,column=0)
        Radiobutton(self, text="torr", font = "Helvetica 12", variable=var2, value=2).grid(row=5,column=1)
        Radiobutton(self, text="ATM", font = "Helvetica 12", variable=var2, value=3).grid(row=5,column=2)
        Radiobutton(self, text="mmHg", font = "Helvetica 12", variable=var2, value=4).grid(row=5,column=3)
        
        Label(self, text="Volume:", font ='Helvetica 14').grid(row=6,column=0,pady=(0,5))
        vN = Entry(self,width=50)
        vN.grid(row=6,column=1,columnspan=3,sticky=W)
        Radiobutton(self, text="L", font = "Helvetica 12", variable=var3, value=1).grid(row=7,column=0)
        Radiobutton(self, text="cm^3", font = "Helvetica 12", variable=var3, value=2).grid(row=7,column=1)
        
        Label(self, text="Mole:", font ='Helvetica 14').grid(row=8,column=0,pady=(0,28))
        nN = Entry(self,width=50)
        nN.grid(row=8,column=1,columnspan=3,sticky=W,pady=(0,30))
        
        Label(self, text="Temperature:", font ='Helvetica 14').grid(row=10,column=0,pady=(0,5))
        tN = Entry(self,width=50)
        tN.grid(row=10,column=1,columnspan=3,sticky=W)
        Radiobutton(self, text="°C", font = "Helvetica 12", variable=var4, value=1).grid(row=11,column=0)
        Radiobutton(self, text="°F", font = "Helvetica 12", variable=var4, value=2).grid(row=11,column=1)
        Radiobutton(self, text="K", font = "Helvetica 12", variable=var4, value=3).grid(row=11,column=2,)
            
        Button(self, text="Solve", font ='Helvetica 12 bold', command=SolveIdeal).grid(row=12,column=1,pady=(30,0))
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(ThermoTab)]).grid(row=12,column=2,pady=(30,0))

"""    Physics Calculators     """
class PhysTab(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        Label(self, text='Choose Your Needed Calculator', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
    
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(Home)]).grid(row=6,column=1,columnspan=2,pady=(100,0))
    
        
"""    Mathematics Calculators     """
class MathTab(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        Label(self, text='Math Tab - Select Calculator', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
        
        Button(self, text='Quadratic Equation', font='Helvetica 12 bold', command=lambda:[controller.show_frame(QuadraticFormula)]).grid(row=1,column=1,columnspan=2,sticky=EW)
        Button(self, text='Vector Operations', font='Helvetica 12 bold', command=lambda:[controller.show_frame(VectorMath)]).grid(row=2,column=1,columnspan=2,sticky=EW)
        
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(Home)]).grid(row=6,column=1,columnspan=2,pady=(100,0))
   
class VectorMath(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
                        
        Label(self, text='Math Tab - Vector Operations', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
        
        Button(self, text="Vector Projection", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(VectorProjection)]).grid(row=1,column=1,columnspan=2,sticky=EW)        
        Button(self, text="Vector Addition", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(VectorAdd)]).grid(row=2,column=1,columnspan=2,sticky=EW)
                
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(MathTab)]).grid(row=6,column=1,columnspan=2,pady=(100,0))

class VectorAdd(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        Label(self, text='Vector Operations - Vector Addition', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
        
        Label
        
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(VectorMath)]).grid(row=6,column=1,columnspan=2,pady=(100,0))

class VectorProjection(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        def solveProjection():
            x = 0
        
        Label(self, text='Vector Operations - Vector Projection', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=8,padx=(200),pady=(0,30))
        
        Label(self, text='Vector 1: <', font='Helvetica 14').grid(row=1,column=1,columnspan=1,pady=(0,20))
        v1c1 = Entry(self)
        v1c1.grid(row=1,column=2,columnspan=1,pady=(0,20))
        Label(self, text=',', font='Helvetica 14').grid(row=1,column=3,columnspan=1,pady=(0,20))
        v1c2 = Entry(self)
        v1c2.grid(row=1,column=4,columnspan=1,pady=(0,20))
        Label(self, text=',', font='Helvetica 14').grid(row=1,column=5,columnspan=1,pady=(0,20))
        v1c3 = Entry(self)
        v1c3.grid(row=1,column=6,columnspan=1,pady=(0,20))
        Label(self, text='>', font='Helvetica 14').grid(row=1,column=7,columnspan=1,pady=(0,20))
        
        
        Label(self, text='Onto', font='Helvetica 14').grid(row=2,column=3,columnspan=2,pady=(0,20))
        
        Label(self, text='Vector 2: <', font='Helvetica 14').grid(row=3,column=1)
        v2c1 = Entry(self)
        v2c1.grid(row=3,column=2,columnspan=1,pady=(0,20))
        Label(self, text=',', font='Helvetica 14').grid(row=3,column=3)
        v2c2 = Entry(self)
        v2c2.grid(row=3,column=4,columnspan=1,pady=(0,20))
        Label(self, text=',', font='Helvetica 14').grid(row=3,column=5)
        v2c3 = Entry(self)
        v2c3.grid(row=3,column=6,columnspan=1,pady=(0,20))
        Label(self, text='>', font='Helvetica 14').grid(row=3,column=7)
        
        Button(self, text="Solve", font ='Helvetica 12 bold', command=solveProjection).grid(row=6,column=3,pady=(100,0))
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(VectorMath)]).grid(row=6,column=4,pady=(100,0))


class QuadraticFormula(tk.Frame):  #Quadratic Formula
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        
        var1 = tk.DoubleVar(self)
        var2 = tk.DoubleVar(self)
        var3 = tk.DoubleVar(self)
            
        """    Activation Code     """
        def SolveQuadratic():
            Calculate = Calculator()
            if aE.get() and bE.get() and cE.get():
                a = float(aE.get())
                b = float(bE.get())
                c = float(cE.get())
                Calculate.QuadraticFormula(a,b,c)
            else:
                EnterNumber()

        Label(self, text="Math Tab - Quadratic Formula", font='Helvetica 24 bold').grid(row=0,rowspan=2,column=0,columnspan=4,padx=(200),pady=(0,20),sticky=N)
        
        Label(self, text="Please enter necessary values", font ='Helvetica 14').grid(row=2,rowspan=1,column=1,columnspan=2,pady=(0,20))
                
        Label(self, text='A:',font='Helvetica 14').grid(row=3,column=1,sticky=W,pady=(30,0))
        aE = Entry(self,width=40)
        aE.grid(row=3,column=1,columnspan=2,pady=(30,0))
                
        Label(self, text='B:',font='Helvetica 14').grid(row=4,column=1,sticky=W,pady=(30,0))
        bE = Entry(self,width=40)
        bE.grid(row=4,column=1,columnspan=2,pady=(30,0))
                
        Label(self, text='C:',font='Helvetica 14').grid(row=5,column=1,sticky=W,pady=(30,0))
        cE = Entry(self,width=40)
        cE.grid(row=5,column=1,columnspan=2,pady=(30,0))
                
        Button(self, text="Solve", font ='Helvetica 12 bold', command=SolveQuadratic).grid(row=6,column=1,pady=(100,0))
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(MathTab)]).grid(row=6,column=2,pady=(100,0))       
        
"""    Conversions Calculators     """
class ConvertTab(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)

        Label(self, text='What do you need to Convert?', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
        
        Button(self, text="Pressure", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(PressureConvert)]).grid(row=1,column=1,columnspan=2,sticky=EW)
        Button(self, text="Volume", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(VolumeConvert)]).grid(row=2,column=1,columnspan=2,sticky=EW)
        Button(self, text="Temperature", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(TempConvert)]).grid(row=3,column=1,columnspan=2,sticky=EW)
        
        Button(self, text="Quit", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(Home)]).grid(row=6,column=1,columnspan=2,pady=(100,0))
        
"""    Pressure Conversions     """
class PressureConvert(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
    
        Label(self, text='Pressure Conversions', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
        
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(ConvertTab)]).grid(row=6,column=1,columnspan=2,pady=(100,0))

"""    Volume Conversions     """
class VolumeConvert(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
     
        Label(self, text='Volume Conversions', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
            
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(ConvertTab)]).grid(row=6,column=1,columnspan=2,pady=(100,0))

"""    Temperature Conversions     """
class TempConvert(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
     
        Label(self, text='Temperature Conversions', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(200),pady=(0,30))
            
        Button(self, text="Return", font ='Helvetica 12 bold', command=lambda:[controller.show_frame(ConvertTab)]).grid(row=6,column=1,columnspan=2,pady=(100,0))
    
start = CalculatorGUI()
start.title('© 2020 Heath Marchese All Rights Reserved')
start.grid_columnconfigure(0,weight=1)
start.grid_columnconfigure(1,weight=1)
start.grid_columnconfigure(2,weight=1)
start.grid_columnconfigure(3,weight=1)
start.grid_columnconfigure(4,weight=1)
start.grid_columnconfigure(5,weight=1)
start.grid_columnconfigure(6,weight=1)
start.grid_columnconfigure(7,weight=1)
start.mainloop()

