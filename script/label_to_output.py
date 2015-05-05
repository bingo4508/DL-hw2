import sys

try:
    label = sys.argv[1]
    out_data = sys.argv[2]
    mapping = sys.argv[3]
except:
    print("args: $label_file $output_file $map_file\n")
    print("Transform -")
    print("\tXXX_XXX_01,sil")
    print("\tXXX_XXX_02,sil")
    print("\t.....")
    print("To -")
    print("\tXXX_XXX 37 37 ...")
    sys.exit()

label_map = {}
label_map2 = {}

with open(label, 'r') as f:
    for l in f:
        l = l.strip().split(',')
	x = '_'.join(l[0].split('_')[:-1])
	if x in label_map:
            label_map[x].append(l[1])
	else:
	    label_map[x] = [l[1]]

with open(mapping, 'r') as f:
    for l in f:
        l = l.strip().split()
        label_map2[l[0]] = l[1]

# Output data
with open(out_data, 'w') as fo:
    for k,v in label_map.items():
	fo.write("%s %d %s\n" % (k, len(v),' '.join([label_map2[e] for e in v])))

