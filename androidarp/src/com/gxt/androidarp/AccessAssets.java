package com.gxt.androidarp;


import java.io.FileOutputStream;
import java.io.InputStream;

import android.content.Context;

public class AccessAssets {

	public static int copyAssetsFile(Context context,String fn,String dest){
		try {
			InputStream in = context.getAssets().open(fn);
			int size = in.available();
			if (size > 0) {
				
				byte []buf = new byte[size];
				in.read(buf);
				in.close();
				FileOutputStream fout = new FileOutputStream(dest);
				fout.write(buf);
				fout.close();
				
				return size;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		return -1;
	}
	
}
