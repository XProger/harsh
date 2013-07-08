package com.xproger.ls;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import java.util.ArrayList;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Bundle;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;

import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

public class GameActivity extends Activity {	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, 
        		WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON |
                WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD |
                WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED |
                WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);

    	super.onCreate(savedInstanceState);
    	
    	String path = null;
    	try {
    		path = getPackageManager().getPackageInfo(getPackageName(), 1).applicationInfo.sourceDir;
    	} catch (PackageManager.NameNotFoundException e) {
    		//
    	}        

    	mGLView = new GameGLSurfaceView(this);
        setContentView(mGLView);
        nativeInit(path);
        soundInit();
        
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        soundFree();
        nativeFree();
    }    
    
    @Override
    protected void onPause() {
        super.onPause();
        nativePause();
        mGLView.onPause();
        audioTrack.pause();
    }

    @Override
    protected void onResume() {
        super.onResume();
    //    nativeResume();
        mGLView.onResume();
        audioTrack.play();
    }

	public short buffer[];
	public static AudioTrack audioTrack;

	protected void soundInit() {		
		int bufferSize = AudioTrack.getMinBufferSize(22050, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT);
		System.out.println(String.format("sound buffer size: %d", bufferSize));

		buffer = new short [bufferSize / 2];
		audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
				22050,
				AudioFormat.CHANNEL_CONFIGURATION_STEREO,
				AudioFormat.ENCODING_PCM_16BIT,
				bufferSize,
				AudioTrack.MODE_STREAM);
		audioTrack.play();
		
        new Thread( new Runnable() {
        	public void run() {        		        		
				while ( audioTrack.getPlayState() != AudioTrack.PLAYSTATE_STOPPED ) {
					if (audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
						synchronized (audioTrack) {
							nativeSoundFill(buffer);
						}
						audioTrack.write(buffer, 0, buffer.length);
						audioTrack.flush();
					} else try { Thread.sleep(100); } catch(Exception e) {};
				}
        	}
        } ).start();		
	}

	protected void soundFree() {
		audioTrack.flush();
		audioTrack.stop();
		audioTrack.release();		
	}
	
    private GLSurfaceView mGLView;

    static {
        System.loadLibrary("game");
    }
 
    private static native void nativeInit(String path);
    private static native void nativeFree();
    public static native void nativePause();
    public static native void nativeResume();
    public static native void nativeSoundFill(short buffer[]);
}

class Touch {
	int id, x, y, state;
	public Touch(int _id, int _x, int _y, int _state) {
		id = _id;
		x = _x;
		y = _y;
		state = _state;		
	};
}

class GameGLSurfaceView extends GLSurfaceView {
	public static ArrayList<Touch> touch = new ArrayList<Touch>();

	public static void sendTouches() {
		for (int i = 0; i < touch.size(); i++) {
			Touch t = touch.get(i);	    	
			nativeTouch(t.id, t.x, t.y, t.state);
		}
		touch.clear();		
	}

    private void sendTouchState(final MotionEvent ev, int id, int state) {
		for (int p = 0; p < ev.getPointerCount(); p++) 
			if (id == ev.getPointerId(p)) {
				touch.add(new Touch(id, (int)ev.getX(p), (int)ev.getY(p), state));		    	
			//	nativeTouch(id, (int)ev.getX(p), (int)ev.getY(p), state);	// INPUT THREAD FUUUUU!!!
				return;
			}    
    }

    public GameGLSurfaceView(Context context) {
        super(context);
        touch.clear();	
        setEGLContextClientVersion(2);
        mRenderer = new GameRenderer();
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        setRenderer(mRenderer);
        getHolder().setFormat(PixelFormat.RGBA_8888);
    }
    
    @Override
    public boolean onTouchEvent(final MotionEvent ev) {
    	int action = ev.getAction();         	 
    	switch (action & MotionEvent.ACTION_MASK) {
    		case MotionEvent.ACTION_DOWN :
    			sendTouchState(ev, ev.getPointerId(0), 0);
    			break;
    		case MotionEvent.ACTION_UP :
    			sendTouchState(ev, ev.getPointerId(0), 1);
    			break;    	
    		case MotionEvent.ACTION_MOVE :
    	        for (int p = 0; p < ev.getPointerCount(); p++)
    	        	sendTouchState(ev, ev.getPointerId(p), 2);
    			break;    		
    		case MotionEvent.ACTION_POINTER_DOWN :    		
				sendTouchState(ev, action >> MotionEvent.ACTION_POINTER_ID_SHIFT, 0);
    			break;    		
    		case MotionEvent.ACTION_POINTER_UP:
				sendTouchState(ev, action >> MotionEvent.ACTION_POINTER_ID_SHIFT, 1);
    			break;
    	}
        
        return true;
    }

    GameRenderer mRenderer;
    
    private static native void nativeTouch(int id, int x, int y, int state);
}

class GameRenderer implements GLSurfaceView.Renderer {
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        nativeReset();
        GameActivity.nativeResume();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        nativeResize(w, h);
    }

    public void onDrawFrame(GL10 gl) {
    	GameGLSurfaceView.sendTouches();
    	synchronized (GameActivity.audioTrack) {
    		nativeUpdate();
    	}
        nativeRender();
    }

    private static native void nativeReset();
    private static native void nativeResize(int w, int h);
    private static native void nativeUpdate();
    private static native void nativeRender();
}
