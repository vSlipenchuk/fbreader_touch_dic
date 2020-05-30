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

#ifndef __LITRESBOOKSHELFITEM_H__
#define __LITRESBOOKSHELFITEM_H__

#include <ZLResource.h>
#include <ZLExecutionUtil.h>

#include "../NetworkComparators.h"
#include "../NetworkItems.h"

class NetworkLink;

class LitResBookshelfItem : public NetworkCatalogItem {

public:
	LitResBookshelfItem(
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const UrlInfoCollection &urlByType,
		AccessibilityType accessibility = ALWAYS
	);

private:
	void onDisplayItem();
	std::string loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener);
	void onReloaded(ZLUserDataHolder &data, const std::string &error);

private:
	bool myForceReload;
};

#endif /* __LITRESBOOKSHELFITEM_H__ */
