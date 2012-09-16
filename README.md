# Window Merge

This file contains instructions related to GitHub written in markdown format.
For basic information about the project in plain text, see the _README_ file.
Also check the [project wiki page](http://github.com/dm0-/window_merge/wiki)
for step-by-step instructions regarding more complex build scenarios.

## About the Project

Window Merge is a [Pidgin](http://pidgin.im/) plugin that merges a conversation
window with the Buddy List window.  It can be used to achieve a rudimentary
"single window mode" for Pidgin.  Both Window Merge and Pidgin itself should be
kept updated for the most stable experience.

This project was not written and is not maintained by Pidgin developers.  The
nature of this project can potentially destabilize Pidgin or its other plugins,
so use it at your own risk.

## Preparing the Source

The source files available at the GitHub repository are intended for project
maintainers, not necessarily distributors or end users.  The [project downloads
page](http://github.com/dm0-/window_merge/downloads) offers some distribution
tarballs suitable for compiling and prebuilt binaries for Microsoft Windows.

The GitHub repository contains all the configuration files necessary to compile
or create distribution archives with the latest commits.  This will require the
following tools to be installed and available on your system:

* [git](http://git-scm.com/)
* [autoconf](http://www.gnu.org/software/autoconf/)
* [automake](http://www.gnu.org/software/automake/)
* [libtool](http://www.gnu.org/software/libtool/)
* [gettext](http://www.gnu.org/software/gettext/)

On a command line of any given operating system, running the following should
create a copy of the project's git repository and change to its directory.

    git clone https://github.com/dm0-/window_merge.git
    cd window_merge

Now the build system scripts will be generated, and boilerplate project files
will be copied into the source tree.

    mkdir m4
    autoreconf -fi

At this point, the source in the current directory is ready to be compiled.  To
create and verify a distribution archive, run the following commands.

    ./configure
    make distcheck

## System Packaging

The GitHub repository provides two system-specific packaging scripts that are
not included in the distribution archives.  (Remember to check the [project
wiki](http://github.com/dm0-/window_merge/wiki) for step-by-step instructions
on how to use them.)

### RPM (GNU/Linux)

The file _pidgin-window_merge.spec_ can be used with a source archive, as would
have been created with _make distcheck_, to produce an RPM package.  The spec
may use conventions that require a relatively recent version of RPM, but it
should work without much hassle on any GNU/Linux distribution conforming to
sensible packaging standards.

### NSIS (Microsoft Windows)

The build system should be able to determine when it was configured to use the
MinGW compiler, and it should write a Windows DLL file as a result.  When built
in this manner (i.e. after _make_ successfully ends), the included script
_pidgin-window_merge.nsi_ can be run to create an executable installer.  The
required _makensis_ command is a part of [NSIS](http://nsis.sourceforge.net/).
