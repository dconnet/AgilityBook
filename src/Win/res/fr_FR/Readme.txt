Note: The comments in fr.po have been changed so that all .po files have the
same header. msgcat then keeps a single header which makes poedit much happier.

The fr.po file originated in <wx>/locale/fr.po (2.8.12), copied here, edited
in poedit to change charset to utf8, then the file header was modified so
all files have the same strings (including revision date!)

NOTE: Fixed 3 bugs identified while compiling:
- .\fr_FR\autogen.po:2578: `msgid' and `msgstr' entries do not both end with '\n'
-- Changed to english version - the translation is from the previous msgid

#: ../src/common/debugrpt.cpp:565
#, fuzzy
msgid "A debug report has been generated. It can be found in"
msgstr "Un rapport de débogage a été créé dans le dossier\n"

- .\fr_FR\autogen.po:4297: number of format specifications in 'msgid' and 'msgstr' does not match
-- Added %s

#: ../src/common/textfile.cpp:177
#, fuzzy, c-format
msgid "Failed to convert file \"%s\" to Unicode."
msgstr "La conversion du contenu du fichier en Unicode a échoué."

- .\fr_FR\autogen.po:5764: number of format specifications in 'msgid' and 'msgstr' does not match
-- Added %s

#: ../src/dfb/fontmgr.cpp:156
#, fuzzy, c-format
msgid "No fonts found in %s."
msgstr "Aucune entrée trouvée."

- .\fr_FR\autogen.po:7458: number of format specifications in 'msgid' and 'msgstr' does not match
-- Added %s

#: ../src/mac/carbon/aboutdlg.cpp:73
#, fuzzy, c-format
msgid "Version %s"
msgstr "Version "

Original header:
"Project-Id-Version: wxWidgets-2.8.0\n"
"Report-Msgid-Bugs-To: \n"
"POT-Creation-Date: 2008-04-24 09:56+0200\n"
"PO-Revision-Date: 2006-11-15 23:55+0100\n"
"Last-Translator: Stephane Junique <sj.nospam@one.se>\n"
"Language-Team: French <debian-l10n-french@lists.debian.org>\n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=iso-8859-15\n"
"Content-Transfer-Encoding: 8bit\n"
"plural-forms: nplurals=2; plural=n > 1\n"
