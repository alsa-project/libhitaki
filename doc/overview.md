Title: Overview

The library produces some objects as an abstraction to operate system resources in Linux Sound
Subsystem. This library supports some model-specific operations for Audio and Music units in
IEEE 1394 bus supported by drivers in ALSA firewire stack.

The library expects userspace applications to use [struct@GLib.MainContext] to run event
dispatcher with [struct@GLib.Source] retrieved from the library.

The library supports gobject introspection, thus this library is available with GObject
Introspection bindings of each language such as Python, Ruby and so on. When using this mechanism,
please refer to documents in each language implementation.
