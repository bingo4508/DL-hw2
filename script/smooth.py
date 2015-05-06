import sys

def most_common(lst):
    return max(set(lst), key=lst.count)


if len(sys.argv) < 4:
	print "args: $input $output $win_size"
	sys.exit(1)

input = sys.argv[1]
output = sys.argv[2]
win_size = int(sys.argv[3])


with open(output, 'w') as fo:
    with open(input, 'r') as f:
        for l in f:
            l=l.strip().split()
	    smooth_l = []
	    l_len = len(l[2:])
	    for i, e in enumerate(l[2:]):
		left = 0 if i<win_size else i-win_size
		right = l_len-1 if i+win_size>l_len-1 else i+win_size
		tl = [l[2:][ee] for ee in range(left, right+1)]
		smooth_l.append(most_common(tl))
	    fo.write("%s %s %s\n" % (l[0], l[1], ' '.join(smooth_l)))
	 
