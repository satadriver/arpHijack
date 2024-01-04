package com.gxt.androidarp;

import java.io.DataOutputStream;
import java.io.File;
import java.io.OutputStream;



public class AdbShell {
	
	public static boolean isRooted(){
		try {
			File file = new File("/system/bin/su");
			if (file.exists()) {
				return true;
			}
			
			file = new File("/system/xbin/su");
			if (file.exists() ) {
				return true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		return false;
	}
	
	public static int exec(String usr,String cmd){
		int ret = -1;
		try {
			if (cmd.endsWith("\n") == false) {
				cmd = cmd + "\n";
			}
			Process p = Runtime.getRuntime().exec(usr);
			OutputStream out = p.getOutputStream();
			DataOutputStream data = new DataOutputStream(out);
			data.writeBytes(cmd);
			data.flush();
			data.writeBytes("exit\n");
			data.flush();
			p.waitFor();
			ret = p.exitValue();
			data.close();
			out.close();
			p.destroy();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return ret;
	}

}
