=====================
The libhitaki project
=====================

2024/02/16
Takashi Sakamoto

Introduction
============

This is a child project of libhinawa.

- https://github.com/alsa-project/libhinawa

Originally the function produced by ALSA firewire stack was operated via libhinawa API. Nowadays
libhitaki does the work instead.

I design the library for userspace applications to operate ALSA HwDep character device for
specific functions implemented in ALSA drivers for Audio and Music unit in IEEE 1394 bus.

The latest release is `0.2.1 <https://github.com/alsa-project/libhitaki/tags/v0.2.1>`_.

License
=======

Released under GNU Lesser General Public License version 2.1 or later, including UAPI header of ALSA
firewire stack released under GNU General Public License version 2.0.

Repository
==========

- `<https://github.com/alsa-project/libhitaki>`_

Documentation
=============

- `<https://alsa-project.github.io/gobject-introspection-docs/hitaki/>`_

Dependencies
============

* GLib `<https://gitlab.gnome.org/GNOME/glib>`_
* GObject introspection `<https://gi.readthedocs.io/>`_
* Linux kernel version 4.5 or later

Requirements to build
=====================

* UAPI header of Linux kernel
* Meson build system `<https://mesonbuild.com/>`_
* Ninja build system `<https://ninja-build.org/>`_
* PyGObject `<https://pygobject.readthedocs.io/>`_ (optional to run unit tests)
* gi-docgen `<https://gnome.pages.gitlab.gnome.org/gi-docgen/>`_ (optional to generate API documentation)

How to build and install
========================

Build and install ::

    $ meson setup (--prefix=directory-to-install) build-directory
    $ meson compile -C build-directory
    $ meson install -C build-directory

After installed, C headers for APIs are available and pkg-config returns arguments for them.
For example ::

    $ pkg-config --cflags --libs hitaki
    -I/usr/include/hitaki -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lhitaki

How to refer document
=====================

::

    $ meson setup --prefix=directory-to-install -D doc=true build-directory
    $ meson install -C build-directory
    $ xdg-open directory-to-install/share/doc/hitaki/index.html

Supplemental information for language bindings
==============================================

* PyGObject `<https://pygobject.readthedocs.io/>`_ is a dynamic loader for libraries compatible
  with g-i.
* hitaki-rs `<https://github.com/alsa-project/hitaki-rs/>`_ includes creates to use these
  libraries.

About Hitaki
============

``Hitaki`` is a family of bird in Japanese. The representative species of family is
``Joubitaki`` (``Phoenicurus auroreus`` in binary nomenclature), known as ``Daurian redstart``.

The small bird sings in several kind of voices, one of which is like the sound to strike flints
against steel to produce sparks in human perception. The name `Hitaki` might come from the voice
according to legend since the name was written by two successive Kanji characters in days past;
``Hi`` (U+2F55 |kanji-hi|) and ``Taki`` (U+713C |kanji-taki|). The former means
``fire``. The latter often means ``bake`` or ``burn`` something, however it means ``ignite`` in
the context. The name is written by one Kanji character (U+9DB2 |kanji-hitaki|) later.

.. |kanji-hi| unicode:: &#x2f55 .. Hi spelled in Kanji
.. |kanji-taki| unicode:: &#x713c .. Taki spelled in Kanji
.. |kanji-hitaki| unicode:: &#x9d82 .. Hitaki spelled in Kanji

We can see ``Hitaki`` as a bird to sing in a voice like igniting fire. The bird definitely operates
our devices in IEEE 1394 bus to process audio.

End
