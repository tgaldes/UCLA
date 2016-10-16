from random import randint
from random import gauss
from math import sqrt
import scipy
from scipy.stats import norm
import numpy



CASE_RELATEDNESS = 0.0
CONTROL_RELATEDNESS = 0.0

def printPatients():
	for i in range (1000):
		for j in range (201):
			print patients[i][j],
		print " "
print("This is project 1 for CS 124")
print("Coded by Tyler Galdes, 5/4/15")
print("Written on linux VM with vim")

def printPatientSubset(cases, controls, numGenes):
	print("We will print out the first " + str(numGenes) + " for each patient")
	print("Here are the genotypes of " + str(cases) + " cases.") 
	for i in range(cases):
		for j in range(numGenes):
			print patients[i][j],
		print " "
	print("Here are the genotypes of " + str(controls) + " controls.")
	
	for i in range(numCases, numCases + controls):
		for j in range(0, numGenes):
			print patients[i][j],
		print " "

def createSNPProbabilities():
#first get case probabilites, this will be a column-wise traversal
	for i in range(1, 201):
		runningTotal = 0
		for j in range(numCases):
			runningTotal += patients[j][i]
		#now calculate the probability
		probabilities[0][i - 1] = runningTotal/float(numCases)#here we make sure to store the 0th SNP in the 0th column, ignoring the case/control column
	
	#now do the same for the controls
	for i in range(1, 201):
		runningTotal = 0
		for j in range(numCases, 1000):
			runningTotal += patients[j][i]
		probabilities[1][i - 1] = runningTotal/float(numControls)

def individualSNPTest(a, alpha, M):

	pplus = probabilities[0][a]
	pminus = probabilities[1][a]
	pa = (pplus + pminus)/2
	
	ncp = ((pplus - pminus)/(sqrt(2/float(1000))*sqrt(pa*(1-pa))))
	ncps[a] = ncp
	alphas = alpha/M
	power = norm.cdf(norm.ppf(alphas/2) + ncp) + 1 -norm.cdf(-norm.ppf(alphas/2) + ncp)

	powers[a] = power

def printPowers():
	for i in range(200):
		print powers[i]

def doNumPowerTest(alpha, M):
	for i in range(M):
		individualSNPTest(i, alpha, M) 

def significanceTest(alpha, M, maxPower):
	if maxNcp > -1*norm.ppf(alpha/float(M)):
		print("The study had a significant SNP")
	else:
	 	print("The study had no significant SNP")



def correlationMatrix(printing, a, b):
	for i in range(a):
		for j in range(i, b):
			cor[i][j] = numpy.corrcoef(SNPs[i], SNPs[j])[0][1]
	if printing == 1:
		for i in range(a):
			for j in range(b):
				print("%.2f" % cor[i][j]),
			print " "
	#now we can set up the indices matrix when we have correlated SNPs
	count = 0
	for i in range(a):
		for j in range(i, b):
			if i != j and abs(cor[i][j]) >= .1:
				corIndices[count][0] = i
				corIndices[count][1] = j
				count = count + 1
	#now we'll print as a sanity check to see if we got anything
	print("The number of correlations we have is: " + str(count))
	if printing == 1:
		for i in range(count):
			print corIndices[i][0],
			print("    "),
			print corIndices[i][1]
	
	#we can separate the correlated SNPs from the non-correlated SNPs 
	#because we know we will have to tag every SNP that is not correlated with another SNP
	#This means our greedy algorithm only really needs to look at the SNPs that have correlations

	#first we will set up numCors
	for i in range(count):
		numCors[corIndices[i][0]] = numCors[corIndices[i][0]] + 1
		numCors[corIndices[i][1]] = numCors[corIndices[i][1]] + 1
		
	if printing == 1:
	 	for i in range(200):
			print numCors[i]
	#now the greedy algorithm comes into play, we need to loop through numCors repeadtedly
	#and tag the SNP with the most correlations and mark the SNPs that are correlated with it as tagged
	#NOTE: were I more concerned with the efficiency it would have been neccessary to do a quick/merge
	#sort for the numCors to avoid looping through a 200 element array 200 times, which is by no means
	#good programming (in this inplementation I would also need to have numCors be a 2d array, where the
	#SNP numbers were linked with the number of correlations).
	numSNPsTagged = 0
	for i in range(a):
		maxIndex = 0
		maxCors = 0
		for j in range(b):
			if tagged[j] == 0 and numCors[j] > maxCors:
				maxCors = numCors[j]
				maxIndex = j
		#if the maxCors is still zero, we have enough SNPs tagged to cover all 200
		if maxCors == 0:
			break
		#now that we found the next SNP to tag, we need to set it as tagged and set
		#every SNP it is correlated with as tagged
		tagged[maxIndex] = 1
		for j in range(count):
			if corIndices[j][0] == maxIndex or corIndices[j][1] == maxIndex:
				tagged[corIndices[j][0]] = 1
				tagged[corIndices[j][1]] = 1
		bestSet[numSNPsTagged] = maxIndex
		numSNPsTagged += 1
	print("This is a list of all the SNPs we'll need to tag to cover all 200")
	for i in range(numSNPsTagged):
		print bestSet[i]
	print("The total number of SNPs we tagged was: " + str(numSNPsTagged))


def transposeMatrix():
	for i in range(1000):
		for j in range(1, 201):
			SNPs[j-1][i] = patients[i][j]

def simulatePower(numDraws, NCP):
#we will simulate power by repeatedly taking a random draw from a distribution
#with S.D. 1 and mean equal to the ncp of the causal SNP
	total = 0
	for i in range(numDraws):
		total += gauss(NCP, 1)
	average = total / numDraws
	power = norm.cdf(average)
	print("The power of the test is: " + str(power))
		

patients = [[0 for x in range(201)] for x in range (1000)] #2d array for patients
#patient[0][0-200] is the patients entire set of SNPs

#create array to hold the case and controls probabilities for every SNP
#prob[0][i] is the cases probability of SNP i, and vice versa for prob[1][i]
probabilities = [[0 for x in range(200)] for x in range(2)]

#set up array for power of each SNP test
powers = [0 for s in range(200)]

#set up array for ncps
ncps = [0 for x in range(200)]

#set up correlation matrix
cor = [[0 for x in range(200)] for x in range(200)]

#set up array for SNPs (transpose of patients)
SNPs = [[0 for x in range(1000)] for x in range(200)]

#set up array to indicate whether or not an SNP is tagged for greedy algo
tagged = [0 for x in range(200)]
#set up indices for every correlation > 0.10 (arbitrariliy long)
corIndices = [[0 for x in range(2)] for x in range(1000000)]
#set up number of Correlations for every SNP
numCors = [1 for x in range(200)]

#set up array for the SNPs we would actually tag
bestSet = [0 for x in range(200)]


numCases = 100
numControls = 1000 - numCases

#set up who is a case and who is a control
for i in range(numCases):
	patients[i][0] = 1 #case
for i in range(numCases, numControls):
	patients[i][0] = 0 #controls


#now we will set up the truly random SNPs for each patient
#these are in ranges [0-999][2-200] since patients [i][1] is the causal SNP
for i in range(1000): #go through each patient
	for j in range(2, 201): #go through each genotype
		patients[i][j] = randint(0, 1)

#now comes the trick part of setting up genotypes, the causal SNP (patients[i][1])
#this loop is to set up the cases causal SNP
for i in range(numCases):
	rand = randint(0,99)
	if patients[i][0] == 1 and rand > 94:#cases have 95% probability of having a 1 in [i][1]
		patients[i][1] = 0 
	else:
	 	patients[i][1] = 1

#this loop is to set up the controls causal SNP
for i in range(numCases, numControls):
	rand = randint(0,3)
	if patients[i][0] == 0 and rand == 3:#controls have 25% probability of having a 1 in [i][1]
		patients[i][1] = 1
	else:
		patients[i][1] = 0

#here we will set up the related people if we are doing part two
related = 1
if related == 1:
	numRelatedCases = numCases * CASE_RELATEDNESS #changing this constant will make the patients more or less related to each other, and will affect
	numRelatedControls = numControls * CONTROL_RELATEDNESS #how many correlated SNPs we end up with

	for i in range(int(numRelatedCases)):
		patients[i] = patients[0]
	for i in range(numCases, int(numRelatedControls)):
		patients[i] = patients[numCases]
transposeMatrix()
createSNPProbabilities()
doNumPowerTest(0.05, 200)
printPatientSubset(20, 20, 5)


#to find whether or not the test is significant, we need to find the maximum power
maxPower = 0
maxNcp = 0
for i in range(200):
	if powers[i] > max:
		maxPower = powers[i]
	if ncps[i] > maxNcp:
		maxNcp = ncps[i]

#test if this is significant
significanceTest(0.05, 200, maxNcp)
simulatePower(1000, ncps[0])
correlationMatrix(0, 200, 200)









