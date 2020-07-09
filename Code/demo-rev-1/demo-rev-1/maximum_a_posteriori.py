import math
import numpy as np

# POSTERIOR_GIVEN_READINGS
'''
	
	This function takes in the preset constants and input variables and outputs
	a posterior distribution.
	
		Preset Constants
			mean profile for each sensor (means)
			covariance matrices (cov)
			inverse covariance matrices (cov_inv)
		
		Input Variables
			prior distribution (prior)
			readings from each sensor (readings)
	
		Intermediate Variables
			implied position (x_star)
			area of the posterior distribution prior to normalization
				(post_area)
			probability of current implied position given readings (post_temp)
		
		Output
			posterior distribution (posterior)
	
	P( x* | {g_i} ) = 1/Z({g_i}) * P( {g_i} | x* ) * P( x* )
	***
		for x* in {x*_i}
			calculate P( {g_i} | x* )
			multiply by prior(x*)
			add to area under posterior distribution
			append to posterior
		
		elementwise divide the posterior by the area under the distribution
		
		return posterior
	***
	
'''
def posterior_given_readings(means,cov,cov_inv,prior,readings):
	posterior = []
	post_area = 0
	K = len(readings)
	for x_star,p in enumerate(prior):
		post_temp = readings_given_position(means,cov,cov_inv,K,readings,x_star)
		post_temp *= p
		post_area += post_temp
		posterior.append(post_temp)
	posterior = list(map(lambda x: x/post_area, posterior))
	return posterior

# READINGS_GIVEN_POSITION
'''

	This function takes in the preset constants and input variables, and it
	outputs the probability of the readings received for the position in
	question.
	
		Preset Constants
			mean profile for each sensor (means)
			covariance matrices (cov)
			inverse covariance matrices (cov_inv)
			number of readings (K)
		
		Input Variables
			readings from each sensor (readings)
			implied position (x_star)
		
		Intermediate Variables
			dimension of the covariance matrix (K)
			
		Output
			likelihood of the readings given the implied position (likelihood)
			
	P( {g_i} | x* ) = e^(-chi_K_squared( {g_i}, x* )/2) / sqrt( (2pi)^K * det(C(x)))
	***
		num = chi_K_squared( {g_i}, x* )
		num = num / 2
		num = exp(-num)
		den = pow(2*pi,K)
		den = den * det(C( x* ))
		likelihood = num / den
		return likelihood
	***
	
'''
def readings_given_position(means,cov,cov_inv,K,readings,x_star):
	#return math.exp(-chi_K_squared(means,cov_inv,K,readings,x_star)/2)/math.sqrt((math.pow(2*math.pi,K) * np.linalg.det(cov[x_star])))
	chi = chi_K_squared(means,cov_inv,K,readings,x_star)
	num = math.exp(-chi/2)
	den = math.sqrt(math.pow(2*math.pi,K)*np.linalg.det(cov[x_star]))
	return num / den

# CHI_K_SQUARED
'''
	
	This function takes in the preset constants and input variables, and it
	outputs the similarity of the readings to the mean sensor activation
	profiles for a given position.
	
		Preset Constants
			mean profile for each sensor (means)
			inverse covariance matrices (cov_inv)
			width of the inverse covariance matrix (K)
			
		Input Variables
			readings from each sensor (readings)
			position (x)
			
		Intermediate Variables
			sum of terms (sum)
			temporary product of ith,jth terms (temp)
		
		Output
			similarity of the readings to the mean sensor activation profiles
				for a given position (chi)
	
	chi = \Sigma_{i,j=1}^K (g_i - \bar{g}_i(x)) * (\hat{C}^{-1}(x))_{ij} * (g_j - \bar{g}_j(x))
	***
		sum = 0
		for i in K
			for j in K
				product = g_i - means[i][x]
				product = product * (g_j - means[j][x])
				product = product * cov_inv[x][i][j]
				sum = sum + product
		chi = sum
		
		return chi
	***
	
'''
def chi_K_squared(means,cov_inv,K,readings,x):
	chi = 0
	for i in range(K):
		for j in range(K):
			chi += (readings[i] - means[x][i])*(readings[j] - means[x][j])*(cov_inv[x][i][j])
	return chi
