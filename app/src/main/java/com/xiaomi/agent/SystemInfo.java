package com.xiaomi.agent;

import android.os.Environment;
import android.content.Context;

import java.io.File;
import android.net.wifi.WifiManager;
import android.net.wifi.ScanResult;
import android.util.Log;

import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class WifiInfo {
	public String BSSID;
	public String SSID;
	String getBssid() {
		return BSSID;
	}
	String getSsid () {
		return SSID;
	}

} 

public class SystemInfo {
    private static final String CT_S_Sdcard_Sign_Storage_emulated = "storage/emulated/";
    private static final String CT_S_Sdcard_Sign_Storage_sdcard = "storage/sdcard";

    static String getStoragePath(Context context) {
        boolean sdCardExist = Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);
        String path = "";
        if (sdCardExist) {
            path = Environment.getExternalStorageDirectory().getAbsolutePath();
            if(!pathCanWrite(path))
            {
                Log.e("getStoragePath", "getAbsolutePath can't write:"+path);
                //get path can't read or can't write
                path = checkAndReplaceEmulatedPath(path);
                if(!pathCanWrite(path))
                {
                    Log.e("getStoragePath", "replace path can't write:"+path);
                    path = "/sdcard";
                    if(!pathCanWrite(path))
                    {
                        Log.e("getStoragePath", "hardcode path can't write:"+path);
                    }
                }
            }
        } else {
            if (context != null) {
                File fileDir = context.getFilesDir();
                    if (fileDir != null) {
                    path  = fileDir.toString();
                }
            }
        }
        return path;
    }

    private static boolean pathCanWrite(String path)
    {
        File sdcardPath = new File(path);
        if(!sdcardPath.isDirectory() || !sdcardPath.canWrite())
        {
            return false;
        }
        return true;
    }

    private static String checkAndReplaceEmulatedPath(String strSrc){

        Pattern p = Pattern.compile("/?storage/emulated/\\d{1,2}");
        Matcher m = p.matcher(strSrc);
        if (m.find()){
            strSrc = strSrc.replace(SystemInfo.CT_S_Sdcard_Sign_Storage_emulated, SystemInfo.CT_S_Sdcard_Sign_Storage_sdcard);
        }

//		if (strSrc.contains(CommonType.CT_S_Sdcard_Sign_Storage_emulated) && !CD_S_SdcardPath.contains(CommonType.CT_S_Sdcard_Sign_Storage_emulated_legacy))
//			strSrc = strSrc.replace(CommonType.CT_S_Sdcard_Sign_Storage_emulated, CommonType.CT_S_Sdcard_Sign_Storage_sdcard);

        return strSrc;
    }

    public static WifiInfo getCurrentWifiInfo(Context context) {

        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        android.net.wifi.WifiInfo wifiInfo = wifiManager == null ? null : wifiManager.getConnectionInfo();
        WifiInfo data = new WifiInfo();
        if (wifiInfo != null) {
                data.BSSID = String.valueOf(wifiInfo.getBSSID());
                data.SSID = String.valueOf(wifiInfo.getSSID());
        }

        return data;
    }
    
    public static WifiInfo[] getAvailableWifiInfo(Context context) {
        List<WifiInfo> list =  new ArrayList<WifiInfo>();
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        StringBuilder scanBuilder= new StringBuilder();
        List<ScanResult> scanResults=wifiManager.getScanResults();//搜索到的设备列表
        for (ScanResult scanResult : scanResults) {
	    WifiInfo data = new WifiInfo();
            data.SSID = scanResult.SSID;
            data.BSSID = scanResult.BSSID;
            list.add(data);
        }
        WifiInfo[] array = (WifiInfo[])list.toArray(new WifiInfo[list.size()]);
        return array;
    }
}
