package com.xiaomi.agent;
import android.content.Context;

/**
 * Created by mi on 2017/3/1.
 */
public class MiLiveAgent {

    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("kqueue");
        System.loadLibrary("p2pengine");
    }
    public final static native void startAgent(Context context);

    public final static native void stopAgent();
    public final static native String getLocalUrlByCdnUrl(String cdnUrl);
    public final static native void setMaxCacheSize(long size);
    public final static native void setCacheDiskPath(String path);
}
