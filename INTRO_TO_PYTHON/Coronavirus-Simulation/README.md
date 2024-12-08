# Coronavirus-Simulation
This is a simulation code written in python. It takes data from get_data() function. It can be changed. As a summary, this program takes people which has properties as listed below:
1- coordination given as (x,y)
2- mask status (masked or unmasked)
3- infection status (infected or not-infected)
The purpose of the program is creating a grid which has people on it as green or red dots. It adds a movement algorithm to that people. Whenever they come closer than a given number (5 in default), if one of them is infected, the other one may get infected too. The possibility of getting infected depends on the mask status and the distence between these two people. The possibility increases while getting closer and being unmasked. The mask status and probability of getting infected relationship is in below (Think like first person is infected and second is not-infected):
1- masked, masked  --> lowest possibility
2- masked, unmasked  --> second lowest
3- unmasked, masked  --> second highest
4- unmasked, unmasked  --> highest
To sum up, this code creates a coronavirus simulation based on some movement and getting infected algorithms.
