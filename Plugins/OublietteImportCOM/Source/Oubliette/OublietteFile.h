/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plugin
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005-2009 Sebastian Schuberth <sschuberth@gmail.com>
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
#include <sstream>
#include <vector>

#include "../Delphi/DateTime.h"
#include "../Delphi/ShortString.h"

#pragma pack(push,1)

class OublietteFile
{
  public:

    enum Algorithm {
        ALGO_BLOWFISH
    ,   ALGO_IDEA
    };

    // This is a security limit for corrupted files; otherwise memory allocation
    // for strings would take ages for large values.
    static int const MAX_STRING_LENGTH=65535;

    // This is the plain text header that starts the Oubliette file. It needs to
    // be a POD object of 12 bytes.
    class PlainTextHeader
    {
        friend OublietteFile;

      public:

        PlainTextHeader() {
            assert(sizeof(*this)==12);
            memset(this,0,sizeof(*this));
        }

        std::string getID() const {
            return std::string(m_id,_countof(m_id));
        }

        std::string getVersion() const {
            std::stringstream stream;
            stream << m_major_ver << '.' << m_minor_ver;
            return stream.str();
        }

        Algorithm getAlgorithm() const {
            return static_cast<OublietteFile::Algorithm>(m_algo);
        }

        unsigned long getDataSize() const {
            return m_size;
        }

        bool isValid() const {
            return getID()=="OUBPF"
                && (m_major_ver>=1 && m_major_ver<=4)
                && (getAlgorithm()==ALGO_BLOWFISH || getAlgorithm()==ALGO_IDEA)
                && getDataSize()>0;
        }

        bool isEncrypted() const {
            return m_minor_ver!=255;
        }

      private:

        char m_id[5];
        unsigned char m_major_ver,m_minor_ver;
        unsigned char m_algo;
        unsigned long m_size;
    };

    // This is the cipher text header that starts the encrypted data. It needs
    // to be a POD object of 182 bytes.
    class CipherTextHeader
    {
      public:

        static unsigned char const MAX_COMMENT_LENGTH=80;

        CipherTextHeader() {
            assert(sizeof(*this)==182);
            memset(this,0,sizeof(*this));
        }

        DateTime getCreationTime() const {
            return DateTime(m_created);
        }

        DateTime getModificationTime() const {
            return DateTime(m_modified);
        }

        std::string getComment() const {
            return m_comment.getString();
        }

        int getCount() const {
            return m_count;
        }

        bool isValid() const {
            return m_tag==1 && m_count>0;
        }

      private:

        unsigned char m_tag; // Unused, thus not exported via access methods.
        double m_created,m_modified;
        ShortString<MAX_COMMENT_LENGTH> m_comment;
        int m_count;
    };

    struct BitmapFileHeader {
        unsigned short Type;
        unsigned int   Size;
        unsigned short Reserved1;
        unsigned short Reserved2;
        unsigned int   OffBits;
    };

    struct BitmapInfoHeader {
        unsigned int   Size;
        int            Width;
        int            Height;
        unsigned short Planes;
        unsigned short BitCount;
        unsigned int   Compression;
        unsigned int   SizeImage;
        int            XPelsPerMeter;
        int            YPelsPerMeter;
        unsigned int   ClrUsed;
        unsigned int   ClrImportant;
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

    OublietteFile(std::string const& name);
    ~OublietteFile();

    operator bool() const;
    std::string const& getLastErrorMessage() const;

    CipherTextHeader const* decryptData(std::string const& password);

    Account processNext();
    std::string const& getCategoryName(int index) const;

  private:

    template<typename CIPHER>
    CipherTextHeader const* decrypt(std::string const& password);

    PlainTextHeader m_plain_header;
    unsigned char m_password_hash[32];
    char *m_data_chunk,*m_data_entry;
    unsigned long m_data_size_padded;

    std::vector<std::string> m_category_names;

    std::string m_last_error_msg;
};

#pragma pack(pop)

inline OublietteFile::~OublietteFile()
{
    delete [] m_data_chunk;
}

inline OublietteFile::operator bool() const
{
    return m_plain_header.isValid();
}

inline std::string const& OublietteFile::getLastErrorMessage() const
{
    return m_last_error_msg;
}

inline std::string const& OublietteFile::getCategoryName(int index) const
{
    static std::string const empty;
    if (index>=0 && index<static_cast<int>(m_category_names.size()))
        return m_category_names[index];
    return empty;
}

#endif // OUBLIETTEFILE_H
