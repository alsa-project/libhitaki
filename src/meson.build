# Depends on glib-2.0 and gobject-2.0
gobject = dependency('gobject-2.0',
  version: '>=2.44.0'
)
dependencies = [
  gobject,
]

sources = [
  'alsa_firewire.c',
  'quadlet_notification.c',
  'snd_unit.c',
]

headers = [
  'hitaki.h',
  'hitaki_enum_types.h',
  'alsa_firewire.h',
  'quadlet_notification.h',
  'snd_unit.h',
]

privates = [
  'alsa_firewire_private.h',
  'alsa_firewire_private.c',
]

inc_dir = meson.project_name()

# Generate marshallers for GObject signals.
marshallers = gnome.genmarshal('hitaki_sigs_marshal',
  prefix: 'hitaki_sigs_marshal',
  sources: 'hitaki_sigs_marshal.list',
  install_header: true,
  install_dir: join_paths(get_option('includedir'), inc_dir),
  stdinc: true,
)

enums = gnome.mkenums_simple('hitaki_enums',
  sources: ['hitaki_enum_types.h'],
  identifier_prefix: 'Hitaki',
  symbol_prefix: 'hitaki',
  install_header: true,
  install_dir: join_paths(get_option('includedir'), inc_dir),
  header_prefix: '#include <hitaki.h>',
)

mapfile = 'hitaki.map'
vflag = '-Wl,--version-script,' + join_paths(meson.current_source_dir(), mapfile)

libhitaki = library('hitaki',
  sources: sources + headers + privates + marshallers + enums,
  version: meson.project_version(),
  soversion: meson.project_version().split('.')[0],
  install: true,
  include_directories: backport,
  dependencies: dependencies,
  link_args : vflag,
  link_depends : mapfile,
)

install_headers(headers,
  subdir: inc_dir,
)

pkg = import('pkgconfig')
pkg.generate(libhitaki,
  description: 'Input/Output library to audio and music units on IEEE 1394 bus',
  subdirs: inc_dir,
)

hitaki_gir = gnome.generate_gir(libhitaki,
  sources: enums + headers + sources,
  nsversion: '0.0',
  namespace: 'Hitaki',
  symbol_prefix: 'hitaki_',
  identifier_prefix: 'Hitaki',
  export_packages: 'hitaki',
  dependencies: dependencies,
  includes: [
    'GLib-2.0',
    'GObject-2.0',
  ],
  header: 'hitaki.h',
  install: true,
)

# For test.
builddir = meson.current_build_dir()
