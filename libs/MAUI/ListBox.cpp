/* Copyright (C) 2009 Mobile Sorcery AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

#include <ma.h>
#include <conprint.h>
#include "ListBox.h"
#include "Label.h"
#include "Image.h"
#include "Engine.h"
#include <maxtoa.h>
#include <mastdlib.h>
#include <mavsprintf.h>
#include <madmath.h>

#include <MAUtil/Graphics.h>
#include "Screen.h"

#define FRAMES 1000
#define FPS 50
#define MS_PER_FRAME (1000/FPS)
#define DURATION 250

#define SHORT_PRESS_TIME 150

namespace MAUI {

	class ShortPressTrigger : public TimerListener {
	public:
		ShortPressTrigger(ListBox* listBox, int time) :  mListBox(listBox), mTime(time) {
		}

		void start() {
			Environment::getEnvironment().addTimer(this,mTime, 1);
		}

		void stop() {
			Environment::getEnvironment().removeTimer(this);
		}

		void runTimerEvent() {
			//Screen::getCurrentScreen()->getCurrentScreen()->setFocusedWidget(w);
			//w->pointerPressed(p, 0);
			mListBox->setFocusedWidget(mWidget);
			mWidget->pointerPressed(mPoint, 0);
		}

		void setWidget(Widget* w) {
			mWidget = w;
		}

		void setPoint(MAPoint2d p) {
			mPoint = p;
		}

	private:
		MAPoint2d mPoint;
		Widget* mWidget;
		ListBox *mListBox;
		int mTime;
	};

	ListBox::ListBox(int x, int y, int width, int height, Widget *parent) : 
	Widget(x, y, width, height, parent),
		mMustRebuild(false),
		mWrapping(true),
		mAnimationType(LBA_NONE),
		mOrientation(LBO_HORIZONTAL),
		mYOffsetFrom(0),
		mYOffsetTo(0),
		mYOffset(0),
		mSelectedIndex(0),
		mAutoSize(false),
		mTouched(false),
		mFocusedWidget(NULL) {
		requestRepaint();
	}

	ListBox::ListBox(int x, int y, int width, int height, Widget *parent, ListBoxOrientation orientation, ListBoxAnimationType animationType, bool wrapping)
		: Widget(x, y, width, height, parent),
		mMustRebuild(false),
		mWrapping(wrapping),
		mAnimationType(animationType),
		mOrientation(orientation),
		mYOffsetFrom(0),
		mYOffsetTo(0),
		mYOffset(0),
		mSelectedIndex(0),
		mAutoSize(false),
		mTouched(false),
		mFocusedWidget(NULL) {
		requestRepaint();
	}

	void ListBox::setOrientation(ListBoxOrientation orientation) {
		this->mOrientation = orientation;
		//rebuild();
		mMustRebuild = true;
	}

	void ListBox::boundsChanged(Widget *widget, const Rect& bounds) {
		//rebuild();
		mMustRebuild = true;

	}

	void ListBox::rebuild() {
		Vector_each(Widget*, itr, mChildren) {
			(*itr)->removeWidgetListener(this);
		}
		if(mOrientation == LBO_VERTICAL) {
			int size = mChildren.size();
			for(int i = 0; i < size; i++) {
				int y;
				if(i>0)
					y = mChildren[i-1]->getPosition().y +
						mChildren[i-1]->getBounds().height;
				else
					y = 0;
				mChildren[i]->setPosition(0, y);

				if(mAutoSize)
					mChildren[i]->setWidth(mPaddedBounds.width);

			}
		} else {
			int size = mChildren.size();
			for(int i = 0; i < size; i++) {
				int x;
				if(i>0)
					x = mChildren[i-1]->getPosition().x +
						mChildren[i-1]->getBounds().width;
				else
					x = 0;
				mChildren[i]->setPosition(x, 0);

				if(mAutoSize)
					mChildren[i]->setHeight(mPaddedBounds.height);
			}
		}
		Vector_each(Widget*, itr, mChildren) {
			(*itr)->addWidgetListener(this);
		}

		mMustRebuild = false;
	}

	void ListBox::setAutoSize(bool as) {
		this->mAutoSize = as;
		//rebuild();
		mMustRebuild = true;
	}

	void ListBox::add(Widget *child) {
		int size = mChildren.size();

		if(mOrientation == LBO_VERTICAL) {
			int y;
			if(size>0)
				y = mChildren[size-1]->getPosition().y +
					mChildren[size-1]->getBounds().height;
			else
				y = 0;

			Widget::add(child);
			child->setPosition(0, y);
			if(mAutoSize)
				mChildren[mChildren.size()-1]->setWidth(mPaddedBounds.width);
		}
		else if(mOrientation == LBO_HORIZONTAL) {
			int x;
			if(size>0)
				x = mChildren[size-1]->getPosition().x +
					mChildren[size-1]->getBounds().width;
			else
				x = 0;

			Widget::add(child);
			child->setPosition(x, 0);
			if(mAutoSize)
				mChildren[mChildren.size()-1]->setHeight(mPaddedBounds.height);
		}
		
		//listen
		child->addWidgetListener(this);

		if(mSelectedIndex>mChildren.size()) {
			mSelectedIndex = 0;
			mYOffset = 0;
			/*
			Vector_each(ItemSelectedListener*, i, itemSelectedListeners) {
				(*i)->itemSelected(this, mChildren[this->mSelectedIndex], NULL);
			}
			*/
		}
		if(mChildren.size() == 1)
			child->setFocused(true);

		requestRepaint();
	}

	void ListBox::clear() {
		for(int i = 0; i < mChildren.size(); i++)
			mChildren[i]->removeWidgetListener(this);
		Widget::clear();
		mMustRebuild = true;
		requestRepaint();
	}

	bool ListBox::listFrontOutsideBounds() const {
		switch(mOrientation) {
			case LBO_VERTICAL:
			for(int i = 0; i < mChildren.size(); i++) {
				int y = (mChildren[i]->getPosition().y + (mYOffset>>16));
				if(y < 0) {
					return true;
				}
			}
			break;
			case LBO_HORIZONTAL:
			for(int i = 0; i < mChildren.size(); i++) {
				int x = (mChildren[i]->getPosition().x + (mYOffset>>16));
				if(x < 0) {
					return true;
				}
			}
			break;
		}
		return false;
	}

	bool ListBox::listBackOutsideBounds() const {
		switch(mOrientation) {
			case LBO_VERTICAL:
			for(int i = 0; i < mChildren.size(); i++) {
				int y = (mChildren[i]->getPosition().y + mChildren[i]->getHeight() + (mYOffset>>16));
				if(y > mPaddedBounds.height) {
					return true;
				}
			}
			break;
			case LBO_HORIZONTAL:
			for(int i = 0; i < mChildren.size(); i++) {
				int x = (mChildren[i]->getPosition().x + mChildren[i]->getWidth() + (mYOffset>>16));
				if(x > mPaddedBounds.width) {
					return true;
				}
			}
			break;
		}
		return false;
	}

	int ListBox::getScrollOffset() const {
		return mYOffset>>16;
	}

	void ListBox::update() {
		Widget::update();	
		if(mMustRebuild) rebuild();
	}

#if 0
	static void drawRect(int x1, int y1, int x2, int y2, int col) {
		maSetColor(col);
		Gfx_line(x1, y1, x2, y1);
		Gfx_line(x2, y1, x2, y2);
		Gfx_line(x1, y1, x1, y2);
		Gfx_line(x1, y2, x2, y2);
	}
#endif

	void ListBox::draw(bool forceDraw) {
		//Engine& engine = Engine::getSingleton();

		if(mOrientation == LBO_VERTICAL) {
		//	int x = mPaddedBounds.x;
		//	int y = mPaddedBounds.y+(mYOffset>>16);

			int i = 0;
			//int cy = (mYOffset>>16);
			//int startIndex = 0;
			//int size = mChildren.size();
			//int endIndex = size;

			//printf("numWidgets: %d\n", size);

			//bool res = engine.pushClipRectIntersect(bounds.x, bounds.y,
			//bounds.width, bounds.height);
			Gfx_pushMatrix();
			Gfx_translate(mRelX, mRelY);
				
			BOOL res = Gfx_intersectClipRect(0, 0, mBounds.width, mBounds.height);

			if(res) {
				if(isDirty() || forceDraw) {
					drawBackground();
				}

				//bool res = engine.pushClipRectIntersect(mPaddedBounds.x, mPaddedBounds.y,
				//mPaddedBounds.width, mPaddedBounds.height);
				Gfx_translate(mPaddingLeft, mPaddingTop);
				res = Gfx_intersectClipRect(0, 0, mPaddedBounds.width, mPaddedBounds.height);

				MAPoint2d tBefore = Gfx_getTranslation();
				Gfx_translate(0, (mYOffset>>16));
				MAPoint2d translation = Gfx_getTranslation();
				
				if(res) 
				{	
					for(i = 0; i < mChildren.size(); i++)
					{
						/**
						 * The check whether the child should be drawn or
						 * not is done in Widget::draw(), and we postpone
						 * the check until then.
						 *
						 * // Mattias
						 */
						mChildren[i]->draw();
					}
				}

				//engine.popClipRect();
				Gfx_popClipRect();
			}
			setDirty(false);

			//engine.popClipRect();
			Gfx_popMatrix();
			Gfx_popClipRect();
		} else if(mOrientation == LBO_HORIZONTAL) {
			//int x = mPaddedBounds.x+(mYOffset>>16);
			//int y = mPaddedBounds.y;

			int i = 0;
			//int cx = (mYOffset>>16);
			//int startIndex = 0;
			//int size = mChildren.size();
			//int endIndex = size;

			//printf("numWidgets: %d\n", size);

			//bool res = engine.pushClipRectIntersect(bounds.x, bounds.y, bounds.width, bounds.height);	
			Gfx_pushMatrix();	
			Gfx_translate(mRelX, mRelY);
			BOOL res = Gfx_intersectClipRect(0, 0, mBounds.width, mBounds.height);

			if(res) 
			{
				if(isDirty() || forceDraw) {
					drawBackground();
				}
	
				//bool res = engine.pushClipRectIntersect(mPaddedBounds.x, mPaddedBounds.y,
				//mPaddedBounds.width, mPaddedBounds.height);
				Gfx_translate(mPaddingLeft, mPaddingTop);
				res = Gfx_intersectClipRect(0, 0, mPaddedBounds.width, mPaddedBounds.height);

				MAPoint2d tBefore = Gfx_getTranslation();
				Gfx_translate((mYOffset>>16), 0);
				MAPoint2d translation = Gfx_getTranslation();
				if(res) 
				{
					for(i = 0; i < mChildren.size(); i++)
					{
						/**
						 * The check whether the child should be drawn or
						 * not is done in Widget::draw(), and we postpone
						 * the check until then.
						 *
						 * // Mattias
						 */
						mChildren[i]->draw();
					}
				}
				setDirty(false);

				//engine.popClipRect();
				Gfx_popClipRect();
			}
	
			Gfx_popMatrix();
			//engine.popClipRect();
			Gfx_popClipRect();
		}

	}

	void ListBox::addItemSelectedListener(ItemSelectedListener *listener) {
		mItemSelectedListeners.add(listener);
	}

	void ListBox::setSelectedIndex(int selectedIndex) {
		if(selectedIndex < 0 || selectedIndex >= mChildren.size()) {
			maPanic(0, "ListBox::setSelectedIndex, index out of bounds");
		}

		if(selectedIndex == this->mSelectedIndex) {
			Vector_each(ItemSelectedListener*, i, mItemSelectedListeners) {
				(*i)->itemSelected(this, mChildren[this->mSelectedIndex], mChildren[this->mSelectedIndex]);
			}
			return;
		}

		Widget *unselectedWidget = mChildren[this->mSelectedIndex];
		unselectedWidget->setFocused(false);
		int lastIndex = this->mSelectedIndex;
		int offset = (lastIndex-selectedIndex);
		if(offset>0) {
			while(offset--) {
				selectPreviousItem(false);
			}
		} else {
			offset=-offset;
			while(offset--){
				selectNextItem(false);
			}
		}
		Widget *selectedWidget = mChildren[this->mSelectedIndex];
		selectedWidget->setFocused(true);

		Vector_each(ItemSelectedListener*, i, mItemSelectedListeners) {
			(*i)->itemSelected(this, selectedWidget, unselectedWidget);
		}

		requestRepaint();
	}

	int ListBox::getSelectedIndex() const {
		return mSelectedIndex;
	}

	void ListBox::setAnimationType(ListBoxAnimationType type) {
		mAnimationType = type;
	}

	void ListBox::setWrapping(bool mode) {
		mWrapping = mode;
	}

	ListBox::ListBoxAnimationType ListBox::getAnimationType() const {
		return mAnimationType;
	}

	bool	ListBox::isWrapping() const {
		return mWrapping;
	}

	ListBox::~ListBox() {
	}

	void ListBox::selectPreviousItem(bool shouldFireListeners) {
		Widget *c;
		if(!getChildren().size()) return;

		int pos = 0;

		int prevIndex = mSelectedIndex;

		mSelectedIndex--;

		if(mSelectedIndex < 0) {
			mSelectedIndex = 0;
			if(!mWrapping) {
				if(shouldFireListeners) {
					Vector_each(ItemSelectedListener*, i, mItemSelectedListeners) {
						(*i)->blocked(this, -1);
					}
				}
			} else {
				setSelectedIndex(mChildren.size()-1);
			}
			return;
		}
		//printf("element: %d\n", selectedIndex);

		Widget *unselectedWidget = mChildren[prevIndex];
		mChildren[prevIndex]->setFocused(false);

		c = mChildren[mSelectedIndex];

		c->setFocused(true);

		switch(mOrientation) {
			case LBO_VERTICAL: pos = (c->getPosition().y<<16); break;
			case LBO_HORIZONTAL: pos = (c->getPosition().x<<16); break;
		}
		if((pos + mYOffset) < 0) {
			mYOffsetFrom = mYOffset>>16;
			mYOffset -= pos + mYOffset;
			mYOffsetTo = mYOffset>>16;
			mYOffset = mYOffsetFrom<<16;
			mAnimTimeStart = maGetMilliSecondCount();
			switch(mAnimationType) {
				case LBA_LINEAR:
					Environment::getEnvironment().addTimer(this, MS_PER_FRAME, FRAMES+1);
					mYOffsetInc = ((mYOffsetTo - mYOffsetFrom)<<16)/FRAMES;
					break;
				case LBA_NONE:
					mYOffset = mYOffsetTo<<16;
					requestRepaint();
					break;
			}
		}

		if(shouldFireListeners) {
			Vector_each(ItemSelectedListener*, i, mItemSelectedListeners) {
				(*i)->itemSelected(this, c, unselectedWidget);
			}
		}

		requestRepaint();
	}

	void ListBox::selectNextItem(bool shouldFireListeners) {
		Widget *c;
		if(!getChildren().size()) return;

		int prevIndex = mSelectedIndex;

		if(mSelectedIndex < mChildren.size() - 1) {
			mSelectedIndex++;
		} else {
			if(!mWrapping) {
				if(shouldFireListeners) {
					Vector_each(ItemSelectedListener*, i, mItemSelectedListeners) {
						(*i)->blocked(this, 1);
					}
				}
			} else {
				setSelectedIndex(0);
			}
			return;
		}

		Widget *unselectedWidget = mChildren[prevIndex];
		mChildren[prevIndex]->setFocused(false);

		c = mChildren[mSelectedIndex];

		c->setFocused(true);

		//printf("element: %d\n", selectedIndex);

		int pos = 0;
		int bound = 0;
		int newPos = 0;
		switch(mOrientation) {
			case LBO_VERTICAL:
				newPos = c->getPosition().y<<16;
				pos = ((c->getPosition().y + c->getHeight())<<16);
				bound =  (mPaddedBounds.height<<16);
				break;
			case LBO_HORIZONTAL:
				newPos = c->getPosition().x<<16;
				pos = ((c->getPosition().x + c->getWidth())<<16);
				bound =  (mPaddedBounds.width<<16);
				break;
		}

		if( pos + mYOffset > bound) {
			mYOffsetFrom = mYOffset>>16;
			mYOffset -= (pos + mYOffset) - bound;
			mYOffsetTo = mYOffset>>16;
			mYOffset = mYOffsetFrom<<16;
			mAnimTimeStart = maGetMilliSecondCount();
			switch(mAnimationType) {
				case LBA_LINEAR:
					Environment::getEnvironment().addTimer(this, MS_PER_FRAME, FRAMES+1);
					mYOffsetInc = ((mYOffsetTo - mYOffsetFrom)<<16)/FRAMES;
					break;
				case LBA_NONE:
					mYOffset = mYOffsetTo<<16;
					requestRepaint();
					break;
			}
		}

		if(shouldFireListeners) {
			Vector_each(ItemSelectedListener*, i, mItemSelectedListeners) {
				(*i)->itemSelected(this, c, unselectedWidget);
			}
		}

		requestRepaint();
	}

#define ABS_IS_LESS(x, y) (fabs(x)<(y))
	void ListBox::runTimerEvent()
	{
		//mYOffset += mYOffsetInc;
		if(mTouched) {
			if(mOrientation == LBO_HORIZONTAL) {
				setScrollOffset((mYOffset + (int)(mTouchVelX*65536.0*20.0))>>16);
				mTouchVelX*=0.966;
				if(ABS_IS_LESS(mTouchVelX,0.001)) Environment::getEnvironment().removeTimer(this);
			} else {
				setScrollOffset((mYOffset + (int)(mTouchVelY*65536.0*20.0))>>16);
				mTouchVelY*=0.966;
				if(ABS_IS_LESS(mTouchVelY,0.001)) Environment::getEnvironment().removeTimer(this);
			}

		} else {
			mYOffset = (mYOffsetFrom<<16) + (mYOffsetTo-mYOffsetFrom)*(((maGetMilliSecondCount()-mAnimTimeStart)<<16)/DURATION);
			if(mYOffsetInc<0 && mYOffset<=mYOffsetTo<<16) {
				mYOffset = mYOffsetTo<<16;
				Environment::getEnvironment().removeTimer(this);
			}
			else if(mYOffsetInc>0 && mYOffset>=mYOffsetTo<<16) {
				mYOffset = mYOffsetTo<<16;
				Environment::getEnvironment().removeTimer(this);
			}
		}


		requestRepaint();
	}

	void ListBox::drawWidget() {
	}

	void ListBox::setWidth(int w) {
		Widget::setWidth(w);
		if(mAutoSize) {
			if(mOrientation == LBO_VERTICAL) {
				for(int i = 0; i < mChildren.size(); i++) {
					mChildren[i]->setWidth(mPaddedBounds.width);
				}
			}
		}
	}

	void ListBox::setHeight(int h) {
		Widget::setHeight(h);
		if(mAutoSize) {
			if(mOrientation == LBO_HORIZONTAL) {
				for(int i = 0; i < mChildren.size(); i++) {
					mChildren[i]->setHeight(mPaddedBounds.height);
				}
			}
		}
	}

	bool ListBox::isTransparent() const {
		return true;
	}


	bool ListBox::isFocusable() const {
		return true;
	}

	void ListBox::setScrollOffset(int ofs) {
		if (mChildren.size() == 0 || ofs > 0) {
			ofs = 0;
			mYOffset = 0;
			requestRepaint();
			return;
		}

		Widget* lastChild = mChildren[mChildren.size() - 1];
		int bound = (lastChild->getPosition().y + lastChild->getBounds().height)
				- this->getBounds().height;
		if(bound<0) bound = 0;
		if (ofs < -bound) {
			ofs = -bound;
		}

		mYOffset = ofs << 16;
		requestRepaint();
	}

	void ListBox::setFocusedWidget(Widget *w) {
		mFocusedWidget = w;
		w->setFocused(true);
	}

	bool ListBox::pointerPressed(MAPoint2d p, int id) {
		Environment::getEnvironment().removeTimer(this);
		mTouchMotionTracker.reset();
		mTouchMotionTracker.addPoint(p);

		int x = (mOrientation==LBO_HORIZONTAL)?(p.x-(mYOffset>>16)):p.x;
		int y = (mOrientation==LBO_VERTICAL)?(p.y-(mYOffset>>16)):p.y;
		Vector_each(Widget *, it, mChildren) {
			Widget *ret = (*it)->focusableWidgetAt(x, y);
			if(ret) {
				if(ret->pointerPressed(p, id)) {
					setFocusedWidget(ret);
				}
				break;
			}
		}
		return false;
	}

	bool ListBox::pointerMoved(MAPoint2d p, int id) {
		MAUI_LOG("Got event %d, %d", p.x, p.y);
		if(mFocusedWidget) {
			if(!mFocusedWidget->pointerMoved(p, id)) {
				mFocusedWidget->setFocused(false);
				mFocusedWidget = NULL;
			}
			return false;
		}
		if(id==0) {
			int relX, relY;
			mTouchMotionTracker.addPoint(p, relX, relY);
			if(relX==0 && relY == 0) return false;

			if(mOrientation == LBO_VERTICAL)
				setScrollOffset((mYOffset>>16)+relY);
			else
				setScrollOffset((mYOffset>>16)+relX);


			mTouchMotionTracker.calculateVelocity(mTouchDirX, mTouchDirY, mTouchVelX, mTouchVelY);
		}
		return false;
	}

	bool ListBox::pointerReleased(MAPoint2d p, int id) {
		if(mFocusedWidget) {
			mFocusedWidget->pointerReleased(p, id);
			mFocusedWidget->setFocused(false);
			mFocusedWidget = NULL;
			return false;
		}

		if(id==0) {
			mTouched = true;
			mTouchMotionTracker.calculateVelocity(mTouchDirX, mTouchDirY, mTouchVelX, mTouchVelY);
			Environment::getEnvironment().addTimer(this, MS_PER_FRAME, -1);
		}
		return false;
	}

}
