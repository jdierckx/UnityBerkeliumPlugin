// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// Part of this source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Contributors (Unity forum usernames): reissgrant, agentdm
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

using UnityEngine;
using System;
using System.Collections;

public class BerkeliumTestGUI : MonoBehaviour
{
	private RenderWebsite website;

	private string javascript = "";
	private string javascriptMessages = "";
	private int numMessages = 0;
	private Vector2 consolePos = Vector2.zero;
	
    void Start ()
	{
		if(website == null)
			website = (RenderWebsite) GetComponent(typeof(RenderWebsite));
    }

	void OnExternalHost(string message)
	{
		// Add to the console
		javascriptMessages += message + "\n";
		++numMessages;
		consolePos.y += 13;
	}
	
	void testFunction(Hashtable args)
	{
		string msg = "testFunction called with args ";
		
		IDictionaryEnumerator e = args.GetEnumerator();
		bool firstArg = true;
		while(e.MoveNext())
		{
			if(firstArg)
				firstArg = false;
			else
				msg += "; ";
			
			msg += e.Key.ToString() + " = " + e.Value.ToString();
		}
		
		// Add to the console
		javascriptMessages += msg + "\n";
		++numMessages;
		consolePos.y += 13;
		
		// DEBUG
		print("testFunction param3: " + args["param3"]);
	}

	void OnGUI()
	{
		// Steal focus from GUI when clicking outside it
		if(Event.current.type == EventType.MouseDown)
			GUIUtility.keyboardControl = 0;

		// A text edit box to enter a URL
		GUI.SetNextControlName("urlField");
		website.url = GUI.TextField(new Rect(50, 10, Screen.width-100, 20), website.url);
		
		// Pressing enter while the text edit has focus changes the URL
		if(Event.current.isKey && Event.current.keyCode == KeyCode.Return && GUI.GetNameOfFocusedControl () == "urlField")
			navigateTo(website.url);
		
		// Clicking this button also changes the URL
		if(GUI.Button(new Rect(Screen.width - 50, 10, 30, 20), "GO"))
			navigateTo(website.url);
		
		// A text edit to run javascript
		GUI.SetNextControlName("javascriptField");
		javascript = GUI.TextField(new Rect(10, Screen.height - 140, Screen.width - 80, 20), javascript);
		
		// Pressing enter while the text edit has focus executes the javascript
		if(Event.current.isKey && Event.current.keyCode == KeyCode.Return && GUI.GetNameOfFocusedControl () == "javascriptField")
			executeJavascript(javascript);
		
		// Clicking this button also executes the javascript
		if(GUI.Button(new Rect(Screen.width - 70, Screen.height - 140, 60, 20), "Execute"))
			executeJavascript(javascript);
		
		// A console for javascript externalHost messages
		int consoleHeight = 10 + numMessages * 13;
		if(consoleHeight < 100)
			consoleHeight = 100;
		
		consolePos = GUI.BeginScrollView(new Rect(10, Screen.height - 110, Screen.width - 20, 100), consolePos, new Rect(0,0, Screen.width - 40, consoleHeight));
		GUI.Box(new Rect(0,0, Screen.width - 40, consoleHeight), javascriptMessages);
		GUI.EndScrollView();
	}
	
	void navigateTo(string url)
	{
		website.navigateTo(url);
		
		// Steal focus from the GUI
		GUIUtility.keyboardControl = 0;
	}
	
	void executeJavascript(string javascript)
	{
		website.executeJavascript(javascript);
		
		// Steal focus from the GUI
		GUIUtility.keyboardControl = 0;
	}
}
