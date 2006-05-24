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

#include "Oubliette/OublietteFile.h"

using namespace std;

string getBaseName(const string& name) {
    string::size_type begin=name.find_last_of(":\\/");
    begin=(begin!=string::npos)?(begin+1):0;
    string::size_type end=name.find_last_of(".");
    if (end!=string::npos)
        end-=begin;
    return name.substr(begin,end);
}

int main(int argc,char* argv[]) {
    if (argc!=1+2) {
        cout << "Usage: " << getBaseName(argv[0]) << " <OublietteFile> <Password>" << endl;
        system("pause");
        return -1;
    }

    OublietteFile file(argv[1]);
    if (!file) {
        cerr << file.getLastErrorMessage() << endl;
        system("pause");
        return -1;
    }

    const OublietteFile::Header *header=file.decryptData(argv[2]);
    if (!header) {
        cerr << file.getLastErrorMessage() << endl;
        system("pause");
        return -1;
    }
    if (!header->isValid()) {
        cerr << file.getLastErrorMessage() << endl;
        system("pause");
        return -1;
    }

    cout << "The file was created on       : ";
    cout << header->getCreationTime().getYear() << "/";
    cout << header->getCreationTime().getMonth() << "/";
    cout << header->getCreationTime().getDay() << endl;
    cout << "The file was last modified on : ";
    cout << header->getModificationTime().getYear() << "/";
    cout << header->getModificationTime().getMonth() << "/";
    cout << header->getModificationTime().getDay() << endl;
    cout << "File comment                  : ";
    cout << header->getComment() << endl;
    cout << "Number of contained accounts  : ";
    cout << header->getCount() << endl;

    cout << endl;

    for (int i=0;i<header->getCount();++i) {
        OublietteFile::Account account=file.processNext();

        cout << "Account name    : ";
        cout << account.name << endl;
        cout << "Login username  : ";
        cout << account.username << endl;
        cout << "Login password  : ";
        cout << account.password << endl;
        cout << "URL             : ";
        cout << account.url << endl;
        cout << "E-mail          : ";
        cout << account.email << endl;
        cout << "Memo sheet      : ";
        cout << account.memo << endl;
        cout << "Category        : ";
        cout << account.category << endl;

        cout << "Creation date   : ";
        cout << account.created.getYear() << "/";
        cout << account.created.getMonth() << "/";
        cout << account.created.getDay() << endl;
        cout << "Expiration date : ";
        cout << account.expires.getYear() << "/";
        cout << account.expires.getMonth() << "/";
        cout << account.expires.getDay() << endl;

        cout << "Account note    : ";
        cout << account.note << endl;

        cout << endl;

#ifdef _DEBUG
        system("pause");
#endif
    }

    return 0;
}
