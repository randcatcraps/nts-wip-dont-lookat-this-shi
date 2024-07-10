package com.notabletsar.app;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Rect;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.notabletsar.app.BaseFullscreenActivity;
import com.notabletsar.app.NTSAccessory;

public class ControllerActivity extends BaseFullscreenActivity {
  static {
    System.loadLibrary("app_jni");
  }

  private BroadcastReceiver mReceiver;

  private native static void setUSBFdJNI(int fd);

  private native static void closeUSBFdIfAnyJNI();

  private native static void doHandShakeJNI(int scr_w, int scr_h);

  private native static void reportToHostJNI(int action, int cur_id, float x, float y);

  private void unregisterDisconnectReceiver()
  {
    if (mReceiver != null)
    {
      unregisterReceiver(mReceiver);
      mReceiver = null;
    }
  }

  private void setupDisconnectNotifier()
  {
    IntentFilter filter;

    filter = new IntentFilter();
    filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);  // with data

    mReceiver = new BroadcastReceiver() {
      @Override
      public void onReceive(Context ctx, Intent intent)
      {
        String action;
        UsbAccessory usb;

        action = intent.getAction();
        if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action))
        {
          usb = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY, UsbAccessory.class);
          if (usb != null && NTSAccessory.checkVendorId(usb))
          {
            // unregister is in onDestroy()

            Toast.makeText(ctx, R.string.disconnected, Toast.LENGTH_LONG).show();

            finish();  // stop
          }
        }
      }
    };
    registerReceiver(mReceiver, filter);
  }

  private boolean setupUSBConnection()
  {
    UsbManager usbman;
    Intent intent;
    UsbAccessory usb;
    int fd;

    usbman = (UsbManager)getSystemService(Context.USB_SERVICE);
    intent = getIntent();
    usb = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY, UsbAccessory.class);
    if (usb == null || !NTSAccessory.checkVendorId(usb) || !usbman.hasPermission(usb))
    {
      // bad intent

      finish();  // abort
      return false;
    }

    setupDisconnectNotifier();
    fd = usbman.openAccessory(usb).detachFd();
    setUSBFdJNI(fd);

    return true;
  }

  protected void doHandShake()
  {
    Rect rect;

    rect = getWindowManager().getCurrentWindowMetrics().getBounds();
    doHandShakeJNI(rect.width(), rect.height());
  }

  private void displayWizardDialog()
  {
    AlertDialog.Builder builder;
    AlertDialog dialog;

    builder = new AlertDialog.Builder(this).setMessage(R.string.prepare);
    builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
      @Override
      public void onCancel(DialogInterface dialog)
      {
        // quit
        finish();
      }
    });
    builder.setPositiveButton(R.string.start, null);

    dialog = builder.create();
    dialog.setCanceledOnTouchOutside(false);
    dialog.setOnShowListener(new DialogInterface.OnShowListener() {
      @Override
      public void onShow(DialogInterface dialog)
      {
        Button btn;

        btn = ((AlertDialog)dialog).getButton(DialogInterface.BUTTON_POSITIVE);
        btn.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View view)
        {
          ((BaseFullscreenActivity)ControllerActivity.this).enterFullscreen();
          doHandShake();

          dialog.dismiss();

          Toast.makeText(((Dialog)dialog).getContext(),
                        R.string.ready, Toast.LENGTH_SHORT).show();
        }
        });
      }
    });
    dialog.show();
  }

  @Override
  public void onCreate(Bundle bundle)
  {
    super.onCreate(bundle);
    if (!setupUSBConnection())
      return;
    displayWizardDialog();
  }

  @Override
  public void onDestroy()
  {
    // cleanup
    closeUSBFdIfAnyJNI();
    unregisterDisconnectReceiver();

    super.onDestroy();
  }

  @Override
  public boolean onKeyDown(int key, KeyEvent ev)
  {
    if (key == KeyEvent.KEYCODE_BACK)
      finish();  // destroy

    return super.onKeyDown(key, ev);
  }

  @Override
  public boolean onTouchEvent(MotionEvent ev)
  {
    int action;
    int cur_idx;

    action = ev.getAction();
    cur_idx = (action & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
    reportToHostJNI(action, ev.getPointerId(cur_idx), ev.getX(cur_idx), ev.getY(cur_idx));

    return true;
  }
}
