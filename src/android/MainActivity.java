package com.mutantstargoat.stereotunnel;

import android.os.Bundle;
import android.app.NativeActivity;
import android.view.*;
import android.view.WindowManager.LayoutParams;
//import android.util.Log;

public class MainActivity extends NativeActivity
{
	//public static String tag = "stereotunnel";

	@Override
	protected void onCreate(Bundle saved_inst)
	{
		super.onCreate(saved_inst);

		// go fullscreen
		int winflags = LayoutParams.FLAG_FULLSCREEN |
			LayoutParams.FLAG_LAYOUT_NO_LIMITS | LayoutParams.FLAG_LAYOUT_IN_SCREEN;
		Window win = getWindow();
		win.setFlags(winflags, winflags);
	}

	@Override
	public void onWindowFocusChanged(boolean focus)
	{
		super.onWindowFocusChanged(focus);
		if(focus) {
			set_fullscreen();
		}
	}

	protected void onResume()
	{
		super.onResume();
		set_fullscreen();
	}

	public void set_fullscreen()
	{
		int uiflags = View.SYSTEM_UI_FLAG_FULLSCREEN |
			View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
			View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
			View.SYSTEM_UI_FLAG_LOW_PROFILE;

		if(android.os.Build.VERSION.SDK_INT >= 19) {
			uiflags |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
				View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
				View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
		}

		View decor = getWindow().getDecorView();
		decor.setSystemUiVisibility(uiflags);
	}
}
