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

#ifndef __ZLQTITEMSLISTWIDGET_H__
#define __ZLQTITEMSLISTWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QFrame>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>

#include <ZLTreeTitledNode.h>

#include "QtWaitingSpinner.h"

class ZLQtTreeItem;

class ZLQtItemsListWidget : public QScrollArea {
	Q_OBJECT
public:
	ZLQtItemsListWidget(QWidget *parent = 0);
	void fillNodes(const ZLTreeNode *rootNode);
	void fillNewNodes(const ZLTreeNode *rootNode);
	QSize sizeHint() const;
	void setMinimumWidth(int w);
	QList<ZLQtTreeItem*> getItems() const;


Q_SIGNALS:
	void nodeClicked(ZLQtTreeItem* item);
	void nodeDoubleClicked(ZLQtTreeItem* item);
	void wantMoreChildren();

public Q_SLOTS:
	void onNodeClicked(ZLQtTreeItem* item);
	void onSliderMoved(int value);

private:
	void clear();
	void addNode(ZLTreeTitledNode *node);

private:
	QWidget *myContainerWidget;
	QVBoxLayout *myLayout;
	QList<ZLQtTreeItem*> myItems;
};

class QtWaitingSpinner;

class ZLQtTreeItem : public QFrame {
	Q_OBJECT

public:
	ZLQtTreeItem(QWidget *parent = 0);
	void fill(ZLTreeTitledNode *node);
	void setActive(bool active);
	bool isActive() const;

public:
	ZLTreeTitledNode *getNode() const;
	QtWaitingSpinner *getWaitingIcon() const;

Q_SIGNALS:
	void clicked(ZLQtTreeItem* item);
	void doubleClicked(ZLQtTreeItem* item);

protected:
	 void mousePressEvent(QMouseEvent *event);
	 void mouseDoubleClickEvent(QMouseEvent *event);
	 void paintEvent(QPaintEvent *event);

protected:
	 void fillImage();
	 void clear();

private:
	 ZLTreeTitledNode *myNode;

	 QLabel *myIcon;
	 QLabel *myTitle;
	 QLabel *mySubtitle;
	 QtWaitingSpinner *myWaitingIcon;
	 bool myImageRequested;

	 bool myIsActive;
};

#endif /* __ZLQTITEMSLISTWIDGET_H__ */
