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

#include <cstdlib>
#include <algorithm>

#include <ZLFile.h>
#include <ZLFileImage.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "MobipocketHtmlBookReader.h"
#include "PalmDocStream.h"
#include "../html/HtmlTagActions.h"
#include "../../bookmodel/BookModel.h"

class MobipocketHtmlImageTagAction : public HtmlTagAction {

public:
	MobipocketHtmlImageTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlHrTagAction : public HtmlTagAction {

public:
	MobipocketHtmlHrTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlHrefTagAction : public HtmlHrefTagAction {

public:
	MobipocketHtmlHrefTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlGuideTagAction : public HtmlTagAction {

public:
	MobipocketHtmlGuideTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlReferenceTagAction : public HtmlTagAction {

public:
	MobipocketHtmlReferenceTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlPagebreakTagAction : public HtmlTagAction {

public:
	MobipocketHtmlPagebreakTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

MobipocketHtmlImageTagAction::MobipocketHtmlImageTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlImageTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "RECINDEX") {
				int index = std::atoi(tag.Attributes[i].Value.c_str());
				if (index > 0) {
					int &imageCounter = ((MobipocketHtmlBookReader&)myReader).myImageCounter;
					imageCounter = std::max(imageCounter, index);
					bool stopParagraph = bookReader().paragraphIsOpen();
					if (stopParagraph) {
						bookReader().endParagraph();
					}
					std::string id;
					ZLStringUtil::appendNumber(id, index);
					bookReader().addImageReference(id);
					if (stopParagraph) {
						bookReader().beginParagraph();
					}
				}
				break;
			}
		}
	}
}

MobipocketHtmlHrTagAction::MobipocketHtmlHrTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlHrTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		if (bookReader().contentsParagraphIsOpen()) {
			bookReader().endContentsParagraph();
			bookReader().exitTitle();
		}
		bookReader().insertEndOfSectionParagraph();
	}
}

MobipocketHtmlHrefTagAction::MobipocketHtmlHrefTagAction(HtmlBookReader &reader) : HtmlHrefTagAction(reader) {
}

MobipocketHtmlPagebreakTagAction::MobipocketHtmlPagebreakTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlPagebreakTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		if (bookReader().contentsParagraphIsOpen()) {
			bookReader().endContentsParagraph();
			bookReader().exitTitle();
		}
		bookReader().insertEndOfSectionParagraph();
	}
}

MobipocketHtmlBookReader::TOCReader::TOCReader(MobipocketHtmlBookReader &reader) : myReader(reader) {
	reset();
}

void MobipocketHtmlBookReader::TOCReader::reset() {
	myEntries.clear();

	myIsActive = false;
	myStartOffset = (std::size_t)-1;
	myEndOffset = (std::size_t)-1;
	myCurrentEntryText.erase();
}

bool MobipocketHtmlBookReader::TOCReader::rangeContainsPosition(std::size_t position) {
	return (myStartOffset <= position) && (myEndOffset > position);
}

void MobipocketHtmlBookReader::TOCReader::startReadEntry(std::size_t position) {
	myCurrentReference = position;
	myIsActive = true;
}

void MobipocketHtmlBookReader::TOCReader::endReadEntry() {
	if (myIsActive && !myCurrentEntryText.empty()) {
		std::string converted;
		myReader.myConverter->convert(converted, myCurrentEntryText);
		myReader.myConverter->reset();
		myEntries[myCurrentReference] = converted;
		myCurrentEntryText.erase();
	}
	myIsActive = false;
}

void MobipocketHtmlBookReader::TOCReader::appendText(const char *text, std::size_t len) {
	if (myIsActive) {
		myCurrentEntryText.append(text, len);
	}
}

void MobipocketHtmlBookReader::TOCReader::addReference(std::size_t position, const std::string &text) {
	myEntries[position] = text;
	if (rangeContainsPosition(position)) {
		setEndOffset(position);
	}
}

void MobipocketHtmlBookReader::TOCReader::setStartOffset(std::size_t position) {
	myStartOffset = position;
	std::map<std::size_t,std::string>::const_iterator it = myEntries.lower_bound(position);
	if (it != myEntries.end()) {
		++it;
		if (it != myEntries.end()) {
			myEndOffset = it->first;
		}
	}
}

void MobipocketHtmlBookReader::TOCReader::setEndOffset(std::size_t position) {
	myEndOffset = position;
}

const std::map<std::size_t,std::string> &MobipocketHtmlBookReader::TOCReader::entries() const {
	return myEntries;
}

void MobipocketHtmlHrefTagAction::run(const HtmlReader::HtmlTag &tag) {
	MobipocketHtmlBookReader &reader = (MobipocketHtmlBookReader&)myReader;
	if (tag.Start) {
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "FILEPOS") {
				const std::string &value = tag.Attributes[i].Value;
				if (!value.empty()) {
					std::string label = "&";
					int intValue = std::atoi(value.c_str());
					if (intValue > 0) {
						if (reader.myTocReader.rangeContainsPosition(tag.Offset)) {
							reader.myTocReader.startReadEntry(intValue);
							if (reader.myTocReader.rangeContainsPosition(intValue)) {
								reader.myTocReader.setEndOffset(intValue);
							}
						}
						reader.myFileposReferences.insert(intValue);
						ZLStringUtil::appendNumber(label, intValue);
						setHyperlinkType(INTERNAL_HYPERLINK);
						bookReader().addHyperlinkControl(INTERNAL_HYPERLINK, label);
						return;
					}
				}
			}
		}
	} else {
		reader.myTocReader.endReadEntry();
	}
	HtmlHrefTagAction::run(tag);
}

MobipocketHtmlGuideTagAction::MobipocketHtmlGuideTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlGuideTagAction::run(const HtmlReader::HtmlTag &tag) {
	MobipocketHtmlBookReader &reader = (MobipocketHtmlBookReader&)myReader;
	reader.myInsideGuide = tag.Start;
}

MobipocketHtmlReferenceTagAction::MobipocketHtmlReferenceTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlReferenceTagAction::run(const HtmlReader::HtmlTag &tag) {
	MobipocketHtmlBookReader &reader = (MobipocketHtmlBookReader&)myReader;
	if (reader.myInsideGuide) {
		std::string title;
		std::string filepos;
		bool isTocReference = false;
		for (std::size_t i = 0; i < tag.Attributes.size(); ++i) {
			const std::string &name = tag.Attributes[i].Name;
			const std::string &value = tag.Attributes[i].Value;
			if (name == "TITLE") {
				title = value;
			} else if (name == "FILEPOS") {
				filepos = value;
			} else if ((name == "TYPE") && (ZLUnicodeUtil::toUpper(value) == "TOC")) {
				isTocReference = true;
			}
		}
		if (!title.empty() && !filepos.empty()) {
			int position = std::atoi(filepos.c_str());
			if (position > 0) {
				reader.myTocReader.addReference(position, title);
				if (isTocReference) {
					reader.myTocReader.setStartOffset(position);
				}
			}
		}
	}
}

shared_ptr<HtmlTagAction> MobipocketHtmlBookReader::createAction(const std::string &tag) {
	if (tag == "IMG") {
		return new MobipocketHtmlImageTagAction(*this);
	} else if (tag == "HR") {
		return new MobipocketHtmlHrTagAction(*this);
	} else if (tag == "A") {
		return new MobipocketHtmlHrefTagAction(*this);
	} else if (tag == "GUIDE") {
		return new MobipocketHtmlGuideTagAction(*this);
	} else if (tag == "REFERENCE") {
		return new MobipocketHtmlReferenceTagAction(*this);
	} else if (tag == "MBP:PAGEBREAK") {
		return new MobipocketHtmlPagebreakTagAction(*this);
	}
	return HtmlBookReader::createAction(tag);
}

void MobipocketHtmlBookReader::startDocumentHandler() {
	HtmlBookReader::startDocumentHandler();
	myImageCounter = 0;
	myInsideGuide = false;
	myFileposReferences.clear();
	myPositionToParagraphMap.clear();
	myTocReader.reset();
}

bool MobipocketHtmlBookReader::tagHandler(const HtmlTag &tag) {
	std::size_t paragraphNumber = myBookReader.model().bookTextModel()->paragraphsNumber();
	if (myBookReader.paragraphIsOpen()) {
		--paragraphNumber;
	}
	myPositionToParagraphMap.push_back(std::make_pair(tag.Offset, paragraphNumber));
	return HtmlBookReader::tagHandler(tag);
}

MobipocketHtmlBookReader::MobipocketHtmlBookReader(const ZLFile &file, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlBookReader("", model, format, encoding), myFileName(file.path()), myTocReader(*this) {
	setBuildTableOfContent(false);
	setProcessPreTag(false);
}

bool MobipocketHtmlBookReader::characterDataHandler(const char *text, std::size_t len, bool convert) {
	myTocReader.appendText(text, len);
	return HtmlBookReader::characterDataHandler(text, len, convert);
}

void MobipocketHtmlBookReader::readDocument(ZLInputStream &stream) {
	HtmlBookReader::readDocument(stream);

	PalmDocStream &pdStream = (PalmDocStream&)stream;
	int index = pdStream.firstImageLocationIndex(myFileName);

	if (index >= 0) {
		for (int i = 0; i < myImageCounter; i++) {
			std::pair<int,int> imageLocation = pdStream.imageLocation(pdStream.header(), i + index);
			if ((imageLocation.first > 0) && (imageLocation.second > 0)) {
				std::string id;
				ZLStringUtil::appendNumber(id, i + 1);
				myBookReader.addImage(id, new ZLFileImage(ZLFile(myFileName), imageLocation.first, imageLocation.second));
			}
		}
	}

	std::vector<std::pair<std::size_t,std::size_t> >::const_iterator jt = myPositionToParagraphMap.begin();
	for (std::set<std::size_t>::const_iterator it = myFileposReferences.begin(); it != myFileposReferences.end(); ++it) {
		while (jt != myPositionToParagraphMap.end() && jt->first < *it) {
			++jt;
		}
		if (jt == myPositionToParagraphMap.end()) {
			break;
		}
		std::string label = "&";
		ZLStringUtil::appendNumber(label, *it);
		myBookReader.addHyperlinkLabel(label, jt->second);
	}

	jt = myPositionToParagraphMap.begin();
	const std::map<std::size_t,std::string> &entries = myTocReader.entries();
	for (std::map<std::size_t,std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
		while (jt != myPositionToParagraphMap.end() && jt->first < it->first) {
			++jt;
		}
		if (jt == myPositionToParagraphMap.end()) {
			break;
		}
		myBookReader.beginContentsParagraph(jt->second);
		myBookReader.addContentsData(it->second);
		myBookReader.endContentsParagraph();
	}
}
