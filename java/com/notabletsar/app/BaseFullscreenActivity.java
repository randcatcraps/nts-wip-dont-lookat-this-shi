package com.notabletsar.app;

import android.app.Activity;
import android.graphics.Color;
import android.view.View;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;

public class BaseFullscreenActivity extends Activity {
  protected void enterFullscreen()
  {
    Window window;
    View view;
    WindowInsetsController wic;

    getActionBar().hide();

    window = getWindow();
    view = window.getDecorView();
    wic = view.getWindowInsetsController();

    wic.hide(WindowInsets.Type.systemBars());  // statusbar + navigationbar
    wic.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);  // auto rehide

    // kill cutouts
    window.getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;

    view.setBackgroundColor(Color.BLACK);

    window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
  }
}
