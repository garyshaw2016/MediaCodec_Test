package com.cidana.testcustomdatasource;


import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;
//import android.support.v4.app.NavUtils;

public class FileListActivity extends Activity {

	private ListView mMediaFileListView;
	private ArrayAdapter<String> mVideoListAdapter;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Log.d("FileListActivity", "onCreate");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_file_list);
		// Show the Up button in the action bar.
		//getActionBar().setDisplayHomeAsUpEnabled(true);
		
		mMediaFileListView = (ListView)findViewById(R.id.listView1);	
		mVideoListAdapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_list_item_1, MediaFileList.getInstance().mlistMediaFile);
		mMediaFileListView.setAdapter(mVideoListAdapter);
		mMediaFileListView.setOnItemClickListener(mVideoListListener);
	}

	private OnItemClickListener mVideoListListener = new OnItemClickListener() {
		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
			// TODO Auto-generated method stub
			Intent intent = new Intent();
			Bundle b = new Bundle();
			b.putInt("index", position);
			intent.putExtras(b);
			intent.setClass(FileListActivity.this, SimpleMediaPlayer.class);
			startActivity(intent);
		}
	};
}
