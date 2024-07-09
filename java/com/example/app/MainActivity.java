package com.example.app;

import android.app.Activity;
import android.os.Bundle;
import android.view.Gravity;
import android.widget.TextView;

public class MainActivity extends Activity {
  static {
    System.loadLibrary("app_jni");
  }

  private native int getNum();

  @Override
  public void onCreate(Bundle bundle)
  {
    super.onCreate(bundle);

    TextView hello = new TextView(this);
    hello.setText(String.format("num from jni: %d", getNum()));
    hello.setGravity(Gravity.CENTER);

    setContentView(hello);
  }
}
