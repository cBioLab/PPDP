import random

source = 'qwertyuioplkjhgfdsazxcvbnm'
filem = '.dat'

for l in xrange(1,11):
	str1 = ""
	for i in xrange(0,l*10):
		str1 = str1 + random.choice(source)
	str2 = ""
	for i in xrange(0,l*10):
		str2 = str2 + random.choice(source)
	filename1 = 'server_26/' + str(l*10) + filem
	filename2 = 'client_26/' + str(l*10) + filem
	f1 = open(filename1,'w')
	f1.write(str1)
	f1.close
	f2 = open(filename2,'w')
	f2.write(str2)
	f2.close()

