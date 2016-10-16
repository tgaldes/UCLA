from termcolor import colored
import time

file  = open("minimum.txt")
line = file.readline()

print("\n\nThe goal of haplotype phasing is to find a set of haploytpes that satisfies every patients genotype\n with the fewest distinct haplotypes. We also want to find the haplotypes that patients actually have.\n To test the program we can create a set of haplotypes, use them to create genotypes,\n and run the program on that data. This way we can figure out if we are \nfinding haplotypes that actually appear in our patients.\n\n This gives us two metrics to test the effectiveness of our program:\n 1\) size of the set compared to the original set \n 2\) % of haplotypes in the found set that are also in the original set\n")
time.sleep(20)
print("We print our data in color to depict it better.")
print colored("Green is the best", 'green')
print colored("Blue is almost as good", 'blue')
print("Black is in the middle")
print colored("Red isn't great", 'red')
print colored("And cyan is the worst", 'cyan')
time.sleep(5)
print("\nHere is how we do finding the minimum size set of haplotypes with the RANDOM method:"),
while line != '':
    num = ''
    for i in range(len(line)):
        if line[i] == ',': #delimiter
#do things here
            a = float(num)
            if a > .8 and a < 1.2:
                print("%.2f" % a),
            elif a >= 1.2 and a < 1.4:
                print colored("%.2f" % a, 'red'),
            elif a <= .8 and a > .5:
                print colored("%.2f" % a, 'blue'),
            elif a <= .5:
                print colored("%.2f" % a, 'green'),
            else:
                print colored("%.2f" % a, 'cyan'),
            num = ''
        else:
            num += line[i]
    line = file.readline()
    print(" ")


file  = open("minimumG.txt")
line = file.readline()
print("\nHere is how we do finding the minimum size set of haplotypes with the GREEDY method:"),
while line != '':
    num = ''
    for i in range(len(line)):
        if line[i] == ',': #delimiter
#do things here
            a = float(num)
            if a > .8 and a < 1.2:
                print("%.2f" % a),
            elif a >= 1.2 and a < 1.4:
                print colored("%.2f" % a, 'red'),
            elif a <= .8 and a > .5:
                print colored("%.2f" % a, 'blue'),
            elif a <= .5:
                print colored("%.2f" % a, 'green'),
            else:
                print colored("%.2f" % a, 'cyan'),
            num = ''
        else:
            num += line[i]
    line = file.readline()
    print(" ")

print("\nHere the random method has outperformed the greedy by a slight but noticeable margin\n")
time.sleep(10)
file  = open("correctness.txt")
line = file.readline()


print("\nHere is how we do finding the correct haplotypes with the RANDOM method:"),
while line != '':
    num = ''
    for i in range(len(line)):
        if line[i] == ',': #delimiter
#do things here
            a = float(num)
            if a > .4 and a < .6:
                print("%.2f" % a),
            elif a >= .6 and a < .8:
                print colored("%.2f" % a, 'blue'),
            elif a <= .4 and a > .2:
                print colored("%.2f" % a, 'red'),
            elif a <= .2:
                print colored("%.2f" % a, 'cyan'),
            else:
                print colored("%.2f" % a, 'green'),
            num = ''
        else:
            num += line[i]
    line = file.readline()
    print(" ")
file  = open("correctnessG.txt")
line = file.readline()


print("\nHere is how we do finding the correct haplotypes with the GREEDY method:"),
while line != '':
    num = ''
    for i in range(len(line)):
        if line[i] == ',': #delimiter
#do things here
            a = float(num)
            if a > .4 and a < .6:
                print("%.2f" % a),
            elif a >= .6 and a < .8:
                print colored("%.2f" % a, 'blue'),
            elif a <= .4 and a > .2:
                print colored("%.2f" % a, 'red'),
            elif a <= .2:
                print colored("%.2f" % a, 'cyan'),
            else:
                print colored("%.2f" % a, 'green'),
            num = ''
        else:
            num += line[i]
    line = file.readline()
    print(" ")

print("\nHere the random method has outperformed the greedy by a wide margin.")
