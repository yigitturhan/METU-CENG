import math
import random
random.seed(111)
inpt = [100, 100, 5, 70, 30, 0.5, [[(37, 55), 6, 'notmasked', 'notinfected'], [(41, 78), 0, 'masked', 'notinfected'],
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
universal_state = inpt[6]
M , N , D , k , lmbda, mu = inpt[0], inpt[1], inpt[2] , inpt[3] , inpt[4], inpt[5]
def new_move():
	global universal_state
	placeholder , placeholderV2 = [] , [] #INITIAL VERSION and version after movement
	correctplaceholder = []   # correct version(whether a place is full or whether this place is in universe)
	lastmove = []  #firstversion
	lastmoveV2 = []  #secondversion
	maskstatus = [] #THIS LIST WILL NOT BE CHANGED
	infectionstatus = [] #INITIAL
	infectionstatusV3 = [] #AFTER INTERACTION
	for i in universal_state:
		placeholder.append(i[0])
		lastmove.append(i[1])
		maskstatus.append(i[2])
		infectionstatus.append(i[3])
	def distance(c,d): #DISTANCE CALCULATOR
		return math.sqrt((d[0]-c[0])**2 + (d[1]-c[1])**2)
	colors = ["green", "yellow2", "blue2", "purple2", "grey", "purple1", "blue1", "yellow1"]
	possibilitiesofcolors = [mu / 2, mu / 8, 0.5 * (1 - mu - mu ** 2), 0.4 * mu ** 2, 0.2 * mu ** 2, 0.4 * mu ** 2,
							 0.5 * (1 - mu - mu ** 2), mu / 8]
	color_offsets = {
		"yellow1": (0, 1),
		"green": (-1, 1),
		"yellow2": (-1, 0),
		"blue1": (1, 1),
		"blue2": (-1, -1),
		"purple1": (1, 0),
		"grey": (1, -1),
		"purple2": (0, -1),
	}
	# BEGINNING OF MOVEMENT PART
	for i in universal_state:
		a = random.choices(colors, weights=possibilitiesofcolors, k=1)[0]
		offset = color_offsets[a]
		x, y = i[0][0] + offset[0], i[0][1] + offset[1]
		placeholderV2.append((x, y))
	for i in range(0,len(placeholder)):  #THIS WILL CHECK WHETHER THE PLACE IS EXİSTED OR OCCUPIED BEFORE
		if (0 <= placeholderV2[i][0] < N) and (0 <= placeholderV2[i][1] < M) and (placeholderV2[i] not in placeholder[i+1:]) and (placeholderV2[i] not in placeholderV2[:i]):
			correctplaceholder.append(placeholderV2[i])
		else:
			placeholderV2[i] = placeholder[i]
			correctplaceholder.append(placeholder[i])
	#BEGINNING OF LASTMOVE PART
	lastmove_map = {
		(0, 1): 0,
		(-1, 1): 1,
		(-1, 0): 2,
		(-1, -1): 3,
		(0, -1): 4,
		(1, -1): 5,
		(1, 0): 6,
		(1, 1): 7,
	}
	lastmoveV2 = [lastmove_map((i[0] - placeholder[a][0], i[1] - placeholder[a][1])) for a, i in
				  enumerate(correctplaceholder)]
	#BEGINNING OF INFECTION PART
	for i in range(len(infectionstatus)):
		infectionstatusV2 = []
		if infectionstatus[i] == "infected":
			infectionstatusV3.append("infected")
		if infectionstatus[i] == "notinfected":
			for s in range(len(infectionstatus)):
				if infectionstatus[s] == "infected" and distance(correctplaceholder[i], correctplaceholder[s]) <= D:
					if maskstatus[i] == "masked" and maskstatus[s] == "masked":
						dstsqr = distance(correctplaceholder[i], correctplaceholder[s])**2
						olslk = min(1, k / (dstsqr))/(lmbda**2)
						w = random.choices(["infected", "notinfected"], weights=[olslk, (1-olslk)], k=1)
						infectionstatusV2.append(w[0])
					if maskstatus[i] == "notmasked" and maskstatus[s] == "masked":
						dstsqr = distance(correctplaceholder[i], correctplaceholder[s])**2
						olslk = min(1, k / (dstsqr))/(lmbda)
						w = random.choices(["infected", "notinfected"], weights=[olslk, (1-olslk)], k=1)
						infectionstatusV2.append(w[0])
					if maskstatus[i] == "masked" and maskstatus[s] == "notmasked":
						dstsqr = distance(correctplaceholder[i], correctplaceholder[s])**2
						olslk = min(1, k / (dstsqr))/(lmbda)
						w = random.choices(["infected", "notinfected"], weights=[olslk, (1-olslk)], k=1)
						infectionstatusV2.append(w[0])
					if maskstatus[i] == "notmasked" and maskstatus[s] == "notmasked":
						dstsqr = distance(correctplaceholder[i], correctplaceholder[s])**2
						olslk = min(1, k/(dstsqr))
						w = random.choices(["infected", "notinfected"], weights=[olslk,(1-olslk)], k=1)
						infectionstatusV2.append(w[0])
			if "infected" in infectionstatusV2:
				infectionstatusV3.append("infected")
			else:
				infectionstatusV3.append("notinfected")
	#THE DATA GATHERING PART
	universal_state = []
	for i in range(0,len(infectionstatus)):
		universal_state.append([correctplaceholder[i]] + [lastmoveV2[i]] + [maskstatus[i]] + [infectionstatusV3[i]]) #I PREFERRED TO COLLECT ALL THE DATA IN SEPERATE LISTS THAN BRING THEM TOGETHER
	return universal_state
