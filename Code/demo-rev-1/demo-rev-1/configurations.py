# NOTE: a "reading" is a list of values, field strength measurements taken from each of the sensors
# DONE: generate an idealized curve representing the activation profile of a simulated sensor centered at the origin
# DONE: displace the idealized activation curve to represent the idealized activation profile of each sensor
# TODO: for each point x on the length of the gauge, generate a list of N noisy readings
# TODO: for each point x on the length of the gauge, generate a covariance matrix from the noisy readings
# TODO: for each point x on the length of the gauge, generate an inverse matrix from the covariance matrix

import math
import numpy as np
from scipy import linalg as spla
import matplotlib.pyplot as plt

def in_to_mm(i):
	return int(i * 25.4) + 1
def translate(x,f_min,f_max,g_min,g_max): # translates x from range [f_min,f_max] to range [g_min,g_max]
	return (x-f_min) * (g_max-g_min) / (f_max-f_min) + g_min
def compose2(f,g):
	return lambda x: f(g(x))
def make_subtractor_lambda(n):
	return lambda x: x-n
def idealized_profile(w,h):
	mapper = lambda x: translate(x,0,w,0,0.860333589)
	result = lambda x: 0 if (abs(mapper(x)) > 4*w) else h*math.sin(mapper(x)) * math.exp(-mapper(x)**2 * 0.5) / math.sqrt(2*math.pi)
	return result
def sensor_curves(profile,placements):
	offset_functions = map(lambda x: make_subtractor_lambda(x),placements)
	return list(map(lambda x: compose2(profile,x),offset_functions))
def noisy_readings(sensors,noise,x,num_readings):
	return [[s(x)+np.random.normal(0,noise) for s in sensors] for r in range(num_readings)]
def artificial_dataset(sensors,noise,gauge,num_readings):
	return [noisy_readings(sensors,noise,x,num_readings) for x in gauge]
def covariance_matrices(dataset):
	C = [np.cov(d,rowvar=False) for d in dataset]
	C_inv = [np.linalg.inv(c) for c in C]
	return C,C_inv
def mean_profiles(dataset):
	pos,read,sens = range(len(dataset)),range(len(dataset[0])),range(len(dataset[0][0]))
	return [
		[np.mean([dataset[x][r][s] for r in read]) for s in sens] for x in pos]
def uniform_prior_distribution(l):
	return [1/l for x in range(l)]
def draw_actual_position(pos,height):
	plt.plot([pos,pos],[0,height],'r',linewidth=2.0)
