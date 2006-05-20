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

#ifndef OUBLIETTEPLAINHEADER_H
#define OUBLIETTEPLAINHEADER_H

#include <sstream>

inline OublietteFile::PlainHeader::PlainHeader() {
    memset(this,0,sizeof(*this));
}

inline std::string OublietteFile::PlainHeader::getID() const {
    return std::string(m_id,OI_ARRAY_LENGTH(m_id));
}

inline std::string OublietteFile::PlainHeader::getVersion() const {
    std::stringstream stream;
    stream << m_major_ver << '.' << m_minor_ver;
    return stream.str();
}

inline OublietteFile::Algorithm OublietteFile::PlainHeader::getAlgorithm() const {
    return static_cast<OublietteFile::Algorithm>(m_algo);
}

inline unsigned long OublietteFile::PlainHeader::getDataSize() const {
    return m_size;
}

inline bool OublietteFile::PlainHeader::isValid() const {
    return getID()=="OUBPF"
        && (m_major_ver>=1 && m_major_ver<=4)
        && (getAlgorithm()==ALGO_BLOWFISH || getAlgorithm()==ALGO_IDEA)
        && getDataSize()>0;
}

inline bool OublietteFile::PlainHeader::isEncrypted() const {
    return m_minor_ver!=255;
}

#endif // OUBLIETTEPLAINHEADER_H
