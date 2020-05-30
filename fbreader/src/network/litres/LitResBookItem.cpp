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

#include <ZLResource.h>
#include <ZLStringUtil.h>

#include "LitResBooksFeedItem.h"
#include "LitResUtil.h"

#include "LitResBookItem.h"

LitResBookItem::LitResBookItem(
	const NetworkLink &link,
	const std::string &id,
	unsigned int index,
	const std::string &title,
	const std::string &summary,
	const std::string &language,
	const std::string &date,
	const std::vector<AuthorData> &authors,
	const std::vector<std::string> &tags,
	const std::string &seriesTitle,
	unsigned int indexInSeries,
	const UrlInfoCollection &urlByType,
	const std::vector<shared_ptr<BookReference> > references,
	const std::vector<std::string> authorIds
) :
	NetworkBookItem(link, id, index, title, summary, language, date, authors,
					tags, seriesTitle, indexInSeries, urlByType, references),
	myAuthorIds(authorIds)
{

}

std::vector<shared_ptr<NetworkItem> > LitResBookItem::getRelatedCatalogsItems() const {
	std::vector<shared_ptr<NetworkItem> > items;

	UrlInfoCollection urlByType = URLByType;

	urlByType[URL_CATALOG] = LitResUtil::generateAlsoReadUrl(Id);
	items.push_back(new LitResBooksFeedItem(false, Link, resource("alsoRead").value(), std::string(), urlByType));

	for (std::size_t i = 0; i < myAuthorIds.size(); ++i) {
		urlByType[URL_CATALOG] = LitResUtil::generateBooksByAuthorUrl(myAuthorIds.at(i));
		std::string title = ZLStringUtil::printf(resource("sameAuthor").value(), Authors.at(i).DisplayName);
		items.push_back(new LitResBooksFeedItem(false, Link, title, std::string(), urlByType));
	}

	return items;
}

const ZLResource &LitResBookItem::resource(const std::string &resourceKey) const {
	return ZLResource::resource("networkView")["bookNode"][resourceKey];
}
