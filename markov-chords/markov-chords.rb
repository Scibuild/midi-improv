require 'time'

sleep 0.1
chords = ['c', 'f', 'g', 'g7', 'c']

weights = {
  "c": {"c": 0.3, "f": 0.4, "g": 0.2, "am": 0.1},
  "f": {"c": 0.1, "f": 0.1, "g": 0.4, "am": 0.4},
  "g": {"c": 0.5, "g": 0.1, "g7": 0.3, "am": 0.1},
  "am": {"g": 0.6, "f": 0.4},
  "g7": {"g7": 0.1, "c": 0.9}
}


def choose_weighted map
  r = rand()
  cum = 0
  map.each_pair {|k, v|
    cum += v
    if r <= cum
      return k
    end
  }
end

# chords.each {|x| 
#   puts x
#   puts 'bsleep 4'
# }

puts weights[:c]
puts weights["c"]

curr_chord = "c"
10.times {|i|
  puts curr_chord
  puts 'bsleep 4'
  curr_chord = choose_weighted(weights[curr_chord])
}
puts 'quit'
