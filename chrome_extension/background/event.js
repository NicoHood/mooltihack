
// Masquerade event var into a different variable name ( while event is not reserved, many websites use it and creates problems )
var mooltipassEvent = {};

// Keep event for backwards compatibility
var event = mooltipassEvent;

mooltipassEvent.onMessage = function(request, sender, callback) {
	console.log("onMessage(" + request.action + ")", request);

	if (request.action == 'content_script_loaded') {
		console.log('setting allLoaded to true ');
		page.allLoaded = true;
		callback({response: "response from background script"});
	}

	if (request.action in mooltipassEvent.messageHandlers) {

		if(!sender.hasOwnProperty('tab') || sender.tab.id < 1) {
			sender.tab = {};
			sender.tab.id = page.currentTabId;
		}

		mooltipassEvent.invoke(mooltipassEvent.messageHandlers[request.action], callback, sender.tab.id, request.args);

		// onMessage closes channel for callback automatically
		// if this method does not return true
		if(callback) {
			return true;
		}
	}
}

/**
 * Get interesting information about the given tab.
 * Function adapted from AdBlock-Plus.
 *
 * @param {function} handler to call after invoke
 * @param {function} callback to call after handler or null
 * @param {integer} senderTabId
 * @param {array} args
 * @param {bool} secondTime
 * @returns null (asynchronous)
 */
mooltipassEvent.invoke = function(handler, callback, senderTabId, args, secondTime) {
	if(senderTabId < 1) {
		return;
	}

	if(!page.tabs[senderTabId]) {
		page.createTabEntry(senderTabId);
	}

	// remove information from no longer existing tabs
	page.removePageInformationFromNotExistingTabs();

	chrome.tabs.get(senderTabId, function(tab) {
        if (chrome.runtime.lastError) {
            console.log('failed to invoke function for tab: '+chrome.runtime.lastError);
            return;
        }

	//chrome.tabs.query({"active": true, "windowId": chrome.windows.WINDOW_ID_CURRENT}, function(tabs) {
		//if (tabs.length === 0)
		//	return; // For example: only the background devtools or a popup are opened
		//var tab = tabs[0];

		if(!tab) {
			return;
		}

		if (!tab.url) {
			// Issue 6877: tab URL is not set directly after you opened a window
			// using window.open()
			if (!secondTime) {
				window.setTimeout(function() {
					mooltipassEvent.invoke(handler, callback, senderTabId, args, true);
				}, 250);
			}
			return;
		}

		if(!page.tabs[tab.id]) {
			page.createTabEntry(tab.id);
		}

		args = args || [];

		args.unshift(tab);
		args.unshift(callback);

		if(handler) {
			handler.apply(this, args);
		}
		else {
			console.log("undefined handler for tab " + tab.id);
		}
	});
}


mooltipassEvent.onShowAlert = function(callback, tab, message) {
	alert(message);
}

mooltipassEvent.onLoadSettings = function(callback, tab) {
	page.settings = (typeof(localStorage.settings) == 'undefined') ? {} : JSON.parse(localStorage.settings);
	if (isFirefox) page.settings.useMoolticute = true;
    mooltipass.backend.loadSettings();
    //console.log('onLoadSettings: page.settings = ', page.settings);
}

mooltipassEvent.onLoadKeyRing = function(callback, tab) {
    console.log('mooltipassEvent.onLoadKeyRing()');
}

mooltipassEvent.onGetSettings = function(callback, tab) {
	mooltipassEvent.onLoadSettings();
	var settings = page.settings;
	settings.status = mooltipass.device._status;
	callback({ data: settings });
}

mooltipassEvent.onSaveSettings = function(callback, tab, settings) {
	localStorage.settings = JSON.stringify(settings);
	mooltipassEvent.onLoadSettings();
}

mooltipassEvent.onGetStatus = function(callback, tab) {
    console.log('mooltipassEvent.onGetStatus()');

	browserAction.showDefault(null, tab);
    page.tabs[tab.id].errorMessage = undefined;  // XXX debug

	callback({
		status: mooltipass.device.getStatus(),
		error: page.tabs[tab.id].errorMessage
	});
}

mooltipassEvent.onPopStack = function(callback, tab) {
	browserAction.stackPop(tab.id);
	browserAction.show(null, tab);
}

mooltipassEvent.onGetTabInformation = function(callback, tab) {
	var id = tab.id || page.currentTabId;

	callback(page.tabs[id]);
}

mooltipassEvent.onGetConnectedDatabase = function(callback, tab) {
    console.log('mooltipassEvent.onGetConnectedDatabase()');
	callback({
		"count": 10,
		"identifier": 'my_mp_db_id'
	});
}

mooltipassEvent.onRemoveCredentialsFromTabInformation = function(callback, tab) {
	var id = tab.id || page.currentTabId;

	page.clearCredentials(id);
}

mooltipassEvent.onNotifyButtonClick = function(id, buttonIndex) 
{
    console.log('notification',id,'button',buttonIndex,'clicked');
	
	// Check the kind of notification
	if (id.indexOf('mpNotConnected') == 0 || id.indexOf('mpNotUnlocked') == 0)
	{
		console.log('Disabling not unlocked notifications');
		mooltipass.backend.disableNonUnlockedNotifications = true;
	}
	else
	{
		// Check notification type
		if(mooltipassEvent.mpUpdate[id].type == "singledomainadd")
		{
			// Adding a single domain notification
			if (buttonIndex == 0) 
			{
				// Blacklist
				console.log('notification blacklist ',mooltipassEvent.mpUpdate[id].url);
				mooltipass.backend.blacklistUrl(mooltipassEvent.mpUpdate[id].url);
			} 
			else 
			{
			}
		}
		else if(mooltipassEvent.mpUpdate[id].type == "subdomainadd")
		{
			// Adding a sub domain notification
			if (buttonIndex == 0) 
			{
				// Store credentials
				console.log('notification update with subdomain',mooltipassEvent.mpUpdate[id].username,'on',mooltipassEvent.mpUpdate[id].url);
				mooltipass.device.updateCredentials(null, mooltipassEvent.mpUpdate[id].tab, 0, mooltipassEvent.mpUpdate[id].username, mooltipassEvent.mpUpdate[id].password, mooltipassEvent.mpUpdate[id].url);
			} 
			else 
			{
				// Store credentials
				console.log('notification update',mooltipassEvent.mpUpdate[id].username,'on',mooltipassEvent.mpUpdate[id].url2);
				mooltipass.device.updateCredentials(null, mooltipassEvent.mpUpdate[id].tab, 0, mooltipassEvent.mpUpdate[id].username, mooltipassEvent.mpUpdate[id].password, mooltipassEvent.mpUpdate[id].url2);
			}
		}
		delete mooltipassEvent.mpUpdate[id];		
	}

	// Close notification
	chrome.notifications.clear(id);
}

mooltipassEvent.onNotifyClosed = function(id) {
    delete mooltipassEvent.mpUpdate[id];
}

chrome.notifications.onButtonClicked.addListener(mooltipassEvent.onNotifyButtonClick);
chrome.notifications.onClosed.addListener(mooltipassEvent.onNotifyClosed);

mooltipassEvent.notificationCount = 0;
mooltipassEvent.mpUpdate = {};

mooltipassEvent.isMooltipassUnlocked = function()
{
	// prevents "Failed to send to device: Transfer failed" error when device is suddenly unplugged
	if(typeof mooltipass.device._status.state == 'undefined'){
		return false;
	}

	// Don't show notifications right now
	console.log(mooltipass.device._status.state);

	// If the device is not connected and not unlocked and the user disabled the notifications, return
	if (mooltipass.device._status.state != 'Unlocked')
	{
		if (mooltipass.backend.disableNonUnlockedNotifications)
		{
			console.log('Not showing a notification as they are disabled');
			return false;
		}
	}

	// Increment notification count
	mooltipassEvent.notificationCount++;
	var noteId = 'mpNotUnlocked.'+mooltipassEvent.notificationCount.toString();

	// Check that the Mooltipass app is installed and enabled
	if (!mooltipass.device._app || mooltipass.device._app['enabled'] !== true)
	{
		console.log('notify: mooltipass app not ready');

		noteId = "mpNotUnlockedStaticMooltipassAppNotReady";

		// Create notification to inform user
		chrome.notifications.create(noteId,
			{   type: 'basic',
				title: 'Mooltipass App not ready!',
				message: 'The Mooltipass app is not installed or disabled',
				iconUrl: '/icons/warning_icon.png',
				buttons: [{title: 'Don\'t show these notifications', iconUrl: '/icons/forbidden-icon.png'}]});

		return false;
	}

	// Check that our device actually is connected
	if (mooltipass.device._status.state == 'NotConnected')
	{
		console.log('notify: device not connected');

		noteId = "mpNotUnlockedStaticMooltipassNotConnected";

		// Create notification to inform user
		chrome.notifications.create(noteId,
			{   type: 'basic',
				title: 'Mooltipass Not Connected!',
				message: 'Please Connect Your Mooltipass',
				iconUrl: '/icons/warning_icon.png',
				buttons: [{title: 'Don\'t show these notifications', iconUrl: '/icons/forbidden-icon.png'}]});

		return false;
	}
	else if (mooltipass.device._status.state == 'Locked')
	{
		console.log('notify: device locked');

		noteId = "mpNotUnlockedStaticMooltipassDeviceLocked";

		// Create notification to inform user
		chrome.notifications.create(noteId,
				{   type: 'basic',
					title: 'Mooltipass Locked!',
					message: 'Please Unlock Your Mooltipass',
					iconUrl: '/icons/warning_icon.png',
					buttons: [{title: 'Don\'t show these notifications', iconUrl: '/icons/forbidden-icon.png'}]});
					
		return false;
	}
	else if (mooltipass.device._status.state == 'NoCard')
	{
		console.log('notify: device without card');

		noteId = "mpNotUnlockedStaticMooltipassDeviceWithoutCard";

		// Create notification to inform user
		chrome.notifications.create(noteId,
				{   type: 'basic',
					title: 'No Card in Mooltipass!',
					message: 'Please Insert Your Smartcard and Enter Your PIN',
					iconUrl: '/icons/warning_icon.png',
					buttons: [{title: 'Don\'t show these notifications', iconUrl: '/icons/forbidden-icon.png'}]});
					
		return false;
	}
	else if (mooltipass.device._status.state == 'ManageMode')
	{
		console.log('notify: management mode');

		noteId = "mpNotUnlockedStaticMooltipassDeviceInManagementMode";

		var isFirefox = navigator.userAgent.toLowerCase().indexOf('firefox') > -1;
		var notification = {   
			type: 'basic',
			title: 'Mooltipass in Management Mode!',
			message: 'Please leave management mode in the App',
			iconUrl: '/icons/warning_icon.png'
		};

		if (!isFirefox) notification.buttons = [{title: 'Don\'t show these notifications', iconUrl: '/icons/forbidden-icon.png'}];

		// Create notification to inform user
		chrome.notifications.create(noteId,notification);
					
		return false;
	}
	
	return true;
}

mooltipassEvent.onUpdateNotify = function(callback, tab, username, password, url, usernameExists, credentialsList) 
{
	// Don't show notifications right now

	// Parse URL
	var parsed_url = mooltipass.backend.extractDomainAndSubdomain(url);
	var valid_url = false;
	var subdomain;
	var domain;

	console.log('onUpdateNotify', 'parsed_url', parsed_url);
	
	// See if our script detected a valid domain & subdomain
	if(parsed_url.valid == true)
	{
		valid_url = true;
		domain = parsed_url.domain;
		subdomain = parsed_url.subdomain;
	}
	
	// Check if URL is valid
	if(valid_url == true)
	{
		// Check if blacklisted
		if (mooltipass.backend.isBlacklisted(domain))
		{
			console.log('notify: ignoring blacklisted url',url);
			return;
		}
		
		// Check that the Mooltipass is unlocked
		var mp_unlocked = mooltipassEvent.isMooltipassUnlocked();
		
		// Increment notification count
		mooltipassEvent.notificationCount++;
		
		if(mp_unlocked && password.length > 31)
		{		
			var noteId = 'mpPasswordTooLong.'+ mooltipassEvent.notificationCount.toString();
			
			chrome.notifications.create(noteId,
				{   type: 'basic',
					title: 'Password Too Long!',
					message: "We are sorry, Mooltipass only supports passwords that are less than 31 characters",
					iconUrl: '/icons/warning_icon.png'});
			return;
		}

		if(subdomain == null)
		{
			// Single domain
			// Here we should send a request to the mooltipass to know if the username exists!
			if(true)
			{
				// Unknown user
				var noteId = 'mpUpdate.'+mooltipassEvent.notificationCount.toString();

				// Store our event
				mooltipassEvent.mpUpdate[noteId] = { tab: tab, username: username, password: password, url: domain, url2: domain, type: "singledomainadd"};
				
				// Send request by default
				mooltipass.device.updateCredentials(null, tab, 0, username, password, domain);

				// Create notification to blacklist
				if (mooltipass.device._status.unlocked) {
					chrome.notifications.create(noteId,
						{   type: 'basic',
							title: 'Credentials Detected!',
							message: 'Please Approve their Storage on the Mooltipass',
							iconUrl: '/icons/mooltipass-128.png',
							buttons: [{title: 'Black list this website', iconUrl: '/icons/forbidden-icon.png'}] },
							function(id) 
							{
								console.log('notification created for',id);
							});
				}
			}
			else
			{}
		}
		else
		{
			// Subdomain exists
			// Here we should send a request to the mooltipass to know if the username exists!
			
			// first let's check to make sure the device is connected
			if(mooltipass.device._status.state == 'NotConnected')
			{
				console.log('mooltipass not connected - do not ask which domain to store');
			}
			else{

				// Unknown user
				var noteId = 'mpUpdate.'+mooltipassEvent.notificationCount.toString();

				// Store our event
				mooltipassEvent.mpUpdate[noteId] = { tab: tab, username: username, password: password, url: domain, url2: subdomain + "." + domain, type: "subdomainadd"};

				var isFirefox = navigator.userAgent.toLowerCase().indexOf('firefox') > -1;
				var notification = {   
					type: 'basic',
					title: 'Subdomain Detected!',
					message: 'What domain do you want to store?',
					iconUrl: '/icons/question.png',
				};

				// Firefox doesn't support buttons on notifications
				if (!isFirefox) {
					notification.buttons = [{title: 'Store ' + domain}, {title: 'Store ' + subdomain + '.' + domain}];
				} else {
					// Firefox: Use domain (we should check against subdomain and later domain if missing tho...)
					notification.message = 'Please approve Domain storage';
					mooltipass.device.updateCredentials(null, tab, 0, username, password, subdomain + '.' + domain);
				}

				chrome.notifications.create(noteId,notification);
			}
		}		
	}	
}

mooltipassEvent.onUpdate = function(callback, tab, username, password, url, usernameExists, credentialsList) {
    mooltipass.device.updateCredentials(callback, tab, 0, username, password, url);
}

mooltipassEvent.onLoginPopup = function(callback, tab, logins) {
	var stackData = {
		level: 1,
		iconType: "questionmark",
		popup: "popup_login.html"
	}
	browserAction.stackUnshift(stackData, tab.id);

	page.tabs[tab.id].loginList = logins;

	browserAction.show(null, tab);
}

mooltipassEvent.onHTTPAuthPopup = function(callback, tab, data) {
	var stackData = {
		level: 1,
		iconType: "questionmark",
		popup: "popup_httpauth.html"
	}
	browserAction.stackUnshift(stackData, tab.id);

	page.tabs[tab.id].loginList = data;

	browserAction.show(null, tab);
}

mooltipassEvent.onMultipleFieldsPopup = function(callback, tab) {
	var stackData = {
		level: 1,
		iconType: "normal",
		popup: "popup_multiple-fields.html"
	}
	browserAction.stackUnshift(stackData, tab.id);

	browserAction.show(null, tab);
}

// all methods named in this object have to be declared BEFORE this!
mooltipassEvent.messageHandlers = {
	'update': mooltipassEvent.onUpdate,
	'add_credentials': mooltipass.device.addCredentials,
	'blacklist_url': mooltipass.backend.handlerBlacklistUrl,
	'blacklistUrl': mooltipass.backend.blacklistUrl,
	'alert': mooltipassEvent.onShowAlert,
	'get_connected_database': mooltipassEvent.onGetConnectedDatabase,
	'get_settings': mooltipassEvent.onGetSettings,
	'get_status': mooltipassEvent.onGetStatus,
	'get_tab_information': mooltipassEvent.onGetTabInformation,
	'load_keyring': mooltipassEvent.onLoadKeyRing,
	'load_settings': mooltipassEvent.onLoadSettings,
	'pop_stack': mooltipassEvent.onPopStack,
	'popup_login': mooltipassEvent.onLoginPopup,
	'popup_multiple-fields': mooltipassEvent.onMultipleFieldsPopup,
	'remove_credentials_from_tab_information': mooltipassEvent.onRemoveCredentialsFromTabInformation,
	'retrieve_credentials': mooltipass.device.retrieveCredentials,
	'show_default_browseraction': browserAction.showDefault,
	'update_credentials': mooltipass.device.updateCredentials,
	'save_settings': mooltipassEvent.onSaveSettings,
	'update_notify': mooltipassEvent.onUpdateNotify,
	'stack_add': browserAction.stackAdd,
	'generate_password': mooltipass.device.generatePassword,
    'set_current_tab': page.setCurrentTab,
};
