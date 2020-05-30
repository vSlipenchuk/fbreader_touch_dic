/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLStringUtil.h>

#include "ZLProgressDialog.h"
#include "ZLDialogManager.h"

ZLProgressDialog::ZLProgressDialog(const ZLResourceKey &key) : myMessage(ZLDialogManager::waitMessageText(key)) {
}

ZLProgressDialog::~ZLProgressDialog() {
}

class ZLProgressDialog::ProgressListener : public ZLNetworkRequest::Listener {

public:
	ProgressListener(ZLProgressDialog &dialog);

private:
	void showPercent(int ready, int full);
	void finished(const std::string &error);

private:
	ZLProgressDialog &myDialog;
};

const std::string &ZLProgressDialog::messageText() const {
	return myMessage;
}

std::string ZLProgressDialog::messageText(int percent) const {
	std::string message = myMessage + ' ';
	ZLStringUtil::appendNumber(message, percent);
	return message + '%';
}

void ZLProgressDialog::ProgressListener::showPercent(int ready, int full) {
	if (full <= 0) {
		return;
	}

	myDialog.setMessage(myDialog.messageText(std::min(100, (int)(ready * 100. / full))));
}

void ZLProgressDialog::ProgressListener::finished(const std::string &/*error*/) {
}

ZLProgressDialog::ProgressListener::ProgressListener(ZLProgressDialog &dialog) : myDialog(dialog) {
}

shared_ptr<ZLNetworkRequest::Listener> ZLProgressDialog::listener() {
	return new ProgressListener(*this);
}
