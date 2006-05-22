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

#ifndef OUBLIETTEHEADER_H
#define OUBLIETTEHEADER_H

inline OublietteFile::Header::Header() {
    memset(this,0,sizeof(*this));
}

inline const DateTime& OublietteFile::Header::getCreationTime() const {
    return m_created;
}

inline const DateTime& OublietteFile::Header::getModificationTime() const {
    return m_modified;
}

inline std::string OublietteFile::Header::getComment() const {
    return m_comment.getString();
}

inline int OublietteFile::Header::getCount() const {
    return m_count;
}

inline bool OublietteFile::Header::isValid() const {
    return m_tag==1 && m_count>0;
}

#endif // OUBLIETTEHEADER_H
