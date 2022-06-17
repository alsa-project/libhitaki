tests = [
  'hitaki-enum',
  'snd-unit',
  'snd-digi00x',
  'snd-dice',
  'snd-efw',
  'snd-motu',
  'snd-tascam',
  'snd-motu-register-dsp-parameter',
  'alsa-firewire',
  'quadlet-notification',
  'efw-protocol',
  'motu-register-dsp',
]

envs = environment()
envs.append('LD_LIBRARY_PATH', builddir, separator : ':')
envs.append('GI_TYPELIB_PATH', builddir, separator : ':')

foreach test : tests
    name = test
    script = test
    prog = find_program(script)
    test(name, prog,
      env: envs,
    )
endforeach
