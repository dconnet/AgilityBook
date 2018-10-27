3rd Party code. To use in my projects, include .../3rdParty in include path.

catch/catch2: v2.4.2
- https://github.com/philsquared/Catch/releases
-- #include "catch2/catch.hpp"

LibArchive: zlib v1.2.11: My wrapper to hide zlib
- http://www.zlib.net/
- The files in the LibArchive/zlib folder have been reduced to the minimum set.
  - *.c *.h
- In addition, the files from <zlib>contrib/minizip/ have been copied in here.
  - ioapi.* unzip.* zip.*
- Source files are unmodified.
- zlib is only compiled in the non wxWidget case.
-- #include "LibArchive/LibArchive.h"

LibTidyHtml: tidy v5.6.0: My wrapper to hide tidy
- https://github.com/htacg/tidy-html5/releases
- Only 'README', 'include', and 'src' directories are included.
-- #include "LibTidyHtml/LibTidyHtml.h"
