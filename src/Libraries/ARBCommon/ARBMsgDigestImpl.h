/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief File hashing algorithms
 *
 * Revision History
 * 2013-10-29 Added more hash algorithms
 */

#include <string>


extern std::wstring ARBMsgDigestComputeMD5(std::istream& inFile, size_t* outSize);

extern std::wstring ARBMsgDigestComputeSHA1(std::istream& inFile, size_t* outSize);

extern std::wstring ARBMsgDigestComputeSHA256(std::istream& inFile, size_t* outSize);
