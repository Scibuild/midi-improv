import json
import sys
counts = {} 
totals = {}

with open(sys.argv[1]) as f:
    last_chord = ''
    for i in f.readlines():
        trimed = i.strip()
        if trimed != "":
            if last_chord != '':
                if not last_chord in counts:
                    totals[last_chord] = 1
                    counts[last_chord] = {}
                    counts[last_chord][trimed] = 1
                else:
                    totals[last_chord] += 1
                    counts[last_chord][trimed] = counts[last_chord].get(trimed, 0) + 1
    
            last_chord = trimed


weights = counts.copy()
for k, v in weights.items():
    for k2, v2 in v.items():
        weights[k][k2] /= totals[k]

print(json.dumps(weights))
