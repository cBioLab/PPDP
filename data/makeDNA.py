import random

source = 'ACGT'
filem = '.dat'

for l in xrange(1,11):
	DNA1 = ""
	for i in xrange(10*l):
		DNA1 = DNA1 + random.choice(source)
	DNA2 = ""
	for i in xrange(10*l):
		DNA2 = DNA2 + random.choice(source)
	print DNA1
	print DNA2
	filename = 'sq' + str(l*10) + filem
	f = open(filename,'w')
	f.write(DNA1)
	f.write('\n')
	f.write(DNA2)
	f.close()

