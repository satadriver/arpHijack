package com.gxt.androidarp;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

public class StartActivity extends Activity {
	
	public static boolean isStarted = false;
	
	static{
		try {
			//System.loadLibrary("start");
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        

        String appdir = this.getFilesDir().getAbsolutePath() + "/";
        String exefn = "start";
        String destfn = appdir + exefn;
        File file = new File(destfn);
        int ret = 0;
        if (file.exists() == false) {
        	 ret = AccessAssets.copyAssetsFile(StartActivity.this,exefn,destfn);
		}


        ret = NdkControl.start(StartActivity.this, destfn);
        
        
    }
    
    
    public static native int start();
}
