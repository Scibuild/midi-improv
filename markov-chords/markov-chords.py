from time import sleep
import json
import random
import sys

sleep(1)

# weights = {
#   "cm": {"cm": 0.3, "f": 0.2, "fm": 0.2, "gm": 0.2, "ab": 0.1},
#   "fm": {"cm": 0.1, "fm": 0.1, "gm": 0.4, "ab": 0.2, "d": 0.2},
#   "f": {"cm": 0.1, "fm": 0.1, "gm": 0.4, "ab": 0.2, "d": 0.2},
#   "gm": {"cm": 0.3, "g": 0.3, "gm": 0.1, "fm": 0.2, "ab": 0.1},
#   "g": {"cm": 1},
#   "ab": {"gm": 0.5, "g": 0.4, "fm": 0.1},
#   "d": {"fm": 0.2, "gm": 0.5, "g": 0.3}
# }

weights = json.load(open(sys.argv[1]))

def choose_weighted(dic: dict[str, float])-> str:
    r = random.random()
    cum = 0
    for k, v in dic.items():
        cum += v
        if r < cum:
            return k

num_resolutions = 5
if '-m' in sys.argv:
    final_chord = 'cm'
else:
    final_chord = 'c'
curr_chord = final_chord

print("wide")
print("down")
print("tempo 60")
while True:
    print(curr_chord)
    print(curr_chord, file=sys.stderr)
    print("bsleep 4")
    last_chord = curr_chord
    curr_chord = choose_weighted(weights[curr_chord])
    if num_resolutions == 0:
        break
    if curr_chord == final_chord and last_chord != final_chord:
        num_resolutions -= 1

print("bsleep 4")
print("quit")
