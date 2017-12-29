package com.audiobalance.proto;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import android.content.Context;
import android.database.Cursor;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer;
import android.provider.MediaStore;
import android.util.Log;

public class MediaFile {
	public String name;
	public String artist;
	public int duration;
	public boolean isNotification;
	public boolean isAlarm;
	public boolean isRingtone;
	public String path;
	public boolean isVideo;
	
	public FileInputStream fileInputStream;

	public static MediaFile create(String path, Context ctx) {
		MediaFile ret = new MediaFile();
		Log.i("MediaFile", "File path: " + path);
		ret.path = path;
		if (path.startsWith("asset:")) {
			ret.name = path.substring("asset:".length());
			try {
				// Seems like getting the file descriptor directly from the assetdatabase will give you garbage at the end,
				// so write the data into a temporary cache file and then read the data from there...
				int size = (int)ctx.getAssets().openFd(ret.name).getLength();
				FileInputStream fis = ctx.getAssets().openFd(ret.name).createInputStream();
				File outputDir = ctx.getCacheDir();
				File outputFile = File.createTempFile("cache", "mp3", outputDir);
				FileOutputStream tmpfos = new FileOutputStream(outputFile);
				byte [] tmpdata = new byte[size];
				fis.read(tmpdata);
				tmpfos.write(tmpdata);
				tmpfos.close();
				fis.close();
				fis = new FileInputStream(outputFile);
				ret.fileInputStream = fis;
				MediaPlayer mp = new MediaPlayer();
				mp.setDataSource(fis.getFD());
				mp.prepare();
				ret.duration = mp.getDuration();
				mp.release();
			} catch(IOException e) {
				Log.e("MediaFile", "Couldn't open asset " + ret.name);
			}
		} else {
			MediaMetadataRetriever mmr = new MediaMetadataRetriever();
			mmr.setDataSource(path);
	
			ret.name = mmr
					.extractMetadata(MediaMetadataRetriever.METADATA_KEY_TITLE);
			ret.artist = mmr
					.extractMetadata(MediaMetadataRetriever.METADATA_KEY_ARTIST);
	
			//if (ret.name == null || ret.name.isEmpty())
			{
				String selection = MediaStore.Audio.Media.DATA + " == ?";
				Cursor cursor = ctx.getContentResolver().query(
						MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, null, selection,
						new String[] { path }, null);
				
				/*
				findFile(ctx.getContentResolver().query(
						MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, null, selection,
						new String[] { path }, null), "");
				*/
				boolean found = cursor.moveToNext();
				
				if (!found) {
					cursor = ctx.getContentResolver().query(
							MediaStore.Video.Media.EXTERNAL_CONTENT_URI, null, selection,
							new String[] { path }, null);
					ret.isVideo = true;
				} else {
					ret.isVideo = false;
				}
				
				if (found || cursor.moveToNext()) {
			        int titleColumn = cursor.getColumnIndex(MediaStore.Audio.Media.TITLE);
			        int artistColumn = cursor.getColumnIndex(MediaStore.Audio.Media.ARTIST);
			        int durationColumn = cursor.getColumnIndex(MediaStore.Audio.Media.DURATION);
			        ret.name = cursor.getString(titleColumn);
			        ret.artist = cursor.getString(artistColumn);
			        ret.duration = cursor.getInt(durationColumn);
				} else {
					Log.e("MediaFile", "Couldn't get info for file.");
				}
			}
	
			// ret.duration =
			// mmr.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);
		}
		return ret;
	}
	
	static boolean findFile(Cursor c, String name) {
		while(c.moveToNext()) {
			Log.d("MediaFile", "file: " + c.getString(c.getColumnIndex(MediaStore.Audio.Media.DATA)) );
		}
		return true;
	}

	public String formatTitle() {
		if ((artist == null || artist.isEmpty())
				&& (name == null || name.isEmpty())) {
			return "file://" + path;
		} else if (artist == null || artist.isEmpty() || artist == "<unknown>") {
			return name;
		} else {
			return String.format("%s - %s", artist, name);
		}
	}

}
