// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// Part of this source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Contributors (Unity forum usernames): reissgrant, agentdm
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class RenderWebsite : MonoBehaviour
{
	public int width = 512;
	public int height = 512;
	public string url = "http://www.google.com";
	public bool interactive = true;
	public bool transparency = false;
	
	private Texture2D m_Texture;
	private Color[] m_Pixels;
	private GCHandle m_PixelsHandle;
	private int m_TextureID;
	
	private string javascript = "";
	private string javascriptMessages = "";
	private int numMessages = 0;
	private Vector2 consolePos = Vector2.zero;
	
	private UnityBerkelium.SetPixelsFunc m_setPixelsFunc;
	private UnityBerkelium.ApplyTextureFunc m_applyTextureFunc;
	private UnityBerkelium.ExternalHostFunc m_externalHostFunc;

    void Start ()
	{
		// Initialize Berkelium
		UnityBerkelium.init();
		
		// Create the texture that will represent the website (with optional transparency and without mipmaps)
		TextureFormat texFormat = transparency ? TextureFormat.ARGB32 : TextureFormat.RGB24;
		m_Texture = new Texture2D (width, height, texFormat, false);

		// Create the pixel array for the plugin to write into at startup    
		m_Pixels = m_Texture.GetPixels (0);
		// "pin" the array in memory, so we can pass direct pointer to it's data to the plugin,
		// without costly marshaling of array of structures.
		m_PixelsHandle = GCHandle.Alloc(m_Pixels, GCHandleType.Pinned);

		// Save the texture ID
		m_TextureID = m_Texture.GetInstanceID();
		
		// Improve rendering at shallow angles
		m_Texture.filterMode = FilterMode.Trilinear;
		m_Texture.anisoLevel = 2;

		// Assign texture to the renderer
		if (renderer)
		{
			renderer.material.mainTexture = m_Texture;
			
			// Transparency?
			if(transparency)
				renderer.material.shader = Shader.Find("Transparent/Diffuse");
			else
				renderer.material.shader = Shader.Find("Diffuse");
			
			// The texture has to be flipped
			renderer.material.mainTextureScale = new Vector2(1,-1);
		}
		// or gui texture
		else if (GetComponent(typeof(GUITexture)))
		{
			GUITexture gui = GetComponent(typeof(GUITexture)) as GUITexture;
			gui.texture = m_Texture;
		}
		else
		{
			Debug.Log("Game object has no renderer or gui texture to assign the generated texture to!");
		}
		
		// Create new web window
		UnityBerkelium.Window.create(m_TextureID, m_PixelsHandle.AddrOfPinnedObject(), transparency, width,height, url);
		print("Created new web window: " + m_TextureID);
		
		// Paint callbacks
		m_setPixelsFunc = new UnityBerkelium.SetPixelsFunc(this.SetPixels);
		m_applyTextureFunc = new UnityBerkelium.ApplyTextureFunc(this.ApplyTexture);
		UnityBerkelium.Window.setPaintFunctions(m_TextureID, m_setPixelsFunc, m_applyTextureFunc);
		
		// Set the external host callback (for calling Unity functions from javascript)
		m_externalHostFunc = new UnityBerkelium.ExternalHostFunc(this.onExternalHost);
		UnityBerkelium.Window.setExternalHostCallback(m_TextureID, m_externalHostFunc);
    }
	
	void SetPixels(/*int left, int top, int width, int height*/)
	{
		UnityBerkelium.Rect rect = UnityBerkelium.Window.getLastDirtyRect(m_TextureID);
		//print("Painting rect: (" + rect.left + ", " + rect.top + ", " + rect.width + ", " + rect.height + ")");
		m_Texture.SetPixels(rect.left, rect.top, rect.width, rect.height, m_Pixels, 0);
	}
	
	void ApplyTexture()
	{
		//print("Applying texture");
		m_Texture.Apply();
	}
	
	void onExternalHost(/*string message*/)
	{
		string message = Marshal.PtrToStringUni(UnityBerkelium.Window.getLastExternalHostMessage(m_TextureID));
		print("Message from javascript: " + message);
		
		// Add to the console
		javascriptMessages += message + "\n";
		++numMessages;
		consolePos.y += 13;
	}
    
    void OnDisable() {
		// Destroy the web window
		UnityBerkelium.Window.destroy(m_TextureID);
		
        // Free the pinned array handle.
        m_PixelsHandle.Free();
		
		// Destroy Berkelium
		UnityBerkelium.destroy();
		
		print("Destroyed Berkelium");
    }

    void Update ()
	{
		// Update Berkelium
		// TODO This only has to be done once in stead of per object
		UnityBerkelium.update();
    }
	
	void OnMouseEnter()
	{
	}
	
	void OnMouseOver()
	{
		// Only when interactive is enabled
		if(!interactive)
			return;

		RaycastHit hit;
		if (Physics.Raycast (Camera.main.ScreenPointToRay(Input.mousePosition), out hit))
		{
			int x = /*width -*/ (int) (hit.textureCoord.x * width);
			int y = height - (int) (hit.textureCoord.y * height);
	
			UnityBerkelium.Window.mouseMove(m_TextureID, x, y);
		}
	}
	
	void OnMouseExit()
	{
	}
	
	void OnMouseDown()
	{
		// Only when interactive is enabled
		if(!interactive)
			return;
		
		RaycastHit hit;
		if (Physics.Raycast (Camera.main.ScreenPointToRay(Input.mousePosition), out hit))
		{
			int x = /*width -*/ (int) (hit.textureCoord.x * width);
			int y = height - (int) (hit.textureCoord.y * height);
	
			UnityBerkelium.Window.mouseMove(m_TextureID, x, y);
			UnityBerkelium.Window.mouseDown(m_TextureID, 0);
		}
	}
	
	void OnMouseUp()
	{
		// Only when interactive is enabled
		if(!interactive)
			return;

		RaycastHit hit;
		if (Physics.Raycast (Camera.main.ScreenPointToRay(Input.mousePosition), out hit))
		{
			int x = /*width -*/ (int) (hit.textureCoord.x * width);
			int y = height - (int) (hit.textureCoord.y * height);
	
			UnityBerkelium.Window.mouseMove(m_TextureID, x, y);
			UnityBerkelium.Window.mouseUp(m_TextureID, 0);
		}
	}
	
	void OnGUI()
	{
		// Inject input into the page when the GUI doesn't have focus
		if(Event.current.isKey && GUIUtility.keyboardControl == 0)
		{
			// Insert character
			UnityBerkelium.Window.textEvent(m_TextureID, Event.current.character);
			
			KeyCode key = Event.current.keyCode;
			bool pressed = (Event.current.type == EventType.KeyDown);
			
			// Special case for backspace
			if(key == KeyCode.Backspace)
				UnityBerkelium.Window.keyEvent(m_TextureID, pressed, 0, 08, 0);
			// Special case for enter
			else if(key == KeyCode.Return)
				UnityBerkelium.Window.keyEvent(m_TextureID, pressed, 0, 13, 0);
			
			// TODO Handle all keys
			/*int mods = 0;
			int vk_code = UnityBerkelium.convertKeyCode(Event.current.keyCode);
			int scancode = 0;
			UnityBerkelium.Window.keyEvent(m_TextureID, pressed, mods, vk_code, scancode);
			print("Key event: " + pressed + ", " + Event.current.keyCode);*/
		}
		
		// Steal focus from GUI when clicking outside it
		if(Event.current.type == EventType.MouseDown)
			GUIUtility.keyboardControl = 0;

		// A text edit box to enter a URL
		GUI.SetNextControlName("urlField");
		url = GUI.TextField(new Rect(50, 10, Screen.width-100, 20), url);
		
		// Pressing enter while the text edit has focus changes the URL
		if(Event.current.isKey && Event.current.keyCode == KeyCode.Return && GUI.GetNameOfFocusedControl () == "urlField")
			navigateTo(url);
		
		// Clicking this button also changes the URL
		if(GUI.Button(new Rect(Screen.width - 50, 10, 30, 20), "GO"))
			navigateTo(url);
		
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
		print("Changing url to " + url);
		UnityBerkelium.Window.navigateTo(m_TextureID, url);
		
		// Steal focus from the GUI
		GUIUtility.keyboardControl = 0;
	}
	
	void executeJavascript(string javascript)
	{
		print("Executing Javascript: " + javascript);
		UnityBerkelium.Window.executeJavascript(m_TextureID, javascript);
		
		// Steal focus from the GUI
		GUIUtility.keyboardControl = 0;
	}
}
