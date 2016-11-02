import random

source = '01'
filem = '.dat'


str1 = ""
for i in xrange(0,10000):
        str1 = str1 + random.choice(source)
str2 = ""
for i in xrange(0,2000):
        str2 = str2 + random.choice(source)
filename1 = 'server_d/bits_s' + filem
filename2 = 'client_d/bits_c' + filem
f1 = open(filename1,'w')
f1.write(str1)
f1.close
f2 = open(filename2,'w')
f2.write(str2)
f2.close()

