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

#ifndef OUBLIETTEFILE_H
#define OUBLIETTEFILE_H

#include <fstream>
#include <iostream>
#include <string>

#include "../Delphi/DateTime.h"
#include "../Delphi/ShortString.h"

#define OI_ARRAY_LENGTH(x) (sizeof(x)/sizeof(*x))
#define OI_MIN_2(a,b) ((a)<(b)?(a):(b))

#pragma pack(push,1)

class OublietteFile {
  public:
    enum Algorithm { ALGO_BLOWFISH,ALGO_IDEA };

    class PlainHeader {
        friend OublietteFile;

      public:
        PlainHeader();

        std::string getID() const;
        std::string getVersion() const;
        Algorithm getAlgorithm() const;
        unsigned long getDataSize() const;

        bool isValid() const;
        bool isEncrypted() const;

      private:
        char m_id[5];
        unsigned char m_major_ver,m_minor_ver;
        unsigned char m_algo;
        unsigned long m_size;
    };

    class Header {
      public:
        static const unsigned char MAX_COMMENT_LENGTH=80;

        Header();

        const DateTime& getCreationTime() const;
        const DateTime& getModificationTime() const;

        std::string getComment() const;
        int getCount() const;

        bool isValid() const;

      private:
        unsigned char m_tag; // Unused, thus not exported via access methods.
        DateTime m_created,m_modified;
        ShortString<MAX_COMMENT_LENGTH> m_comment;
        int m_count;
    };

    struct Account {
        std::string name;
        std::string username,password;
        std::string url,email;
        std::string memo;
        int category;

        // Stored differently in version 2 and above.
        DateTime created,expires;

        // Only in version 3 and above.
        std::string note;
    };

    OublietteFile(const std::string& name);
    ~OublietteFile();

    operator bool() const;
    const std::string& getLastErrorMessage() const;

    const Header* decryptData(const std::string& password);

    Account processNext();

  private:
    template<typename CIPHER>
    const Header* decrypt(const std::string& password);

    PlainHeader m_plain_header;
    unsigned char m_password_hash[32];
    char *m_data_chunk,*m_data_entry;
    unsigned long m_data_size_padded;

    std::string m_last_error_msg;
};

#pragma pack(pop)

#include "OubliettePlainHeader.h"
#include "OublietteHeader.h"

inline OublietteFile::~OublietteFile() {
    delete [] m_data_chunk;
}

inline OublietteFile::operator bool() const {
    return m_plain_header.isValid();
}

inline const std::string& OublietteFile::getLastErrorMessage() const {
    return m_last_error_msg;
}

#endif // OUBLIETTEFILE_H
