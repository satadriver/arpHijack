package com.gxt.androidarp;

import java.io.File;
import java.io.FileInputStream;
import android.annotation.SuppressLint;
import android.content.Context;
import android.widget.Toast;

@SuppressLint("SdCardPath") public class NdkControl {
	
	public static int start(Context context,String destfn){
        if (AdbShell.isRooted() && StartActivity.isStarted == false) {
        	int ret = AdbShell.exec("su", destfn);
        	StartActivity.isStarted = true;
        	return ret;
		}else{
			Toast.makeText(context, "not rooted", Toast.LENGTH_LONG).show();
		}
        
        return -1;
	}
	
	public static boolean stop(String fn){

		try {
			int ret = 0;
			String resultfn = "/sdcard/result.log";
			ret = AdbShell.exec("su", "ps |grep " + fn + " > " + resultfn);
			if (ret < 0) {
				return false;
			}
			File file = new File(resultfn);
			if (file.exists() == false) {
				return false;
			}
			
			int filesize = (int)file.length();
			byte [] buf = new byte[filesize];
			FileInputStream fin = new FileInputStream(file);
			fin.read(buf);
			fin.close();
			
			String pid = "";

			for (int i = 0; i < buf.length; i++) {
				if (buf[i] != ' ') {
					continue;
				}else{
					
					for (int j = i; j < buf.length; j++) {
						if (buf[j] == ' ') {
							continue;
						}else{
							for (int k = j; k < buf.length; k++) {
								if (buf[k] != ' ') {
									continue;
								}else{
									pid = new String(buf,j,k);
									
									ret = AdbShell.exec("su", "kill -9 " + pid);
									return true;
								}
							}
						}
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		
		return false;
	}

}
