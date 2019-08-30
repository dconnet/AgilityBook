3rd Party code.

Catch: v2.9.2
.../catch2/
- https://github.com/philsquared/Catch/releases
-- #include "catch2/catch.hpp"

LibArchive: zlib v1.2.11: My wrapper to hide zlib
.../LibArchive/
- http://www.zlib.net/
- The files in the LibArchive/zlib folder have been reduced to the minimum set.
  - *.c *.h
- In addition, the files from <zlib>contrib/minizip/ have been copied in here.
  - ioapi.* unzip.* zip.*
- Source files are unmodified.
- zlib is only compiled in the non wxWidget case.
-- #include "LibArchive/LibArchive.h"

LibFmt: fmtlib v5.3.0
.../LibFmt/
DevNote: When updating, update .../res/LibFmt.rc2 with version number and copyright
- https://github.com/fmtlib/fmt/releases
- http://fmtlib.net/dev/index.html
- Only README.rst, LICENSE.rst, include, src

LibTidyHtml: tidy v5.6.0: My wrapper to hide tidy
.../LibTidyHtml/
DevNote: When updating, update .../res/LibTidyHtml.rc2 with version number and copyright
- https://github.com/htacg/tidy-html5/releases
- Only 'README', 'include', and 'src' directories are included.
-- #include "LibTidyHtml/LibTidyHtml.h"
