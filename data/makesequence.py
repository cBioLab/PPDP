import random

source = 'qwertyuioplkjhgfdsazxcvbnm'
filem = '.dat'

for l in xrange(1,101):
	str1 = ""
	for i in xrange(random.randint(5,20)):
		str1 = str1 + random.choice(source)
	str2 = ""
	for i in xrange(random.randint(5,20)):
		str2 = str2 + random.choice(source)
	filename = 'test3/' + str(l) + filem
	f = open(filename,'w')
	f.write(str1)
	f.write('\n')
	f.write(str2)
	f.close()

