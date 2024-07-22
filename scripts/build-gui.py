import sys
import os
import getopt
import shutil
import lib.cmake_build as cmake_build
from lib.build_global_data import BuildGlobalData
import lib.log as log

from tkinter import *
 
# create root window
root = Tk()
 
# root window title and dimension
root.title("Build GUI")
# Set geometry(widthxheight)
root.geometry('600x400')
 
# adding a label to the root window
lbl = Label(root, text = "Args")
lbl.grid()
 
# adding Entry Field
txt = Entry(root, width=20)
txt.grid(column =1, row =0)
 
 
# function to display user text when 
# button is clicked
def clicked():
    pass
    # res = "You wrote" + txt.get()
    # lbl.configure(text = res)
 
# button widget with red color text inside
btn = Button(root, text = "Build" ,
             fg = "red", command=clicked)
# Set Button Grid
btn.grid(column=0, row=2)

# # Change the label text 
# def show(): 
#     label.config( text = clicked.get() ) 
  
options = [ 
    BuildGlobalData.buildDebug, 
    BuildGlobalData.buildRelease,
] 
  
# datatype of menu text 
clicked = StringVar() 
  
# initial menu text 
clicked.set( BuildGlobalData.buildDebug ) 
  
# Create Dropdown menu 
drop = OptionMenu( root , clicked , *options ) 
drop.grid(column=2, row=0)
  
# # Create button, it will change label text 
# button = Button( root , text = "click Me" , command = show )
# button.grid(column=1, row=1)

# # Create Label 
# label = Label( root , text = " " ) 
# label.grid(column=2, row=1)
 
# Execute Tkinter
root.mainloop()