from tkinter import *
import tkinter
import math
import cmath
import turtle
from turtle import*
import pyromat as pm
import matplotlib.pyplot as plt
import numpy as np

master = Tk()

Label(master, text='Test', font='Helvetica 20 bold').pack(padx=(200),pady=(0,25))

var = StringVar(master)
var2 = StringVar(master)

#pm.config['unit_matter'] = 'mol'
#print(pm.config)
#pm.info()

def grab():
    mE = var.get()  #Values are type 'Entry'.
    mE2 = var2.get()
    
    mT ='ig.'+mE[:]
    mT2 ='ig.'+mE2[:]
    
    print(mT[:])  #Verified Correct Values of molecules
    print(mT2[:])
    
    M = pm.get(str(mT))
    M2 = pm.get(str(mT2))
    
    help(M.h)
    
    print()
    print(M.h())
    print(M2.h())

    print()
    print(M.s()*1000)
    print(M2.s()*1000)
    
E = Entry(master,textvariable=var, width=30)
E.pack(pady=(0,25))
E.insert(0,'AlCl3')  #Auto assigning user input for easy testing
E2 = Entry(master,textvariable=var2, width=30)
E2.pack(pady=(0,25))
E2.insert(0,'H2O')

Button(master, text='Solve', command=grab).pack()

master.mainloop()