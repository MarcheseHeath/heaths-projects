def VectorMath():
            master = Tk()
            
            def VectorMag():
                x = 0
                
            def VectorAdd():        
                slave = Tk()
                
                slave.title('Science Calculator © 2020 Heath Marchese Some Rights Reserved')
 
                Label(slave, text='Choose Your Needed Calculator', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(30),pady=(0,30))
        
                Button(slave, text="Return", font ='Helvetica 12 bold', command=slave.destroy).grid(row=6,column=1,columnspan=2,pady=(100,0))
    
                slave.mainloop()
                
            master.title('Science Calculator © 2020 Heath Marchese Some Rights Reserved')
 
            Label(master, text='Choose Your Needed Calculator', font='Helvetica 24 bold').grid(row=0,column=0,columnspan=4,padx=(30),pady=(0,30))
            
            Button(master, text="Vector Addition", font ='Helvetica 12 bold', command=VectorAdd).grid(row=1,column=1,columnspan=2)
            
            Button(master, text="Return", font ='Helvetica 12 bold', command=master.destroy).grid(row=6,column=1,columnspan=2,pady=(100,0))
    
            master.mainloop()