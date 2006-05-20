/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plug-in
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005 Sebastian Schuberth <sschuberth@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef COMPILE_DLL_EX
    #if _MSC_VER>1200 // Modern Visual Studios need a customized StdAfx.h file.
        #include "../StdAfx.h"
    #else
        #include "../../../StdAfx.h"
    #endif
#endif
#include "OublietteFile.h"

// Reduce the warning level to get rid of tons of warnings.
#pragma warning(push,2)
#include <sha.h>
#include <modes.h>
#include <blowfish.h>
#include <idea.h>
#pragma warning(pop)

using namespace std;

OublietteFile::OublietteFile(const string& name)
  :m_data_chunk(NULL),m_data_size_padded(0) {
    // Try to open the specified file for reading.
    ifstream file(name.c_str(),ios::binary);
    if (!file) {
        m_last_error_msg="Unable to open file \""+name+"\" for reading.";
        return;
    }

    // Read the unencrypted plain text header.
    file.read(reinterpret_cast<char*>(&m_plain_header),sizeof(m_plain_header));

    // Check for plausibility.
    if (!m_plain_header.isValid()) {
        m_last_error_msg="The file header is invalid or corrupted.";
        return;
    }

    // Read the encrypted password's SHA-1 hash.
    file.read(reinterpret_cast<char*>(m_password_hash),sizeof(m_password_hash));

    // Compare the remaining file size to what is stated in the header.
    ifstream::pos_type pos=file.tellg();
    file.seekg(0,ios::end);
    if (static_cast<unsigned long>(file.tellg()-pos)!=m_plain_header.getDataSize()) {
        m_last_error_msg="The file size is invalid.";
        return;
    }
    file.seekg(pos,ios::beg);

    // Read the encrypted data; always allocate a multiple of 8 bytes (the block
    // size of Blowfish and IDEA).
    m_data_size_padded=(m_plain_header.getDataSize()+7)&(~7);
    m_data_chunk=new char[m_data_size_padded];
    file.read(m_data_chunk,m_plain_header.getDataSize());
}

#if _MSC_VER>1200
template<typename CIPHER>
const OublietteFile::Header* OublietteFile::decrypt(const string& password) {
    #include "DecryptMethod.h"
}
#else
// Visual Studio 6.0 cannot handle template member functions.
const OublietteFile::Header* OublietteFile::decryptBlowfish(const std::string& password) {
    #define CIPHER Blowfish
    #include "DecryptMethod.h"
    #undef CIPHER
}

const OublietteFile::Header* OublietteFile::decryptIDEA(const std::string& password) {
    #define CIPHER IDEA
    #include "DecryptMethod.h"
    #undef CIPHER
}
#endif

const OublietteFile::Header* OublietteFile::decryptData(const string& password) {
    using namespace CryptoPP;

#if _MSC_VER>1200
    switch (m_plain_header.getAlgorithm()) {
        case ALGO_BLOWFISH:
            return decrypt<Blowfish>(password);
        case ALGO_IDEA:
            return decrypt<IDEA>(password);
    }
#else
    // Stupid Visual Studio 6.0 needs a work-around.
    switch (m_plain_header.getAlgorithm()) {
        case ALGO_BLOWFISH:
            return decryptBlowfish(password);
        case ALGO_IDEA:
            return decryptIDEA(password);
    }
#endif

    return NULL;
}

OublietteFile::Account OublietteFile::processNext() {
    Account result;
    int **ptr=(int**)&m_data_entry,length;

    // This is a security limit for corrupted files; otherwise memory allocation
    // for the string would take ages for large values.
    const MAX_STRING_LENGTH=65535;

    // Account name.
    length=*(*ptr)++;
    if (length<0 || length>MAX_STRING_LENGTH)
        return result;
    result.name=string(m_data_entry,length);
    m_data_entry+=length;

    // Login username.
    length=*(*ptr)++;
    if (length<0 || length>MAX_STRING_LENGTH)
        return result;
    result.username=string(m_data_entry,length);
    m_data_entry+=length;

    // Login password.
    length=*(*ptr)++;
    if (length<0 || length>MAX_STRING_LENGTH)
        return result;
    result.password=string(m_data_entry,length);
    m_data_entry+=length;

    // Associated URL.
    length=*(*ptr)++;
    if (length<0 || length>MAX_STRING_LENGTH)
        return result;
    result.url=string(m_data_entry,length);
    m_data_entry+=length;

    // Associated e-mail address.
    length=*(*ptr)++;
    if (length<0 || length>MAX_STRING_LENGTH)
        return result;
    result.email=string(m_data_entry,length);
    m_data_entry+=length;

    // Memo sheet.
    length=*(*ptr)++;
    if (length<0 || length>MAX_STRING_LENGTH)
        return result;
    result.memo=string(m_data_entry,length);
    m_data_entry+=length;

    result.category=*(*ptr)++;

    // TODO: Use D3_ConvertSixBytesToDateTime for version 1 files.
    assert(m_plain_header.m_major_ver>1);

    double *date_time=(double*)m_data_entry;
    result.created=DateTime(*date_time++);
    result.expires=DateTime(*date_time++);
    m_data_entry=(char*)date_time;

    // Notes were introduced with version 3.
    if (m_plain_header.m_major_ver>=3) {
        length=*(*ptr)++;
        if (length<0 || length>MAX_STRING_LENGTH)
            return result;
        result.note=string(m_data_entry,length);
        m_data_entry+=length;
    }

    return result;
}
