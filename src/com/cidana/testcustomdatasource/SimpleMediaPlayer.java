package com.cidana.testcustomdatasource;


import android.app.Activity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.util.Log;

public class SimpleMediaPlayer extends Activity implements SurfaceHolder.Callback, SurfaceLayer.MessageCallback {
	
	private static final String TAG = "TestCustomDataSource";
	//private String mUrl = "http://192.168.0.37/prometheus_inttrailer_1280.ts";
	//private String mUrl = "http://192.168.0.37/HD_1080_BloodDiamondTrailer1.ts";
	//private String mUrl = "/sdcard/1.mp4";
	//private String mUrl = "/sdcard/2.mp4";
//	private String mUrl = "/sdcard/mpeg2.ts";
	private String mUrl = "/sdcard/HD_2012_10_09_13_42_17000.ts";
   	private SurfaceView mPreview;
    private SurfaceLayer mSurfaceLayer;
    
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
		Log.d (TAG, "onCreate called");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mPreview = (SurfaceView) findViewById(R.id.SurfaceView01);
        mPreview.getHolder().addCallback(this);
        mPreview.getHolder().setKeepScreenOn(true);
        
        Bundle bundle = getIntent().getExtras();
        if (bundle != null)
		{
        	int fileIndex = bundle.getInt("index");
        	mUrl = MediaFileList.getInstance().mlistMediaFile.get(fileIndex);
		}
        else
        {
        //	mUrl = MediaFileList.getInstance().mlistMediaFile.get(0);
        }
        
        mSurfaceLayer = new SurfaceLayer(this);
	}
    
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d (TAG, String.format("surfaceChanged called %dx%d", width, height));
    }

    public void surfaceDestroyed(SurfaceHolder surfaceholder) {
        Log.d (TAG, "surfaceDestroyed called");
        mSurfaceLayer.Stop();
    }


    public void surfaceCreated(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.d (TAG, "surfaceCreated called");
        mSurfaceLayer.setSurface(holder.getSurface());
        mSurfaceLayer.Start(mUrl);
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
	}
    
    public void onVideoSizeChanged(int width, int height) {
    	Log.d(TAG, String.format("onVideoSizeChanged %dx%d", width, height));
//    	mPreview.getHolder().setFixedSize(width, height);
    }
    
	public void onPlaybackEnd() {
		Log.d(TAG, "onPlaybackEnd");
		mSurfaceLayer.Stop();
	}
}
