import sys

if len(sys.argv) < 4:
    print "args: $data1.ark $data2.ark $merge.ark"
    sys.exit(1)

d1 = sys.argv[1]
d2 = sys.argv[2]
output = sys.argv[3]

with open(output, 'w') as fo:
    with open(d1, 'r') as f1:
        with open(d2, 'r') as f2:
	    while True:
	        l1 = f1.readline()
		l2 = f2.readline()
		if not l1:
		    break
		l1 = l1.strip().split()
		l2 = l2.strip().split()
		fo.write("%s %s\n" % (l1[0], ' '.join(l1[1:]+l2[1:])))
