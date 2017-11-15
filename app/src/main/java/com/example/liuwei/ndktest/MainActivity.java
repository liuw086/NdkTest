package com.example.liuwei.ndktest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.xiaomi.agent.MiLiveAgent;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    // Example of a call to a native method
    TextView tv = (TextView) findViewById(R.id.sample_text);


        String mVideoPath = "http://zbuvideo.zb.mi.com/uv_2320914470_dHhnNkp5Q3ZFaHZLbEMwdVkxYk1RZz09_1505701530793.mp4";
        String localUrl = MiLiveAgent.getLocalUrlByCdnUrl(mVideoPath);


        MiLiveAgent.setCacheDiskPath("/storage/emulated/0/voip-data/");
        MiLiveAgent.startAgent(getApplicationContext());


        tv.setText(localUrl);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("kqueue");
        System.loadLibrary("p2pengine");
        System.loadLibrary("native-lib");
    }
}
