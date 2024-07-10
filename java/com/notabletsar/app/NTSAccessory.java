package com.notabletsar.app;

import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;

public class NTSAccessory {
  public static final String MANUFACTURER = "catcraps";
  public static final String MODEL = "osutablet";

  public static boolean checkVendorId(UsbAccessory usb)
  {
    return usb.getManufacturer().equals(MANUFACTURER) &&
           usb.getModel().equals(MODEL);
  }

  public static UsbAccessory findAccessory(UsbManager usbman)
  {
    UsbAccessory[] usbs;

    usbs = usbman.getAccessoryList();
    if (usbs == null)  // happens when nothing connected
      return null;

    for (UsbAccessory usb : usbs)
    {
      if (checkVendorId(usb))
        return usb;
    }

    return null;  // no match
  }
}
