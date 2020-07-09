import numpy as np
import math
import maximum_a_posteriori as m_a_p
import configurations as cfg
import matplotlib.pyplot as plt

w = cfg.in_to_mm(2)
h = 1000
noise = 0.1 * h
K = 4
inches_between_sensors = 2
placements = [cfg.in_to_mm(inches_between_sensors*x) for x in range(1,K+1)]
board_length = cfg.in_to_mm(inches_between_sensors*(K+1))
gauge = list(range(board_length))
num_readings = 100
test_position = placements[0]
prior = cfg.uniform_prior_distribution(board_length)

profile = cfg.idealized_profile(w,h)
sensors = cfg.sensor_curves(profile,placements)
dataset = cfg.artificial_dataset(sensors,noise,gauge,num_readings)
C,C_inv = cfg.covariance_matrices(dataset)
means = cfg.mean_profiles(dataset)

test_reading = [s(test_position)+np.random.normal(0,noise) for s in sensors]

#PLOT MEAN CURVES
#for i,s in enumerate(sensors):
	#plt.plot(gauge,[means[x][i] for x in gauge],label='Sensor {}'.format(i))
#plt.scatter([test_position]*K,test_reading,label='Test Readings')
#plt.legend()
#plt.show()

#PLOT DATSET FOR TeST POSITION (only works for K=2)
#data = np.transpose(dataset[test_position])
#cov = np.transpose(C[test_position])
#plt.scatter(data[0],data[1])
#plt.plot([0,cov[0][0]],[0,cov[0][1]])
#plt.plot([0,cov[1][0]],[0,cov[1][1]])
#plt.show()

likelihoods = []
for x in gauge:
	likelihood = m_a_p.readings_given_position(means,C,C_inv,K,test_reading,x)
	likelihoods.append(likelihood)

posterior = prior
plt.plot(gauge,posterior)
plt.show()

for i in range(10):
	prior = posterior
	test_reading = [s(test_position)+np.random.normal(0,noise) for s in sensors]
	posterior = m_a_p.posterior_given_readings(means,C,C_inv,prior,test_reading)
	height = max([max(posterior),max(prior)])
	plt.plot(gauge,prior,label="prior")
	plt.plot(gauge,posterior,label="posterior")
	cfg.draw_actual_position(test_position,height)
	plt.legend()
	plt.show()

test_position = board_length-1
print("moving float")

for i in range(20):
	prior = posterior
	test_reading = [s(test_position)+np.random.normal(0,noise) for s in sensors]
	posterior = m_a_p.posterior_given_readings(means,C,C_inv,prior,test_reading)
	height = max([max(posterior),max(prior)])
	plt.plot(gauge,prior,label="prior")
	plt.plot(gauge,posterior,label="posterior")
	cfg.draw_actual_position(test_position,height)
	plt.legend()
	plt.show()
