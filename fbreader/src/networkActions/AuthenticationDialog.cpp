/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <ZLDialog.h>
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>

#include <optionEntries/ZLSimpleOptionEntry.h>

#include "../network/NetworkLink.h"
#include "AuthenticationDialog.h"
#include "NetworkOperationRunnable.h"
#include "../network/UserList.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

class UserNamesEntry : public ZLComboOptionEntry {

public:
	UserNamesEntry(UserList &userList, ZLStringOption &userNameOption);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	UserList &myUserList;
	ZLStringOption &myUserNameOption;
};

UserNamesEntry::UserNamesEntry(UserList &userList, ZLStringOption &userNameOption) :
	ZLComboOptionEntry(true), myUserList(userList), myUserNameOption(userNameOption) {
}

const std::string &UserNamesEntry::initialValue() const {
	return myUserNameOption.value();
}

const std::vector<std::string> &UserNamesEntry::values() const {
	return myUserList.users();
}

void UserNamesEntry::onAccept(const std::string &value) {
	myUserList.addUser(value);
	myUserNameOption.setValue(value);
}


class PasswordOptionEntry : public ZLPasswordOptionEntry {

public:
	PasswordOptionEntry(std::string &password);

	virtual const std::string &initialValue() const;
	virtual void onAccept(const std::string &value);

private:
	std::string &myPassword;
};

PasswordOptionEntry::PasswordOptionEntry(std::string &password) : myPassword(password) {
}

const std::string &PasswordOptionEntry::initialValue() const {
	static const std::string _empty;
	return _empty;
}

void PasswordOptionEntry::onAccept(const std::string &value) {
	myPassword = value;
}

AuthenticationDialog::AuthenticationDialog(ZLStringOption &userNameOption, UserList &userList, const std::string &errorMessage, std::string &password) :
	myUserList(userList) {
	myDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("AuthenticationDialog"));

	if (!errorMessage.empty()) {
		myDialog->addOption("", "", new ZLSimpleStaticTextOptionEntry(errorMessage));
	}

	myDialog->addOption(ZLResourceKey("login"), new UserNamesEntry(myUserList, userNameOption));
	myDialog->addOption(ZLResourceKey("password"), new PasswordOptionEntry(password));

	myDialog->addButton(ZLDialogManager::OK_BUTTON, true);
	myDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
}

bool AuthenticationDialog::run(ZLStringOption &userNameOption, UserList &userList, const std::string &errorMessage, std::string &password) {
	AuthenticationDialog dlg(userNameOption, userList, errorMessage, password);
	if (dlg.dialog().run()) {
		dlg.dialog().acceptValues();
		return true;
	}
	return false;
}

bool AuthenticationDialog::run(const std::string &siteName, std::string &userName, std::string &password, const std::string &message) {
	UserList userList(siteName);
	//TODO fix it: using unexisted string option, just for dialog showing
	ZLStringOption userNameOption(ZLCategoryKey::NETWORK, "", "userName", userName);
	userNameOption.setValue(std::string());
	bool result = run(userNameOption, userList, message, password);
	userName = userNameOption.value();
	userNameOption.clearGroup("");
	return result;
}
