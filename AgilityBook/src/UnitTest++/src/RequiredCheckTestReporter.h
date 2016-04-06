#ifndef UNITTEST_REQUIRED_CHECK_TEST_REPORTER_H
#define UNITTEST_REQUIRED_CHECK_TEST_REPORTER_H

#include "HelperMacros.h"
#include "ThrowingTestReporter.h"

#if defined(_MSC_VER) && _MSC_VER <= 1600
#pragma warning(push)
#pragma warning(disable : 4512)
#endif

namespace UnitTest {

   class TestResults;

   // This RAII class decorates the current TestReporter with
   // a version that throws after reporting a failure.
   class UNITTEST_LINKAGE RequiredCheckTestReporter
   {
   public:
      explicit RequiredCheckTestReporter(TestResults& results);
      ~RequiredCheckTestReporter();

      bool Next();

   private:
      TestResults& m_results;
      TestReporter* m_originalTestReporter;
      ThrowingTestReporter m_throwingReporter;
      int m_continue;
   };
}

#if defined(_MSC_VER) && _MSC_VER <= 1600
#pragma warning(pop)
#endif

#endif
