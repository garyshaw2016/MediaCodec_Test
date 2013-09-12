package com.cidana.testcustomdatasource;

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.List;

//import android.util.Log;

public class MediaFileList {
	public List<String> mlistMediaFile = new ArrayList<String>();
	
	public static MediaFileList getInstance() {
		return MediaFileList.mMediaFileList;
	}
	
	protected MediaFileList() {
		refreshMediaFileList();
	}
	
	protected void refreshMediaFileList() {
		mlistMediaFile.clear();
		File searchPath = android.os.Environment.getExternalStorageDirectory();
		collectMediaFile(searchPath);
	}
	
	protected void collectMediaFile(File file) {
		if (file.getName().charAt(0) == '.') {
			return;
		}
		
		File[] files = file.listFiles(mediaFileFilter);
		for (File mediaFile : files) {
			if (mediaFile.isDirectory()) {
			//	Log.d("", "Go into dir " + mediaFile.getPath());
				collectMediaFile(mediaFile);
			} else {
				mlistMediaFile.add(mediaFile.getPath());
			}
		}
	}
	
	FilenameFilter mediaFileFilter = new FilenameFilter() {
		public boolean accept(File dir, String name) {
			String lowercaseName = name.toLowerCase();
			if (lowercaseName.endsWith(".mp4")
					|| lowercaseName.endsWith(".mkv")
					|| lowercaseName.endsWith(".ts")
					|| lowercaseName.endsWith(".3gp")) {
				return true;
			}
			else {
				File file = new File(dir, name);
				if (file.isDirectory()) {
					return true;
				}
				return false;
			}
		}
	};
	
	private static MediaFileList mMediaFileList = new MediaFileList();
}
