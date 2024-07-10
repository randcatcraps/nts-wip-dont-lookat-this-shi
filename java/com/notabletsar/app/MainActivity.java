package com.notabletsar.app;

import java.lang.Runnable;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Toast;

import com.notabletsar.app.ControllerActivity;
import com.notabletsar.app.NTSAccessory;

public class MainActivity extends Activity {
  public static final int CHECK_USB_DELAY_MS = 2000;

  private static final String ACTION_USB_PERM = "com.notabletsar.app.USB_PERMISSION";

  protected Handler mHandler;

  private Runnable mRunnable;

  protected boolean mDestroy = false;

  /* first run of checkUSBAccessory() */
  protected boolean mFlag = true;

  private UsbManager mUsbman;

  private BroadcastReceiver mReceiver;

  private View mLayout;

  private void scheduleRunnable()
  {
    if (!mDestroy)
      mHandler.postDelayed(mRunnable, CHECK_USB_DELAY_MS);
  }

  protected void unscheduleRunnable()
  {
    mDestroy = true;
    mHandler.removeCallbacks(mRunnable);
  }

  protected void jumpToControllerActivity(UsbAccessory usb)
  {
    Intent intent;

    intent = new Intent(this, ControllerActivity.class);
    intent.putExtra(UsbManager.EXTRA_ACCESSORY, usb);
    startActivity(intent);
    finish();  // end current
  }

  protected void unregisterReceiver()
  {
    if (mReceiver != null)
    {
      unregisterReceiver(mReceiver);
      mReceiver = null;
    }
  }

  protected void waitForGrantAccess(UsbAccessory usb)
  {
    IntentFilter filter;
    PendingIntent intent;

    filter = new IntentFilter();
    filter.addAction(ACTION_USB_PERM);  // with data

    mReceiver = new BroadcastReceiver() {
      @Override
      public void onReceive(Context ctx, Intent intent)
      {
        String action;
        UsbAccessory usb_;

        action = intent.getAction();
        if (ACTION_USB_PERM.equals(action))
        {
          synchronized(this)
          {
            unregisterReceiver();

            usb_ = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY,
                                             UsbAccessory.class);
            if (usb_ == null ||
                !intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED,
                                        false))
            {
              Toast.makeText(ctx, R.string.denied, Toast.LENGTH_LONG).show();

              finish();  // quit
              return;
            }

            jumpToControllerActivity(usb_);
          }
        }
      }
    };
    registerReceiver(mReceiver, filter, RECEIVER_NOT_EXPORTED);

    intent = PendingIntent.getBroadcast(this, 0  /* requestCode */,
                                        new Intent(ACTION_USB_PERM).setPackage(getPackageName()),
                                        PendingIntent.FLAG_MUTABLE);
    mUsbman.requestPermission(usb, intent);
  }

  protected void checkUSBAccessory()
  {
    UsbAccessory usb;

    usb = NTSAccessory.findAccessory(mUsbman);
    if (usb != null)
    {
      if (mFlag)
      {
        Toast.makeText(this, R.string.reconnect, Toast.LENGTH_LONG).show();

        // abort
        finish();
        return;
      }

      Toast.makeText(this, R.string.detected, Toast.LENGTH_SHORT).show();

      if (mUsbman.hasPermission(usb))
        jumpToControllerActivity(usb);
      else
        waitForGrantAccess(usb);

      return;  // end loop
    }

    if (mFlag)
    {
      mFlag = false;

      mLayout.setVisibility(View.VISIBLE);
    }

    scheduleRunnable();  // loop
  }

  private void setupLoopCheckUsbAccessory()
  {
    mHandler = new Handler(getMainLooper());
    mRunnable = new Runnable() {
      @Override
      public void run()
      {
        checkUSBAccessory();
      }
    };
    mUsbman = (UsbManager)getSystemService(Context.USB_SERVICE);

    checkUSBAccessory();
  }

  @Override
  public void onCreate(Bundle bundle)
  {
    super.onCreate(bundle);
    mLayout = getLayoutInflater().inflate(R.layout.main_activity, null);
    setContentView(mLayout);

    setupLoopCheckUsbAccessory();
  }

  @Override
  public void onDestroy()
  {
    // cleanup
    unscheduleRunnable();
    unregisterReceiver();

    super.onDestroy();
  }

  @Override
  public boolean onKeyDown(int key, KeyEvent ev)
  {
    if (key == KeyEvent.KEYCODE_BACK)
      finish();  // destroy

    return super.onKeyDown(key, ev);
  }
}
