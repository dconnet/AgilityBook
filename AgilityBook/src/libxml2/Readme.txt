Extract libxml2-2.9.1.tar.gz here.
Update "libxml2Dir" in ComputeLibxml.py
Run "ComputeLibxml.py -a"
Copy ...\win32\build\... into the appropriate directories
Archive lib/* and include/* and add to source control.

---- Changes done ----

libxml2 2.9.0:
timsort.h line 40:
int compute_minrun(uint64_t);
change to:
int compute_minrun(const uint64_t);

include\libxml\tree.h line 121:
XMLPUBFUN size_t XMLCALL         xmlBufUse      (xmlBufPtr buf);
change to:
XMLPUBFUN size_t XMLCALL         xmlBufUse      (const xmlBufPtr buf);

==========

libxml2 2.9.1:
none
