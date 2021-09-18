project('midi-improv', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

dep_alsa = dependency('alsa')


executable('midi-improv',
           'main.c',
					 dependencies: [dep_alsa],
           install : false)
