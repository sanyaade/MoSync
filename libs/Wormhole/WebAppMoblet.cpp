/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

/**
 * @file WebAppMoblet.cpp
 * @author Mikael Kindborg
 *
 * @brief High-level moblet that has a WebView and supports
 * communication between a JavaScript and C++.
 */

#include <conprint.h>
#include <NativeUI/WebViewListener.h>
#include "WebAppMoblet.h"

namespace Wormhole
{
	/**
	 * Class that listens for WebView events.
	 * Here we receive messages from JavaScript.
	 */
	class WebAppMoblet_WebViewListener : public NativeUI::WebViewListener
	{
	public:
		/**
		 * Constructor that saves the pointer to the moblet.
		 */
		WebAppMoblet_WebViewListener(WebAppMoblet* moblet)
		{
			mMoblet = moblet;
		}

		/**
		 * This method is called when a "mosync://" url is
		 * invoked in the WebView.
		 *
		 * Note: The urlData is deallocated automatically
		 * by the WebView after this call finishes.
		 */
		virtual void webViewHookInvoked(
			NativeUI::WebView* webView,
			int hookType,
			MAHandle urlData)
		{
			// Note: urlData is deallocated automatically by
			// the framework, we should not deallocate it here.

			mMoblet->handleWebViewMessage(webView, urlData);
		}

	private:
		/**
		 * Pointer to the moblet.
		 */
		WebAppMoblet* mMoblet;
	};
	// End of class WebAppMoblet_WebViewListener

	/**
	 * Constructor.
	 */
	WebAppMoblet::WebAppMoblet()
	{
		extractFileSystem();
		createUI();
	}

	/**
	 * Destructor.
	 */
	WebAppMoblet::~WebAppMoblet()
	{
		// Deleting the root widget will also delete child widgets.
		delete mScreen;

		// Delete the WebView listener.
		if (NULL != mWebViewListener)
		{
			delete mWebViewListener;
			mWebViewListener = NULL;
		}

		// Delete the file utility object.
		delete mFileUtil;
	}

	/**
	 * Get the WebView widget displayed by this moblet.
	 * @return Pointer to a WebView instance.
	 */
	NativeUI::WebView* WebAppMoblet::getWebView()
	{
		return mWebView;
	}

	/**
	 * Get a file utility object used for accessing the
	 * device's local file system.
	 * @return Pointer to a FileUtil instance.
	 */
	FileUtil* WebAppMoblet::getFileUtil()
	{
		return mFileUtil;
	}

	/**
	 * Enable JavaScript to C++ communication.
	 */
	void WebAppMoblet::enableWebViewMessages()
	{
		if (NULL == mWebViewListener)
		{
			mWebViewListener = new WebAppMoblet_WebViewListener(this);
			mWebView->addWebViewListener(mWebViewListener);
			mWebView->enableWebViewMessages();
		}
	}

	/**
	 * Disable JavaScript to C++ communication.
	 */
	void WebAppMoblet::disableWebViewMessages()
	{
		if (NULL != mWebViewListener)
		{
			mWebView->removeWebViewListener(mWebViewListener);
			mWebView->disableWebViewMessages();
			delete mWebViewListener;
			mWebViewListener = NULL;
		}
	}

	/**
	 * Display a page in the WebView of this moblet.
	 * @param url Url of page to open.
	 */
	void WebAppMoblet::showPage(const MAUtil::String& url)
	{
		mWebView->openURL(url);
	}

	/**
	 * Run JavaScript code in the WebView.
	 */
	void WebAppMoblet::callJS(const MAUtil::String& script)
	{
		mWebView->callJS(script);
	}

	/**
	 * This method handles messages sent from the WebView.
	 * Implement this method in a subclass of this class.
	 * @param webView The WebView that sent the message.
	 * @param urlData Data object that holds message content.
	 * Note that the data object will be valid only during
	 * the life-time of the call of this method, then it
	 * will be deallocated.
	 */
	void WebAppMoblet::handleWebViewMessage(
		NativeUI::WebView* webView,
		MAHandle urlData)
	{
	}

	/**
	 * This method is called when a key is pressed. It closes
	 * the application when the back key (on Android) is pressed.
	 * Override to implement your own behaviour.
	 */
	void WebAppMoblet::keyPressEvent(int keyCode, int nativeCode)
	{
		if (MAK_BACK == keyCode)
		{
			// Call close to exit the application.
			close();
		}
	}

	/**
	 * Extract HTML/CSS/JS/Media files to the local file system.
	 */
	void WebAppMoblet::extractFileSystem()
	{
		// Create file utility object.
		mFileUtil = new FileUtil();

		// Extract bundled files to the local file system.
		mFileUtil->extractLocalFiles();
	}

	/**
	 * Create the user interface of the application.
	 * This creates a full screen WebView and configures
	 * it to receive messages from JavaScript.
	 */
	void WebAppMoblet::createUI()
	{
		// Create and configure the WebView.
		mWebView = new NativeUI::WebView();
		mWebView->fillSpaceHorizontally();
		mWebView->fillSpaceVertically();

		// Create and show the screen that holds the WebView.
		mScreen = new NativeUI::Screen();
		mScreen->setMainWidget(mWebView);
		mScreen->show();
	}

} // namespace
