# -*- coding: utf-8 -*-
"""
Created on Fri Nov 20 15:41:30 2020

@author: Heath
"""
from tkinter import *
import tkinter
import math
import turtle
from turtle import*

class Conversions: #Name of first unit respective to variable in methods. Name of second unit is returned
    """     Pressure Conversions     """
    def mmHg_ATM(master,a):
        return (a / 760)
    def mmHg_kPa(master,a):
        return (a / 7.50062)
        
    def kPa_mmHg(master,a):
        return ''
    def kPa_ATM(master,a):
        return (a / 101.325)
    def kPa_torr(master,a):
        return ''
        
    def ATM_torr(master,a):
        return ''
    def ATM_mmHg(master,a):
        return (a * 760)
    def ATM_kPa(master,a):
        return ''
    
    def torr_mmHg(master,a):
        return ''
    def torr_ATM(master,a):
        return (a / 760)
    def torr_kPa(master,a):
        return ''
    
    """     Volume Conversions     """
    def cm_L(master,a):
        return (a / 1000)
    
    def L_cm(master,a):
        return (a * 1000)
    
    """     Temperature Conversions     """
    def C_K(master,a):
        return (a + 273.15)
    def C_F(master,a):
        return ''
    
    def F_C(master,a):
        return (a / 101.325)
    def F_K(master,a):
        return (((a - 32) * 5 / 9) + 273.15)
    
    def K_C(master,a):
        return ''
    def K_F(master,a):
        return (a / 101.325)
 
    