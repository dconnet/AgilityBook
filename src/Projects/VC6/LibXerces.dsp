# Microsoft Developer Studio Project File - Name="LibXerces" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LibXerces - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LibXerces.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LibXerces.mak" CFG="LibXerces - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LibXerces - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LibXerces - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LibXerces - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../bin/VC6/Debug"
# PROP Intermediate_Dir "../../../bin/VC6/Debug/LibXerces"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\xml" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_CRTDBG_MAP_ALLOC" /D "PROJ_SAX2" /D "PROJ_XMLPARSER" /D "PROJ_XMLUTIL" /D "PROJ_PARSERS" /D "PROJ_SAX4C" /D "PROJ_DOM" /D "PROJ_VALIDATORS" /D "XML_USE_WIN32_TRANSCODER" /D "XML_USE_INMEM_MESSAGELOADER" /D "XML_USE_NETACCESSOR_WINSOCK" /D "XML_LIBRARY" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LibXerces - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/VC6/Release"
# PROP Intermediate_Dir "../../../bin/VC6/Release/LibXerces"
# PROP Target_Dir ""
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\xml" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_CRTDBG_MAP_ALLOC" /D "PROJ_SAX2" /D "PROJ_XMLPARSER" /D "PROJ_XMLUTIL" /D "PROJ_PARSERS" /D "PROJ_SAX4C" /D "PROJ_DOM" /D "PROJ_VALIDATORS" /D "XML_USE_WIN32_TRANSCODER" /D "XML_USE_INMEM_MESSAGELOADER" /D "XML_USE_NETACCESSOR_WINSOCK" /D "XML_LIBRARY" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "LibXerces - Win32 Debug"
# Name "LibXerces - Win32 Release"
# Begin Group "util"

# PROP Default_Filter ".cpp,.hpp"
# Begin Group "Win32"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Platforms\Win32\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Compilers\VCPPDefs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Platforms\Win32\Win32Defs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Platforms\Win32\Win32PlatformUtils.cpp
# ADD CPP /Ze
# End Source File
# End Group
# Begin Group "MsgLoaders"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\util\MsgLoaders\InMemory\InMemMsgLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\MsgLoaders\InMemory\InMemMsgLoader.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\MsgLoaders\InMemory\XercesMessages_en_US.hpp
# End Source File
# End Group
# Begin Group "Transcoders"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Transcoders\Win32\Win32TransService.cpp
# ADD CPP /Ze
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Transcoders\Win32\Win32TransService.hpp
# End Source File
# End Group
# Begin Group "NetAccessors"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NetAccessors\WinSock\BinHTTPURLInputStream.cpp
# ADD CPP /Ze
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NetAccessors\WinSock\BinHTTPURLInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NetAccessors\WinSock\WinSockNetAccessor.cpp
# ADD CPP /Ze
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NetAccessors\WinSock\WinSockNetAccessor.hpp
# End Source File
# End Group
# Begin Group "regx"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ASCIIRangeFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ASCIIRangeFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\BlockRangeFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\BlockRangeFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\BMPattern.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\BMPattern.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\CharToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\CharToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ClosureToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ClosureToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ConcatToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ConcatToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ConditionToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ConditionToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\Match.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\Match.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ModifierToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ModifierToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\Op.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\Op.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\OpFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\OpFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ParenToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ParenToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ParserForXMLSchema.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\ParserForXMLSchema.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RangeFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RangeFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RangeToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RangeToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RangeTokenMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RangeTokenMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RegularExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RegularExpression.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RegxDefs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RegxParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RegxParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RegxUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\RegxUtil.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\StringToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\StringToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\Token.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\Token.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\TokenFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\TokenFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\TokenInc.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\UniCharTable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\UnicodeRangeFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\UnicodeRangeFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\UnionToken.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\UnionToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\XMLRangeFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\XMLRangeFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\XMLUniCharacter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\regx\XMLUniCharacter.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ArrayIndexOutOfBoundsException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\AutoSense.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Base64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Base64.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BaseRefVectorOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BaseRefVectorOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BinFileInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BinFileInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BinInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BinInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BinMemInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BinMemInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BitOps.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BitSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\BitSet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\CountedPointer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\DefaultPanicHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\DefaultPanicHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\EmptyStackException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\EncodingValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\EncodingValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\FlagJanitor.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\FlagJanitor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HashBase.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HashCMStateSet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HashPtr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HashPtr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HashXMLCh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HashXMLCh.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HeaderDummy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HexBin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\HexBin.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\IllegalArgumentException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\InvalidCastException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\IOException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Janitor.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Janitor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\KeyRefPair.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\KeyRefPair.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\KeyValuePair.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\KVStringPair.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\KVStringPair.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\LogicalPath.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Mutexes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\Mutexes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NameIdPool.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NameIdPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NoSuchElementException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NullPointerException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\NumberFormatException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\OutOfMemoryException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\PanicHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\PanicHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ParseException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\PlatformUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\PlatformUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\QName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\QName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefArrayOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefArrayOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefArrayVectorOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefArrayVectorOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefHash2KeysTableOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefHash2KeysTableOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefHash3KeysIdPool.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefHash3KeysIdPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefHashTableOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefHashTableOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefStackOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefStackOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefVectorOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RefVectorOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\RuntimeException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\SchemaDateTimeException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\SecurityManager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\StringPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\StringPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\SynchronizedStringPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\SynchronizedStringPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\TranscodingException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\TransENameMap.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\TransENameMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\TransService.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\TransService.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\UnexpectedEOFException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\UnsupportedEncodingException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\UTFDataFormatException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ValueArrayOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ValueHashTableOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ValueHashTableOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ValueStackOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ValueStackOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ValueVectorOf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\ValueVectorOf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XercesDefs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XercesVersion.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMemory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMemory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XML256TableTranscoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XML256TableTranscoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XML88591Transcoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XML88591Transcoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLAbstractDoubleFloat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLAbstractDoubleFloat.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLASCIITranscoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLASCIITranscoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLBigDecimal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLBigDecimal.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLBigInteger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLBigInteger.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLChar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLChar.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLChTranscoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLChTranscoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLDateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLDateTime.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLDeleterFor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLDOMMsg.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLDouble.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLDouble.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLEBCDICTranscoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLEBCDICTranscoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLEntityResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLEnumerator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLExceptMsgs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLFloat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLFloat.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLHolder.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLHolder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLIBM1047Transcoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLIBM1047Transcoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLIBM1140Transcoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLIBM1140Transcoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLInitializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLInitializer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLInteger.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLMsgLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLMsgLoader.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLNetAccessor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLNumber.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLNumber.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLRegisterCleanup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLRegisterCleanup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLResourceIdentifier.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLString.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLStringTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLStringTokenizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUCS4Transcoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUCSTranscoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUni.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUni.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUniDefs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUri.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUri.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLURL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLURL.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUTF16Transcoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUTF16Transcoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUTF8Transcoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLUTF8Transcoder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLWin1252Transcoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\util\XMLWin1252Transcoder.hpp
# End Source File
# End Group
# Begin Group "framework"

# PROP Default_Filter ""
# Begin Group "psvi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIAttribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIAttribute.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIAttributeList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIAttributeList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIElement.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIElement.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\PSVIItem.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAnnotation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAnnotation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAttributeDeclaration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAttributeDeclaration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAttributeGroupDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAttributeGroupDefinition.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAttributeUse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSAttributeUse.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSComplexTypeDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSComplexTypeDefinition.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSConstants.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSElementDeclaration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSElementDeclaration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSFacet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSFacet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSIDCDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSIDCDefinition.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSModel.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSModelGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSModelGroup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSModelGroupDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSModelGroupDefinition.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSMultiValueFacet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSMultiValueFacet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSNamedMap.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSNamedMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSNamespaceItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSNamespaceItem.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSNotationDeclaration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSNotationDeclaration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSParticle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSParticle.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSSimpleTypeDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSSimpleTypeDefinition.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSTypeDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSTypeDefinition.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSValue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSValue.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSWildcard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\psvi\XSWildcard.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\BinOutputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\BinOutputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\LocalFileFormatTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\LocalFileFormatTarget.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\LocalFileInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\LocalFileInputSource.Hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\MemBufFormatTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\MemBufFormatTarget.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\MemBufInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\MemBufInputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\MemoryManager.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\StdInInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\StdInInputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\StdOutFormatTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\StdOutFormatTarget.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\URLInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\URLInputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\ValidationContext.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\Wrapper4DOMInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\Wrapper4DOMInputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\Wrapper4InputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\Wrapper4InputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLAttDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLAttDef.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLAttDefList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLAttDefList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLAttr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLAttr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLBufferMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLBufferMgr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLContentModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLContentModel.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLDocumentHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLDTDDescription.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLDTDDescription.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLElementDecl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLElementDecl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLEntityDecl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLEntityDecl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLEntityHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLErrorCodes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLErrorReporter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLFormatter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLFormatter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLGrammarDescription.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLGrammarDescription.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLGrammarPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLNotationDecl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLNotationDecl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLPScanToken.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLRecognizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLRecognizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLRefInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLRefInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLSchemaDescription.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLSchemaDescription.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\framework\XMLValidityCodes.hpp
# End Source File
# End Group
# Begin Group "internal"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\BinFileOutputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\BinFileOutputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\BinMemOutputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\BinMemOutputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\CharTypeTables.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\DGXMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\DGXMLScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\ElemStack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\ElemStack.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\EndOfEntityException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\IANAEncodings.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\IGXMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\IGXMLScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\IGXMLScanner2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\MemoryManagerArrayImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\MemoryManagerArrayImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\MemoryManagerImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\MemoryManagerImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\ReaderMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\ReaderMgr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\SGXMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\SGXMLScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\ValidationContextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\ValidationContextImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\VecAttributesImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\VecAttributesImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\VecAttrListImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\VecAttrListImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\WFXMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\WFXMLScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLGrammarPoolImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLGrammarPoolImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLInternalErrorHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLReader.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLScannerResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XMLScannerResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XProtoType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XProtoType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSAXMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSAXMLScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSerializable.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSerializationException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSerializeEngine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSerializeEngine.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSObjectFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XSObjectFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XTemplateSerializer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\internal\XTemplateSerializer.hpp
# End Source File
# End Group
# Begin Group "sax"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\AttributeList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\DocumentHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\DTDHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\Dummy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\EntityResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\ErrorHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\HandlerBase.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\InputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\InputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\Locator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\Parser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\SAXException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\SAXException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\SAXParseException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax\SAXParseException.hpp
# End Source File
# End Group
# Begin Group "parsers"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\AbstractDOMParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\AbstractDOMParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\DOMBuilderImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\DOMBuilderImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\SAX2XMLFilterImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\SAX2XMLFilterImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\SAX2XMLReaderImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\SAX2XMLReaderImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\SAXParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\SAXParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\XercesDOMParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\parsers\XercesDOMParser.hpp
# End Source File
# End Group
# Begin Group "validators"

# PROP Default_Filter ".cpp,.hpp"
# Begin Group "common"

# PROP Default_Filter "*.cpp, *.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\AllContentModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\AllContentModel.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMAny.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMAny.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMBinaryOp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMBinaryOp.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMLeaf.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMStateSet.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMUnaryOp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\CMUnaryOp.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\ContentLeafNameTypeVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\ContentLeafNameTypeVector.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\ContentSpecNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\ContentSpecNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\DFAContentModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\DFAContentModel.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\Grammar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\Grammar.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\GrammarResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\GrammarResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\MixedContentModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\MixedContentModel.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\SimpleContentModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\common\SimpleContentModel.hpp
# End Source File
# End Group
# Begin Group "datatype"

# PROP Default_Filter "*.cpp, *.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AbstractNumericFacetValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AbstractNumericFacetValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AbstractNumericValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AbstractNumericValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AbstractStringValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AbstractStringValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AnySimpleTypeDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AnySimpleTypeDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AnyURIDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\AnyURIDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\Base64BinaryDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\Base64BinaryDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\BooleanDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\BooleanDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DatatypeValidatorFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DatatypeValidatorFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DateDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DateDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DateTimeDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DateTimeDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DateTimeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DateTimeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DayDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DayDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DecimalDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DecimalDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DoubleDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DoubleDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DurationDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\DurationDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\ENTITYDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\ENTITYDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\FloatDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\FloatDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\HexBinaryDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\HexBinaryDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\IDDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\IDDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\IDREFDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\IDREFDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\InvalidDatatypeFacetException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\InvalidDatatypeValueException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\ListDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\ListDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\MonthDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\MonthDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\MonthDayDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\MonthDayDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\NameDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\NameDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\NCNameDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\NCNameDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\NOTATIONDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\NOTATIONDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\QNameDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\QNameDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\StringDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\StringDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\TimeDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\TimeDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\UnionDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\UnionDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\XMLCanRepGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\XMLCanRepGroup.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\YearDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\YearDatatypeValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\YearMonthDatatypeValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\datatype\YearMonthDatatypeValidator.hpp
# End Source File
# End Group
# Begin Group "DTD"

# PROP Default_Filter "*.cpp, *.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DocTypeHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDAttDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDAttDef.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDAttDefList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDAttDefList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDElementDecl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDElementDecl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDEntityDecl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDEntityDecl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDGrammar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDGrammar.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDScanner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDScanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\DTDValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\XMLDTDDescriptionImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\DTD\XMLDTDDescriptionImpl.hpp
# End Source File
# End Group
# Begin Group "schema"

# PROP Default_Filter "*.cpp, *.hpp"
# Begin Group "identity"

# PROP Default_Filter "*.cpp, *.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\FieldActivator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\FieldActivator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\FieldValueMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\FieldValueMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Field.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Field.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Key.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Key.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_KeyRef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_KeyRef.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Selector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Selector.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Unique.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IC_Unique.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IdentityConstraint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IdentityConstraint.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IdentityConstraintHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\IdentityConstraintHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\ValueStore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\ValueStore.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\ValueStoreCache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\ValueStoreCache.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XercesXPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XercesXPath.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XPathException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XPathMatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XPathMatcher.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XPathMatcherStack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XPathMatcherStack.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XPathSymbols.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\identity\XPathSymbols.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\ComplexTypeInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\ComplexTypeInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\GeneralAttributeCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\GeneralAttributeCheck.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\NamespaceScope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\NamespaceScope.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\PSVIDefs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaAttDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaAttDef.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaAttDefList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaAttDefList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaElementDecl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaElementDecl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaGrammar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaGrammar.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaSymbols.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaSymbols.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaValidator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SchemaValidator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SubstitutionGroupComparator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\SubstitutionGroupComparator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\TraverseSchema.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\TraverseSchema.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XercesAttGroupInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XercesAttGroupInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XercesElementWildcard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XercesElementWildcard.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XercesGroupInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XercesGroupInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XMLSchemaDescriptionImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XMLSchemaDescriptionImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XSDDOMParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XSDDOMParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XSDErrorReporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XSDErrorReporter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XSDLocator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XSDLocator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\validators\schema\XUtil.hpp
# End Source File
# End Group
# End Group
# Begin Group "sax2"

# PROP Default_Filter ".cpp,.hpp"
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\Attributes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\ContentHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\DeclHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\DefaultHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\LexicalHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\sax2Dummy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\SAX2XMLFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\SAX2XMLReader.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\sax2\XMLReaderFactory.hpp
# End Source File
# End Group
# Begin Group "dom"

# PROP Default_Filter ""
# Begin Group "impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMAttrImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMAttrImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMAttrMapImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMAttrMapImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMAttrNSImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMAttrNSImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMCasts.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMCDATASectionImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMCDATASectionImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMCharacterDataImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMCharacterDataImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMChildNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMChildNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMCommentImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMCommentImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMConfigurationImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMConfigurationImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDeepNodeListImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDeepNodeListImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDeepNodeListPool.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDeepNodeListPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDocumentFragmentImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDocumentFragmentImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDocumentImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDocumentImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDocumentTypeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMDocumentTypeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMElementImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMElementImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMElementNSImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMElementNSImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMEntityImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMEntityImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMEntityReferenceImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMEntityReferenceImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMErrorImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMErrorImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMImplementationImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMImplementationimpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMImplementationRegistry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMLocatorImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMLocatorImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNamedNodeMapImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNamedNodeMapImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeIDMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeIDMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeIteratorImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeIteratorImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeListImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeListImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNodeVector.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNormalizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNormalizer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNotationImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMNotationImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMParentNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMParentNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMProcessingInstructionImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMProcessingInstructionImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMRangeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMRangeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMStringPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMStringPool.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMTextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMTextImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMTreeWalkerImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMTreeWalkerImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMTypeInfoImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMTypeInfoImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMWriterImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\DOMWriterImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\XSDElementNSImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\impl\XSDElementNSImpl.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOM.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMAttr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMBuilder.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMCDATASection.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMCharacterData.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMComment.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMConfiguration.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMDocument.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMDocumentFragment.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMDocumentRange.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMDocumentTraversal.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMDocumentType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMElement.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMEntity.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMEntityReference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMEntityResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMError.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMErrorHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMImplementation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMImplementationLS.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMImplementationRegistry.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMImplementationSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMInputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMLocator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMNamedNodeMap.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMNode.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMNodeFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMNodeIterator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMNodeList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMNotation.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMProcessingInstruction.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMPSVITypeInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMRange.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMRangeException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMRangeException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMText.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMTreeWalker.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMTypeInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMUserDataHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMWriter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMWriterFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMXPathEvaluator.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMXPathException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMXPathException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMXPathExpression.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMXPathNamespace.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMXPathNSResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\DOMXPathResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\xml\xercesc\dom\StDOMNode.hpp
# End Source File
# End Group
# End Target
# End Project
