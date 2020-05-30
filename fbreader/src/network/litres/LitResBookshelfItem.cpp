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

#include <algorithm>

#include <ZLNetworkRequest.h>
#include <ZLExecutionUtil.h>

#include "LitResBookshelfItem.h"
#include "../authentication/litres/LitResAuthenticationManager.h"

#include "../NetworkLink.h"
#include "../NetworkComparators.h"
#include "../NetworkErrors.h"

#include "SortedCatalogItem.h"

LitResBookshelfItem::LitResBookshelfItem(
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const UrlInfoCollection &urlByType,
	AccessibilityType accessibility
) : NetworkCatalogItem(
	link,
	title,
	summary,
	urlByType,
	accessibility
) {
	myForceReload = false;
}

void LitResBookshelfItem::onDisplayItem() {
	myForceReload = false;
}

class LitResBookshelfItemLoaderScope : public ZLUserData {
public:
	LitResBookshelfItemLoaderScope(NetworkItem::List &children) : Children(children) {}
	NetworkItem::List &Children;
	shared_ptr<ZLNetworkRequest::Listener> listener;
};


std::string LitResBookshelfItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
	LitResAuthenticationManager &mgr = (LitResAuthenticationManager&)*Link.authenticationManager();
	if (mgr.isAuthorised().Status == B3_FALSE) {
		listener->finished(NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED));
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}

	LitResBookshelfItemLoaderScope *scope = new LitResBookshelfItemLoaderScope(children);
	scope->listener = listener;

	shared_ptr<ZLUserDataHolder> scopeData = new ZLUserDataHolder;
	scopeData->addUserData("scope", scope);
	if (myForceReload) {
		mgr.reloadPurchasedBooks(ZLExecutionUtil::createListener(scopeData, this, &LitResBookshelfItem::onReloaded));
		return std::string();
	}
	onReloaded(*scopeData, std::string());
	return std::string();
}

void LitResBookshelfItem::onReloaded(ZLUserDataHolder &data, const std::string &error) {
	LitResBookshelfItemLoaderScope &scope = static_cast<LitResBookshelfItemLoaderScope&>(*data.getUserData("scope"));
	LitResAuthenticationManager &mgr = static_cast<LitResAuthenticationManager&>(*Link.authenticationManager());
	myForceReload = true;
	NetworkItem::List tmpChildren;
	mgr.collectPurchasedBooks(tmpChildren);
	std::sort(tmpChildren.begin(), tmpChildren.end(), NetworkBookItemComparator());


	NetworkItem::List &children = scope.Children;

	if (tmpChildren.size() <= 5) {
		children.assign(tmpChildren.begin(), tmpChildren.end());
		std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	} else {
		children.push_back(SortedCatalogItem::create(*this, "byDate",   tmpChildren, FLAG_SHOW_AUTHOR));
		children.push_back(SortedCatalogItem::create(*this, "byAuthor", tmpChildren, FLAG_GROUP_BY_AUTHOR, NetworkBookItemComparator()));
		children.push_back(SortedCatalogItem::create(*this, "byTitle",  tmpChildren, FLAG_SHOW_AUTHOR, NetworkBookItemByTitleComparator()));
		SortedCatalogItem* bySeries = SortedCatalogItem::create(*this, "bySeries", tmpChildren, FLAG_SHOW_AUTHOR | FLAG_GROUP_BY_SERIES,
																NetworkBookItemBySeriesComparator(), SortedCatalogItem::BySeriesFilter());

		if (!bySeries->isEmpty()) {
			children.push_back(bySeries);
		} else {
			delete bySeries;
		}
	}

	scope.listener->finished(error);
}
