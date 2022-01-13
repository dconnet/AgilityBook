3rd Party code.

Catch: v2.13.8
.../catch/catch2/
- https://github.com/philsquared/Catch/releases
-- #include "catch2/catch.hpp"

LibFmt: fmtlib v8.1.1
.../LibFmt/
DevNote: When updating, update .../res/LibFmt.rc2 with version number and copyright
- https://github.com/fmtlib/fmt/releases
- http://fmtlib.net/dev/index.html
- Only README.rst, LICENSE.rst, include, src
(note: diffs are agains 8.1.0)
-- Changes (diff -c e:original.h mine.h)
*** e:core.h	Mon Jan 03 06:30:43 2022
--- core.h	Mon Jan 03 09:35:08 2022
***************
*** 2769,2775 ****
    case presentation_type::hexfloat_lower:
      result.format = float_format::hex;
      break;
!   default:
      eh.on_error("invalid type specifier");
      break;
    }
--- 2769,2783 ----
    case presentation_type::hexfloat_lower:
      result.format = float_format::hex;
      break;
!   case presentation_type::dec:
!   case presentation_type::oct:
!   case presentation_type::hex_lower:
!   case presentation_type::hex_upper:
!   case presentation_type::bin_lower:
!   case presentation_type::bin_upper:
!   case presentation_type::chr:
!   case presentation_type::string:
!   case presentation_type::pointer:
      eh.on_error("invalid type specifier");
      break;
    }
*** e:format.h	Thu Dec 30 06:58:47 2021
--- format.h	Mon Jan 03 09:34:42 2022
***************
*** 1621,1627 ****
    }
    case presentation_type::chr:
      return write_char(out, static_cast<Char>(abs_value), specs);
!   default:
      throw_format_error("invalid type specifier");
    }
    return out;
--- 1621,1636 ----
    }
    case presentation_type::chr:
      return write_char(out, static_cast<Char>(abs_value), specs);
!   case presentation_type::hexfloat_lower:
!   case presentation_type::hexfloat_upper:
!   case presentation_type::exp_lower:
!   case presentation_type::exp_upper:
!   case presentation_type::fixed_lower:
!   case presentation_type::fixed_upper:
!   case presentation_type::general_lower:
!   case presentation_type::general_upper:
!   case presentation_type::string:
!   case presentation_type::pointer:
      throw_format_error("invalid type specifier");
    }
    return out;

LibTidyHtml: tidy v5.8.0: My wrapper to hide tidy
.../LibTidyHtml/
DevNote: When updating, update .../res/LibTidyHtml.rc2 with version number and copyright
- https://github.com/htacg/tidy-html5/releases
- Only 'README', 'include', and 'src' directories are included.
-- #include "LibTidyHtml/LibTidyHtml.h"
