from sys import argv
import time
from random import randint
import pdb

patientNum = 0
patientNum2 = 0
numlines = 10
numpatients = 40
maxHaps = numpatients * 2
numHaps = 0
correctness = 0.0
outOfBounds = False
startTime = time.time()
finishTime = time.time()
matrix = [['X' for x in range(2)] for x in range(numlines + 1)]
geno = [['X' for x in range(numpatients * 2)] for x in range(numlines)]
genoNums = [[3 for x in range(numlines)] for x in range(numpatients)]
haps = [[3 for x in range(numlines)] for x in range(maxHaps)]
indivHaps = [[3 for x in range(numlines)] for x in range(numpatients * 2)]
#this will track which genes we've saved haplotypes for
covered = [0 for x in range(numpatients)]
def initializeMatrices(nnumpatients, nnumlines, random):
    global matrix
    global geno
    global genoNums
    global haps
    global indivHaps
    global covered
    global numHaps
    global patientNum
    global maxHaps
    global numlines
    global numpatients
    maxHaps = numpatients * 2
    matrix = [['X' for x in range(2)] for x in range(nnumlines + 1)]
    geno = [['X' for x in range(nnumpatients * 2)] for x in range(nnumlines)]
    if not random:
        genoNums = [[3 for x in range(nnumlines)] for x in range(nnumpatients)]
    haps = [[3 for x in range(nnumlines)] for x in range(maxHaps)]
    indivHaps = [[3 for x in range(nnumlines)] for x in range(nnumpatients * 2)]
#this will track which genes we've saved haplotypes for
    covered = [0 for x in range(nnumpatients)]
    numHaps = 0
    patientNum = 0
    numlines = nnumlines
    numpatients = nnumpatients
def printMajorMinorMatrix():
    for i in range(1, numlines + 1):
        print("Major is " + matrix[i][0] + ", and minor is " + matrix[i][1] + " for line number " + str(i))

def printGenoMatrix():
    for i in range(numlines):
        for j in range(numpatients * 2):
            print(geno[i][j] + " "),
        print(" ")
def printHaplotypes():
    print("Here are our haplotypes")
    for i in range(maxHaps):
        for j in range(numlines):
            print(haps[i][j]),
        print(" ")

def printGenoNumsMatrix():
    for i in range(numpatients):
        for j in range(numlines):
            print(genoNums[i][j]),
        print(" ")
def printIndividualHaplotypes():
    for i in range(numpatients * 2):
        for j in range(numlines):
            print(indivHaps[i][j]),
        print(" ")

#returns true if newHaplotype is unique, false otherwise
def checkNewHaplotype(numHaplotypes, newHaplotype):
    moduleDebug = False
    if moduleDebug:
        print("We have " + str(numHaplotypes) + " so far.")
    for i in range(maxHaps):
        perfectMatch = True
        for j in range(numlines):
            if haps[i][j] != newHaplotype[j]:
                perfectMatch = False
                break
        if perfectMatch:
            if moduleDebug:
                print("The hap we attempted to add isn't new")
            return False
    if moduleDebug:
        print("The following hap is a new hap")
        print newHaplotype
    return True
#return true if new hap can match patient, false otherwise
def checkUncoveredPatient(i, newHap):
    moduleDebug = False
    if moduleDebug:
        print("We are comparing the geno and hap to see if they match (uncovered)")
        print genoNums[i]
        print newHap
    for j in range(numlines):
        if genoNums[i][j] == newHap[j]:
            continue
        elif genoNums[i][j] == 2:
            continue
        else:
            if moduleDebug:
                print("They do not match")
            return False
    if moduleDebug:
        print("They match")
    return True

#this will function returns true if the input hap can mesh with the first hap assigned to the patient, and false otherwise
def checkHalfCoveredPatient(i, newHap):
    if covered[i] != 1:
        print("ERROR: Patient in question should be half covered but is not\n\n\n\n\n")
    if indivHaps[i*2][0] == 3:
        print("ERROR: Patients first hap was not saved in the indiv hap matrix")

    moduleDebug = 0
    if moduleDebug:
        print("We are comparing genotype, first hap, and newhap (respectively)")
        print genoNums[i]
        print indivHaps[i*2]
        print newHap
    for j in range(numlines):
        #patient has a 0 or 1 that matches the new hap
        if genoNums[i][j] == newHap[j]:
            continue
        #patient's first hap is opposite of this one at position j
        elif genoNums[i][j] == 2 and newHap[j] != indivHaps[i*2][j]:
            continue
        else:
            if moduleDebug:
                print("They do not match")
            return False
    if moduleDebug:
        print("They match")
    return True
#this function will add the input hap to the specified patient's indiv hap entry in the matrix and update the value of covered
def addOldHaplotype(i, newHap):
    if covered[i] == 2:
        print("ERROR: Tried to add haplotype to fully covered patient\n\n\n\n\n\n\n\n")
    indivHaps[i*2 + covered[i]] = newHap
    covered[i] += 1


#this one will see if we can infer any new haps from half covered patients, and will make a call to addNewHaplotype if we can
def lookForMoreHaplotypes():
    moduleDebug =0 
    for i in range(numpatients):
#any half covered patient determines a new haplotype, if we find one we need to go through their first hap and genotype and create our new hap
        if covered[i] == 1:
            newHap = [0 for x in range(numlines)]
            for j in range(numlines):
                #case 0 is already taken care of
                if genoNums[i][j] == 1:
                    newHap[j] = 1
#here we make the new hap the complement of the patients first hap
                elif genoNums[i][j] == 2:
                    #case 0 is already taken care of again
                    if indivHaps[i*2][j] == 0:
                        newHap[j] = 1
            #done with making the new haplotype
            if moduleDebug:
                print("\nInside looking for more haplotypes")
                print("Here is genome, old hap, and newly created hap")
                print genoNums[i]
                print indivHaps[i*2]
                print newHap
                print ("\n")
            #now we recursively add the new happlotype
            addNewHaplotype(i, False, newHap)
def fill012matrix():
#now we need to go through and create a matrix of 0/1/2 representation
#for every patients genome
    moduleDebug = 0
#get every patient
    for j in range(numlines):
        #get every SNP
        for i in range(numpatients):
            #for each we'll compare their two alleles and see
            #what case they fall under
            one = geno[j][i*2]
            two = geno[j][i*2 + 1]
            if moduleDebug:
                print("First gene is " + one)
                print("Second gene is " + two)
                print(matrix[j][0] + matrix[j][1])
            #now we need to compare one and two against the major
            #and minor alleles

            #case 1: 0
            if one == matrix[j][0] and two == matrix[j][0]:
                genoNums[i][j] = 0
                if moduleDebug:
                    print "We found a double dominant genotype!"
            #case 2: 1
            elif one == matrix[j][1] and two == matrix[j][1]:
                genoNums[i][j] = 1
                if moduleDebug:
                    print "We found a double recessive genotype!"

            #case 3: 2
            else:
                genoNums[i][j] = 2
                if moduleDebug:
                    print "We found a split pair!"

#this returns the number of patients we can cover if we added the input hap, but does not actually modify the arrays
def testNewHaplotype(newHap):
    returnNumHaps = 0
    moduleDebug = 0
    for i in range(numpatients):
        if moduleDebug:
            print("Now looking for matches between test hap and patient: "+ str(i))
        if covered[i] == 2:
            continue
        elif covered[i] == 0 and checkUncoveredPatient(i, newHap):
            returnNumHaps += 1
        if covered[i] == 1 and checkHalfCoveredPatient(i, newHap):
            returnNumHaps += 1
    return returnNumHaps
#this sets everyone as covered
def cover(): 
    for i in range(numpatients):
        covered[i] = 2
#inserts a new hap, and checks to see if it can explain other patients
#returns how many patients were explained by this specific hap
def addNewHaplotype(patientIndex, firstCall, newHap):
    moduleDebug = False
    global numHaps
    global outOfBounds
    returnVal = 1
    if moduleDebug:
        print haps[numHaps]
    if numHaps == 40:
        printHaplotypes()
#the error is occuring, so cover all of the patients, print an error, and return
    if len(haps) == numHaps:
        cover()
        outOfBounds = True
        #print("THE TEST HAD AN OUT OF BOUNDS ERROR")
        return 0
    haps[numHaps] = newHap
#add this haplotype to the patients individual genome
    if covered[patientIndex] < 2:
        indivHaps[patientIndex*2 + covered[patientIndex]] = haps[numHaps]
        covered[patientIndex] += 1
    returnVal += lookForMoreMatches(numHaps, patientIndex)
    numHaps += 1
    if firstCall:
        lookForMoreHaplotypes()
    return returnVal 


#registers the new hap for all patients that can be covered by the new hap
def lookForMoreMatches(hapIndex, patientIndex):
    #go through all genotypes and see if the new haplotype explains anything
    #if it explains something with a 2, add the complement for that as
    #well, with a recursive call to addNewHaplotype
    returnNumHaps = 0
    moduleDebug = 0
    for i in range(numpatients):
        if moduleDebug:
            print("Now looking for matches between old hap and patient: "+ str(i))
#here there are three cases
#CASE #1: if patient is fully covered, we can move to the next patient to look for a genotype that can match this haplotype
        if covered[i] == 2:
            continue
#CASE #2: if a patient isn't covered at all, we check if we can make any match from their genotype to the new haplotype
        elif covered[i] == 0 and checkUncoveredPatient(i, haps[hapIndex]):
            addOldHaplotype(i, haps[hapIndex])
            returnNumHaps += 1
#CASE #3: if a patient is half covered, we check slightly differently, note that the above elif and this if can both be true (ex, all 0's)
        if covered[i] == 1 and checkHalfCoveredPatient(i, haps[hapIndex]):
            addOldHaplotype(i, haps[hapIndex])
            returnNumHaps += 1
    return returnNumHaps

#this returns true if the hap is all zeroes, false otherwise
def equalsZero(hap):
    for i in range(numlines):
        if hap[i] != 0:
            return False
    return True
#this function will return the hap that matches the specific patient and the largest number of other patients
#callNum will tell us where we are in the array
#currentHap is the hap we have so far constructed with the recursion
def recursiveTest(patientIndex, callNum, currentHap, usingZero, random):
    moduleDebug = False
    if moduleDebug:
        print("   " * callNum + "Recursive call with following hap. callNum="+ str(callNum))
        print currentHap
    #base case: we have filled the array and can now return the hap from this branch of recursion
    if callNum == numlines:
        return currentHap
    #recursive case 1: we know exactly what allele will go in position callNum, so set the array and make a recursive call
    if genoNums[patientIndex][callNum] == 0 or genoNums[patientIndex][callNum] == 1:
        currentHap[callNum] = genoNums[patientIndex][callNum]
        return recursiveTest(patientIndex, callNum + 1, currentHap, usingZero, random)
    #recursive case 2: there is a two, so we need to return whichever combo will cover more patients
    elif genoNums[patientIndex][callNum] == 2:
        if moduleDebug:
            print("\n" + "   " * callNum + " Current hap:")
            print currentHap
        
        dupHap = [0 for x in range(numlines)]
        dupHap1 = [0 for x in range(numlines)]
        for i in range(numlines):
            dupHap1[i] = currentHap[i]
            dupHap[i] = currentHap[i]
        dupHap[callNum] = 0
        hapZero = recursiveTest(patientIndex, callNum + 1, dupHap, usingZero, random)
        dupHap1[callNum] = 1
        hapOne = recursiveTest(patientIndex, callNum + 1, dupHap1, usingZero, random)

#if we are not using the all zero haplotype, check to make sure the haps we return aren't all zero
        if not usingZero and equalsZero(hapOne):
            return hapZero
        elif not usingZero and equalsZero(hapZero):
            return hapOne
#if we are picking a random haplotype, we are randomly return a haplotype
        if random:
            a = randint(0, 1)
            if a:
                return hapOne
            else:
                return hapZero
        one = testNewHaplotype(hapOne)
        zero = testNewHaplotype(hapZero)
        if one > zero:
            if moduleDebug:
                print("   " * callNum + "We are returning one")
            return hapOne
        else:
            if moduleDebug:
                print("   " * callNum + "We are returning zero")
            return hapZero

def testRecursiveTest():
    test = [3 for x in range(numlines)]
    usingZero = True
    test = recursiveTest(0,0,test, usingZero, False)
    #print test
    print testNewHaplotype(test)

#this returns true if we have a spanning set of haps, false otherwise
def checkCoverage():
    for i in range(numpatients):
        if covered[i] != 2:
            return False
    return True
#this returns true if newHap is already in our list of haps, false otherwise
def alreadyExists(newHap):
    global numHaps
    returnVal = False
    for i in range(numHaps):
        matchesThisHap = True
        for j in range(numlines):
            if haps[i][j] != newHap[j]:
                matchesThisHap = False
        if matchesThisHap:
            return True
    return False

#this will return the haplotype that explains the most uncovered patients
def getGreedyHaplotype(usingZero, doubleGreedy, random):
    bestCoverage = 0
    bestCoverage2 = 0
    global patientNum
    global numHaps
    global patientNum2
    #go through every patient and get their best hap, then see which covers the most
    bestHap = [0 for x in range(numlines)]
    tempHap = [0 for x in range(numlines)]
    bestHap2 = [0 for x in range(numlines)]
    twoHaps = [[3 for x in range(numlines)] for x in range(2)]
    for i in range(numpatients):
       tempHap = recursiveTest(i, 0, tempHap, usingZero, random)
       num = testNewHaplotype(tempHap)
       exists = not alreadyExists(tempHap)
       new = checkNewHaplotype(numHaps, tempHap)
       if num > bestCoverage and new and exists:
           bestHap = tempHap
           bestCoverage = num
           patientNum = i
       elif doubleGreedy and num > bestCoverage2 and new and exists:
            bestHap2 = tempHap
            bestCoverage2 = num
            patientNum2 = i
    twoHaps[0] = bestHap
    if doubleGreedy:
        twoHaps[1] = bestHap2

    return twoHaps
def doubleGreedy(usingZero):
    global patientNum
    global startTime
    global finishTime
    global patientNum2
    startTime = time.time()
    while not checkCoverage():
        tempCount = 0
        newHaps = [[0 for x in range(numlines)] for x in range(2)]
        newHap = getGreedyHaplotype(usingZero, True, False)
        addNewHaplotype(patientNum,False , newHap[0])
        addNewHaplotype(patientNum2, True, newHap[1])
    finishTime = time.time()

def findMinimumSpanningSet(usingZero, random):
    global patientNum
    global startTime
    global finishTime
    startTime = time.time()
    firstTime = random
    while not checkCoverage():
        tempCount = 0
        newHap = [[0 for x in range(numlines)] for x in range(2)]
        if firstTime:
            newHap = getGreedyHaplotype(usingZero , False, random)
            firstTime = False
        else:
            newHap = getGreedyHaplotype(usingZero, False, False)
        addNewHaplotype(patientNum,True , newHap[0])
    finishTime = time.time()

#here we will fill the 012 matrix by creating a known set of haplotypes and handing them out to every patient
def createData(numpatients, numlines, numHaps):
    #first we need to create a set of haplotypes
    global created
    #print("\nWe are creating a patient set with " + str(numHaps) + " haplotypes")
    moduleDebug = 0
    created = [[ 3 for x in range(numlines)] for x in range(numHaps)]
    for i in range(numHaps):
        for j in range(numlines):
            a = randint(0, 3)
            if a == 0:
                created[i][j] = 1
            else:
                created[i][j] = 0
    if(moduleDebug):
        for i in range(numHaps):
            for j in range(numlines):
                print created[i][j],
            print " "

#now we need to randomly assign each patient with two haps
    hapIndexes = [[0 for x in range(2)] for x in range(numpatients)]
    for i in range(numpatients):
        a = randint(0, numHaps -1)
        hapIndexes[i][0] = a
        b = randint(0, numHaps -1)
        hapIndexes[i][1] = b
        #print("Patient " + str(i) + " has haplotype number: " + str(a))
        #print("Patient " + str(i) + " has haplotype number: " + str(b))

#now we need to use this information to create the 012 matrix
    for i in range(numpatients):
        for j in range(numlines):
            one = created[hapIndexes[i][0]][j]
            two = created[hapIndexes[i][1]][j]
            if one == two:
                if one == 0:
                    genoNums[i][j] = 0
                else:
                    genoNums[i][j] = 1
            else:
                genoNums[i][j] = 2
    if(moduleDebug):
        printGenoNumsMatrix()
def checkCorrectness(numActualHaps):
    trueHaps = 0
    global numHaps
    global numlines
    global haps
    global created
    moduleDebug = 0
#iterate through the actual haps
    for i in range(numActualHaps):
#iterate through the haps we found
        for j in range(numHaps):
            isMatch = True
#iterate through the individual base pairs
            for k in range(numlines):
                if moduleDebug:
                    print("I is: " + str(i))
                    print("J is: " + str(j))
                    print("K is: " + str(k))
                if created[i][k] != haps[j][k]:
                    isMatch = False
                    break
            if isMatch:
                trueHaps += 1
                if moduleDebug:
                    print("We found a matching haplotype")
                break
    correctness = trueHaps
    return correctness

#this function will print various things about our test
def printMetrics(usingZero, random, matched, numActualHaps):
    print("Here is a summary of the test's performance:\n")
    print("The number of patients: " + str(numpatients))
    print("The length of each haplotype: " + str(numlines))
    print("Is one of our haplotypes all zeroes?  " + str(usingZero))
    print("Is our first haplotype?  " + str(random))
    print("Number of unique haplotypes in the minimum spanning set: " + str(numHaps))
    print(str(matched) + " of our haplotypes were in the \"true\" set of " + str(numActualHaps) + " haplotypes.")
    time = finishTime - startTime
    print("Time spent finding the minimum spanning set: " + str(time))

def readFile():
    script, filename = argv
    debug = False
    txt = open(filename)
    for i in range(numlines + 1):
        if i == 0:
            line = txt.readline()
            continue
        else:
            line = txt.readline()
            g = 0
            a = 0
            t = 0
            c = 0
            count = 0
            for j in range(len(line)):
                if count >= numpatients * 2:
                    break
                if line[j] == 'A':
                    a += 1
                if line[j] == 'T':
                    t += 1
                if line[j] == 'G':
                    g += 1
                if line[j] == 'C':
                    c += 1
                if line[j] == 'T' or line[j] == 'C' or line [j] == 'A' or line[j] == 'G':
                    geno[i - 1][count] = line[j]
                    count += 1
            if debug:
                print("Specs for line " + str(i + 1) + ":")
                print("There are " + str(a) + " A's")
                print("There are " + str(t) + " T's")
                print("There are " + str(g) + " G's")
                print("There are " + str(c) + " C's\n")
            major = max(a, t, g, c)
            if major == a:
                matrix[i][0] = 'A'
                #the minor allele will be the only other nonzero value
                if t > 0:
                    matrix[i][1] = 'T'
                if c > 0:
                    matrix[i][1] = 'C'
                if g > 0:
                    matrix[i][1] = 'G' 
            if major == t:
                matrix[i][0] = 'T'
                #the minor allele will be the only other nonzero value
                if a > 0:
                    matrix[i][1] = 'A'
                if c > 0:
                    matrix[i][1] = 'C'
                if g > 0:
                    matrix[i][1] = 'G' 
            if major == c:
                matrix[i][0] = 'C'
                #the minor allele will be the only other nonzero value
                if t > 0:
                    matrix[i][1] = 'T'
                if a > 0:
                    matrix[i][1] = 'A'
                if g > 0:
                    matrix[i][1] = 'G' 
            if major == g:
                matrix[i][0] = 'G'
                #the minor allele will be the only other nonzero value
                if t > 0:
                    matrix[i][1] = 'T'
                if c > 0:
                    matrix[i][1] = 'C'
                if a > 0:
                    matrix[i][1] = 'A' 
            #shift entries up by one
            matrix[i - 1][0] = matrix [i][0]
            matrix[i - 1][1] = matrix[i][1]

#this will call all of our high level functions do a complete test
#low printing prints everything, high level prints only the performance
def runTest(usingZero, lowPrinting, highPrinting, newNumpatients, newNumlines, random, numActualHaps):
    global numpatients
    global numlines
    global correctHaps
    numpatients = newNumpatients
    numlines = newNumlines
        
    initializeMatrices(newNumpatients, newNumlines, random)
    if not random or testsRun == 0:
        createData(numpatients, numlines, numActualHaps)
    #readFile()
    if lowPrinting:
        printMajorMinorMatrix()
    #fill012matrix()
    if lowPrinting:
        printGenoNumsMatrix()
    #true parameter means we are using the random first haplotype
    findMinimumSpanningSet(usingZero, random)
    #doubleGreedy(usingZero)
    correctHaps = checkCorrectness(numActualHaps)
    if lowPrinting:
        printIndividualHaplotypes()
        printHaplotypes()
    if highPrinting:
        printMetrics(usingZero, True, correctHaps, numActualHaps)
    global testsRun
    testsRun += 1
def runRepeatedGreedyTest(usingZero, lowPrinting, highPrinting, minPatients, maxPatients, minNumCreatedHaps, maxNumCreatedHaps, numlines):
    global testsRun
    global minHapsFound
    global correctnessRatio
    global outOfBounds
    global correctHaps
    for i in range(minNumCreatedHaps, maxNumCreatedHaps):
        testsRun = 0
        res = [0 for x in range(numRepeats)]
        correct = [0 for x in range(numRepeats)]
        j = minPatients
        while j < maxPatients:
            runTest(True, lowPrinting, highPrinting, j, numlines, False, i)
            if outOfBounds:
                outOfBounds = False
                continue
#add the results to our data
            minHapsFound[i- minNumCreatedHaps][j - minPatients] = numHaps
            correctnessRatio[i - minNumCreatedHaps][j - minPatients] = correctHaps/float(numHaps)
            #print("There were " + str(correctHaps) + "correct haplotypes")
            testsRun = 0
            j += 1 

def runRepeatedRandomTest(usingZero, lowPrinting, highPrinting, minPatients, maxPatients, minNumCreatedHaps, maxNumCreatedHaps, numlines, numRepeats):
    global testsRun
    global minHapsFound
    global correctnessRatio
    global outOfBounds
    global correctHaps
    for i in range(minNumCreatedHaps, maxNumCreatedHaps):
        testsRun = 0
        res = [0 for x in range(numRepeats)]
        correct = [0 for x in range(numRepeats)]
        j = minPatients
        while j < maxPatients:
            #print j
            for k in range(numRepeats):
                runTest(True, lowPrinting, highPrinting, j, numlines, True, i)
                if outOfBounds:
#need to retest
                    break
                res[k] = numHaps
                correct[k] = correctHaps
            if not outOfBounds:
                min = 10000
                c = 0
                for k in range(numRepeats):
                    if res[k] < min:
                        min = res[k]
                        c = correct[k]
                #print("The best we were able to do was: " + str(min))
                #print("This test had " + str(c) + " out of " + str(min) + " we found actually in the set of " + str(minNumCreatedHaps) + " created haplotypes")
#add the results to our data
            if outOfBounds:
                outOfBounds = False
                #print j
                testsRun = 0
                continue
            minHapsFound[i- minNumCreatedHaps][j - minPatients] = min
            correctnessRatio[i - minNumCreatedHaps][j - minPatients] = c/float(min)
            testsRun = 0
            j += 1

numCreatedHaps = 15
correctHaps = 0
created = [[3 for x in range(numlines)] for x in range(numHaps)]
testsRun = 0
#here we'll create a 2 2d matrices to hold the values we get
#one will hold the minimum number of haplotypes we find, the other will hold how many of those haplotypes are correct
minPatients = 10
maxPatients = 15
minNumCreatedHaps = 10
maxNumCreatedHaps = 15
numRepeats = 40
minHapsFound = [[0 for x in range(maxPatients - minPatients)] for x in range(maxNumCreatedHaps - minNumCreatedHaps)]
correctnessRatio = [[0.0 for x in range(maxPatients - minPatients)] for x in range(maxNumCreatedHaps - minNumCreatedHaps)]

#the following 4 functions output the results to files
def printMinHapsFound():
    global minHapsFound
    global maxPatients
    global minPatients
    global minNumCreatedHaps
    global maxNumCreatedHaps
    file = open("minimumG.txt", "w")
    file.write("Here are our minimum haplotypes found ratios:\n")
    for j in range(maxNumCreatedHaps - minNumCreatedHaps):
        for i in range(maxPatients - minPatients):
            a = minHapsFound[i][j] / float(minNumCreatedHaps + j)
            file.write("%.2f" % a),
            if i != maxPatients - minPatients:
                file.write(",")
        file.write("\n")
 
def printCorrectnessRatio():
    global correctnessRatio
    global maxPatients
    global minPatients
    global minNumCreatedHaps
    global maxNumCreatedHaps
    file = open("correctnessG.txt", "w")
    file.write("Here are our correctness ratios:\n")
    for j in range(maxNumCreatedHaps - minNumCreatedHaps):
        for i in range(maxPatients - minPatients):
            file.write("%.2f" % correctnessRatio[j][i]),
            file.write(",")
        file.write("\n")


def printMinHapsFoundR():
    global minHapsFound
    global maxPatients
    global minPatients
    global minNumCreatedHaps
    global maxNumCreatedHaps
    file = open("minimum.txt", "w")
    file.write("Here are our minimum haplotypes found ratios:\n")
    for j in range(maxNumCreatedHaps - minNumCreatedHaps):
        for i in range(maxPatients - minPatients):
            a = minHapsFound[i][j] / float(minNumCreatedHaps + j)
            file.write("%.2f" % a),
            if i != maxPatients - minPatients:
                file.write(",")
        file.write("\n")
 
def printCorrectnessRatioR():
    global correctnessRatio
    global maxPatients
    global minPatients
    global minNumCreatedHaps
    global maxNumCreatedHaps
    file = open("correctness.txt", "w")
    file.write("Here are our correctness ratios:\n")
    for j in range(maxNumCreatedHaps - minNumCreatedHaps):
        for i in range(maxPatients - minPatients):
            file.write("%.2f" % correctnessRatio[j][i]),
            file.write(",")
        file.write("\n")

def printExplanation():
    print("Coded in May 2015 for CS 124 at UCLA")
    print("---------Haplotype Phasing----------")
    print("------------Tyler Galdes------------")
    print("\n\tThis was the final project I wrote for Computational Genetics.\n The goal of the project was to create a program that could\n reverse engineer the best possible set of haplotypes \n from a know set of genotypes. This is a problem faced by\n researchers in genetic related fields.\n")
    print("We were tasked with creating an algorithm that could\n outperform the \"greedy\" solution to the problem.\n My algorithm uses an element of randomness and \n repetition to get better results than the greedy algorithm.")
    print("\nPlease run the command:")
    print("python readData.py")
    print("When the command line prompt appears. The\n progam is creating a set of haplotypes \n with both the greedy and random algorithm.")
printExplanation()
#run the greedy test and output to the files
runRepeatedGreedyTest(True, False, False, minPatients, maxPatients, minNumCreatedHaps, maxNumCreatedHaps, 10)
printCorrectnessRatio()
printMinHapsFound()


runRepeatedRandomTest(True, False, False, minPatients, maxPatients, minNumCreatedHaps, maxNumCreatedHaps, 10, 40)
printCorrectnessRatioR()
printMinHapsFoundR()
