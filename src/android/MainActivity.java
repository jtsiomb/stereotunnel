package com.mutantstargoat.stereotunnel;

import android.os.Bundle;
import android.app.NativeActivity;
import android.view.*;
import android.view.WindowManager.LayoutParams;
import android.view.ViewGroup.MarginLayoutParams;
import android.widget.*;
import com.google.android.gms.ads.*;
import android.util.Log;
import android.content.res.Configuration;

public class MainActivity extends NativeActivity
{
	public static String tag = "stereotunnel";
	public static String ad_id_test = "ca-app-pub-3940256099942544/6300978111";
	public static String ad_id_prod = "ca-app-pub-3466324067850759/4861471428";

	public static String dev_id_gnexus = "B6FBA93004067A8DA892B85127D9454C";
	public static String dev_id_nexus7 = "EC3418DBCF3628D89527BCE612BBA299";

	MainActivity act;

	AdView ad_view = null;
	PopupWindow ad_win;
	LinearLayout ad_layout, ad_main_layout;
	boolean ad_ready = false;
	boolean waiting_for_ad = false;

	@Override
	protected void onCreate(Bundle saved_inst)
	{
		super.onCreate(saved_inst);

		// go fullscreen
		int winflags = LayoutParams.FLAG_FULLSCREEN |
			LayoutParams.FLAG_LAYOUT_NO_LIMITS |
			LayoutParams.FLAG_LAYOUT_IN_SCREEN |
			LayoutParams.FLAG_KEEP_SCREEN_ON;

		Window win = getWindow();
		win.setFlags(winflags, winflags);
	}

	@Override
	public void onDestroy()
	{
		destroy_ad_popup();
		super.onDestroy();
	}

	@Override
	public void onAttachedToWindow()
	{
		super.onAttachedToWindow();

		create_ad_popup();
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
		Log.i(tag, "[JAVA] onResume()");
		super.onResume();
		if(ad_view != null) {
			ad_view.resume();
		}
		set_fullscreen();
	}

	protected void onPause()
	{
		Log.i(tag, "[JAVA] onPause()");
		super.onPause();
		if(ad_view != null) {
			ad_view.pause();
		}
	}

	/*
	public void onConfigurationChanged(Configuration config)
	{
		super.onConfigurationChanged(config);

		destroy_ad_popup();
		create_ad_popup();
	}
	*/

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

	// ads ...
	public void create_ad_popup()
	{
		Log.i(tag, "[JAVA] create_ad_popup called");
		if(ad_view != null) return;

		act = this;

		this.runOnUiThread(new Runnable() {
			@Override
			public void run()
			{
				Log.i(tag, "[JAVA] Creating Ad popup");

				ad_win = new PopupWindow(act);
				// set minimum size
				int xsz = 320;//AdSize.SMART_BANNER.getWidthInPixels(act);
				int ysz = 50;//AdSize.SMART_BANNER.getHeightInPixels(act);
				ad_win.setWidth(xsz);	// orig:320
				ad_win.setHeight(ysz);	// orig:50
				ad_win.setWindowLayoutMode(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
				ad_win.setClippingEnabled(false);

				ad_main_layout = new LinearLayout(act);
				MarginLayoutParams params = new MarginLayoutParams(LayoutParams.WRAP_CONTENT,
						LayoutParams.WRAP_CONTENT);
				params.setMargins(0, 0, 0, 0);
				act.setContentView(ad_main_layout, params);


				ad_view = new AdView(act);
				ad_view.setAdSize(AdSize.BANNER);
				ad_view.setAdUnitId(ad_id_test);

				ad_layout = new LinearLayout(act);
				ad_layout.setPadding(-5, -5, -5, -5);
				ad_layout.setOrientation(LinearLayout.VERTICAL);
				ad_layout.addView(ad_view, params);
				ad_win.setContentView(ad_layout);

				ad_view.setAdListener(new AdListener() {
					@Override
					public void onAdLoaded()
					{
						Log.i(tag, "[JAVA] ad loaded");
						super.onAdLoaded();
						ad_ready = true;
						waiting_for_ad = false;
						show_ad();
					}
					@Override
					public void onAdFailedToLoad(int error_code)
					{
						Log.e(tag, "[JAVA] ad failed to load, error code: " + error_code);
						super.onAdFailedToLoad(error_code);
						ad_ready = false;
						waiting_for_ad = false;
						request_ad();
					}
				});

				if(!waiting_for_ad) {
					request_ad();
				}

				Log.i(tag, "[JAVA] Done creating ad popup");
			}
		});
	}

	public void destroy_ad_popup()
	{
		Log.i(tag, "[JAVA] destroy_ad_popup called");

		if(ad_view != null) {
			ad_view.destroy();
			ad_view = null;
		}
		if(ad_win != null) {
			ad_win.dismiss();
			ad_win = null;
		}
	}

	public void request_ad()
	{
		Log.i(tag, "[JAVA] requesting ad");

		if(ad_view == null) {
			Log.e(tag, "[JAVA] request_ad called without an ad_view");
			return;
		}

		AdRequest.Builder reqbuild = new AdRequest.Builder();
		reqbuild.addTestDevice(AdRequest.DEVICE_ID_EMULATOR);
		reqbuild.addTestDevice(dev_id_gnexus);
		reqbuild.addTestDevice(dev_id_nexus7);
		ad_view.loadAd(reqbuild.build());

		waiting_for_ad = true;
	}

	private void show_ad()
	{
		if(ad_ready) {
			Log.i(tag, "[JAVA] show_ad called with ad ready");
			ad_view.setVisibility(View.VISIBLE);
			ad_win.showAtLocation(ad_main_layout, Gravity.TOP, 0, 0);
			ad_win.update();
		} else {
			if(!waiting_for_ad) {
				Log.i(tag, "[JAVA] show_ad called with ad neither ready nor pending");
				request_ad();
			} else {
				Log.i(tag, "[JAVA] show_ad called with ad pending: nop");
			}
		}
	}

	private void hide_ad()
	{
		Log.i(tag, "[JAVA] hide_ad called");
		//ad_view.setVisibility(View.GONE);
		ad_win.dismiss();
		//ad_win.update();
		//destroy_ad_popup();
		ad_ready = false;
		waiting_for_ad = false;
	}


	/*
    @Override
    public void onAdOpened()
	{
        // Code to be executed when an ad opens an overlay that
        // covers the screen.
    }

    @Override
    public void onAdLeftApplication()
	{
        // Code to be executed when the user has left the app.
    }

    @Override
    public void onAdClosed()
	{
        // Code to be executed when when the user is about to return
        // to the app after tapping on an ad.
    }
	*/
}
