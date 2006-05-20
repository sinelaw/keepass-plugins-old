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
    Header* header=reinterpret_cast<Header*>(m_data_chunk);
    if (!header->isValid()) {
        m_last_error_msg="The file is empty or corrupted.";
        return NULL;
    }

    // Move the internal data pointer right after the header so calls to
    // processNext() will start at the first entry.
    m_data_entry=m_data_chunk;
    m_data_entry+=sizeof(Header);

    if (m_plain_header.m_major_ver>=4) {
        // Skip embedded category icons if present.
        int **ptr=(int**)&m_data_entry;
        int flag=*(*ptr)++;
        int count=*(*ptr)++;
        while (flag && count-->0) {
            unsigned int length=*(*ptr)++;
            m_data_entry+=length;
        }
    }

    return header;
