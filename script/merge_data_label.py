import sys

try:
    data = sys.argv[1]
    label = sys.argv[2]
    out_data = sys.argv[3]
    mapping = sys.argv[4]
except:
    print("args: $data_file $label_file $output_file $map_file")
    sys.exit()

label_map = {}
label_map2 = {}

with open(label, 'r') as f:
    for l in f:
        l = l.strip().split(',')
        label_map[l[0]] = l[1]

with open(mapping, 'r') as f:
    for l in f:
        l = l.strip().split()
        label_map2[l[0]] = l[1]

print("Be patient...")
# Output merged data
with open(data, 'r') as f:
    with open(out_data, 'w') as fo:
        for l in f:
            l = l.strip().split(' ')
            l.append(label_map2[label_map[l[0]]])
            fo.write("%s\n" % ' '.join(l))

print("Yo Done!!")
