package com.example.app;

import android.app.Activity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

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
    hello.setText(String.format(getString(R.string.show_num), getNum()));
    hello.setGravity(Gravity.CENTER);
    hello.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view)
      {
        Toast.makeText(view.getContext(), "owo",
                       Toast.LENGTH_SHORT).show();
      }
    });

    setContentView(hello);
  }
}
