package com.audiobalance.proto;

import java.io.File;
import java.util.ArrayList;

import android.net.Uri;
import android.os.*;
import android.support.v4.app.*;
import android.support.v4.view.*;
import android.app.ActionBar;
import android.app.Activity;
import android.app.FragmentTransaction;
import android.app.ActionBar.Tab;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.*;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class AudioSelectorActivity extends FragmentActivity {

	ViewPager viewPager;
	AudioSelectorPagerAdapter audioSelectorPagerAdapter;

	public static class AudioSelectorTabInfos {
		public static final CharSequence [] tabNames = {
			"Device", "Built-in", "History"
		};
		public static final Fragment [] tabFragments = {
			new DeviceAudioSelectorFragment(),
			new BuiltinAudioSelectorFragment(),
			new HistoryAudioSelectorFragment()
		};
	}
	
	@SuppressWarnings("deprecation")
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_audioselector);
		audioSelectorPagerAdapter = new AudioSelectorPagerAdapter(getSupportFragmentManager());
		
		final ActionBar actionBar = getActionBar();
		actionBar.setDisplayHomeAsUpEnabled(true);
		
		viewPager = (ViewPager)findViewById(R.id.pager);
		viewPager.setAdapter(audioSelectorPagerAdapter);
		
		actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);
		actionBar.addTab(actionBar.newTab().setText(AudioSelectorTabInfos.tabNames[0]).setTabListener(new TabListener(0, viewPager)));
		actionBar.addTab(actionBar.newTab().setText(AudioSelectorTabInfos.tabNames[1]).setTabListener(new TabListener(1, viewPager)));
		actionBar.addTab(actionBar.newTab().setText(AudioSelectorTabInfos.tabNames[2]).setTabListener(new TabListener(2, viewPager)));
		
		viewPager.setOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {			
			public void onPageSelected(int page) {
				actionBar.setSelectedNavigationItem(page);
			}
		});
	}
	
	public class TabListener implements ActionBar.TabListener {
	
		private int item;
		private ViewPager viewPager;
		
		public TabListener(int item, ViewPager viewPager) {
			this.item = item;
			this.viewPager = viewPager;
		}
		
		@Override
		public void onTabUnselected(Tab tab, FragmentTransaction ft) {}
		@Override
		public void onTabSelected(Tab tab, FragmentTransaction ft) {
			viewPager.setCurrentItem(item);
		}
		@Override
		public void onTabReselected(Tab tab, FragmentTransaction ft) {}
		
	}
	
	public class AudioSelectorPagerAdapter extends FragmentPagerAdapter {
		public AudioSelectorPagerAdapter(FragmentManager fm) {
			super(fm);
		}
		
		@Override
		public Fragment getItem(int i) {
			Fragment fragment = AudioSelectorTabInfos.tabFragments[i];			
			return fragment;
		}
		
		@Override
		public int getCount() {
			return 3;
		}
		
		@Override
		public CharSequence getPageTitle(int position) {
			return AudioSelectorTabInfos.tabNames[position];
		}
		
	}
	
	
	public static class AudioSelectorFragment extends Fragment {
		
		protected MediaFile [] createDummyItems() {
			ArrayList<MediaFile> itemList = new ArrayList<MediaFile>();
			for (int i = 0; i < 10; i++) {
				MediaFile mf = new MediaFile();
				mf.name = "FileName" + (i+1) + ".mp3";
				itemList.add(mf);
			}
			MediaFile [] itemArray = new MediaFile[itemList.size()];
			itemList.toArray(itemArray);
			return itemArray;
		}
		
		protected void CreateList(View rootView, MediaFile [] items) {			
			ListView list = (ListView)rootView.findViewById(R.id.listview_audioselector);
			
			final MediaFile [] finalItems = filterItems(items);
			
			AudioListArrayAdapter adapter = new AudioListArrayAdapter(rootView.getContext(), finalItems);
			list.setAdapter(adapter);
			
			list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
				@Override
				public void onItemClick(AdapterView<?> parent, View view,
						int position, long id) {
					openMediaPlayer(finalItems[position]);
				}
			});
		}
		
		private MediaFile [] filterItems(MediaFile [] items) {
			ArrayList<MediaFile> retList = new ArrayList<MediaFile>();
			for(int i = 0; i < items.length; i++) {
				MediaFile mf = items[i];
				if (!mf.isAlarm && !mf.isNotification && !mf.isRingtone) {
					retList.add(mf);
				}
			}
			MediaFile [] ret = new MediaFile[retList.size()];
			return retList.toArray(ret);
		}
		
		private void openMediaPlayer(MediaFile mediaFile) {
			Activity activity = getActivity();
	    	Intent openMediaplayerIntent = new Intent(activity, MediaPlayerActivity.class);
	    	openMediaplayerIntent.setAction(Intent.ACTION_VIEW);
	    	Log.d("AudioSelectorActivity", "Opening file " + mediaFile.path);
	    	openMediaplayerIntent.setData(Uri.fromFile(new File(mediaFile.path)));
	    	if (mediaFile.path.startsWith("asset:")) {
	    		openMediaplayerIntent.putExtra("use_asset", true);
	    		openMediaplayerIntent.putExtra("asset_path", mediaFile.path);
	    	} else {
	    		openMediaplayerIntent.putExtra("use_asset", false);	    		
	    	}
	    	//openMediaplayerIntent.putExtra("path", mediaFile.path);
	    	startActivity(openMediaplayerIntent);
	    	activity.overridePendingTransition(R.anim.push_left_in, R.anim.push_left_out);    	
		}
	
		
		public static class AudioListArrayAdapter extends ArrayAdapter<MediaFile> {
			  private final Context context;
			  private final MediaFile[] values;

			  public AudioListArrayAdapter(Context context, MediaFile[] values) {
			    super(context, R.layout.listitem_audiofile, values);
			    this.context = context;
			    this.values = values;
			  }
			  
			  @Override
			  public View getView(int position, View convertView, ViewGroup parent) {
				    LayoutInflater inflater = (LayoutInflater) context
				            .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				    View rowView = inflater.inflate(R.layout.listitem_audiofile, parent, false);
				    
				    MediaFile mediaFile = values[position];
				    
				    TextView fileText = (TextView)rowView.findViewById(R.id.textview_audiofile_filename);
				    fileText.setText(mediaFile.formatTitle());
				    
				    TextView descText = (TextView)rowView.findViewById(R.id.textview_audiofile_desc);
				    if (mediaFile.duration > 0) {
				    	descText.setText(String.format("Length: %s", formatDuration(mediaFile.duration)));
				    	descText.setVisibility(View.VISIBLE);
				    } else {
				    	descText.setVisibility(View.INVISIBLE);
				    }
				    
				    return rowView;
			  }
			  
			  private String formatDuration(int durationMs) {
				  int seconds = durationMs / 1000;
				  int minutes = seconds / 60;
				  int hours = minutes / 60;
				  if (hours > 0) {
					  return String.format("%d:%02d:%02d", hours, minutes % 60, seconds % 60);
				  } else {
					  return String.format("%02d:%02d", minutes, seconds % 60);
				  }
			  }
			  
		}
		
	}
		

	
}

