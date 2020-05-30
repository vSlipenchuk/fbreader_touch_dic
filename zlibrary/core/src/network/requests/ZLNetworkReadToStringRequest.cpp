/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#include <cstdlib>
#include <algorithm>

#include <ZLStringUtil.h>

#include "ZLNetworkReadToStringRequest.h"


ZLNetworkReadToStringRequest::ZLNetworkReadToStringRequest(const std::string &url, std::string &buffer) :
	ZLNetworkRequest(url),
	myBuffer(buffer),
	myDataSize(0) {
}

bool ZLNetworkReadToStringRequest::doBefore() {
	return true;
}

bool ZLNetworkReadToStringRequest::doAfter(const std::string &error) {
	finished(error);
	return true;
}

bool ZLNetworkReadToStringRequest::handleHeader(void *ptr, std::size_t size) {
	static const std::string prefix = "Content-Length: ";
	std::string headerData((const char *) ptr, size);
	if (ZLStringUtil::stringStartsWith(headerData, prefix)) {
		int len = std::atoi(headerData.c_str() + prefix.length());
		if (len > 0) {
			myDataSize = len;
		}
	}
	setPercent(0, myDataSize);
	return true;
}

bool ZLNetworkReadToStringRequest::handleContent(void *ptr, std::size_t size) {
	myBuffer.append((const char *) ptr, size);
	// TODO: call setPercent(...)
	return true;
}
