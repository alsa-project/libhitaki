project('hitaki', 'c',
  version: '0.2.1',
  license: 'LGPL-2.1-or-later',
  meson_version: '>= 0.46.0',
)

# Detect support level in Linux sound subsystem.
cc = meson.get_compiler('c')

backport = []
if not cc.has_header_symbol('include/sound/firewire.h',
                            'SNDRV_FIREWIRE_EVENT_FF400_MESSAGE')
  # Use backport header from Linux kernel v6.3 prepatch.
  backport += include_directories('include')
endif

gnome = import('gnome')

subdir('src')
subdir('tests')

if get_option('doc')
  subdir('doc')
endif
