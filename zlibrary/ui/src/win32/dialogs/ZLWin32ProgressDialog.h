/*
 * Copyright (C) 2007-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLWIN32PROGRESSDIALOG_H__
#define __ZLWIN32PROGRESSDIALOG_H__

#include <windows.h>

#include <ZLProgressDialog.h>
#include <ZLUnicodeUtil.h>

class ZLWin32ApplicationWindow;

class ZLWin32ProgressDialog : public ZLProgressDialog {

public:
	ZLWin32ProgressDialog(ZLWin32ApplicationWindow *window, const ZLResourceKey &key);
	~ZLWin32ProgressDialog();

	const ZLUnicodeUtil::Ucs2String &wcharMessage() const;

private:
	void run(ZLRunnable &runnable);
	void setMessage(const std::string &message);

private:
	ZLWin32ApplicationWindow *myApplicationWindow;
	HWND myMessageWindow;
	ZLUnicodeUtil::Ucs2String myWCharMessage;
};

#endif /* __ZLWIN32PROGRESSDIALOG_H__ */
