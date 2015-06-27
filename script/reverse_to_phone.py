import sys

# Transform-
# id,phone_sequence
# sample_id,sfsifjijijisdf
# To-
# sample_id,sil sd d 

try:
	map_f = sys.argv[1]
	input = sys.argv[2]
	output = sys.argv[3]
except:
	print "param: 48_idx.map output.kaggle output.phone"
	sys.exit(1)


m = {}
with open(map_f, 'r') as f:
	for l in f:
		l=l.strip().split()
		m[l[2]] = l[0]


with open(output, 'w') as fo:
	with open(input, 'r') as f:
		f.readline()
		for l in f:
			l=l.strip().split(',')
			x = [m[e] for e in l[1]]
			fo.write("%s,%s\n" % (l[0], ' '.join(x)))

