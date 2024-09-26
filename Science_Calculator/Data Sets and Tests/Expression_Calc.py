# -*- coding: utf-8 -*-
"""
Created on Sat Nov 21 01:00:39 2020

@author: Heath
"""

from tkinter import *
import tkinter
import math
import turtle
from turtle import*

def IdealGasGUI():  #IDEAL GAS LAW
       
        master = Tk()
        master.geometry('574x500')
        master.title('Science Calculator © 2020 Heath Marchese All Rights Reserved')

        Label(master, text="Science Calculator - Differential Calc", font='Helvetica 24 bold').grid(row=0,rowspan=2,column=0,columnspan=4,pady=(0,30),sticky=N)

        Label(master, text="What's your Expression?", font ='Helvetica 14').grid(row=10,rowspan=1,column=0,columnspan=4,pady=(0,20))
        
        textentry = Entry(master, width=50)
        textentry.grid(row=11, column= 0, columnspan=4)
        
        Button(master, text="Solve", font ='Helvetica 12 bold', command="").grid(row=12,column=1,columnspan=2,pady=(270,0))
        Button(master, text="Return", font ='Helvetica 12 bold', command=master.quit).grid(row=13,column=1,pady=(10,0))
        Button(master, text="Quit", font ='Helvetica 12 bold', command=master.quit).grid(row=13,column=2,pady=(10,0),padx=(20))
        
        master.mainloop()
        
IdealGasGUI()

