package com.cidana.testcustomdatasource;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.Surface;
import java.lang.ref.WeakReference;

public class SurfaceLayer {

	private Surface mSurface; // accessed by native methods
	private EventHandler mEventHandler;
	
	private final int MSG_VIDEO_SIZE_CHANGED = 0;
	private final int MSG_PLAYBACK_END = 1;
	
	private static native final void native_init();
	private native final void native_setup(Object thisSurface);
    private native final void native_start(String url);
    private native final void native_stop();
    
    private MessageCallback mCallback;
	public interface MessageCallback
	{
		public void onVideoSizeChanged(int width, int height);
		public void onPlaybackEnd();
	}
    
    static {
    	//System.loadLibrary("videoparser");
    	//System.loadLibrary("cimpeg2ts");
    	//System.loadLibrary("omxcodec-15");
    	//System.loadLibrary("omxcodec-14");
    	System.loadLibrary("codec-14");
        System.loadLibrary("testcustomdatasource_jni");
        native_init();
    }
    
    public SurfaceLayer(MessageCallback cb) {
    	Looper looper;
		if ((looper = Looper.myLooper()) != null) {
			mEventHandler = new EventHandler(looper);
		} else if ((looper = Looper.getMainLooper()) != null) {
			mEventHandler = new EventHandler(looper);
		} else {
			mEventHandler = null;
		}
		
		mCallback = cb;
    }

    public void setSurface(Surface surface) {
    	mSurface = surface;
    	native_setup(new WeakReference<SurfaceLayer>(this));
    }
    
    public void Start(String url) {
    	native_start(url);
    }
    
    public void Stop() {
    	native_stop();
    }
    
    private void onReceiveMessage(int code, int arg1, int arg2) {
    	Message m = mEventHandler.obtainMessage(code, arg1, arg2, null);
       	mEventHandler.sendMessage(m);
    }
    
    private class EventHandler extends Handler
    {
        public EventHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
        	if (mCallback != null)
        	{
        		switch(msg.what) {
        		case MSG_VIDEO_SIZE_CHANGED:
        			mCallback.onVideoSizeChanged(msg.arg1, msg.arg2);
        			break;
        		case MSG_PLAYBACK_END:
        			mCallback.onPlaybackEnd();
        			break;
        		}
        	}
        }
    }
}
