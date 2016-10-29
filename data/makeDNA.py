import random

source = 'ACGT'
filem = '.dat'

for l in xrange(1,11):
	DNA1 = ""
#	for i in xrange(random.randint(5,20)):
	for i in xrange(l*10):
		DNA1 = DNA1 + random.choice(source)
	DNA2 = ""
#	for i in xrange(random.randint(5,20)):
	for i in xrange(l*10):
		DNA2 = DNA2 + random.choice(source)
#	print DNA1
#	print DNA2
	filename1 = 'server/' + str(l*10) + filem
	filename2 = 'client/' + str(l*10) + filem
	f1 = open(filename1,'w')
	f1.write(DNA1)
	f1.close()
#	f.write('\n')
	f2 = open(filename2,'w')
	f2.write(DNA2)
	f2.close()

