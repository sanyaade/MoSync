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

/** 
* \file WidgetSkin.h 
* \brief Class for defining the visual appearance of a widget
* \author Patrick Broman and Niklas Nummelin
*/

#ifndef _SE_MSAB_MAUI_WIDGET_SKIN_H_
#define _SE_MSAB_MAUI_WIDGET_SKIN_H_

#include <ma.h>

//#include <MAUtil/Vector.h>
#include <MAUtil/HashMap.h>
//#include <MAUtil/Map.h>
//#define HashMap Map

using namespace MAUtil;

namespace MAUI {

	/** \brief A customizable skin for widgets.
	 * 
	 * A WidgetSkin is a class used to define the visual appearance
	 * of a widget. It is instantiated by passing an image , and two
	 * coordinates telling how to divide the image into 9 segments.
	 * These segments are then used as information of how to tile the
	 * skin to different widget sizes. The constructor also takes parameters
	 * telling if the image is transparent or not, used to
	 * optimize MAUI drawing.
	 **/
	class WidgetSkin {
	public:
		/**
		 * The default constructor.
		 */
		WidgetSkin();

		/**
		 * The WidgetSkin constructor takes one image
		 * and 2 coordinate pairs, defining the 9 segments of the image. Finally
		 * it takes one bool that specifies if the image is transparent or not
		 * (default true).
		 **/
		WidgetSkin(MAHandle image, int x1, int x2, int y1, int y2, bool transparent=true);

		/** 
		 * Set the start x coordinate of the WidgetSkin rect. 
		 **/
		void setStartX(int x);

		/** 
		 * Set the end x coordinate of the WidgetSkin rect. 
		 **/
		void setEndX(int x);

		/** 
		 * Set the start y coordinate of the WidgetSkin rect. 
		 **/
		void setStartY(int y);

		/** 
		 * Set the end y coordinate of the WidgetSkin rect. 
		 **/
		void setEndY(int y);

		/** 
		 * Get the start x coordinate of the WidgetSkin rect. 
		 **/
		int	getStartX() const;

		/** 
		 * Get the start y coordinate of the WidgetSkin rect. 
		 **/
		int getStartY() const;

		/** 
		 * Get the end x coordinate of the WidgetSkin rect. 
		 **/
		int getEndX() const;

		/** 
		 * Get the end y coordinate of the WidgetSkin rect. 
		 **/
		int getEndY() const;

		/** 
		 * Get the image used to display an unselected state. 
		 **/
		MAHandle getImage() const;

		/** 
		 * Set the image, returns false if there is an
		 * image and the dimensions of the image
		 * does not equal the image.
		 **/
		void setImage(MAHandle image);
		/**
		 * Use this to draw the WidgetSkin. The upper-left corner
		 * is placed at 'x' and 'y' and the skin is automatically
		 * resized to 'width' and 'height. 'type' specifies which
		 * drawing state should be used when drawing the widget skin.
		 **/
		
		/**
		 * Draws the widget skin, and if room exists in the cache, put the image in the cache. Next time the same
		 * WidgetSkin with the same dimensions is drawn, the cached bitmap is used.
		 */
		void draw(int x, int y, int width, int height);
		
		/**
		 * Draws the widget skin, not taking the cache in acount.
		 */
		void drawDirect(int x, int y, int width, int height);

		/**
		 * Draws the widget skin to a 32 bit argb buffer.
		 */
		void drawToData(int *data, int x, int y, int width, int height);

		/**
		 * Calculates the number of tiles needed for the widget skin drawn with dimension width and height.
		 */
		int calculateNumTiles(int width, int height);		

		/**
		 * Helper function that draws a region of data to an image resource.
		 */
		void drawRegion(MAHandle image, int* data, int scanLength, const MARect* srcRect, const MAPoint2d *dstPoint);
			
		/** 
		 * Get the image height.
		 **/
		int getImageHeight() const;

		/** 
		 * Get the image width.
		 **/
		int getImageWidth() const;

		/** 
		 * Returns whether the image is transparent or not.
		 **/
		bool isTransparent() const;

/** 
* \brief Key to cache element for skins
*/
		struct CacheKey {
			CacheKey() {
			}
			CacheKey(WidgetSkin* s, int width, int height) : skin(s), w(width), h(height) {
			}
			
			bool operator==(const CacheKey& c) const {
				return (skin==c.skin && w==c.w && h==c.h);
			}
			
			bool operator<(const CacheKey& c) const {
				return (skin<c.skin && w<c.w && h<c.h);
			}
					
			WidgetSkin *skin;
			int w, h;
		};		
	
/** 
* \brief Cache element for skins
*/

		struct CacheElement {
			/*
			// key
			int w, h;
			eType type;
			*/
			
			// value
			MAHandle image;
			
			// timestamp
			int lastUsed;
		};		
		
		// in pixels.
		static void setMaxCacheSize(int c);
		static void setCacheEnabled(bool e=true);

		static void flushCache();		
		static void flushCacheUntilNewImageFits(int numPixels); 
		static void addToCache(const CacheKey& key, const CacheElement& elem);
		static MAHandle getFromCache(const CacheKey& key);
		
	private:
		void rebuildRects();

		static int sMaxCacheSize;
		static bool sUseCache;
		
		//Vector<CacheElement> cache;
		static HashMap<CacheKey, CacheElement> sCache;

		int mImageWidth;
		int mImageHeight;
		MAHandle mImage;
		MARect mTopLeft;
		MARect mTop;
		MARect mTopRight;
		MARect mLeft;
		MARect mCenter;
		MARect mRight;
		MARect mBottomLeft;
		MARect mBottom;
		MARect mBottomRight;
		int mStartX, mEndX;
		int mStartY, mEndY;
		bool mTransparent;

	};	
}

#endif
