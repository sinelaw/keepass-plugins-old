/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plug-in
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005-2006 Sebastian Schuberth <sschuberth@gmail.com>
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
    #if _MSC_VER>1200
        // Modern Visual Studios need a customized StdAfx.h file.
        #include "../StdAfx.h"
    #else
        #include "../../../WinGUI/StdAfx.h"
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

template<typename CIPHER>
const OublietteFile::CipherTextHeader* OublietteFile::decrypt(const string& password) {
    using namespace CryptoPP;

    // Calculate the password's 20-byte SHA-1 hash and pad to 32 bytes.
    byte digest[sizeof(m_password_hash)];
    memset(digest,0xff,sizeof(digest));
    SHA().CalculateDigest(
        digest,
        reinterpret_cast<const byte*>(password.c_str()),
        static_cast<unsigned int>(password.length())
    );

    // Clamp the key length to its allowed maximum.
    unsigned int key_length=OI_MIN_2(
        static_cast<unsigned int>(sizeof(digest)),
        static_cast<unsigned int>(CIPHER::MAX_KEYLENGTH)
    );

    // Generate a "random" IV like DCPcrypt does.
    byte iv[CIPHER::BLOCKSIZE];
    memset(iv,0xff,sizeof(iv));
    ECB_Mode<CIPHER>::Encryption enc_iv(digest,key_length);
    enc_iv.ProcessData(iv,iv,sizeof(iv));

    // Initialize the decryption incl. encryption needed for padding.
    CBC_Mode<CIPHER>::Decryption dec_data(digest,key_length,iv);
    CIPHER::Encryption enc_pad(digest,key_length);

    // Encrypt the padded password's SHA-1 hash.
    CBC_Mode<CIPHER>::Encryption enc_digest(digest,key_length,iv);
    enc_digest.ProcessData(digest,digest,sizeof(digest));

    // Compare to the stored encrypted hash.
    unsigned int *a=(unsigned int*)m_password_hash;
    unsigned int *b=(unsigned int*)digest;
    for (unsigned int i=0;i<sizeof(m_password_hash)/sizeof(unsigned int);++i) {
        if (*a!=*b) {
            m_last_error_msg="The specified password is invalid.";
            return NULL;
        }
        ++a;
        ++b;
    }

    // For data chunks not evenly divisible by the cipher's block size some
    // extra work is required.
    unsigned long process_size=m_data_size_padded;
    if (m_data_size_padded>m_plain_header.getDataSize()) {
        byte *pad_block=reinterpret_cast<byte*>(m_data_chunk+m_data_size_padded-CIPHER::BLOCKSIZE);
        enc_pad.ProcessAndXorBlock(
            pad_block-CIPHER::BLOCKSIZE,
            pad_block,
            pad_block
        );
        process_size-=CIPHER::BLOCKSIZE;
    }

    // Decrypt the data chunk.
    dec_data.ProcessData(
        reinterpret_cast<byte*>(m_data_chunk),
        reinterpret_cast<const byte*>(m_data_chunk),
        process_size
    );

    // Check the decrypted header for plausibility.
    CipherTextHeader* header=reinterpret_cast<CipherTextHeader*>(m_data_chunk);
    if (!header->isValid()) {
        m_last_error_msg="The file is empty or corrupted.";
        return NULL;
    }

    // Move the internal data pointer right after the header so future calls to
    // processNext() will start at the first entry.
    m_data_entry=m_data_chunk;
    m_data_entry+=sizeof(CipherTextHeader);

    if (m_plain_header.m_major_ver>=4) {
        // Skip embedded category icons if present.
        int **ptr=(int**)&m_data_entry;
        int flag=*(*ptr)++;
        int count=*(*ptr)++;
        while (flag && count-->0) {
            int length=*(*ptr)++;
            if (length<0 || length>MAX_STRING_LENGTH)
                return NULL;
            m_data_entry+=length;
        }
    }

    return header;
}

const OublietteFile::CipherTextHeader* OublietteFile::decryptData(const string& password) {
    using namespace CryptoPP;

    switch (m_plain_header.getAlgorithm()) {
        case ALGO_BLOWFISH:
            return decrypt<Blowfish>(password);
        case ALGO_IDEA:
            return decrypt<IDEA>(password);
    }

    return NULL;
}

OublietteFile::Account OublietteFile::processNext() {
    Account result;
    int **ptr=(int**)&m_data_entry,length;

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
