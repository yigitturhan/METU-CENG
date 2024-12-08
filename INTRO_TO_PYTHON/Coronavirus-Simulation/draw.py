from math import *
from tkinter import *
from evaluator import *
from the2 import *   # provides the new_move() which is called in move_individuals()


DELAY = 1000 # that is the default value, can be changed. It is in [miliseconds]

WINDOW_MAXIMAL_WIDTH = 1000
WINDOW_MAXIMAL_HEIGHT= 800

DATA =  [100, 100, 5, 70, 30, 0.5, [[(37, 55), 6, 'notmasked', 'notinfected'], [(41, 78), 0, 'masked', 'notinfected'],
                                   [(44, 8), 0, 'notmasked', 'notinfected'], [(47, 8), 0, 'masked', 'notinfected'],
                                   [(50, 15), 0, 'notmasked', 'notinfected'], [(53, 63), 0, 'masked', 'notinfected'],
                                   [(51, 48), 0, 'notmasked', 'notinfected'], [(20, 11), 6, 'notmasked', 'notinfected'],
                                   [(23, 11), 2, 'masked', 'notinfected'], [(37, 13), 6, 'notmasked', 'notinfected'],
                                   [(10, 13), 2, 'notmasked', 'notinfected'], [(37, 16), 6, 'notmasked', 'infected'],
                                   [(55, 16), 2, 'masked', 'infected'], [(37, 18), 6, 'notmasked', 'notinfected'],
                                   [(4, 18), 2, 'notmasked', 'notinfected'], [(28, 21), 2, 'masked', 'infected'],
                                   [(37, 22), 6, 'notmasked', 'infected'], [(37, 25), 6, 'notmasked', 'notinfected'],
                                   [(54, 85), 2, 'notmasked', 'notinfected'], [(54, 28), 2, 'masked', 'notinfected'],
                                   [(37, 42), 6, 'notmasked', 'infected'], [(37, 32), 6, 'notmasked', 'notinfected'],
                                   [(14, 2), 2, 'notmasked', 'notinfected'], [(15, 35), 6, 'notmasked', 'infected'],
                                   [(54, 15), 2, 'masked', 'notinfected'], [(37, 37), 6, 'notmasked', 'notinfected'],
                                   [(53, 37), 2, 'notmasked', 'notinfected'], [(3, 40), 6, 'notmasked', 'notinfected'],
                                   [(23, 40), 3, 'masked', 'notinfected'], [(31, 43), 6, 'notmasked', 'notinfected'],
                                   [(53, 43), 2, 'notmasked', 'notinfected']]]
[M,N,D,KAPPA,LAMBDA,MU] = DATA[:6]

SCALE = min(WINDOW_MAXIMAL_WIDTH/N,WINDOW_MAXIMAL_HEIGHT/M)

WINDOW_STEP = round(SCALE)
RADIUS = max(1, SCALE//2)

def draw_individual(canv,x,y,rad,out_color,in_color):
	return canv.create_oval((x-rad),(y-rad),(x+rad),(y+rad),width=1, outline=out_color, fill=in_color)

def redraw_individual(canv,item,x,y,rad,out_color,in_color):
    canv.itemconfig(item,outline=out_color, fill=in_color)
    canv.coords(item,(x-rad),(y-rad),(x+rad),(y+rad))

def draw_legend(canvas): 
  def draw_one(canvas, X, Y, outline, fill, text):
    canvas.create_oval(X-RADIUS, Y-RADIUS, X+RADIUS, Y+RADIUS, width=1, outline=outline, fill=fill)
    canvas.create_text(X+2*RADIUS+RADIUS, Y, anchor=W, font="Purisa", text=text)
  
  draw_one(canvas, 3+RADIUS, 3+RADIUS,   "red", "white", "Masked & Infected")
  draw_one(canvas, 3+RADIUS, 3+RADIUS*4, "green", "white", "Masked & Not Infected")
  draw_one(canvas, 3+RADIUS, 3+RADIUS*7, "red", "red", "Not Masked & Infected")
  draw_one(canvas, 3+RADIUS, 3+RADIUS*10, "green", "green", "Not Masked & Not Infected")

def draw_individuals(): #inialization of _individuals 
    global My_canvas, My__individuals, DATA, RADIUS, SCALE, DELAY
    My__individuals=[]   # hold tctk entities
    for [(x,y),last_move,mask_status, infection_status]  in DATA[6:][0]:
        center_x = int(x*SCALE)
        center_y = int(y*SCALE)
        out_color = "red" if infection_status=="infected" else "green"
        in_color = out_color if mask_status=="notmasked" else "white"
        My__individuals.append(draw_individual(My_canvas,center_x,center_y,RADIUS,out_color,in_color))
    draw_legend(My_canvas)
    My_canvas.after(DELAY,callback)

def move_individuals():   
    global My__individuals, My_canvas
    new_universe = new_move() # the2.py  provides new_move()
    for i,[(x,y), last_move,mask_status, infection_status] in enumerate(new_universe):
        new_center_x = x*SCALE
        new_center_y = y*SCALE
        new_out_color = "red" if infection_status=="infected" else "green"
        new_in_color = new_out_color if mask_status=="notmasked" else "white"
        redraw_individual(My_canvas, My__individuals[i], new_center_x, new_center_y, RADIUS, new_out_color, new_in_color)
        
def callback(): 
    global My_canvas,DELAY
    move_individuals()
    My_canvas.after(DELAY,callback)
        
Master = Tk()

My_canvas = Canvas(Master, width=N*SCALE, height=M*SCALE)

My_canvas.pack()
draw_individuals()

Master.mainloop()
