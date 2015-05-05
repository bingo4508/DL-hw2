import sys

try:
    predict = sys.argv[1]
    answer = sys.argv[2]
except:
    print("args: $predict $answer\n")
    print("Format -")
    print("\tXXX_XXX 37 37 37 37 ...")
    print("\tXXX_XXX 36 36 36 ...")
    print("\t.....")
    sys.exit()


predict_map = {}
answer_map = {}

with open(predict, 'r') as f:
    for l in f:
	l = l.strip().split()
        predict_map[l[0]] = l[1:]

with open(answer, 'r') as f:
    for l in f:
	l = l.strip().split()
        answer_map[l[0]] = l[1:]

accuracy = 0
for k, v in predict_map.items():
    if(len(v) != len(answer_map[k])):
        print("ERROR %s: %d/%d" % (k, len(v), len(answer_map[k])))
    a = float(len(set(v) & set(answer_map[k])))/len(v)
    print("%s: %f" % (k, a))
    accuracy += a

print("\nAverage accuracy: %f" % (accuracy/len(predict_map)))
