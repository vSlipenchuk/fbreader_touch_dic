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

#ifndef __LITRESBOOKFEEDITEM_H__
#define __LITRESBOOKFEEDITEM_H__

#include "../NetworkItems.h"

class LitResBooksFeedItem : public NetworkCatalogItem {

public:
	struct LoadingState {
		int CurrentPage;
		int AllPagesCount;
	} myLoadingState;

public:
	LitResBooksFeedItem(
		bool shouldSort,
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const UrlInfoCollection &urlByType,
		AccessibilityType accessibility = ALWAYS,
		int flags = FLAGS_DEFAULT
	);

private:
	void onDisplayItem();
	std::string loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener);
	bool supportsResumeLoading();
	std::string resumeLoading(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener);

private:
	shared_ptr<ZLNetworkRequest> getRequest(NetworkItem::List &children);
	static std::string withLimitParameters(std::string url, const LoadingState &state);

private:
	bool myShouldSort;

friend class LitResBooksFeedItemRunnable;
};

#endif /* __LITRESBOOKFEEDITEM_H__ */
