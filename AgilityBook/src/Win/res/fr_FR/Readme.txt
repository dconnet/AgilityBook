The fr.po file originated in <wx>/locale/fr.po (2.8.12), copied here, edited
in poedit to change charset to utf8 and set the catalog language properly.
Also changed 'plural-forms' to 'Plural-Forms' to make poedit happy.

NOTE: Fixed 4 bugs identified while compiling:


- `msgid' and `msgstr' entries do not both end with '\n'
-- Changed to english version - the translation is from the previous msgid

#: ../src/common/debugrpt.cpp:565
#, fuzzy
msgid "A debug report has been generated. It can be found in"
msgstr "Un rapport de débogage a été créé dans le dossier\n"


- number of format specifications in 'msgid' and 'msgstr' does not match
-- Added %s

#: ../src/common/textfile.cpp:177
#, fuzzy, c-format
msgid "Failed to convert file \"%s\" to Unicode."
msgstr "La conversion du contenu du fichier en Unicode a échoué."


- number of format specifications in 'msgid' and 'msgstr' does not match
-- Added %s

#: ../src/dfb/fontmgr.cpp:156
#, fuzzy, c-format
msgid "No fonts found in %s."
msgstr "Aucune entrée trouvée."


- number of format specifications in 'msgid' and 'msgstr' does not match
-- Added %s

#: ../src/mac/carbon/aboutdlg.cpp:73
#, fuzzy, c-format
msgid "Version %s"
msgstr "Version "
