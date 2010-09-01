// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// This source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Contributors (Unity forum usernames): reissgrant, agentdm
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class UnityBerkelium : MonoBehaviour {

    public int width = 512;
    public int height = 512;
	public string url = "http://www.google.com";
	public bool interactive = true;
	public bool transparency = false;
	
    private Texture2D m_Texture;
    private Color[] m_Pixels;
    private GCHandle m_PixelsHandle;
	private int m_TextureID;
        
	// A rectangle (same structure as the Berkelium Rect)
	struct Rect
	{
		public int left, top;
		public int width, height;
	}
	
	// The delegate that is called when there is a dirty rectangle
	// Note: Unity crashes when using a struct or more than two parameters
	//delegate void SetPixelsFunc(Rect rect);
	delegate void SetPixelsFunc(/*int left, int top, int width, int height*/);
	private SetPixelsFunc m_setPixelsFunc;
	
	// The delgate that is called when all dirty rectangles can be applied
	delegate void ApplyTextureFunc();
	private ApplyTextureFunc m_applyTextureFunc;
	
	// The delegate that is called when javascript calls to Unity
	delegate void ExternalHostFunc(/*string message*/);
	private ExternalHostFunc m_externalHostFunc;

	// Imported functions
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_init();
    
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_destroy();
    
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_update();
    
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern int Berkelium_Window_create(int windowID, IntPtr colors, bool transparency, int width, int height, string url);

	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_destroy(int windowID);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_setPaintFunctions(int windowID, SetPixelsFunc setPixelsFunc, ApplyTextureFunc applyTextureFunc);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern Rect Berkelium_Window_getLastDirtyRect(int windowID);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_mouseDown(int windowID, int button);

	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_mouseUp(int windowID, int button);

	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_mouseMove(int windowID, int x, int y);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_textEvent(int windowID, char c);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_keyEvent(int windowID, bool pressed, int mods, int vk_code, int scancode);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_executeJavascript(int windowID, string javascript);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_setExternalHostCallback(int windowID, ExternalHostFunc callback);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern IntPtr Berkelium_Window_getLastExternalHostMessage(int windowID);
	
    void Start ()
	{
		// Initialize Berkelium
		Berkelium_init();
		
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
		Berkelium_Window_create(m_TextureID, m_PixelsHandle.AddrOfPinnedObject(), transparency, width,height, url);
		print("Created new web window: " + m_TextureID);
		
		// Paint callbacks
		m_setPixelsFunc = new SetPixelsFunc(this.SetPixels);
		m_applyTextureFunc = new ApplyTextureFunc(this.ApplyTexture);
		Berkelium_Window_setPaintFunctions(m_TextureID, m_setPixelsFunc, m_applyTextureFunc);
		
		// Set the external host callback (for calling Unity functions from javascript)
		m_externalHostFunc = new ExternalHostFunc(this.onExternalHost);
		Berkelium_Window_setExternalHostCallback(m_TextureID, m_externalHostFunc);
    }
	
	void SetPixels(/*int left, int top, int width, int height*/)
	{
		Rect rect = Berkelium_Window_getLastDirtyRect(m_TextureID);
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
		string message = Marshal.PtrToStringUni(Berkelium_Window_getLastExternalHostMessage(m_TextureID));
		print("Message from javascript: " + message);
	}
    
    void OnDisable() {
		// Destroy the web window
		Berkelium_Window_destroy(m_TextureID);
		
        // Free the pinned array handle.
        m_PixelsHandle.Free();
		
		// Destroy Berkelium
		Berkelium_destroy();
    }

    void Update ()
	{
		// Update Berkelium
		// TODO This only has to be done once in stead of per object
		Berkelium_update();
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
	
			Berkelium_Window_mouseMove(m_TextureID, x, y);
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
	
			Berkelium_Window_mouseMove(m_TextureID, x, y);
			Berkelium_Window_mouseDown(m_TextureID, 0);
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
	
			Berkelium_Window_mouseMove(m_TextureID, x, y);
			Berkelium_Window_mouseUp(m_TextureID, 0);
		}
	}
	
	void OnGUI()
	{
		if(Event.current.isKey)
		{
			// Insert character
			Berkelium_Window_textEvent(m_TextureID, Event.current.character);
			
			KeyCode key = Event.current.keyCode;
			bool pressed = (Event.current.type == EventType.KeyDown);
			
			// Special case for backspace
			if(key == KeyCode.Backspace)
				Berkelium_Window_keyEvent(m_TextureID, pressed, 0, 08, 0);
			// Special case for enter
			else if(key == KeyCode.Return)
				Berkelium_Window_keyEvent(m_TextureID, pressed, 0, 13, 0);
			
			// TODO Handle all keys
			/*int mods = 0;
			int vk_code = convertKeyCode(Event.current.keyCode);
			int scancode = 0;
			Berkelium_Window_keyEvent(m_TextureID, pressed, mods, vk_code, scancode);
			print("Key event: " + pressed + ", " + Event.current.keyCode);*/
		}
		
		// A button to test javascript execution
		/*if(GUI.Button(Rect(10,10,150,100), "Execute Javascript"))
		{
			print("Executing Javascript");
			Berkelium_Window_executeJavascript(m_TextureID, "callMeBack()");
		}*/
	}
	
	int convertKeyCode(KeyCode keyCode)
	{
		return (int) keyCode;
	}
}
