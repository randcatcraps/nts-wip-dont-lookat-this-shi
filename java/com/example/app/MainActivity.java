package com.example.app;

import android.app.Activity;
import android.os.Bundle;
import android.view.Gravity;
import android.widget.TextView;

public class MainActivity extends Activity {
  @Override
  public void onCreate(Bundle bundle)
  {
    super.onCreate(bundle);

    TextView hello = new TextView(this);
    hello.setText("Hello, world!");
    hello.setGravity(Gravity.CENTER);

    setContentView(hello);
  }
}
