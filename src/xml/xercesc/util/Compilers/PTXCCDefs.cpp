/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: PTXCCDefs.cpp 568078 2007-08-21 11:43:25Z amassari $
 */ 

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include <xercesc/util/Compilers/PTXCCDefs.hpp>
#include <strings.h>


int stricmp(const char* const str1, const char* const  str2) 
{
    return strcasecmp(str1, str2);
}

int strnicmp(const char* const str1, const char* const  str2, const unsigned int count)
{
    if (count == 0)
        return 0;

    return strncasecmp( str1, str2, (size_t)count);
}

